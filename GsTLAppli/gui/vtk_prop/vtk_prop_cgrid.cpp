/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
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



/*Author Alexandre Boucher

*/
#include <GsTLAppli/gui/vtk_prop/vtk_prop_cgrid.h>

#include <GsTLAppli/gui/utils/colorscale.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <GsTL/math/math_functions.h>

#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkImagePlaneWidget.h>
#include <vtkCellPicker.h>
#include <vtkImageMapToColors.h>
#include <vtkBitArray.h>
#include <vtkMath.h>
#include <vtkAppendPolyData.h>


#include <vtkVolumeRayCastCompositeFunction.h>

#include <qstring.h>
#include <qstringlist.h>

#include <iterator>
#include <algorithm>
#include <sstream> 

#include <vtkBitArray.h>

#include <fstream>
 


Named_interface* vtkProp_cgrid::create_new_interface( std::string& ) {
  return new vtkProp_cgrid;
}

bool vtkProp_cgrid::requires_manual_override( float x, float y, float z ) {
	const float min_ratio = 0.05;
  if( x/y < min_ratio || y/x < min_ratio ) return true;
  if( x/z < min_ratio || z/x < min_ratio ) return true;
  if( z/y < min_ratio || y/z < min_ratio ) return true;

	return false;
}

vtkProp_cgrid::vtkProp_cgrid()
  : GsTL_vtkProp(),  grid_( 0 ),
    initialized_(false),
     mapper_(0),image_data_(0),
    vtk_blanking_(0)

 {

}
 
vtkProp_cgrid::~vtkProp_cgrid() {

  renderer_->RemoveActor(actor_);
  image_data_->Delete();
  mapper_->Delete();
  actor_->Delete();
  if(vtk_blanking_)
    vtk_blanking_->Delete();

}



vtkProp* vtkProp_cgrid::prop(){

	return actor_;
}
void vtkProp_cgrid::set_visibility(bool on) {

  actor_->SetVisibility(on);
  /*
  if(on) {
    if(is_section_active_) {
      actor_->SetVisibility(false);
      section_map::iterator it = section_pipelines_->begin();
      for( ; it != section_pipelines_.end(); ++it) {
        if(it->second.enabled) {
          it->second.actor->SetVisibility(on);
        }
      }
      }
    else {
	    actor_->SetVisibility(on);
    }
  }
  else {
    actor_->SetVisibility(false);
      section_map::iterator it = section_pipelines_->begin();
      for( ; it != section_pipelines_.end(); ++it)
        it->second.actor->SetVisibility(false);
  }

  */
	/*
	if( on ) {
		if( is_thresold_active_ ) {
			this->enable_thresholding();
		}
		else
			this->enable_full_view();
	}
	else {
		if( is_thresold_active_ ) {
			this->disable_thresholding();
		}
		this->disable_full_view();
	}
*/
}

bool vtkProp_cgrid::is_visibile(){
	return actor_->GetVisibility() == 1; // || region_actor_->GetVisibility() == 1;

}


 void vtkProp_cgrid::init( Geostat_grid* grid, vtkRenderer* renderer ) {
	 renderer_ = renderer;
    geostat_grid_ = grid;
    grid_ = dynamic_cast<Cartesian_grid*>( grid );
  GsTLCoordVector cell_dims = grid_->cell_dimensions();
  GsTLPoint origin = grid_->origin();

  // Need to double check the origin to be sure it fits the center of the cell

  image_data_ = vtkUniformGrid::New();
  image_data_->SetExtent(0,grid_->nx(), 0,grid_->ny(), 0,grid_->nz());
  image_data_->SetOrigin(origin.x()-cell_dims.x()/2, origin.y()-cell_dims.y()/2,origin.z()-cell_dims.z()/2);
  image_data_->SetSpacing(cell_dims.x(),cell_dims.y(),cell_dims.z());
//  image_data_->SetScalarTypeToFloat ();
  image_data_->GetCellData()->SetScalars(0);
  image_data_->GetPointData()->SetScalars(0);

// Set the thresholder for the region; perform the thresholding based on the visibility array
  region_threshold_ = vtkThreshold::New();
  region_threshold_->SetInputData(image_data_);
  region_threshold_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
  region_threshold_->ThresholdBetween(-1e9,1e9);

  data_pass_through_filter_ = vtkPassThrough::New();
  data_pass_through_filter_->SetInputData(image_data_);

  surface_extractor_ = vtkDataSetSurfaceFilter::New();
  surface_extractor_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
  //surface_extractor_->SetInput((vtkDataSet*)data_pass_through_filter_->GetOutput());
  surface_extractor_->SetInputConnection(data_pass_through_filter_->GetOutputPort());

	vtk_property_->SetRepresentationToSurface();
	vtk_property_->EdgeVisibilityOff();

  mapper_ = vtkDataSetMapper::New();
  mapper_->SetInputConnection(surface_extractor_->GetOutputPort());

  actor_ = vtkActor::New();
  actor_->SetProperty(vtk_property_);
  actor_->SetMapper(mapper_);
  actor_->PickableOn();
  actor_->RotateZ(-1.0*grid_->geometry()->rotation_z());

  this->set_visibility(false);
  renderer_->AddActor(actor_);

  //build a cutter for testing
  
}




