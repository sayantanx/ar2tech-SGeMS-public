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



/*
 * Author: Alexandre Boucher, @ar2tech
 */

#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
#include <GsTLAppli/gui/utils/colorscale.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <algorithm>
#include <qstring.h>

#include <vtkDataSetAttributes.h>
#include <vtkTextProperty.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarRepresentation.h>

GsTL_vtkProp::GsTL_vtkProp()
  : geostat_grid_( 0 ),
    current_property_( 0 ),
    current_region_(0),
    cmap_( 0 ),
    renderer_(0),
    vtk_property_(0),
    threshold_poly_data_(0),
    section_poly_data_(0),
    is_section_active_(false),
    is_thresold_active_(false),
    is_cbar_requested_(false)
{

  cbar_widget_ = vtkScalarBarWidget::New();
  cbar_widget_->GetScalarBarActor()->GetLabelTextProperty()->BoldOff();
  cbar_widget_->GetScalarBarActor()->GetLabelTextProperty()->ShadowOff();
  cbar_widget_->GetScalarBarActor()->GetLabelTextProperty()->SetFontSize(8);
  cbar_widget_->GetScalarBarActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  cbar_widget_->EnabledOff();
    

  //background is 
  this->set_colorbars_text_color(false);

	vtk_property_ = vtkProperty::New();

	threshold_poly_data_ = vtkAppendPolyData::New();
  section_poly_data_ = vtkAppendPolyData::New();

}


GsTL_vtkProp::~GsTL_vtkProp() {


  cbar_widget_->Delete();
	vtk_property_->Delete();

  //the vtkProp reference must be deleted in the implementation class
  
  // delete the colormaps
  for( CmapMap::iterator it = cmap_map_.begin(); it != cmap_map_.end(); ++it )
    delete it->second.second;
}




bool GsTL_vtkProp::add_thresholding(int id, float min, float max, bool is_visible){


	threshold_map::iterator it = threshold_pipelines_.find(id);
	if( it !=  threshold_pipelines_.end()) {  // Already exist
		return false;
	}

	threshold_pipeline threshold;
	threshold.thresholder = vtkThreshold::New();
	threshold.thresholder->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
	threshold.thresholder->ThresholdBetween(min,max);

	threshold.surface = vtkDataSetSurfaceFilter::New();
  threshold.surface->SetInputConnection(threshold.thresholder->GetOutputPort() );
	threshold.surface->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
  //threshold.surface->SetInput((vtkDataSet*)threshold.thresholder->GetOutput() );
  
	this->connect_threshold_to_data(threshold.thresholder);
	threshold.enabled = is_visible;
	threshold_pipelines_[id] = threshold;

	if(is_visible) {
		//threshold_poly_data_->AddInputData(threshold.surface->GetOutput());
    threshold_poly_data_->AddInputConnection(0,threshold.surface->GetOutputPort());
		if(is_thresold_active_ == false) {
			this->enable_threshold_pipeline();
			is_thresold_active_ = true;
		}
	}



	return true;

}

bool GsTL_vtkProp::remove_all_thresholding(){

	threshold_map::iterator it = threshold_pipelines_.begin();

	this->disable_threshold_pipeline();

	//threshold_poly_data_->RemoveAllInputs();
  threshold_poly_data_->RemoveAllInputConnections(0);

	for( ; it != threshold_pipelines_.end(); ++it) {
		it->second.surface->Delete();
		it->second.thresholder->Delete();
	}
	threshold_pipelines_.clear();

	is_thresold_active_ == false;
	this->disable_threshold_pipeline();

	return true;

}


bool GsTL_vtkProp::remove_thresholding(int id){


	threshold_map::iterator it = threshold_pipelines_.find(id);

	if(it == threshold_pipelines_.end()) return true;

	//threshold_poly_data_->RemoveInputData(it->second.surface->GetOutput());
  threshold_poly_data_->RemoveInputConnection(0,it->second.surface->GetOutputPort());

	it->second.surface->Delete();
	it->second.thresholder->Delete();

	threshold_pipelines_.erase(it);

	if( threshold_poly_data_->GetTotalNumberOfInputConnections() == 0) {
		is_thresold_active_= false;
		this->disable_threshold_pipeline();
	}

	is_thresold_active_ = false;
	for(int i=0; i<threshold_pipelines_.size();++i) {
		if( threshold_pipelines_[i].enabled ) {
			is_thresold_active_= true;
			break;
		}
	}

	if(is_thresold_active_ == false) {
		this->disable_threshold_pipeline();
//		this->enable_full_view();
	}

	return true;
}

