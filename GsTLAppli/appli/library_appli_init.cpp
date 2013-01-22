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
** This file is part of the "appli" module of the Geostatistical Earth
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

#include <GsTLAppli/appli/library_appli_init.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/utils/gstl_messages.h>


int library_appli_init::references_ = 0;


int library_appli_init::init_lib() {

//  std::cout << "number of subscriber to GsTLlog: "  
//    << GsTLlog.subscribers() << "  second opinion: " 
//    << GsTLAppli_logging::instance()->subscribers() << std::endl;
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "appli library already registered \n";
    return 2;
  }
     
  GsTLlog << "\n\n registering appli library \n"; 
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    projects_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << projects_manager << "\n";
    return 1;
  }
  
  bind_appli_factories( dir );
  GsTLlog << "Registration done\n\n" ;

  // Bind the items models
  GsTLlog << "Creating item models factory" << "\n";
  SmartPtr<Named_interface> ni_model =
    Root::instance()->new_interface("directory",
    		qitem_model_manager);
  dir = dynamic_cast<Manager*>( ni_model.raw_ptr() );

  if( !dir ) {
    GsTLlog << "could not create directory "
	      << qitem_model_manager << "\n";
    return 1;
  }
 // bind_model_factories(dir);




  Root::instance()->list_all( GsTLlog );
  return 0;
}


int library_appli_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( projects_manager );
  }
  return 0;
}


bool library_appli_init::bind_appli_factories(Manager* dir) {
  dir->factory( "project", Create_gstl_project );
  return true;
}
/*
bool library_appli_init::bind_model_factories(Manager* dir){

	Root::instance()->model(new Root_model() );
  return true;
}
*/

extern "C" {
  int libGsTLAppli_appli_init() {
    return library_appli_init::init_lib();
  }
  int libGsTLAppli_appli_release(){
    return library_appli_init::release_lib();
  }
}
