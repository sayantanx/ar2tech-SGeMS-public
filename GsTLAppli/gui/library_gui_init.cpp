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
  
#include <GsTLAppli/gui/library_gui_init.h>
#include <GsTLAppli/gui/viewer/vtk_project_view_gui.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_cgrid.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_mgrid.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_pointset.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_log.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_structured_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/log_data_grid.h>
#include <GsTLAppli/grid/grid_model/structured_grid.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qcombobox_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qlineedit_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qradiobutton_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qcheckbox_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qspin_box_accessor.h>
#include <GsTLAppli/gui/QWidget_value_accessors/qtable_accessor.h>
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/gui/utils/colorscale.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/gui/appli/visu_actions.h>
#include <GsTLAppli/gui/appli/qt_grid_summary.h>
#include <GsTLAppli/gui/models/project_ItemModel.h>
#include <GsTLAppli/gui/viewer/grid_param_viewer.h>
#include <GsTLAppli/gui/viewer/property_param_viewer.h>
#include <GsTLAppli/gui/viewer/region_param_viewer.h>
#include <GsTLAppli/gui/viewer/group_param_viewer.h>
#include <GsTLAppli/gui/models/context_menu_interface.h>

#include <QTableWidget>

int library_gui_init::references_ = 0;


int library_gui_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "gui library already registered" << "\n";
    return 2;
  }
     
  GsTLlog << "\n\n registering gui library" << "\n"; 
  
  //---------------------
  // Create the Project_views manager
  
  GsTLlog << "Creating Project_view manager" << "\n";
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    projectViews_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << projectViews_manager << "\n";
    return 1;
  }
  
  bind_project_views_factories( dir );
  

  //---------------------
  // Create the qt_accessors manager

  GsTLlog << "Creating QWidget_value_accessors manager" << "\n";
  
  SmartPtr<Named_interface> ni_utils = 
    Root::instance()->interface( geostatParamUtils_manager );

  // If directory "geostatParamUtils_manager" does not already exist, 
  // create it and set it up.
  if( ni_utils.raw_ptr() == 0 ) {
    ni_utils = 
      Root::instance()->new_interface("directory",
				      geostatParamUtils_manager);

    dir = dynamic_cast<Manager*>( ni_utils.raw_ptr() );
    if( !dir ) {
      GsTLlog << "could not create directory " 
	      << geostatParamUtils_manager << "\n";
      return 1;
    }

    dir->factory("directory", create_directory);
  }

  //----------------------
  // Now create subdirectory "xmlGeostatParamUtils_manager" of directory
  // "geostatParamUtils_manager" if it does not already exist
  SmartPtr<Named_interface> ni_xml_utils = 
    Root::instance()->interface( xmlGeostatParamUtils_manager );

  if( ni_xml_utils.raw_ptr() == 0 )
    ni_xml_utils = 
      Root::instance()->new_interface("directory://QTaccessors/xml",
				      xmlGeostatParamUtils_manager );

  dir = dynamic_cast<Manager*>( ni_xml_utils.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not get nor create directory " 
	      << xmlGeostatParamUtils_manager << "\n";
    return 1;
  }
  
  dir->factory("Value_collector", 
	       QWidgets_values_collector_xml::create_new_interface);
  bind_qt_accessors_factories( dir );


  //----------------------
  // Create the manager for the colormaps ("color scales" actually)
  GsTLlog << "Creating Colorscales manager" << "\n";
  SmartPtr<Named_interface> ni_cmaps = 
    Root::instance()->new_interface("directory://colorscales",
				    colorscale_manager );
      
  dir = dynamic_cast<Manager*>( ni_cmaps.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << colorscale_manager << "\n";
    return 1;
  }
  dir->factory( "colorscale", Color_scale::create_new_interface );


  //----------------------
  // Create the manager for Open Inventor descriptions
  // We assume that directory /GridObject already exists
  GsTLlog << "Creating VTK props manager" << "\n";
  SmartPtr<Named_interface> ni_desc =
    Root::instance()->new_interface("directory",
                                    vtkProps_manager);
  
  dir = dynamic_cast<Manager*>( ni_desc.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory"
              << vtkProps_manager << "\n";
    return 1;
  }
  
  bind_vtk_props( dir );
  

  //----------------------
  // Create the manager for Qt summary descriptions
  // We assume that directory /GridObject already exists
  GsTLlog << "Creating Qt summary descriptions manager" << "\n";
  SmartPtr<Named_interface> ni_qtdesc =
    Root::instance()->new_interface("directory",
                                    QtSummaryDescription_manager);
  
  dir = dynamic_cast<Manager*>( ni_qtdesc.raw_ptr() );
  if( !dir ) {
    GsTLlog << "could not create directory"
              << QtSummaryDescription_manager << "\n";
    return 1;
  }
  
  bind_qtsummary_descriptions( dir );
  


  //----------------------
  // Bind the GUI-related actions
  SmartPtr<Named_interface> actions_ni = 
    Root::instance()->interface( actions_manager );
      
  Manager* actions_manager = dynamic_cast<Manager*>( actions_ni.raw_ptr() );
    
  if( !actions_manager ) {
    GsTLlog << "could not find directory " 
	      << actions_manager << "\n";
    return 1;
  }
  
  bind_gui_actions( actions_manager );


  //----------------------
  // Bind the colormap

  GsTLlog << "Creating colormap factory" << "\n";
  SmartPtr<Named_interface> ni_cmap =
    Root::instance()->new_interface("directory",
    		colormap_manager);

  dir = dynamic_cast<Manager*>( ni_cmap.raw_ptr() );
  bind_colormap_factories(dir);




  // Bind the items models
  GsTLlog << "Creating item models factory" << "\n";
  /*
  SmartPtr<Named_interface> ni_model =
    Root::instance()->new_interface("directory",
    		qitem_model_manager);
*/
  SmartPtr<Named_interface> ni_model =
      Root::instance()->interface(qitem_model_manager);
  dir = dynamic_cast<Manager*>( ni_model.raw_ptr() );

  if( !dir ) {
    GsTLlog << "could not find "
	      << qitem_model_manager << "\n";
    return 1;
  }
  bind_qitem_models(dir);


  //----------------------
  // Bind the preferences viewer

  GsTLlog << "Creating preferences viewer" << "\n";
  SmartPtr<Named_interface> ni_viewer =
    Root::instance()->new_interface("directory",
    		object_viewer_manager);

  dir = dynamic_cast<Manager*>( ni_viewer.raw_ptr() );
  dir->factory("directory", create_directory);
  bind_viewer_factories(dir);


  //----------------------
  // Bind the preferences viewer

  GsTLlog << "Creating Custom Menus" << "\n";
  SmartPtr<Named_interface> ni_menu =
    Root::instance()->new_interface("directory",
    		contextMenus_manager);

  dir = dynamic_cast<Manager*>( ni_menu.raw_ptr() );
  bind_context_menus(dir);


  /*  NOW moved into the project charts
  //----------------------
  // Bind the eda charts
  GsTLlog << "Creating EDA charts" << "\n";
  SmartPtr<Named_interface> ni_charts =
    Root::instance()->new_interface("directory",
    		eda_charts_manager);

  dir = dynamic_cast<Manager*>( ni_charts.raw_ptr() );
  bind_eda_charts(dir);
  */

  //----------------------
  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}



