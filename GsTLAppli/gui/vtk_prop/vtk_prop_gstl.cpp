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

//TL modified
bool GsTL_vtkProp::update_desc(QString & olds, QString & news)
{
  
  CmapMap::iterator found = cmap_map_.find( String_Op::qstring2string(olds) );
	if (found == cmap_map_.end())
		return false;

	std::pair<Bounds_pair, Colormap*> p = found->second;
	cmap_map_.erase(found);
	cmap_map_[String_Op::qstring2string(news)] = p;
  return true;
}

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
    is_thresold_active_(false)
{
	cbar_actor_ = vtkScalarBarActor::New();
	cbar_actor_->GetLabelTextProperty()->BoldOff();
	cbar_actor_->GetLabelTextProperty()->ShadowOff();
	cbar_actor_->GetLabelTextProperty()->SetFontSize(8);
	cbar_actor_->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();

	legend_actor_ = vtkLegendBoxActor::New();
	legend_actor_->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	legend_actor_->SetHeight(0.5);
	legend_actor_->SetWidth(0.5);
	legend_actor_->GetEntryTextProperty()->SetFontSize(8);

  //Hack need to be done propoerly
  this->set_colorbars_text_color(true);

	vtk_property_ = vtkProperty::New();

	threshold_poly_data_ = vtkAppendPolyData::New();
  section_poly_data_ = vtkAppendPolyData::New();

//	legend_actor_->UseBackgroundOff();
//	legend_actor_->BorderOff();

  property_display_mode_ = VTK::NOT_PAINTED;
}


