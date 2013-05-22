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
** This file is part of the "grid" module of the Geostatistical Earth
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

#include <grid/library_grid_init.h>
#include <grid/grid_model/cartesian_grid.h>
#include <grid/grid_model/rgrid.h>
#include <grid/grid_model/point_set.h>
#include <grid/grid_model/property_copier.h>
#include <utils/gstl_messages.h>
#include <grid/grid_model/reduced_grid.h>
#include <grid/grid_model/log_data_grid.h>
#include <grid/grid_model/structured_grid.h>
#include <grid/grid_model/grid_downscaler.h>
#include <grid/property_transformer.h>
#include <grid/grid_filter.h>

int library_grid_init::references_ = 0;

int library_grid_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "grid library already registered" << "\n";
    return 2;
  }
      
  GsTLlog << "registering grid library" << "\n"; 
    
  /* Set-up directory structure: /GridObject, /GridObject/Model and
   * /GridObject/OinvDescription
   */
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory://grids",
				    gridObject_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << gridObject_manager << "\n";
    return 1;
  }
    
  dir->factory("directory", create_directory);

  SmartPtr<Named_interface> ni_models = 
    Root::instance()->new_interface("directory",
				    gridModels_manager);
    

  /* Bind the factory methods for grid models
   */
  dir = dynamic_cast<Manager*>( ni_models.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory" 
	      << gridModels_manager << "\n";
    return 1;
  }
      
  bind_models_factories( dir );


  /* Bind the factory methods for grid descriptions
   */
  /*
  dir = dynamic_cast<Manager*>( ni_desc.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory" 
	      << oinvDescription_manager << "\n";
    return 1;
  }     
 
  bind_desc_factories( dir );
  */

  init_property_copier_factory();
  init_downscaler_factory();
  init_categorical_definition_factory();
  init_property_transformer_factory();
  init_grid_filter_factory();



  GsTLlog << "Registration done" << "\n";
  Root::instance()->list_all( GsTLlog );

  return 0;
}

  
//------------------------


int library_grid_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( gridObject_manager );
    Root::instance()->delete_interface( gridModels_manager );
    Root::instance()->delete_interface( vtkProps_manager );
    Root::instance()->delete_interface( categoricalDefinition_manager );
  }
  return 0;
}


//------------------------

bool library_grid_init::bind_models_factories(Manager* dir) {
  dir->factory( "cgrid", create_Cgrid );
  dir->factory( "rgrid", create_Rgrid );
  dir->factory( "point_set", Point_set::create_new_interface );
  dir->factory( "log_grid", Log_data_grid::create_new_interface );
  dir->factory( "Structured_grid", Structured_grid::create_new_interface );

  //TL modified
  dir->factory("reduced_grid", create_reduced_grid);
  return true;
}



//------------------------
bool library_grid_init::init_property_copier_factory() {
  Property_copier_factory::add_method( Point_set().classname(), 
                                       Cartesian_grid().classname(), 
                                       Pset_to_cgrid_copier::create_new_interface );
  Property_copier_factory::add_method( Log_data_grid().classname(), 
                                       Cartesian_grid().classname(), 
                                       Pset_to_cgrid_copier::create_new_interface );
  Property_copier_factory::add_method( Cartesian_grid().classname(), 
                                       Cartesian_grid().classname(), 
                                       Cgrid_to_cgrid_copier::create_new_interface );
  Property_copier_factory::add_method( Point_set().classname(), 
                                       Point_set().classname(), 
                                       Pset_to_pset_copier::create_new_interface );
  Property_copier_factory::add_method( Log_data_grid().classname(), 
                                       Log_data_grid().classname(), 
                                       Pset_to_pset_copier::create_new_interface );
  //TL modified
  Property_copier_factory::add_method( Reduced_grid().classname(), 
                                       Reduced_grid().classname(), 
                                       Mask_to_mask_copier::create_new_interface );
  Property_copier_factory::add_method( Point_set().classname(), 
                                       Reduced_grid().classname(), 
                                       Pset_to_mask_copier::create_new_interface );
  Property_copier_factory::add_method( Log_data_grid().classname(), 
                                       Reduced_grid().classname(), 
                                       Pset_to_mask_copier::create_new_interface );

  Property_copier_factory::add_method( Cartesian_grid().classname(), 
                                       Point_set().classname(), 
                                       Rgrid_to_pset_copier::create_new_interface );
  Property_copier_factory::add_method( Reduced_grid().classname(), 
                                       Point_set().classname(), 
                                       Rgrid_to_pset_copier::create_new_interface );

  Property_copier_factory::add_method( Point_set().classname(), 
                                       Structured_grid().classname(), 
                                       Pset_to_structured_grid_copier::create_new_interface );

  Property_copier_factory::add_method( Log_data_grid().classname(), 
                                       Structured_grid().classname(), 
                                       Pset_to_structured_grid_copier::create_new_interface );
  return true;
}