int library_gui_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( projectViews_manager );
  }
  return 0;
}



bool library_gui_init::bind_project_views_factories(Manager* dir) {
  dir->factory( "vtk_view", Create_vtk_view );
  return true;
}


bool library_gui_init::bind_qt_accessors_factories(Manager* dir) {
  // ComboBox
  QComboBox combobox;
  std::string classname( combobox.metaObject()->className() );
  dir->factory(classname, QComboBox_accessor::create_new_interface);
      
  // SpinBox
  QSpinBox spinbox;
  classname = spinbox.metaObject()->className();
  dir->factory(classname, QSpinBox_accessor::create_new_interface);

  // DoubleSpinBox
  QDoubleSpinBox double_spinbox;
  classname = double_spinbox.metaObject()->className();
  dir->factory(classname, QDoubleSpinBox_accessor::create_new_interface);

  // RadioButton
  QRadioButton radiobutton(0);
  classname = radiobutton.metaObject()->className();
  dir->factory(classname, QRadioButton_accessor::create_new_interface);

  // CheckBox
  QCheckBox checkbox(0);
  classname = checkbox.metaObject()->className();
  dir->factory(classname, QCheckBox_accessor::create_new_interface);

  // LineEdit
  QLineEdit lineedit(0);
  classname = lineedit.metaObject()->className();
  dir->factory(classname, QLineEdit_accessor::create_new_interface);
  
  // QTable
  QTableWidget table;
  classname = table.metaObject()->className();
  dir->factory(classname, QTable_accessor::create_new_interface);
    
  return true;
}


