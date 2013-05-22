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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#include <gui/vtk_prop/vtk_prop_mgrid.h>

#include <gui/utils/colorscale.h>
#include <gui/utils/colormap.h>
#include <appli/manager_repository.h>

#include <GsTL/math/math_functions.h>

#include <qstring.h>
#include <qstringlist.h>

#include <iterator>
#include <algorithm>
#include <sstream> 

#include <vtkBitArray.h>
#include <vtkFloatArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataSetAttributes.h>
#include <vtkOutlineFilter.h>
//#include <vtkDataSetSurfaceFilter.h>

#include <fstream>



Named_interface* vtkProp_mgrid::create_new_interface( std::string& ) {
  return new vtkProp_mgrid;
}

vtkProp_mgrid::vtkProp_mgrid()
  : GsTL_vtkProp(),
    grid_( 0 ),
    initialized_(false),
    mapper_(0),image_data_(0)
 {

}


vtkProp* vtkProp_mgrid::prop(){
  return actor_;
}

void vtkProp_mgrid::set_visibility(bool on) {
	actor_->SetVisibility(on);
  this->update_colorbar_visibility();
}

bool vtkProp_mgrid::is_visibile(){
	return actor_->GetVisibility() == 1;
}

void vtkProp_mgrid::init( Geostat_grid* grid, vtkRenderer* renderer ) {
	renderer_ = renderer;
    geostat_grid_ = grid;
    grid_ = dynamic_cast< Reduced_grid*>( grid );
  unsigned int full_grid_size = grid_->rgrid_size();
  GsTLCoordVector cell_dims = grid_->cell_dimensions();
  GsTLPoint origin = grid_->origin();

  image_data_ = vtkUniformGrid::New();
  image_data_->SetExtent(0,grid_->nx(), 0,grid_->ny(), 0,grid_->nz());
  image_data_->SetOrigin(origin.x()-cell_dims.x()/2, origin.y()-cell_dims.y()/2,origin.z()-cell_dims.z()/2);
  image_data_->SetSpacing(cell_dims.x(),cell_dims.y(),cell_dims.z());
  int n_cells = image_data_->GetNumberOfCells();

  vtkBitArray *mask = vtkBitArray::New();
  mask->SetName("mask");
  mask->SetNumberOfValues(n_cells);
  std::vector<bool> b = grid_->mask();
  for(int i=0; i<n_cells;++i) {
	  mask->SetValue(i,b[i]);
  }
  image_data_->GetCellData()->AddArray(mask);


  vtk_mask_threshold_ = vtkThreshold::New();
  vtk_mask_threshold_->SetInputData(image_data_);
  vtk_mask_threshold_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,"mask");
  vtk_mask_threshold_->ThresholdBetween(1,1);
  vtk_mask_threshold_->Update();

  ugrid_ = vtk_mask_threshold_->GetOutput();

  data_pass_through_ = vtkPassThrough::New();
  data_pass_through_->SetInputData(ugrid_);

  surface_extractor_ = vtkDataSetSurfaceFilter::New();
  surface_extractor_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,vtkDataSetAttributes::SCALARS);
  surface_extractor_->SetInputConnection(data_pass_through_->GetOutputPort());


	vtk_property_->SetRepresentationToSurface();
	vtk_property_->EdgeVisibilityOff();

  mapper_ = vtkDataSetMapper::New();
  mapper_->SetInputConnection(surface_extractor_->GetOutputPort());

  actor_ = vtkActor::New();
  actor_->SetProperty(vtk_property_);
  actor_->RotateZ(-1.0*grid_->geometry()->rotation_z());

  this->set_visibility(false);
  actor_->SetMapper(mapper_);


  // Set the thresholder for the region; perform the thresholding based on the visibility array

  vtkBitArray* maskArray = vtkBitArray::New();
  maskArray->SetNumberOfValues(grid_->size());
  maskArray->SetName("RegionArray");
  ugrid_->GetCellData()->AddArray(maskArray);

    region_threshold_ = vtkThreshold::New();
    region_threshold_->SetInputData(ugrid_);
    region_threshold_->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_CELLS,"RegionArray");
    region_threshold_->ThresholdBetween(1,1);

    renderer_->AddActor(actor_);