bool library_grid_init::init_downscaler_factory(){
  GsTLlog << "Creating Downscaler manager" << "\n";
  SmartPtr<Named_interface> ni_dscaler = 
    Root::instance()->new_interface("directory://downscaler",
				    downscaler_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni_dscaler.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << downscaler_manager << "\n";
    return false;
  }
  dir->factory( "Cgrid", CGrid_downscaler::new_interface );
  dir->factory( "Masked_grid", MGrid_downscaler::new_interface );
}


bool library_grid_init::init_property_transformer_factory(){
  GsTLlog << "Creating transformer manager" << "\n";
  SmartPtr<Named_interface> ni_transformer = 
    Root::instance()->new_interface("directory://transformer",
      transformer_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni_transformer.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << transformer_manager << "\n";
    return false;
  }
  dir->factory( "PCA_transformer", PCA_transformer::create_new_interface );


}

bool library_grid_init::init_categorical_definition_factory() {
  //----------------------
  // Create the manager for the categorical definition
  GsTLlog << "Creating Categorical definition manager" << "\n";
  SmartPtr<Named_interface> ni_cdefs = 
    Root::instance()->new_interface("directory://categoricaldefinitions",
				    categoricalDefinition_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni_cdefs.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << categoricalDefinition_manager << "\n";
    return false;
  }
  dir->factory( "categoricaldefinition", create_new_categorical_definition );

// Add the Default definition
  SmartPtr<Named_interface>  ni =
  		Root::instance()->new_interface( "categoricaldefinition://Default",
																				categoricalDefinition_manager +"/Default");

  if(ni.raw_ptr() == 0) {
    GsTLlog << "could not the default categorical definition \n";
    return false;
  }
  return true;
}

bool library_grid_init::init_grid_filter_factory(){

  //----------------------
  // Create the manager for the categorical definition
  GsTLlog << "Creating Grid_filter manager" << "\n";
  SmartPtr<Named_interface> ni_cdefs = 
    Root::instance()->new_interface("directory://gridfilter",
				    gridFilter_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni_cdefs.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << gridFilter_manager << "\n";
    return false;
  }


  dir->factory( Grid_filter_union().item_type().toStdString(), Grid_filter_union::create_new_interface );
  dir->factory( Grid_filter_intersection().item_type().toStdString(), Grid_filter_intersection::create_new_interface );
  dir->factory( Grid_filter_region().item_type().toStdString(), Grid_filter_region::create_new_interface );
  dir->factory( Grid_filter_category().item_type().toStdString(), Grid_filter_category::create_new_interface );
  dir->factory( Grid_filter_less_than().item_type().toStdString(), Grid_filter_less_than::create_new_interface );
  dir->factory( Grid_filter_lessor_or_equal_than().item_type().toStdString(), Grid_filter_lessor_or_equal_than::create_new_interface );
  dir->factory( Grid_filter_greater_than().item_type().toStdString(), Grid_filter_greater_than::create_new_interface );
  dir->factory( Grid_filter_greater_or_equal_than().item_type().toStdString(), Grid_filter_greater_or_equal_than::create_new_interface );
  dir->factory( Grid_filter_bounded().item_type().toStdString(), Grid_filter_bounded::create_new_interface );
  dir->factory( Grid_filter_x_coord_bounded().item_type().toStdString(), Grid_filter_x_coord_bounded::create_new_interface );
  dir->factory( Grid_filter_y_coord_bounded().item_type().toStdString(), Grid_filter_y_coord_bounded::create_new_interface );
  dir->factory( Grid_filter_z_coord_bounded().item_type().toStdString(), Grid_filter_z_coord_bounded::create_new_interface );
  dir->factory( Grid_filter_log_names().item_type().toStdString(), Grid_filter_log_names::create_new_interface );

  return true;

}

extern "C" {
  int libGsTLAppli_grid_init() {
    return library_grid_init::init_lib();
  }
  int libGsTLAppli_grid_release() {
    return library_grid_init::release_lib();
  }
}

