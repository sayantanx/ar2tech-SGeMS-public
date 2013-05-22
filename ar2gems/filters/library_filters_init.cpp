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
** This file is part of the "filters" module of the Geostatistical Earth
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

#include <GsTLAppli/filters/library_filters_init.h>
#include <GsTLAppli/filters/gslib/gslib_filter.h>
#include <GsTLAppli/filters/simulacre_filter.h>
#include <GsTLAppli/filters/csv_filter.h>
#include <GsTLAppli/filters/sgems_folder_filter.h>
#include <GsTLAppli/filters/distribution_filter.h>
#include <GsTLAppli/filters/save_project_objects.h>
#include <GsTLAppli/filters/sgems_folder_grid_geometry.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/log_data_grid.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>
#include <GsTLAppli/grid/grid_model/structured_grid.h>

int library_filters_init::references_ = 0;

int library_filters_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "filters library already registered" << "\n";
    return 2;
  }
      
  GsTLlog << "\n\n registering filters library" << "\n"; 
    

  // Set-up directory structure: /InputFilters, /InputFilters/Gslib

  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    infilters_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << infilters_manager << "\n";
    return 1;
  }
  
  dir->factory("directory", create_directory);

  SmartPtr<Named_interface> ni_toplevel = 
    Root::instance()->new_interface("directory://filters",
				    topLevelInputFilters_manager);
  SmartPtr<Named_interface> ni_gslib =
    Root::instance()->new_interface("directory", 
				    gslibInputFilters_manager);

  SmartPtr<Named_interface> ni_csv =
    Root::instance()->new_interface("directory", 
				    csvInputFilters_manager);


  // Bind the factory methods for top-level filters
   
  dir = dynamic_cast<Manager*>( ni_toplevel.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory" 
	      << topLevelInputFilters_manager << "\n";
    return 1;
  }
      
  bind_toplevel_infilters_factories( dir );


  // Bind the factory methods for gslib filters
   
  dir = dynamic_cast<Manager*>( ni_gslib.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory" 
	      << gslibInputFilters_manager << "\n";
    return 1;
  }     
 
  bind_gslib_infilters_factories( dir );

  
  // Bind the factory methods for csv filters
   
  dir = dynamic_cast<Manager*>( ni_csv.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory" 
	      << csvInputFilters_manager << "\n";
    return 1;
  }     
 
  bind_csv_infilters_factories( dir );


  // Create the directory for output filters
  SmartPtr<Named_interface> outfilter_ni = 
    Root::instance()->new_interface("directory://filters",
				    outfilters_manager );
  dir = dynamic_cast<Manager*>( outfilter_ni.raw_ptr() );
  appli_assert( dir );

  bind_output_filters_factories( dir );


  //create the binding for saving a project
  SmartPtr<Named_interface> geometry_filter_ni = 
    Root::instance()->new_interface("directory://grid_geometry_xml_io",
				    grid_geom_xml_io_manager );
  dir = dynamic_cast<Manager*>( geometry_filter_ni.raw_ptr() );
  appli_assert( dir );

  bind_geometry_xml_managers(dir);


  //create the binding for saving a project
  SmartPtr<Named_interface> project_filter_ni = 
    Root::instance()->new_interface("directory://project_filters",
				    project_filters_manager );
  dir = dynamic_cast<Manager*>( project_filter_ni.raw_ptr() );
  appli_assert( dir );

  bind_project_output(dir);


  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}

  
//------------------------


int library_filters_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( infilters_manager );
    Root::instance()->delete_interface( topLevelInputFilters_manager );
    Root::instance()->delete_interface( gslibInputFilters_manager );
    Root::instance()->delete_interface( csvInputFilters_manager );
  }
  return 0;
}


//------------------------

