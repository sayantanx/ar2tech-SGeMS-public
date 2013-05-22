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


#include <qtplugins/library_qtplugins_init.h>
#include <qtplugins/grid_filter_named_interface.h>
#include <grid/grid_filter.h>

int library_qtplugins_init::references_ = 0;

int library_qtplugins_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "qtplugins library already registered" << "\n";
    return 2;
  }
      
  GsTLlog << "registering qt plugins library" << "\n"; 
    
  /* Set-up directory structure: /GridObject, /GridObject/Model and
   * /GridObject/OinvDescription
   */
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    qitem_grid_filter_model_manager);

      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << qitem_grid_filter_model_manager << "\n";
    return 1;
  }
    
     
  bind_grid_filter_items( dir );


  GsTLlog << "Registration done" << "\n";
  Root::instance()->list_all( GsTLlog );

  return 0;
}

  
//------------------------


int library_qtplugins_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( qitem_grid_filter_model_manager );
  }
  return 0;
}


//------------------------

bool library_qtplugins_init::bind_grid_filter_items(Manager* dir) {

  //  Union and intersection are remove until a better interface is written
//  dir->factory( Grid_filter_union().item_type().toStdString(), Grid_filter_union_item_ni::create_new_interface );
//  dir->factory( Grid_filter_intersection().item_type().toStdString(), Grid_filter_intersection_item_ni::create_new_interface );
  dir->factory( Grid_filter_region().item_type().toStdString(), Grid_filter_region_item_ni::create_new_interface );
  dir->factory( Grid_filter_category().item_type().toStdString(), Grid_filter_category_item_ni::create_new_interface );
  dir->factory( Grid_filter_less_than().item_type().toStdString(), Grid_filter_less_than_item_ni::create_new_interface );
  dir->factory( Grid_filter_greater_than().item_type().toStdString(), Grid_filter_greater_than_item_ni::create_new_interface );
  dir->factory( Grid_filter_x_coord_bounded().item_type().toStdString(), Grid_filter_x_coord_bounded_item_ni::create_new_interface );
  dir->factory( Grid_filter_y_coord_bounded().item_type().toStdString(), Grid_filter_y_coord_bounded_item_ni::create_new_interface );
  dir->factory( Grid_filter_z_coord_bounded().item_type().toStdString(), Grid_filter_z_coord_bounded_item_ni::create_new_interface );
//  dir->factory( Grid_filter_bounded().item_type().toStdString(), Grid_filter_bounded_it::create_new_interface );
//  dir->factory( Grid_filter_x_coord_bounded().item_type().toStdString(), Grid_filter_x_coord_bounded_i::create_new_interface );
//  dir->factory( Grid_filter_y_coord_bounded().item_type().toStdString(), Grid_filter_y_coord_bounded::create_new_interface );
//  dir->factory( Grid_filter_z_coord_bounded().item_type().toStdString(), Grid_filter_z_coord_bounded::create_new_interface );
//  dir->factory( Grid_filter_log_names().item_type().toStdString(), Grid_filter_log_names::create_new_interface );

  return true;
}

extern "C" {
  int lib_qtplugins_init() {
    return library_qtplugins_init::init_lib();
  }
  int lib_qtplugins_release() {
    return library_qtplugins_init::release_lib();
  }
}

