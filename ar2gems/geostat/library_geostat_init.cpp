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
** This file is part of the "geostat" module of the Geostatistical Earth
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

#include <geostat/library_geostat_init.h>
#include <geostat/parameters_handler_impl.h>

#include <geostat/kriging.h>
#include <geostat/indicator_kriging.h>
#include <geostat/cosisim.h>
#include <geostat/cokriging.h>
#include <geostat/sgsim.h>
#include <geostat/sisim.h>
#include <geostat/cosgsim.h>
#include <geostat/snesim_std/snesim_std.h>
#include <geostat/PostKriging.h>
#include <geostat/Postsim.h>
#include <geostat/Postsim_categorical.h>
#include <geostat/trans.h>
#include <geostat/dssim.h>
#include <geostat/LU_sim.h>
#include <geostat/filtersim_std/filtersim_cate.h>
#include <geostat/filtersim_std/filtersim_cont.h>
#include <appli/manager_repository.h>
#include <utils/gstl_messages.h>
#include <geostat/transcat.h>
#include <geostat/ImageProcess.h>
#include <geostat/nuTauModel.h>
#include <geostat/moving_window.h>
#include <geostat/difference_with_base.h>
#include <geostat/kriging_mean.h>
#include <geostat/declus.h>
#include <geostat/kmeans_clustering.h>
#include <geostat/clustering_from_centroids.h>
#include <geostat/kriging_cross_validation.h>
#include <geostat/cokriging_cross_validation.h>

int library_geostat_init::references_ = 0;
 

int library_geostat_init::init_lib() {
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "geostat library already registered" << "\n";
    return 2;
  }
     
  GsTLlog << "\n\n registering geostat library" << "\n"; 
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory://Geostat",
				     geostatAlgo_manager );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << geostatAlgo_manager << "\n";
    return 1;
  }
  
  bind_geostat_factories( dir );

  // Get or create directory for geostat utilities
  SmartPtr<Named_interface> ni_utils = 
    Root::instance()->interface( geostatParamUtils_manager );
  
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
  bind_geostat_utilities( dir );


  GsTLlog << "Registration done" << "\n\n";

  Root::instance()->list_all( GsTLlog );
  return 0;
}


int library_geostat_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( geostatAlgo_manager );
  }
  return 0;
}


bool library_geostat_init::bind_geostat_utilities( Manager* dir ) {
  dir->factory( "Parameters_handler", Parameters_handler_xml::create_new_interface );
  return true;
}
 
bool library_geostat_init::bind_geostat_factories( Manager* dir ) {
  bool ok;
  ok = dir->factory( Kriging().name(), Kriging::create_new_interface );
  ok = dir->factory( Sgsim().name(), Sgsim::create_new_interface );
  ok = dir->factory( Indicator_kriging().name(), 
		Indicator_kriging::create_new_interface );
  ok = dir->factory( Cokriging().name(), Cokriging::create_new_interface );
  ok = dir->factory( Kriging_x_validation().name(), Kriging_x_validation::create_new_interface );
  ok = dir->factory( Cokriging_x_validation().name(), Cokriging_x_validation::create_new_interface );

  ok = dir->factory( Sisim().name(), Sisim::create_new_interface );
  ok = dir->factory( Cosgsim().name(), Cosgsim::create_new_interface );
  ok = dir->factory( Cosisim().name(), Cosisim::create_new_interface );
  ok = dir->factory( Snesim_Std().name(), Snesim_Std::create_new_interface );

  ok = dir->factory( Postsim().name(), Postsim::create_new_interface );
  ok = dir->factory( Postsim_categorical().name(), Postsim_categorical::create_new_interface );
  ok = dir->factory( PostKriging().name(), PostKriging::create_new_interface );
  ok = dir->factory( trans().name(), trans::create_new_interface );
  ok = dir->factory( transcat().name(), transcat::create_new_interface );
  ok = dir->factory( ImageProcess().name(), ImageProcess::create_new_interface );

//  ok = dir->factory( transcat().name(), transcat::create_new_interface );
  ok = dir->factory( dssim().name(), dssim::create_new_interface );
  ok = dir->factory( LU_sim().name(), LU_sim::create_new_interface );
//  ok = dir->factory( LU_sim().name(), LU_sim::create_new_interface );
  ok = dir->factory( Filtersim_Cate().name(), Filtersim_Cate::create_new_interface );
  ok = dir->factory( Filtersim_Cont().name(), Filtersim_Cont::create_new_interface );
  ok = dir->factory( NuTauModel().name(), NuTauModel::create_new_interface );
  ok = dir->factory( Moving_window().name(), Moving_window::create_new_interface );
  ok = dir->factory( DiffProperties().name(), DiffProperties::create_new_interface );
  ok = dir->factory( KrigingMean().name(), KrigingMean::create_new_interface );
  ok = dir->factory( Declus().name(), Declus::create_new_interface );
  ok = dir->factory( Kmeans_clustering().name(), Kmeans_clustering::create_new_interface );
  ok = dir->factory( Clustering_from_centroids().name(), Clustering_from_centroids::create_new_interface );
  


  return true;
}


extern "C" {
  int libGsTLAppli_geostat_init() {
    return library_geostat_init::init_lib();
  }
  int libGsTLAppli_geostat_release(){
    return library_geostat_init::release_lib();
  }
}