bool library_gui_init::bind_vtk_props(Manager* dir) {
  Cartesian_grid cg;
  dir->factory( cg.classname(), vtkProp_cgrid::create_new_interface );
  Point_set pset;
  dir->factory( pset.classname(), vtkProp_pointset::create_new_interface );

  Reduced_grid rg;
  dir->factory( rg.classname(), vtkProp_mgrid::create_new_interface );

  Log_data_grid lg;
  dir->factory( lg.classname(), vtkProp_log::create_new_interface );

  Structured_grid sg;
  dir->factory( sg.classname(), vtkProp_structured_grid::create_new_interface );

  return true;
}

bool library_gui_init::bind_qtsummary_descriptions(Manager* dir) {
  Cartesian_grid cg;
  dir->factory( cg.classname(), Qt_cartesian_grid_summary::create_new_interface );
  Point_set pset;
  dir->factory( pset.classname(), Qt_pointset_summary::create_new_interface );

  //TL modified
  Reduced_grid rg;
  dir->factory( rg.classname(), Qt_masked_grid_summary::create_new_interface );
  return true;
}


bool library_gui_init::bind_gui_actions(Manager* dir) {
  dir->factory( "RotateCamera", Rotate_camera::create_new_interface );
  dir->factory( "ShowHistogram", Show_histogram::create_new_interface );
  dir->factory( "SaveHistogram", Save_histogram::create_new_interface );
  dir->factory( "SaveQQplot", Save_qqplot::create_new_interface );
  dir->factory( "SaveScatterplot", Save_scatterplot::create_new_interface );
  dir->factory( "DisplayObject", Display_object::create_new_interface );
  dir->factory( "HideObject", Hide_object::create_new_interface );
  dir->factory( "TakeSnapshot", Snapshot_action::create_new_interface );
  dir->factory( "ResizeCameraWindow", Resize_camera_window::create_new_interface );
  dir->factory( "SaveCameraSettings", 
    Save_camera_settings_to_file::create_new_interface );
  dir->factory( "LoadCameraSettings", 
    Load_camera_settings::create_new_interface );

  //TL modified
  dir->factory( "LoadColorBar", Load_colorbar::create_new_interface );

  return true;
}


bool library_gui_init::bind_colormap_factories(Manager* dir){

	dir->factory("directory", create_directory);

	// Bind the continuous colormap
	  GsTLlog << "Creating continuous colormap factory" << "\n";
	  SmartPtr<Named_interface> ni_cmap_cont =
	    Root::instance()->new_interface("directory",
	    		colormap_continuous_manager);

	  dir = dynamic_cast<Manager*>( ni_cmap_cont.raw_ptr() );

	  if( !dir ) {
	    GsTLlog << "could not create directory "
		      << colormap_continuous_manager << "\n";
	    return 1;
	  }
	  dir->factory( "ColormapContinuous", Colormap_continuous::create_new_interface );


	  // Bind the categorical colormap
	  GsTLlog << "Creating categorical colormap factory" << "\n";
	  SmartPtr<Named_interface> ni_cmap_cat =
	    Root::instance()->new_interface("directory",
	    		colormap_categorical_manager);

	  dir = dynamic_cast<Manager*>( ni_cmap_cat.raw_ptr() );

	  if( !dir ) {
	    GsTLlog << "could not create directory "
		      << colormap_categorical_manager << "\n";
	    return 1;
	  }
	  dir->factory( "ColormapCategorical", Colormap_categorical::create_new_interface );
}

bool library_gui_init::bind_qitem_models(Manager* dir){

	dir->factory( "project_model", Project_ItemModel::create_new_interface );
  return true;

}

