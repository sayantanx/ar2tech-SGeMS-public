/* -----------------------------------------------------------------------------
** Copyright© 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH) 
** version of the open source software sgems.  It is a derivative work by 
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original 
** version of the software (see notice below) and now sublicensed such that it 
** cannot be distributed or modified without the explicit and written permission 
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this 
** file/software.
**
** This file cannot be modified or distributed without the explicit and written 
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at 
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/





#include <GsTLAppli/gui/vtk_prop/vtk_prop_pointset.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/gui/utils/colormap.h>

#include <vtkProp.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkMaskPoints.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkFloatArray.h>
#include <vtkBitArray.h>
#include <vtkCellData.h>
#include <vtkLookupTableWithEnabling.h>
#include <vtkGeometryFilter.h>
#include <vtkMath.h>

Named_interface* vtkProp_pointset::create_new_interface( std::string& ) {
  return new vtkProp_pointset;
}

vtkProp_pointset::vtkProp_pointset()
  : GsTL_vtkProp(),
    grid_( 0 ), pset_polydata_(0),
    mapper_(0),color_table_(0) {
}


vtkProp_pointset::~vtkProp_pointset() {
	renderer_->RemoveActor(actor_);
  pset_polydata_->Delete();
  mapper_->Delete();
  actor_->Delete();
  if(color_table_) color_table_->Delete();
}

vtkProp* vtkProp_pointset::prop() {
  return actor_;
}

void vtkProp_pointset::set_visibility(bool on) {
	//return volume_;
	actor_->SetVisibility(on);
}

bool vtkProp_pointset::is_visibile(){
	return actor_->GetVisibility() == 1;

}

void vtkProp_pointset::init(  Geostat_grid* grid, vtkRenderer* renderer ) {
	renderer_ = renderer;
    geostat_grid_ = grid;
    grid_ = dynamic_cast< Point_set*>( grid );
  pset_polydata_ = vtkPolyData::New();


// Initialize the geometry of the pset
  std::vector<Geostat_grid::location_type> locs = grid_->point_locations();
  vtkPoints* xyzPts = vtkPoints::New();
  xyzPts->SetDataTypeToDouble();
  xyzPts->SetNumberOfPoints(locs.size());
  for(unsigned int i=0; i< locs.size(); i++) {
    Geostat_grid::location_type loc = locs[i];
    xyzPts->SetPoint(i,loc[0], loc[1], loc[2]);
  }
  pset_polydata_->SetPoints(xyzPts);


  vtkCellArray* pts_connection = vtkCellArray::New();
  for (int i = 0; i < pset_polydata_->GetNumberOfPoints(); i++) {
    int id = pts_connection->InsertNextCell(1);
    pts_connection->InsertCellPoint(i);
  }
  pset_polydata_->SetVerts(pts_connection);

  vtkBitArray *mask = vtkBitArray::New();
  mask->SetName("MaskRegion");
  mask->SetNumberOfValues(pset_polydata_->GetNumberOfPoints());
  pset_polydata_->GetCellData()->AddArray(mask);
  pset_polydata_->GetCellData()->SetScalars(0);

  vtk_property_->SetRepresentationToPoints();

  data_pass_through_ = vtkPassThrough::New();
  data_pass_through_->SetInput(pset_polydata_);

  mapper_ = vtkPolyDataMapper::New();
  mapper_->SetInput((vtkPolyData*)data_pass_through_->GetOutput());

  actor_ = vtkActor::New();
  actor_->SetProperty(vtk_property_);
  actor_->SetMapper(mapper_);
  actor_->PickableOn();

  this->set_visibility(false);
  renderer_->AddActor(actor_);


	// Set the thresholder for the region; perform the thresholding based on the visibility array
	region_threshold_ = vtkThreshold::New();
	region_threshold_->SetInput(pset_polydata_);
	region_threshold_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,"MaskRegion");
	region_threshold_->ThresholdBetween(1,1);

	region_filter_ = vtkGeometryFilter::New();
	region_filter_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
	region_filter_->SetInput(region_threshold_->GetOutput());


	this->point_size(3);

  property_display_mode( VTK::NOT_PAINTED );

}


void vtkProp_pointset::enable_full_view(){
	if( renderer_->HasViewProp(actor_)  ) {
		renderer_->AddActor(actor_);

	}
	actor_->SetVisibility(true);


}


void vtkProp_pointset::disable_full_view(){
	actor_->SetVisibility(false);
	renderer_->RemoveActor(actor_);

 }


void vtkProp_pointset::property_deleted( const std::string& prop_name ) {
  // If a property has been deleted, we have to:
  //   - remove its corresponding colormap entry if it exists, 
  //   - if the deleted property was being displayed, paint the object
  //     in the default color
  CmapMap::iterator found = cmap_map_.find( prop_name );
  if( found == cmap_map_.end() ) return;

  delete found->second.second;
  cmap_map_.erase( found );
  
  if( current_property_name_ == prop_name ) {
    // the deleted property was being displayed
    current_property_name_ = "";
    current_property_ = 0;
    cmap_ = 0;
    property_display_mode( VTK::NOT_PAINTED );
  }
}


void vtkProp_pointset::property_display_mode( VTK::Property_display_mode mode ) {
  property_display_mode_ = mode;

}


void vtkProp_pointset::point_size( int size ) {
  actor_->GetProperty()->SetPointSize(size);
}


void vtkProp_pointset::refresh() {
  refresh_colormap();
  if(current_property_ == 0 ) return;

  vtkFloatArray* vtk_array = vtkFloatArray::New();
  vtk_array->SetNumberOfTuples(1);


  vtk_array->SetNumberOfValues(grid_->size());

  for( int i = 0; i < grid_->size(); i++ ) {
  	if(current_property_->is_informed(i)  )
  		vtk_array->SetValue(i,current_property_->get_value(i));
  	else
  		vtk_array->SetValue(i,vtkMath::Nan());
  }

  pset_polydata_->GetCellData()->SetScalars(vtk_array);
  mapper_->Update();

}

void vtkProp_pointset::set_colormap( Colormap* cmap ){
	cmap_ = cmap;

	mapper_->SetScalarRange(cmap->lower_bound(), cmap->upper_bound());
	mapper_->SetLookupTable(cmap->color_table());
	mapper_->Modified();

	this->update_colorbar();

}

void vtkProp_pointset::refresh_colormap() {
  if(cmap_ == 0) return;
	cmap_->refresh();
	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());
	this->update_colorbar();
  /*
  color_table_->SetNumberOfTableValues(cmap_->color_scale()->colors_count());
  int nColors = color_table_->GetNumberOfTableValues();

  for( int i=0; i < nColors ; i++ ) {
	float r,g,b;
	cmap_->color_scale()->color(i, r,g,b);
	color_table_->SetTableValue(i,r,g,b);
  }
  color_table_->Build();
  mapper_->SetLookupTable(color_table_);
  */
}
void vtkProp_pointset::set_property( const std::string& property_name, Colormap* cmap ){

	cmap_ = cmap;
	this->remove_all_thresholding();

	if(property_name.empty()) {
		pset_polydata_->GetCellData()->SetScalars(0);
		pset_polydata_->Modified();
		current_property_name_.clear();
		current_property_ = 0;
		return;
	}

	if( !geostat_grid_->property( property_name ) ) {
	appli_warning( "grid has no property called " << property_name );
	return;
	}

	if( current_property_name_ == property_name ) return;

	current_property_ = geostat_grid_->property( property_name );
	current_property_->swap_to_memory();
	current_property_name_ = property_name;

	vtkFloatArray* vtk_array = vtkFloatArray::New();
	vtk_array->SetArray(current_property_->data(),current_property_->size(),1);

	pset_polydata_->GetCellData()->SetScalars(vtk_array);
	vtk_array->Delete();

	cmap_->refresh();
	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());


	data_pass_through_->Modified();
	mapper_->Modified();

	this->update_colorbar();

}