bool library_filters_init::bind_toplevel_infilters_factories(Manager* dir) {
  dir->factory( Gslib_infilter().filter_name(),
                Gslib_infilter::create_new_interface );
  dir->factory( Simulacre_input_filter().filter_name(),
                Simulacre_input_filter::create_new_interface );
  dir->factory( Csv_infilter().filter_name(),
                Csv_infilter::create_new_interface );
  dir->factory( Sgems_folder_input_filter().filter_name(),
								Sgems_folder_input_filter::create_new_interface );
  dir->factory( Distribution_input_filter().filter_name(),
								Distribution_input_filter::create_new_interface );
  return true;
}

//------------------------

bool library_filters_init::bind_gslib_infilters_factories(Manager* dir) {
  // Input filter for grids
  Gslib_grid_infilter filter;
  dir->factory( filter.object_filtered(), 
		Gslib_grid_infilter::create_new_interface );

  //TL modified
  Gslib_mgrid_infilter mfilter;
  dir->factory( mfilter.object_filtered(), 
		Gslib_mgrid_infilter::create_new_interface );

  // Input filter for point-sets
  Gslib_poinset_infilter pset_filter;
  dir->factory( pset_filter.object_filtered(), 
		Gslib_poinset_infilter::create_new_interface );

  return true;
}


bool library_filters_init::bind_csv_infilters_factories(Manager* dir) {
  // Input filter for grids
  Csv_grid_infilter filter;
  dir->factory( filter.object_filtered(), 
		Csv_grid_infilter::create_new_interface );

  Csv_mgrid_infilter mfilter;
  dir->factory( mfilter.object_filtered(), 
		Csv_mgrid_infilter::create_new_interface );

  // Input filter for point-sets
  Csv_poinset_infilter pset_filter;
  dir->factory( pset_filter.object_filtered(), 
		Csv_poinset_infilter::create_new_interface );

  return true;
}

bool library_filters_init::bind_output_filters_factories(Manager* dir) {
  Gslib_outfilter gslib_filter;
  dir->factory( gslib_filter.filter_name(),
		Gslib_outfilter::create_new_interface );
  dir->factory( Simulacre_output_filter().filter_name(),
                Simulacre_output_filter::create_new_interface );
  dir->factory( Sgems_folder_output_filter().filter_name(),
								Sgems_folder_output_filter::create_new_interface );
  dir->factory( Csv_outfilter().filter_name(),
								Csv_outfilter::create_new_interface );
  dir->factory( Distribution_output_filter().filter_name(),
								Distribution_output_filter::create_new_interface );
  return true;
}


bool library_filters_init::bind_project_output(Manager* dir){
    dir->factory( "project_filter",
		Save_project_objects::create_new_interface );

    SmartPtr<Named_interface> project_filter_ni = dir->new_interface("project_filter","/project_filters");

    Save_project_objects* project_filter = dynamic_cast<Save_project_objects*>(project_filter_ni.raw_ptr());

    project_filter->add_directory(gridModels_manager,
      std::make_pair(Sgems_folder_output_filter().filter_name(),outfilters_manager+"/")) ;
    project_filter->add_directory(continuous_distributions_manager,
      std::make_pair(Distribution_output_filter().filter_name(),outfilters_manager+"/") );

    return true;

}

bool library_filters_init::bind_geometry_xml_managers(Manager* dir){

    dir->factory( Cartesian_grid().classname(),
		  Cartesian_grid_geometry_xml_io::create_new_interface );
    dir->factory( Point_set().classname(),
		  Pointset_geometry_xml_io::create_new_interface );
    dir->factory( Reduced_grid().classname(),
		  Masked_grid_geometry_xml_io::create_new_interface );
    dir->factory( Log_data_grid().classname(),
		  Log_data_grid_geometry_xml_io::create_new_interface );
    dir->factory( Structured_grid().classname(),
		  Structured_grid_geometry_xml_io::create_new_interface );
    
    return true;

}



extern "C" {
  int libGsTLAppli_filters_init() {
    return library_filters_init::init_lib();
  }

  int libGsTLAppli_filters_release() {
    return library_filters_init::release_lib();
  }
}

