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
#include <GsTLAppli/gui/vtk_prop/vtk_prop_structured_grid.h>

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
 


Named_interface* vtkProp_structured_grid::create_new_interface( std::string& ) {
  return new vtkProp_structured_grid;
}

bool vtkProp_structured_grid::requires_manual_override( float x, float y, float z ) {
	const float min_ratio = 0.05;
  if( x/y < min_ratio || y/x < min_ratio ) return true;
  if( x/z < min_ratio || z/x < min_ratio ) return true;
  if( z/y < min_ratio || y/z < min_ratio ) return true;

	return false;
}

vtkProp_structured_grid::vtkProp_structured_grid()
  : GsTL_vtkProp(),  grid_( 0 ),
    initialized_(false),
     mapper_(0),structured_grid_(0),
    vtk_blanking_(0)

 {

}
 
vtkProp_structured_grid::~vtkProp_structured_grid() {

  renderer_->RemoveActor(actor_);
  structured_grid_->Delete();
  mapper_->Delete();
  actor_->Delete();
  if(vtk_blanking_)
    vtk_blanking_->Delete();

}



vtkProp* vtkProp_structured_grid::prop(){

	return actor_;
}
void vtkProp_structured_grid::set_visibility(bool on) {

  actor_->SetVisibility(on);
  this->update_colorbar_visibility();

}

bool vtkProp_structured_grid::is_visibile(){
	return actor_->GetVisibility() == 1; // || region_actor_->GetVisibility() == 1;

}


 void vtkProp_structured_grid::init( Geostat_grid* grid, vtkRenderer* renderer ) {
	 renderer_ = renderer;
    geostat_grid_ = grid;
    grid_ = dynamic_cast<Structured_grid*>( grid );
  GsTLCoordVector cell_dims = grid_->cell_dimensions();
  GsTLPoint origin = grid_->origin();

  // Need to double check the origin to be sure it fits the center of the cell

  structured_grid_ = vtkStructuredGrid::New();
  structured_grid_->DeepCopy(grid_->get_structured_geometry().GetPointer());
  structured_grid_->GetCellData()->SetScalars(0);
  structured_grid_->GetPointData()->SetScalars(0);

// Set the thresholder for the region; perform the thresholding based on the visibility array
  region_threshold_ = vtkThreshold::New();
  region_threshold_->SetInputData(structured_grid_);
  region_threshold_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
  region_threshold_->ThresholdBetween(-1e9,1e9);

  data_pass_through_filter_ = vtkPassThrough::New();
  data_pass_through_filter_->SetInputData(structured_grid_);

  surface_extractor_ = vtkDataSetSurfaceFilter::New();
  surface_extractor_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
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
  
}




void vtkProp_structured_grid::property_deleted( const std::string& prop_name ) {
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

  }
  
}


void vtkProp_structured_grid::show_bounding_box( bool on ) {
//	out = vtk.vtkOutlineFilter()
//	out.SetInput(uGrid)
//	mapper = vtk.vtkDataSetMapper()
//	mapper.SetInput(out.GetOutput())
//	actor = vtk.vtkActor()
//	actor.SetMapper(mapper)
//	actor_->GetProperty()->SetRepresentationToWireframe();
}


void vtkProp_structured_grid::hide_volume(bool on) {
  if( on )
    actor_->VisibilityOff();
  else
    actor_->VisibilityOn();
}


void vtkProp_structured_grid::refresh() {


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

  structured_grid_->GetCellData()->SetScalars(vtk_array);
  vtk_array->Delete();

  actor_->GetProperty()->SetInterpolationToFlat();

  cmap_->refresh();
  mapper_->Update();


}


void vtkProp_structured_grid::refresh_colormap() {

  if(cmap_ == 0) return;
	cmap_->refresh();

	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());
/*
  structured_section_map::iterator it = structured_section_pipelines_.begin();
  for( ; it != structured_section_pipelines_.end(); ++it) {
    it->second.mapper->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
    it->second.mapper->SetLookupTable(cmap_->color_table());
  }
  */
}

void vtkProp_structured_grid::enable_full_view(){
	if( renderer_->HasViewProp(actor_)  ) {
		renderer_->AddActor(actor_);

	}
	actor_->SetVisibility(true);


}


void vtkProp_structured_grid::disable_full_view(){
	actor_->SetVisibility(false);
	renderer_->RemoveActor(actor_);

 }


void vtkProp_structured_grid::set_property( const std::string& property_name ){

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

	  structured_grid_->GetCellData()->SetScalars(vtk_array);
	  vtk_array->Delete();

}