void vtkProp_cgrid::property_deleted( const std::string& prop_name ) {
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


int vtkProp_cgrid::vtk_3d_slices(vtkRenderWindowInteractor* iren){
  vtkCellPicker* picker = vtkCellPicker::New();
  picker->SetTolerance(0.005);

  vtkImageMapToColors* imageToColor = vtkImageMapToColors::New();
  imageToColor->SetLookupTable(cmap_->color_table());
  imageToColor->SetInputData(image_data_);


// The 3 image plane widgets are used to probe the dataset.
	vtkImagePlaneWidget* planeWidgetX = vtkImagePlaneWidget::New();
	planeWidgetX->DisplayTextOn();
	//planeWidgetX->SetInput(image_data_);
	planeWidgetX->SetInputConnection(imageToColor->GetOutputPort());
	planeWidgetX->SetPlaneOrientationToXAxes();
	planeWidgetX->SetSliceIndex(4);
	planeWidgetX->SetPicker(picker);
	planeWidgetX->SetResliceInterpolateToNearestNeighbour();
	planeWidgetX->RestrictPlaneToVolumeOn ();
	planeWidgetX->TextureInterpolateOff ();
//	planeWidgetX->SetKeyPressActivationValue("x");
	vtkProperty*  propx = planeWidgetX->GetPlaneProperty();
	propx->SetColor(1, 0, 0);
	propx->SetInterpolationToFlat();

//	planeWidgetX->SetLookupTable(color_table_);
	planeWidgetX->SetProp3D(actor_);
	planeWidgetX->SetInteractor(iren);
	planeWidgetX->On();

  return true;
}


int vtkProp_cgrid::add_slice( VTK::Axis axis, int position,
			   bool showed ) {
/*
  GsTLCoordVector cell_dims = grid_->cell_dimensions();

  // notice that the origin returned by the grid is the center 
  // of the origin cell, not its corner (cell-centered grid)
  GsTLPoint origin = grid_->origin();
  
  GsTLVector<int> grid_sizes( grid_->nx(), grid_->ny(), grid_->nz() );
  int max_pos = grid_sizes[axis]-1;

  Oinv_slice_cgrid* new_slice = 
    new Oinv_slice_cgrid( axis, position, grid_sizes, 
                          voxel_data_, &initialized_, cmap_,
			                    cell_dims[0], cell_dims[1], cell_dims[2],
			                    origin.x(), origin.y(), origin.z() );

  new_slice->property_display_mode( property_display_mode_ );
  GsTL_SoNode* node = new_slice->oinv_node();
  node->visible = showed;
  //new_slice->oinv_node()->visible = showed;

  slices_.push_back( new_slice );
  slices_node_->addChild( new_slice->oinv_node() );

  return slices_.size()-1;
  */
  return 0;
}


void vtkProp_cgrid::remove_slice( int slice_id ) {
  /*
  if( slice_id < 0 || slice_id >= int( slices_.size() ) ) {
    appli_warning( "Slice id " << slice_id << " is out of bounds" );
    return;
  }
  
  slices_node_->removeChild( slice_id );

  std::list< Oinv_slice* >::iterator it = slices_.begin();
  std::advance( it, slice_id ); 
  delete *it;
  slices_.erase( it );
  */
}



void vtkProp_cgrid::display_mode( Display_mode mode ) {
 // display_switch_->whichChild = int( mode );
}


void vtkProp_cgrid::property_display_mode( VTK::Property_display_mode mode ) {
	switch(mode) {
	case VTK::NOT_PAINTED :
		image_data_->GetCellData()->SetScalars(0);
//		vtk_property_->SetRepresentationToWireframe();
		current_property_name_ = "";
		break;
	}

}




void vtkProp_cgrid::show_bounding_box( bool on ) {
//	out = vtk.vtkOutlineFilter()
//	out.SetInput(uGrid)
//	mapper = vtk.vtkDataSetMapper()
//	mapper.SetInput(out.GetOutput())
//	actor = vtk.vtkActor()
//	actor.SetMapper(mapper)
//	actor_->GetProperty()->SetRepresentationToWireframe();
}


void vtkProp_cgrid::hide_volume(bool on) {
  if( on )
    actor_->VisibilityOff();
  else
    actor_->VisibilityOn();
}


void vtkProp_cgrid::refresh() {


  if(current_property_ == 0 ) {
  	if( actor_->GetProperty()->GetRepresentation() !=  VTK_WIREFRAME)
  		actor_->GetProperty()->SetRepresentationToWireframe();
  	return;
  }
  refresh_colormap();

  vtkFloatArray* vtk_array = vtkFloatArray::New();
  // Directly use the float* array, the option 1 means that it will not be deleted
  // Had to remove the const from the input grid
  vtk_array->SetArray(current_property_->data(),current_property_->size(),1);


  vtk_array->SetNumberOfTuples(1);

  vtk_array->SetNumberOfValues(grid_->size());

  image_data_->GetCellData()->SetScalars(vtk_array);
  vtk_array->Delete();

  actor_->GetProperty()->SetInterpolationToFlat();

  cmap_->refresh();
  mapper_->Update();


}


void vtkProp_cgrid::refresh_colormap() {

  if(cmap_ == 0) return;
	cmap_->refresh();

	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());
/*
  section_map::iterator it = section_pipelines_.begin();
  for( ; it != section_pipelines_.end(); ++it) {
    it->second.mapper->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
    it->second.mapper->SetLookupTable(cmap_->color_table());
  }
  */
}