bool GsTL_vtkProp::enable_thresholding(){


	is_thresold_active_ = false;
	for(int i=0; i<threshold_pipelines_.size();++i) {
		if( threshold_pipelines_[i].enabled ) {
			is_thresold_active_= true;
			this->connect_threshold_to_data(threshold_pipelines_[i].thresholder);
      threshold_poly_data_->AddInputConnection(0,threshold_pipelines_[i].surface->GetOutputPort());
			//threshold_poly_data_->AddInputData(threshold_pipelines_[i].surface->GetOutput());

		}
	}
	if(is_thresold_active_ == true) {
		this->enable_threshold_pipeline();
	}
  return true;
}


bool GsTL_vtkProp::disable_thresholding(){

	is_thresold_active_ = false;
	for(int i=0; i<threshold_pipelines_.size();++i) {
		if( threshold_pipelines_[i].enabled ) {
			//threshold_poly_data_->RemoveInputData(threshold_pipelines_[i].surface->GetOutput());
      threshold_poly_data_->RemoveInputConnection(0,threshold_pipelines_[i].surface->GetOutputPort());

		}
	}
	this->disable_threshold_pipeline();
  return true;

}

bool GsTL_vtkProp::update_thresholding(int id, float min, float max, bool is_visible){

	int number_connection_entry = threshold_poly_data_->GetTotalNumberOfInputConnections();

	threshold_map::iterator it = threshold_pipelines_.find(id);
	if(it == threshold_pipelines_.end()) {
		this->add_thresholding( id, min, max, is_visible);
		it = threshold_pipelines_.find(id);
	}
	else {

		bool changed = false;
		if( it->second.thresholder->GetLowerThreshold() != min) {
			changed = true;
		}
		if( it->second.thresholder->GetUpperThreshold() != max) {
			changed = true;
		}
		if(changed) {
			it->second.thresholder->ThresholdBetween(min,max);
			it->second.thresholder->Modified();
		}

		if(it->second.enabled == true &&  is_visible == false) {
				it->second.enabled = false;
				//threshold_poly_data_->RemoveInputData(it->second.surface->GetOutput());
        threshold_poly_data_->RemoveInputConnection(0,it->second.surface->GetOutputPort());
		}
		else if(it->second.enabled == false &&  is_visible == true) {
			it->second.enabled = true;
      //threshold_poly_data_->AddInputData(it->second.surface->GetOutput());
			threshold_poly_data_->AddInputConnection(0,it->second.surface->GetOutputPort());
		}

	}

	int number_connection_exit = threshold_poly_data_->GetTotalNumberOfInputConnections();

	if(threshold_poly_data_->GetTotalNumberOfInputConnections() ==0 ){
		this->disable_threshold_pipeline();
	}
	else if(number_connection_entry == 0 && number_connection_exit > 0) {
		this->enable_threshold_pipeline();
	}
	return true;

}


bool GsTL_vtkProp::update_thresholding_colortable(){
//	threshold_map::iterator it = threshold_pipelines_.begin();

//	for( ; it != threshold_pipelines_.end(); ++it) {
//		this->set_colortable_to_mapper(it->second.mapper);
//		it->second.mapper->Modified();
//	}
  return true;
}


/*
*/
/*
int GsTL_vtkProp::add_section(int id, QString orientation, bool is_visible){

	section_map::iterator it = section_pipelines_.find(id);
	if( it !=  section_pipelines_.end()) {  // Already exist
		return -1;
	}

	section_pipeline section;
  section.id = id;
  section.enabled = is_visible;
  section.plane = vtkPlane::New();
  section.cutter = vtkCutter::New();
//  section.mapper = vtkPolyDataMapper::New();
//  section.actor = vtkActor::New();

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
  it->second.plane->SetOrigin(image_data_->GetOrigin());
  it->second.cutter->SetInputConnection(data_pass_through_filter_->GetOutputPort());
  it->second.cutter->SetCutFunction(it->second.plane);
//  it->second.mapper->SetInputConnection(it->second.cutter->GetOutputPort());
//  it->second.mapper->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
//  it->second.mapper->SetLookupTable(cmap_->color_table());
//  it->second.actor->SetMapper(it->second.mapper);
//  renderer_->AddActor(it->second.actor);

	if(is_visible) {
		section_poly_data_->AppendData(it->second.cutter->GetOutputPort());
		if(is_section_active_ == false) {
			this->enable_section_pipeline();
			is_section_active_ = true;
		}
	}

  return max_extent;

}
*/