//  vtkOutlineFilter* outline = vtkOutlineFilter::New();
//  outline->SetInput(ugrid_);
//  vtkDataSetMapper* mapperOutline = vtkDataSetMapper::New();
//  mapperOutline->SetInput(outline->GetOutput());
//
//  actor_outline_ = vtkActor::New();
//  actor_outline_->SetMapper(mapperOutline);
//
//
//  renderer_->AddActor(actor_outline_);
//  renderer_->AddActor(region_actor_);


}


vtkProp_mgrid::~vtkProp_mgrid() {
  renderer_->RemoveActor(actor_);
  vtk_property_->Delete();
  image_data_->Delete();
  mapper_->Delete();
  vtk_mask_threshold_->Delete();
  surface_extractor_->Delete();
}



void vtkProp_mgrid::enable_full_view(){
	if(!renderer_->HasViewProp(actor_) ) {
		renderer_->AddActor(actor_);
	}
	actor_->VisibilityOn();

}



void vtkProp_mgrid::disable_full_view(){
	actor_->VisibilityOff();
	renderer_->RemoveActor(actor_);

  }


void vtkProp_mgrid::property_deleted( const std::string& prop_name ) {
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


void vtkProp_mgrid::show_bounding_box( bool on ) {
  //bbox_switch_->visible = on;
}


void vtkProp_mgrid::hide_volume(bool on) {
  if( on )
    actor_->VisibilityOff() ;
  else
    actor_->VisibilityOn();
}


void vtkProp_mgrid::refresh() {

	  refresh_colormap();
	  if(current_property_ == 0 ) {
	  	if( actor_->GetProperty()->GetRepresentation() !=  VTK_WIREFRAME)
	  		actor_->GetProperty()->SetRepresentationToWireframe();
	  	return;
	  }

	 vtkFloatArray* vtk_array = vtkFloatArray::New();
	 vtk_array->SetArray(current_property_->data(),current_property_->size(),1);


	  ugrid_->GetCellData()->SetScalars(vtk_array);
	  ugrid_->Modified();
	  vtk_array->Delete();

	  cmap_->refresh();
	  mapper_->Update();


  
}

void vtkProp_mgrid::refresh_colormap() {

  if(cmap_ == 0) return;
	cmap_->refresh();

	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());
	this->update_colorbar();

}



void vtkProp_mgrid::set_property( const std::string& property_name, Colormap* cmap ){


	cmap_ = cmap;
//	this->remove_all_thresholding();

	if(property_name.empty()) {

		ugrid_->GetCellData()->SetScalars(0);

		actor_->GetProperty()->SetRepresentationToWireframe();
		surface_extractor_->Modified();
		mapper_->Modified();

		current_property_name_.clear();
		current_property_ = 0;
		return;
	}

	if( !geostat_grid_->property( property_name ) ) {
	appli_warning( "grid has no property called " << property_name );
	return;
	}
	actor_->GetProperty()->SetRepresentationToSurface();

	current_property_ = geostat_grid_->property( property_name );
	current_property_->swap_to_memory();
	current_property_name_ = property_name;

	vtkFloatArray* vtk_array = vtkFloatArray::New();
	vtk_array->SetArray(current_property_->data(),current_property_->size(),1);

	ugrid_->GetCellData()->SetScalars(vtk_array);

	cmap_->refresh();
	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());

	ugrid_->Modified();
	if(current_region_ != 0) {
		region_threshold_->Modified();
	}
	data_pass_through_->Modified();
//	surface_extractor_->Modified();
	mapper_->Modified();
	actor_->Modified();


	this->update_colorbar();

}


void vtkProp_mgrid::set_colormap( Colormap* cmap ){
	cmap_ = cmap;

	mapper_->SetScalarRange(cmap_->lower_bound(), cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());
	mapper_->Modified();

	this->update_colorbar();



}