void vtkProp_structured_grid::set_property( const std::string& property_name, Colormap* cmap ){


	cmap_ = cmap;
	this->remove_all_thresholding();

	if(property_name.empty()) {
		structured_grid_->GetCellData()->SetScalars(0);
//		actor_->GetProperty()->SetRepresentationToWireframe();
		structured_grid_->Modified();
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

	structured_grid_->GetCellData()->SetScalars(vtk_array);
	vtk_array->Delete();

	cmap_->refresh();
	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());

	surface_extractor_->Modified();
	actor_->Modified();

	this->update_colorbar();

}


void vtkProp_structured_grid::set_colormap( Colormap* cmap ){
	cmap_ = cmap;

	mapper_->SetScalarRange(cmap->lower_bound(), cmap->upper_bound());
	mapper_->SetLookupTable(cmap->color_table());
	mapper_->Modified();

	this->update_thresholding_colortable();
	this->update_colorbar();

}




void vtkProp_structured_grid::set_region(const std::string& region_name ){
//  const GsTLGridRegion* region = geostat_grid_->region( region_name );


	if(region_name == "" ) {
		current_region_ = 0;
		for(int i=0; i<structured_grid_->GetNumberOfCells(); ++i) {
			structured_grid_->UnBlankCell(i);
		}

//  Connect the pipeline directly to image_data
		region_threshold_->SetInputData(0);
		data_pass_through_filter_->SetInputData(structured_grid_);
//		mapper_->SetInput(structured_grid_);



	}
	else {
		GsTLGridRegion* region = geostat_grid_->region( region_name );
		if( region == 0 ) {
			appli_warning( "grid has no region called " << region_name );
			return;
		}
		current_region_ = region;

// Connect the region pipeline
		region_threshold_->SetInputData(structured_grid_);
//		mapper_->SetInput(surface_extractor_->GetOutput());
//		pre_mapper_pass_through_filter_->SetInput(surface_extractor_->GetOutput());
		data_pass_through_filter_->SetInputConnection(region_threshold_->GetOutputPort());

//		if( current_region_->name() != cached_region_name_ ) {
		if( true ) {
			GsTLGridRegion::const_iterator it = current_region_->begin();
			for(int i=0; it != current_region_->end();++i, ++it) {
			  if(*it) structured_grid_->UnBlankCell(i);
			  else structured_grid_->BlankCell(i);
			}
			structured_grid_->Modified();
			cached_region_name_ = current_region_->name();

		}
	}
	data_pass_through_filter_->Modified();
	surface_extractor_->Modified();
	mapper_->Modified();



}


bool vtkProp_structured_grid::connect_threshold_to_data(vtkThreshold* thresholder){
	thresholder->SetInputConnection(data_pass_through_filter_->GetOutputPort());
  return true;
}

bool vtkProp_structured_grid::enable_threshold_pipeline(){

//	surface_extractor_->SetInputData(0);
	mapper_->SetInputConnection(threshold_poly_data_->GetOutputPort());
	mapper_->Modified();
  return true;
}

bool vtkProp_structured_grid::disable_threshold_pipeline(){

  if(is_section_active_) {
    this->enable_section_pipeline();
  }
  else {
//	  surface_extractor_->SetInputConnection(data_pass_through_filter_->GetOutputPort());
	  mapper_->SetInputConnection(surface_extractor_->GetOutputPort());
  }

	mapper_->Modified();
  return true;

}



bool vtkProp_structured_grid::enable_section_pipeline(){

	mapper_->SetInputConnection(section_poly_data_->GetOutputPort());
	mapper_->Modified();
  return true;
}