bool GsTL_vtkProp::remove_all_sections(){

  section_poly_data_->RemoveAllInputs();
	section_map::iterator it = section_pipelines_.begin();

	for( ; it != section_pipelines_.end(); ++it) {
    renderer_->RemoveActor(it->second.actor);

    it->second.plane->Delete();
    it->second.cutter->Delete();
    it->second.mapper->Delete();
    it->second.actor->Delete();
	}
	section_pipelines_.clear();

	is_section_active_ == false;
  this->disable_section_pipeline();

	return true;

}


bool GsTL_vtkProp::remove_section(int id){


	section_map::iterator it = section_pipelines_.find(id);

	if(it == section_pipelines_.end()) return true;

  //section_poly_data_->RemoveInputData(it->second.cutter->GetOutput());
  section_poly_data_->RemoveInputConnection(0,it->second.cutter->GetOutputPort());
  //renderer_->RemoveActor(it->second.actor);

  it->second.plane->Delete();
  it->second.cutter->Delete();
//  it->second.mapper->Delete();
//  it->second.actor->Delete();

	section_pipelines_.erase(it);

	is_section_active_ = false;
	for(int i=0; i<section_pipelines_.size();++i) {
		if( section_pipelines_[i].enabled ) {
			is_thresold_active_= true;
			break;
		}
	}

	if(is_thresold_active_ == false) {
		this->disable_section_pipeline();
	}

	return true;
}

bool GsTL_vtkProp::enable_section(int id){

  section_map::iterator it = section_pipelines_.find(id);
  if(it == section_pipelines_.end()) return false;

  it->second.enabled = true;

  section_poly_data_->AddInputData(it->second.cutter->GetOutput());
//  renderer_->AddActor(it->second.actor);
  if(is_section_active_ == false) {
    is_section_active_ = true;
    this->enable_section_pipeline();
  }

  return true;
}


bool GsTL_vtkProp::disable_section(int id){

  section_map::iterator it = section_pipelines_.find(id);
  if(it == section_pipelines_.end()) return false;

  section_poly_data_->RemoveInputData(it->second.cutter->GetOutput());
  it->second.enabled = false;


	is_section_active_ = false;
	for(int i=0; i<section_pipelines_.size();++i) {
		if( section_pipelines_[i].enabled ) {
			is_section_active_  = true;
		}
	}
  if( !is_section_active_  )
	  this->disable_section_pipeline();
  
  return true;

}

bool GsTL_vtkProp::update_section(int id, int steps, bool is_visible){

	return true;

}


void GsTL_vtkProp::set_property( const std::string& property_name, Colormap* cmap )  {
}

bool GsTL_vtkProp::compute_min_max( std::pair<float, float>& min_max,
			const GsTLGridProperty* property ){
	float min = 9e99;
	float max = -9e99;

	GsTLGridProperty::const_iterator it = property->begin(true);
	for( ;it != property->end(); ++it) {
		float val = *it;
		if(val < min ) min =val;
		if(val > max) max = val;
	}
	min_max.first = min;
	min_max.second = max;
	return true;

}
//Functions for cell representation
void GsTL_vtkProp::set_cell_representation(const QString& type ){
	if(type == "Surface") {
		vtk_property_->SetRepresentationToSurface();
	}
	else if(type == "Wireframe") {
		vtk_property_->SetRepresentationToWireframe();
	}
	else if(type == "Points") {
			vtk_property_->SetRepresentationToPoints();
		}
}


void GsTL_vtkProp::show_edges(bool on){
	if(on )  vtk_property_->EdgeVisibilityOn();
	else vtk_property_->EdgeVisibilityOff();
}

//Functions for the color bar

void GsTL_vtkProp::update_colorbar(){

  cbar_widget_->GetScalarBarActor()->SetLookupTable(cmap_->color_table());
  cbar_widget_->GetScalarBarActor()->Modified();
}

void GsTL_vtkProp::remove_colorbar(){
  cbar_widget_->EnabledOff();
}

void GsTL_vtkProp::set_colorbar_visible(bool visibility){
	if( current_property_ == 0 ||  actor_->GetVisibility() == 0) {
    cbar_widget_->EnabledOff();
		return;
	}
  cbar_widget_->SetInteractor( renderer_->GetRenderWindow()->GetInteractor() );
  if(visibility) cbar_widget_->EnabledOn();
  else cbar_widget_->EnabledOff();
}

bool GsTL_vtkProp::is_colorbar_visible(){
  return cbar_widget_->GetEnabled() == 1;

}