void vtkProp_pointset::set_region(const std::string& region_name ) {

	if(region_name == "" ) {
		current_region_ = 0;
		region_threshold_->SetInput(0);
		data_pass_through_->SetInput(pset_polydata_);


	}
	else {
		GsTLGridRegion* region = geostat_grid_->region( region_name );
		if( region == 0 ) {
			appli_warning( "grid has no region called " << region_name );
			return;
		}
		current_region_ = region;

//		if( current_region_->name() != cached_region_name_ ) {
		if( true ) {
			GsTLGridRegion::const_iterator it = current_region_->begin();
			vtkBitArray* region_mask = vtkBitArray::SafeDownCast(pset_polydata_->GetCellData()->GetArray("MaskRegion"));
			for(int i=0; it != current_region_->end();++i, ++it) {
				region_mask->SetValue(i,*it);
			}
		}
		region_threshold_->SetInput(pset_polydata_);
		data_pass_through_->SetInput(region_filter_->GetOutput());
    region_threshold_->Modified();

	}


}


bool vtkProp_pointset::connect_threshold_to_data(vtkThreshold* thresholder){
	thresholder->SetInput(vtkDataSet::SafeDownCast(data_pass_through_->GetOutput()));
  return true;
}

bool vtkProp_pointset::enable_threshold_pipeline(){
//	data_pass_through_->SetInput(0);
	mapper_->SetInput(threshold_poly_data_->GetOutput());
	mapper_->Modified();
  return true;
}

bool vtkProp_pointset::disable_threshold_pipeline(){

	mapper_->SetInput((vtkPolyData*)data_pass_through_->GetOutput());

	mapper_->Modified();

  return true;

}



void vtkProp_pointset::set_colortable_to_mapper(vtkMapper* mapper){
	double *scalar_range = mapper_->GetScalarRange();
	mapper->SetScalarRange(scalar_range[0],scalar_range[1]);
	mapper->SetLookupTable(mapper_->GetLookupTable());
}