bool vtkProp_structured_grid::disable_section_pipeline(){

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


void vtkProp_structured_grid::set_colortable_to_mapper(vtkMapper* mapper){
	double *scalar_range = mapper_->GetScalarRange();
	mapper->SetScalarRange(scalar_range[0],scalar_range[1]);
	mapper->SetLookupTable(mapper_->GetLookupTable());
}


//======================================================

int vtkProp_structured_grid::add_section(int id, QString orientation, bool is_visible){

	structured_section_map::iterator it = structured_section_pipelines_.find(id);
	if( it !=  structured_section_pipelines_.end()) {  // Already exist
		return -1;
	}

  int max_extent;

	structured_section_pipeline section;
  section.id = id;
  section.enabled = is_visible;
  section.plane = vtkStructuredGridGeometryFilter::New();

  it = structured_section_pipelines_.insert(structured_section_pipelines_.begin(),std::make_pair(id,section));
  it->second.plane->SetInputConnection(data_pass_through_filter_->GetOutputPort());

  int dim[3];
  structured_grid_->GetDimensions(dim);

  if(orientation == "X") {
    max_extent = dim[0];
    it->second.plane->SetExtent(0,0,0,dim[1]-1,0,dim[2]-1);
    it->second.orientation = XSECTION;
  }
  else if(orientation == "Y") {
    max_extent = dim[1];
    it->second.plane->SetExtent(0,dim[0]-1,0,0,0,dim[2]-1);
    it->second.orientation = YSECTION;

  }else if(orientation == "Z") {
    max_extent = dim[2];
    it->second.plane->SetExtent(0,dim[0]-1,0,dim[1]-1,0,0);
    it->second.orientation = ZSECTION;
  }

	if(is_visible) {
    section_poly_data_->AddInputConnection(it->second.plane->GetOutputPort());
		//section_poly_data_->AddInputData(it->second.plane->GetOutput());
		if(is_section_active_ == false) {
			this->enable_section_pipeline();
			is_section_active_ = true;
		}
	}
  return max_extent;

}


bool vtkProp_structured_grid::update_section(int id, int step, bool is_visible){

	structured_section_map::iterator it = structured_section_pipelines_.find(id);
	if( it ==  structured_section_pipelines_.end()) {  // does not exist
		return false;
	}
  int dim[3];
  structured_grid_->GetDimensions(dim);

  if(it->second.orientation == XSECTION) {
    if( step >  dim[0]-1) step = dim[0]-1;
    it->second.plane->SetExtent(step,step,0,dim[1]-1,0,dim[2]-1);
  }
  else if(it->second.orientation == YSECTION) {
    if( step >  dim[1]-1) step = dim[1]-1;
    it->second.plane->SetExtent(0,dim[0]-1,step,step,0,dim[2]-1);

  }
  else if(it->second.orientation == ZSECTION) {
    if( step >  dim[2]-1) step = dim[2]-1;
    it->second.plane->SetExtent(0,dim[0]-1,0,dim[1]-1,step,step);

  }

  it->second.plane->Modified();


}



bool vtkProp_structured_grid::remove_all_sections(){

  section_poly_data_->RemoveAllInputs();
	structured_section_map::iterator it = structured_section_pipelines_.begin();

	for( ; it != structured_section_pipelines_.end(); ++it) {
    //section_poly_data_->RemoveInputData(it->second.plane->GetOutput());
    section_poly_data_->RemoveInputConnection(0,it->second.plane->GetOutputPort());
    it->second.plane->Delete();
	}
	structured_section_pipelines_.clear();

	is_section_active_ == false;
  this->disable_section_pipeline();

	return true;

}


bool vtkProp_structured_grid::remove_section(int id){


	structured_section_map::iterator it = structured_section_pipelines_.find(id);

	if(it == structured_section_pipelines_.end()) return true;

  //section_poly_data_->RemoveInputData(it->second.plane->GetOutput());
  section_poly_data_->RemoveInputConnection(0,it->second.plane->GetOutputPort());

  it->second.plane->Delete();

	structured_section_pipelines_.erase(it);

	is_section_active_ = false;
	for(int i=0; i<structured_section_pipelines_.size();++i) {
		if( structured_section_pipelines_[i].enabled ) {
			is_thresold_active_= true;
			break;
		}
	}

	if(is_thresold_active_ == false) {
		this->disable_section_pipeline();
	}

	return true;
}

bool vtkProp_structured_grid::enable_section(int id){

  structured_section_map::iterator it = structured_section_pipelines_.find(id);
  if(it == structured_section_pipelines_.end()) return false;

  it->second.enabled = true;

  //section_poly_data_->AddInputData(it->second.plane->GetOutput());
  section_poly_data_->AddInputConnection(it->second.plane->GetOutputPort());
  if(is_section_active_ == false) {
    is_section_active_ = true;
    this->enable_section_pipeline();
  }

  return true;
}


bool vtkProp_structured_grid::disable_section(int id){

  structured_section_map::iterator it = structured_section_pipelines_.find(id);
  if(it == structured_section_pipelines_.end()) return false;

  //section_poly_data_->RemoveInputData(it->second.plane->GetOutput());
  section_poly_data_->RemoveInputConnection(0,it->second.plane->GetOutputPort());
  it->second.enabled = false;


	is_section_active_ = false;
	for(int i=0; i<structured_section_pipelines_.size();++i) {
		if( structured_section_pipelines_[i].enabled ) {
			is_section_active_  = true;
		}
	}
  if( !is_section_active_  )
	  this->disable_section_pipeline();
  
  return true;

}