void vtkProp_mgrid::set_region(const std::string& region_name ){

	if(region_name == "" ) {
		current_region_ = 0;
		region_threshold_->SetInputData(0);
		data_pass_through_->SetInputData(ugrid_);

	}
	else {
		Grid_region* region = geostat_grid_->region( region_name );
		if( region == 0 ) {
			appli_warning( "grid has no region called " << region_name );
			return;
		}
		current_region_ = region;

//		if( current_region_->name() != cached_region_name_ ) {
		if( true ) {
			Grid_region::const_iterator it = current_region_->begin();
			vtkBitArray* region_mask = vtkBitArray::SafeDownCast(ugrid_->GetCellData()->GetArray("RegionArray"));
			for(int i=0; it != current_region_->end();++i, ++it) {
				region_mask->SetValue(i,*it);
			}
			cached_region_name_ = current_region_->name();

		}
		region_threshold_->SetInputData(ugrid_);
		data_pass_through_->SetInputConnection(region_threshold_->GetOutputPort());
    region_threshold_->Modified();

	}



}

bool vtkProp_mgrid::connect_threshold_to_data(vtkThreshold* thresholder){
	thresholder->SetInputConnection(data_pass_through_->GetOutputPort());
  return true;
}

bool vtkProp_mgrid::enable_threshold_pipeline(){
  surface_extractor_->RemoveAllInputConnections(0);
  mapper_->SetInputConnection(threshold_poly_data_->GetOutputPort());
	mapper_->Modified();
  return true;
}

bool vtkProp_mgrid::disable_threshold_pipeline(){

  if(is_section_active_) {
    this->enable_section_pipeline();
  }
  else {

	  //surface_extractor_->SetInput(vtkDataSet::SafeDownCast(data_pass_through_->GetOutput()));
    surface_extractor_->SetInputConnection(data_pass_through_->GetOutputPort());
	  mapper_->SetInputConnection(surface_extractor_->GetOutputPort());
  }

	mapper_->Modified();
  return true;
}




bool vtkProp_mgrid::enable_section_pipeline(){

	mapper_->SetInputConnection(section_poly_data_->GetOutputPort());
	mapper_->Modified();
  return true;
}

bool vtkProp_mgrid::disable_section_pipeline(){

  if(is_thresold_active_) {
    this->enable_threshold_pipeline();
  }
  else {

	  //surface_extractor_->SetInput(vtkDataSet::SafeDownCast(data_pass_through_->GetOutput()));
    surface_extractor_->SetInputConnection(0,data_pass_through_->GetOutputPort());
	  mapper_->SetInputConnection(surface_extractor_->GetOutputPort());

	  mapper_->Modified();
  }
  return true;

}



void vtkProp_mgrid::set_colortable_to_mapper(vtkMapper* mapper){
	double *scalar_range = mapper_->GetScalarRange();
	mapper->SetScalarRange(scalar_range[0],scalar_range[1]);
	mapper->SetLookupTable(mapper_->GetLookupTable());
}


//======================================================



int vtkProp_mgrid::add_section(int id, QString orientation, bool is_visible){

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
  it->second.plane->SetOrigin(image_data_->GetOrigin());
  it->second.cutter->SetInputConnection(data_pass_through_->GetOutputPort());
  it->second.cutter->SetCutFunction(it->second.plane);
//  it->second.mapper->SetInputConnection(it->second.cutter->GetOutputPort());
//  it->second.mapper->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
//  it->second.mapper->SetLookupTable(cmap_->color_table());
//  it->second.actor->SetMapper(it->second.mapper);
//  renderer_->AddActor(it->second.actor);

	if(is_visible) {
    section_poly_data_->AddInputConnection(it->second.cutter->GetOutputPort());
		if(is_section_active_ == false) {
			this->enable_section_pipeline();
			is_section_active_ = true;
		}
	}
  return max_extent;

}


bool vtkProp_mgrid::update_section(int id, int step, bool is_visible){

	section_map::iterator it = section_pipelines_.find(id);
	if( it ==  section_pipelines_.end()) {  // does not exist
		return false;
	}

  //Get the displacement from the step number
  int min_extent_x,min_extent_y,min_extent_z;
  int max_extent_x,max_extent_y,max_extent_z;
  image_data_->GetExtent(min_extent_x,max_extent_x,min_extent_y,
                         max_extent_y,min_extent_z,max_extent_z);

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