void vtkProp_cgrid::enable_full_view(){
	if( renderer_->HasViewProp(actor_)  ) {
		renderer_->AddActor(actor_);

	}
	actor_->SetVisibility(true);


}


void vtkProp_cgrid::disable_full_view(){
	actor_->SetVisibility(false);
	renderer_->RemoveActor(actor_);

 }


void vtkProp_cgrid::set_property( const std::string& property_name ){

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

	  image_data_->GetCellData()->SetScalars(vtk_array);
	  vtk_array->Delete();

}

void vtkProp_cgrid::set_property( const std::string& property_name, Colormap* cmap ){


	cmap_ = cmap;
	this->remove_all_thresholding();

	if(property_name.empty()) {
		image_data_->GetCellData()->SetScalars(0);
//		actor_->GetProperty()->SetRepresentationToWireframe();
		image_data_->Modified();
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

	image_data_->GetCellData()->SetScalars(vtk_array);
	vtk_array->Delete();

	cmap_->refresh();
	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());

	surface_extractor_->Modified();
	actor_->Modified();

	this->update_colorbar();

}


void vtkProp_cgrid::set_colormap( Colormap* cmap ){
	cmap_ = cmap;

	mapper_->SetScalarRange(cmap->lower_bound(), cmap->upper_bound());
	mapper_->SetLookupTable(cmap->color_table());
	mapper_->Modified();

	this->update_thresholding_colortable();
	this->update_colorbar();

}




void vtkProp_cgrid::set_region(const std::string& region_name ){
//  const GsTLGridRegion* region = geostat_grid_->region( region_name );


	if(region_name == "" ) {
		current_region_ = 0;
		for(int i=0; i<image_data_->GetNumberOfCells(); ++i) {
			image_data_->UnBlankCell(i);
		}

//  Connect the pipeline directly to image_data
		region_threshold_->SetInputData(0);
		data_pass_through_filter_->SetInputData(image_data_);
//		mapper_->SetInput(image_data_);



	}
	else {
		GsTLGridRegion* region = geostat_grid_->region( region_name );
		if( region == 0 ) {
			appli_warning( "grid has no region called " << region_name );
			return;
		}
		current_region_ = region;

// Connect the region pipeline
		region_threshold_->SetInputData(image_data_);
//		mapper_->SetInput(surface_extractor_->GetOutput());

		data_pass_through_filter_->SetInputConnection(region_threshold_->GetOutputPort());

//		if( current_region_->name() != cached_region_name_ ) {
		if( true ) {
			GsTLGridRegion::const_iterator it = current_region_->begin();
			for(int i=0; it != current_region_->end();++i, ++it) {
			  if(*it) image_data_->UnBlankCell(i);
			  else image_data_->BlankCell(i);
			}
			image_data_->Modified();
			cached_region_name_ = current_region_->name();

		}
	}
	data_pass_through_filter_->Modified();
	surface_extractor_->Modified();
	mapper_->Modified();



}


bool vtkProp_cgrid::connect_threshold_to_data(vtkThreshold* thresholder){
	thresholder->SetInputConnection(data_pass_through_filter_->GetOutputPort());
  return true;
}

bool vtkProp_cgrid::enable_threshold_pipeline(){

	//surface_extractor_->SetInputData(0);
  surface_extractor_->RemoveAllInputConnections(0);
	mapper_->SetInputConnection(threshold_poly_data_->GetOutputPort());
  threshold_poly_data_->Modified();
	mapper_->Modified();
  return true;
}

bool vtkProp_cgrid::disable_threshold_pipeline(){

  if(is_section_active_) {
    this->enable_section_pipeline();
  }
  else {
	  surface_extractor_->SetInputConnection(data_pass_through_filter_->GetOutputPort());
	  mapper_->SetInputConnection(surface_extractor_->GetOutputPort());
  }

	mapper_->Modified();
  return true;

}



