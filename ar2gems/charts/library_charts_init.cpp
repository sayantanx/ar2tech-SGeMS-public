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


#include <charts/library_charts_init.h>
#include <charts/continuous_histogram_creator.h>
#include <charts/categorical_histogram_creator.h>
#include <charts/scatter_plot_creator.h>
#include <appli/manager_repository.h>
#include <utils/manager.h>
#include <utils/gstl_messages.h>

#include <QTableWidget>

int library_charts_init::references_ = 0;


int library_charts_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "gui library already registered" << "\n";
    return 2;
  }
     
  GsTLlog << "\n\n registering charts library" << "\n"; 
  
 
  //----------------------
  // Bind the eda charts
  GsTLlog << "Creating EDA charts" << "\n";
  SmartPtr<Named_interface> ni_charts =
    Root::instance()->new_interface("directory",
    		eda_charts_manager);

  Manager* dir = dynamic_cast<Manager*>( ni_charts.raw_ptr() );

  dir->factory( Continuous_histogram_creator_factory().name(), 
                Continuous_histogram_creator_factory::create_new_interface);

  dir->factory( Categorical_histogram_creator_factory().name(), 
                Categorical_histogram_creator_factory::create_new_interface);

  dir->factory( Scatter_plot_creator_factory().name(), 
                Scatter_plot_creator_factory::create_new_interface);

  //----------------------
  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}



int library_charts_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( eda_charts_manager );
  }
  return 0;
}




extern "C" {
  int libGsTLAppli_charts_init() {
    return library_charts_init::init_lib();
  }
  int libGsTLAppli_charts_release(){
    return library_charts_init::release_lib();
  }
}