bool library_gui_init::bind_viewer_factories(Manager* dir){

	dir->factory("directory", create_directory);

	// Bind the grid viewer
	  GsTLlog << "Creating grid preference viewer factory" << "\n";
	  SmartPtr<Named_interface> ni_grid_viewer =
	    Root::instance()->new_interface("directory",
	    		grid_viewer_manager);

	  dir = dynamic_cast<Manager*>( ni_grid_viewer.raw_ptr() );

	  if( !dir ) {
	    GsTLlog << "could not create directory "
		      << grid_viewer_manager << "\n";
	    return 1;
	  }
	  dir->factory( Cartesian_grid().classname(), CGrid_viz_parameters::create_new_interface );
	  dir->factory( Reduced_grid().classname(), MGrid_viz_parameters::create_new_interface );
	  dir->factory( Point_set().classname(), PSet_viz_parameters::create_new_interface );
	  dir->factory( Log_data_grid().classname(), Log_grid_viz_parameters::create_new_interface );

    dir->factory( Structured_grid().classname(), Structured_grid_viz_parameters::create_new_interface );

//	  dir->factory( Cartesian_grid().classname(), CGrid_param_viewer::create_new_interface );
//	  dir->factory( Reduced_grid().classname(), MGrid_param_viewer::create_new_interface );
//	  dir->factory( Point_set().classname(), PSet_param_viewer::create_new_interface );
//	  dir->factory( Log_data_grid().classname(), Log_grid_param_viewer::create_new_interface );

	// Bindind Property Viewers
	  GsTLlog << "Creating property preference viewer factory" << "\n";
	  SmartPtr<Named_interface> ni_prop_viewer =
		Root::instance()->new_interface("directory",
				prop_viewer_manager);

	  dir = dynamic_cast<Manager*>( ni_prop_viewer.raw_ptr() );

	  if( !dir ) {
		GsTLlog << "could not create directory "
			  << prop_viewer_manager << "\n";
		return 1;
	  }
	  dir->factory( "PropertyParamViewer", Property_viz_parameters::create_new_interface );


	// Binding Region Viewers
	  GsTLlog << "Creating region preference viewer factory" << "\n";
	  SmartPtr<Named_interface> ni_region_viewer =
		Root::instance()->new_interface("directory",
				region_viewer_manager);

	  dir = dynamic_cast<Manager*>( ni_region_viewer.raw_ptr() );

	  if( !dir ) {
		GsTLlog << "could not create directory "
			  << region_viewer_manager << "\n";
		return 1;
	  }
	  dir->factory( "RegionParamViewer", Region_viz_parameters::create_new_interface );


	// Binding Group Viewers
	  GsTLlog << "Creating region preference viewer factory" << "\n";
	  SmartPtr<Named_interface> ni_group_viewer =
		Root::instance()->new_interface("directory",
				group_viewer_manager);

	  dir = dynamic_cast<Manager*>( ni_group_viewer.raw_ptr() );

	  if( !dir ) {
		GsTLlog << "could not create directory "
			  << group_viewer_manager << "\n";
		return 1;
	  }
	  dir->factory( "GroupParamViewer", Group_viz_parameters::create_new_interface );

}


bool library_gui_init::bind_context_menus(Manager* dir){
	dir->factory( Cartesian_grid().classname(), Context_menu_grid::create_new_interface);
	dir->factory( Reduced_grid().classname(), Context_menu_grid::create_new_interface);
	dir->factory( Point_set().classname(), Context_menu_grid::create_new_interface);
	dir->factory( Log_data_grid().classname(), Context_menu_grid::create_new_interface);
  dir->factory( Structured_grid().classname(), Context_menu_grid::create_new_interface);

	// Create the context menu
	dir->new_interface(Cartesian_grid().classname(),"/"+Cartesian_grid().classname());
	dir->new_interface(Reduced_grid().classname(),"/"+Reduced_grid().classname());
	dir->new_interface(Point_set().classname(),"/"+Point_set().classname());
	dir->new_interface(Log_data_grid().classname(),"/"+Log_data_grid().classname());
  dir->new_interface(Structured_grid().classname(),"/"+Structured_grid().classname());

	dir->factory( "ContinuousProperty", Context_menu_property::create_new_interface);
	dir->factory( "CategoricalProperty", Context_menu_property::create_new_interface);

  dir->new_interface("ContinuousProperty","/ContinuousProperty");
	dir->new_interface("CategoricalProperty","/CategoricalProperty");

  dir->factory( "Region", Context_menu_region::create_new_interface);
  dir->new_interface("Region","/Region");

  return true;
}

/*
bool library_gui_init::bind_eda_charts(Manager* dir){
  dir->factory( EDA_continuous_histogram_factory().name(), 
                EDA_continuous_histogram_factory::create_new_interface);

  dir->factory( EDA_categorical_histogram_factory().name(), 
                EDA_categorical_histogram_factory::create_new_interface);

  return true;
}
*/

extern "C" {
  int libGsTLAppli_gui_init() {
    return library_gui_init::init_lib();
  }
  int libGsTLAppli_gui_release(){
    return library_gui_init::release_lib();
  }
}