bool vtkProp_cgrid::enable_section_pipeline(){

	mapper_->SetInputConnection(section_poly_data_->GetOutputPort());
	mapper_->Modified();
  return true;
}

bool vtkProp_cgrid::disable_section_pipeline(){

  if(is_thresold_active_) {
    this->enable_threshold_pipeline();
  }
  else {

	  surface_extractor_->SetInputConnection(data_pass_through_filter_->GetOutputPort());
	  mapper_->SetInputConnection(surface_extractor_->GetOutputPort());

	  mapper_->Modified();
  }
  return true;

}


void vtkProp_cgrid::set_colortable_to_mapper(vtkMapper* mapper){
	double *scalar_range = mapper_->GetScalarRange();
	mapper->SetScalarRange(scalar_range[0],scalar_range[1]);
	mapper->SetLookupTable(mapper_->GetLookupTable());
}


//======================================================

int vtkProp_cgrid::add_section(int id, QString orientation, bool is_visible){

	section_map::iterator it = section_pipelines_.find(id);
	if( it !=  section_pipelines_.end()) {  // Already exist
		return -1;
	}

	section_pipeline section;
  section.id = id;
  section.enabled = is_visible;
  section.plane = vtkPlane::New();
  section.cutter = vtkCutter::New();
  section.mapper = vtkPolyDataMapper::New();
  section.actor = vtkActor::New();

  it = section_pipelines_.insert(section_pipelines_.begin(),std::make_pair(id,section));

  int max_extent = -1;

  int min_extent_x,min_extent_y,min_extent_z;
  int max_extent_x,max_extent_y,max_extent_z;
  image_data_->GetExtent(min_extent_x,max_extent_x,min_extent_y,
                         max_extent_y,min_extent_z,max_extent_z);
  if(orientation == "X") {
    max_extent = max_extent_x;
    it->second.plane->SetNormal(1,0,0);
    it->second.orientation = XSECTION;
  }
  else if(orientation == "Y") {
     max_extent = max_extent_y;
     it->second.plane->SetNormal(0,1,0);
     it->second.orientation = YSECTION;

  }else if(orientation == "Z") {
     max_extent = max_extent_z;
     it->second.plane->SetNormal(0,0,1);
     it->second.orientation = ZSECTION;
  }
  
  it->second.plane->SetOrigin(grid_->origin().x(),grid_->origin().y(),grid_->origin().z());
  //it->second.plane->SetOrigin(image_data_->GetOrigin());
  it->second.cutter->SetInputConnection(data_pass_through_filter_->GetOutputPort());
  it->second.cutter->SetCutFunction(it->second.plane);
//  it->second.mapper->SetInputConnection(it->second.cutter->GetOutputPort());
//  it->second.mapper->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
//  it->second.mapper->SetLookupTable(cmap_->color_table());
//  it->second.actor->SetMapper(it->second.mapper);
//  renderer_->AddActor(it->second.actor);

	if(is_visible) {
		section_poly_data_->AddInputConnection(0,it->second.cutter->GetOutputPort());
		if(is_section_active_ == false) {
			this->enable_section_pipeline();
			is_section_active_ = true;
		}
	}
  return max_extent;

}


bool vtkProp_cgrid::update_section(int id, int step, bool is_visible){

	section_map::iterator it = section_pipelines_.find(id);
	if( it ==  section_pipelines_.end()) {  // does not exist
		return false;
	}

  //Get the displacement from the step number
  int min_extent_x,min_extent_y,min_extent_z;
  int max_extent_x,max_extent_y,max_extent_z;
  image_data_->GetExtent(min_extent_x,min_extent_y,min_extent_z,
                         max_extent_x,max_extent_y,max_extent_z);

  double x_cell_dim, y_cell_dim,z_cell_dim;
  image_data_->GetSpacing(x_cell_dim, y_cell_dim,z_cell_dim);

  double x_origin, y_origin,z_origin;
  image_data_->GetOrigin(x_origin, y_origin,z_origin);

  if(it->second.orientation == XSECTION) {
    if( step >  max_extent_x) step = max_extent_x;
    it->second.plane->SetOrigin(x_origin+x_cell_dim*step, y_origin,z_origin );
  }
  else if(it->second.orientation == YSECTION) {
    it->second.plane->SetNormal(0,1,0);
    it->second.plane->SetOrigin(x_origin, y_origin+ y_cell_dim*step,z_origin );

  }
  else if(it->second.orientation == ZSECTION) {
    it->second.plane->SetNormal(0,0,1);
    it->second.plane->SetOrigin(x_origin, y_origin,z_origin + z_cell_dim*step );

  }

  it->second.plane->Modified();


}