GsTL_vtkProp::~GsTL_vtkProp() {

	cbar_actor_->Delete();
	legend_actor_->Delete();
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




/*
*/

void GsTL_vtkProp::set_property( const std::string& property_name, Colormap* cmap )  {
}


void GsTL_vtkProp::update( VTK::Update_reason reason ,
                               const std::vector<std::string>* properties ) {

  switch( reason ) { 

  case VTK::PROPERTY_DELETED:
    if( !properties ) {
      // We don't know which property was removed, so we have to check them all
      appli_assert( geostat_grid_ );
      std::list<std::string> property_list = geostat_grid_->property_list();
      property_list.sort();

      for( CmapMap::iterator it = cmap_map_.begin(); it != cmap_map_.end(); ++it ) {
        bool there = std::binary_search( property_list.begin(), property_list.end(),
				                                 it->first );
      	if( !there ) {
	        property_deleted( it->first );
	      }	
      }
    }      
    else {
      // we know which properties were removed
      std::vector<std::string>::const_iterator it;
      for( it = properties->begin() ; it != properties->end() ; ++it )
        property_deleted( *it );
    }

//    refresh();
    break;
    

  case VTK::PROPERTY_CHANGED:
    if( !properties ) {
      // we don't know which property was changed, so we update all properties 
      for( CmapMap::iterator it = cmap_map_.begin(); it != cmap_map_.end(); ++it ) {
      	compute_min_max( it->second.first, geostat_grid_->property( it->first ) );
      }
    }
    else {
      std::vector<std::string>::const_iterator prop_it = properties->begin();
      for( ; prop_it != properties->end() ; ++prop_it ) {
        CmapMap::iterator it = cmap_map_.find( *prop_it );
      
        if( it != cmap_map_.end() ) {
        	compute_min_max( it->second.first, geostat_grid_->property( it->first ) );
	        Bounds_pair min_max = it->second.first;
	        it->second.second->set_bounds( min_max.first, min_max.second );
	
//      	  refresh();
        }
      }
    }
    break;
    
  }

  refresh();
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

//	if(current_property_->classname() ==  "GsTLGridCategoricalProperty") {  // for some reason generates seg-fault
	if(false) {
		GsTLGridCategoricalProperty* cprop = dynamic_cast<GsTLGridCategoricalProperty*>(current_property_);
		int ncat = cprop->get_number_of_category();
		legend_actor_->SetNumberOfEntries(ncat);
		for(int i=0; i<ncat; ++i) {
			double color[3];
			double r,g,b;
			std::string cat_name = cprop->get_category_definition()->get_category_name(i);
			cmap_->color((float)i,r,g,b);
			color[0] = r;
			color[1] = g;
			color[2] = b;
			legend_actor_->SetEntry(i,static_cast<vtkPolyData*>(0), cat_name.c_str(),color);
//			cmap_->color((float)i,color[0],color[1],color[2]);
//			legend_actor_->SetEntry(i,static_cast<vtkPolyData*>(0), cprop->get_category_definition()->get_category_name(i).c_str(),color);
		}

	}
	else {
		cbar_actor_->SetLookupTable(cmap_->color_table());

	}
}

void GsTL_vtkProp::remove_colorbar(){
	if(renderer_->HasViewProp(cbar_actor_)) {
		renderer_->RemoveActor2D(cbar_actor_);
	}
	if(renderer_->HasViewProp(legend_actor_)) {
		renderer_->RemoveActor2D(legend_actor_);
	}
}

void GsTL_vtkProp::set_colorbar_visible(bool visibility){
	if( current_property_ == 0 ) {
		legend_actor_->SetVisibility(false);
		cbar_actor_->SetVisibility(false);
		return;
	}
	if(current_property_->classname() == "GsTLGridCategoricalProperty") {
		legend_actor_->SetVisibility(visibility);
	} else {
		cbar_actor_->SetVisibility(visibility);
	}

}

bool GsTL_vtkProp::is_colorbar_visible(){
	return cbar_actor_->GetVisibility() || legend_actor_->GetVisibility();

}




/*
 *
 *
void GsTL_vtkProp::set_colorbar_orientation(bool is_vertical){
	if(is_vertical)
		this->set_colorbar_to_left();
	else
		this->set_colorbar_to_bottom();
}
*/

void GsTL_vtkProp::set_colorbar_or_legend(){
	if( current_property_ == 0) return;
	if(current_property_->classname() == "GsTLGridCategoricalProperty") {
		if(renderer_->HasViewProp(cbar_actor_)) {
			renderer_->RemoveActor2D(cbar_actor_);
		}
		if(!renderer_->HasViewProp(legend_actor_)) {
			renderer_->AddActor2D(legend_actor_);
		}

	}else {
		if(renderer_->HasViewProp(legend_actor_)) {
			renderer_->RemoveActor2D(legend_actor_);
		}
		if(!renderer_->HasViewProp(cbar_actor_)) {
			renderer_->AddActor2D(cbar_actor_);
		}
	}
}

void GsTL_vtkProp::set_colorbar_to_left(){
	if( current_property_ == 0) return;
	this->set_colorbar_or_legend();
	legend_actor_->GetPositionCoordinate()->SetValue(0.1,0.25);
	cbar_actor_->GetPositionCoordinate()->SetValue(0.1,0.25);
	cbar_actor_->SetOrientationToVertical();
	cbar_actor_->SetWidth(0.08);
	cbar_actor_->SetHeight(0.5);
	cbar_actor_->SetTextPositionToPrecedeScalarBar();
  if(current_property_ == 0) return;
	if(current_property_->classname() == "GsTLGridCategoricalProperty") {
		legend_actor_->VisibilityOn();

	}else {

		cbar_actor_->VisibilityOn();
	}
	this->set_colorbar_visible(true);
}

void GsTL_vtkProp::set_colorbar_to_right(){
	if( current_property_ == 0) return;
	this->set_colorbar_or_legend();
	legend_actor_->GetPositionCoordinate()->SetValue(0.9,0.25);
	cbar_actor_->GetPositionCoordinate()->SetValue(0.9,0.25);
	cbar_actor_->SetOrientationToVertical();
	cbar_actor_->SetWidth(0.08);
	cbar_actor_->SetHeight(0.5);
	cbar_actor_->SetTextPositionToSucceedScalarBar();
  if(current_property_ == 0) return;
	if(current_property_->classname() == "GsTLGridCategoricalProperty") {
		legend_actor_->VisibilityOn();

	}else {

		cbar_actor_->VisibilityOn();
	}
	this->set_colorbar_visible(true);
}

void GsTL_vtkProp::set_colorbar_to_bottom(){

	this->set_colorbar_or_legend();
	cbar_actor_->SetOrientationToHorizontal();
	cbar_actor_->GetPositionCoordinate()->SetValue(0.25,0.04);
	cbar_actor_->SetWidth(0.5);
	cbar_actor_->SetHeight(0.08);
	cbar_actor_->SetTextPositionToPrecedeScalarBar();
	legend_actor_->GetPositionCoordinate()->SetValue(0.25,0.04);
  if(current_property_ == 0) return;
	if(current_property_->classname() == "GsTLGridCategoricalProperty") {
		legend_actor_->VisibilityOn();

	}else {

		cbar_actor_->VisibilityOn();
	}
}

void GsTL_vtkProp::set_colorbar_to_top(){

	this->set_colorbar_or_legend();
	cbar_actor_->SetOrientationToHorizontal();
	cbar_actor_->GetPositionCoordinate()->SetValue(0.25,0.9);
	cbar_actor_->SetWidth(0.5);
	cbar_actor_->SetHeight(0.08);
	cbar_actor_->SetTextPositionToPrecedeScalarBar();
	legend_actor_->GetPositionCoordinate()->SetValue(0.25,0.96);
  if(current_property_ == 0) return;
	if(current_property_->classname() == "GsTLGridCategoricalProperty") {
		legend_actor_->VisibilityOn();

	}else {

		cbar_actor_->VisibilityOn();
	}
}


void GsTL_vtkProp::set_colorbar_width(float width){
	cbar_actor_->SetWidth(width);
}

void GsTL_vtkProp::set_colorbar_height(float height){
	cbar_actor_->SetHeight(height);
}

void GsTL_vtkProp::set_colorbars_text_color(bool is_background_white){

	if(is_background_white) {
		cbar_actor_->GetLabelTextProperty()->SetColor(0,0,0);
		legend_actor_->GetEntryTextProperty()->SetColor(0,0,0);
	}

	else {
		cbar_actor_->GetLabelTextProperty()->SetColor(1,1,1);
		legend_actor_->GetEntryTextProperty()->SetColor(1,1,1);
	}

}


void GsTL_vtkProp::set_number_of_ticks(int n_ticks){
	cbar_actor_->GetNumberOfLabels() != n_ticks;
	cbar_actor_->SetNumberOfLabels(n_ticks);
}