void GsTL_vtkProp::is_cbar_requested(bool on){
  is_cbar_requested_ = on;
  if(is_cbar_requested_) {
    cbar_widget_->SetInteractor( renderer_->GetRenderWindow()->GetInteractor() );
  }
  this->update_colorbar_visibility();
  /*
  if( is_cbar_requested_  && ( current_property_ == 0 || actor_->GetVisibility() == 0) ) {
    cbar_widget_->EnabledOn();
  }
  else {
    cbar_widget_->EnabledOff();
  }
  */
}

void GsTL_vtkProp::update_colorbar_visibility( ){
	if( !is_cbar_requested_ ) {
    cbar_widget_->EnabledOff();
		return;
	}
	else if( is_cbar_requested_ && (current_property_ == 0 ||  actor_->GetVisibility() == 0)) {
    cbar_widget_->EnabledOff();
		return;
	}
  else {
    cbar_widget_->EnabledOn();
  }
}

void GsTL_vtkProp::set_colorbar_font_size(int size){
  cbar_widget_->GetScalarBarActor()->GetLabelTextProperty()->SetFontSize(size);
  cbar_widget_->GetScalarBarActor()->Modified();
  cbar_widget_->Modified();
}
void GsTL_vtkProp::set_colorbar_title(const QString& title ){
  cbar_widget_->GetScalarBarActor()->SetTitle( title.toStdString().c_str() );
  cbar_widget_->Modified();
}
void GsTL_vtkProp::set_colorbar_height(double height){
  cbar_widget_->GetScalarBarActor()->SetHeight( height );
  cbar_widget_->Modified();
}
void GsTL_vtkProp::set_colorbar_width( double width ){
  cbar_widget_->GetScalarBarActor()->SetWidth( width );
  cbar_widget_->Modified();
}

void GsTL_vtkProp::set_colorbar_position(double x, double y){
  cbar_widget_->GetScalarBarActor()->GetPositionCoordinate()->SetValue(x,y);
  cbar_widget_->Modified();
}

void GsTL_vtkProp::set_colorbar_x_position(double x){
  double xold,y,z;
  double* pos;
  double* pos2;

  vtkScalarBarRepresentation* srep = vtkScalarBarRepresentation::SafeDownCast( cbar_widget_->GetRepresentation() );
  pos = srep->GetPosition();
  pos2 = srep->GetPosition2();

  double dx = pos2[0] - pos[0]; 
  srep->SetPosition( x, pos[1] );
  srep->SetPosition2( x + dx, pos2[1] );

/*
  cbar_widget_->GetScalarBarActor()->GetPositionCoordinate()->GetValue(xold,y,z);
  cbar_widget_->GetScalarBarActor()->GetPositionCoordinate()->SetValue(x,y,z);
  */
  cbar_widget_->Modified();
}
void GsTL_vtkProp::set_colorbar_y_position(double y){
  double x,yold,z;
  cbar_widget_->GetScalarBarActor()->GetPositionCoordinate()->GetValue(x,yold,z);
  cbar_widget_->GetScalarBarActor()->GetPositionCoordinate()->SetValue(x,y,z);
  cbar_widget_->Modified();
}

void GsTL_vtkProp::get_colorbar_position(double& x, double& y){
  double z;
  cbar_widget_->GetScalarBarActor()->GetPositionCoordinate()->GetValue(x,y,z);
  cbar_widget_->Modified();
}



void GsTL_vtkProp::set_colorbar_width(float width){
	cbar_widget_->GetScalarBarActor()->SetWidth(width);
}

void GsTL_vtkProp::set_colorbar_height(float height){
	cbar_widget_->GetScalarBarActor()->SetHeight(height);
}

void GsTL_vtkProp::set_colorbars_text_color(bool is_background_white){
  double r,g,b;

  if(renderer_) {
    renderer_->GetBackground(r,g,b);
    cbar_widget_->GetScalarBarActor()->GetLabelTextProperty()->SetColor(1-r,1-g,1-b);
  }

}

void GsTL_vtkProp::set_colorbars_text_color(){
  double r,g,b;

  if(renderer_) {
    renderer_->GetBackground(r,g,b);
    cbar_widget_->GetScalarBarActor()->GetLabelTextProperty()->SetColor(1-r,1-g,1-b);
  }

}


void GsTL_vtkProp::set_number_of_ticks(int n_ticks){
	cbar_widget_->GetScalarBarActor()->GetNumberOfLabels() != n_ticks;
	cbar_widget_->GetScalarBarActor()->SetNumberOfLabels(n_ticks);
}
