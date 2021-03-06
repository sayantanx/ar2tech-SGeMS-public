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

#ifndef __GSTLAPPLI_GEOSTAT_ALGO_MEANKRIGING_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_MEANKRIGING_H__ 
 
 
#include <geostat/common.h>
#include <geostat/geostat_algo.h> 
#include <geostat/utilities.h> 
#include <grid/grid_model/grid_region_temp_selector.h> 

#include <GsTL/geometry/covariance.h> 
#include <GsTL/geometry/Block_covariance.h>
 
#include <string> 
 
class Neighborhood; 
class RGrid;
 
 
class GEOSTAT_DECL KrigingMean : public Geostat_algo { 
 public: 
  KrigingMean(); 
  ~KrigingMean(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			   Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "kriging_mean"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 protected:
   void clean( const std::string& prop ); 

 protected: 
  typedef Geostat_grid::location_type Location; 
  typedef std::vector<double>::const_iterator weight_iterator; 
  typedef Kriging_combiner< weight_iterator, Neighborhood > KrigingCombiner; 
  typedef Kriging_constraints< Neighborhood, Geovalue > KrigingConstraints;
 
  Geostat_grid* simul_grid_; 
  std::string property_name_; 
   
  Geostat_grid* harddata_grid_; 
  Grid_continuous_property* hdata_prop_;
  Grid_continuous_property* blk_hdata_prop_;
  std::string harddata_property_name_;
   
  SmartPtr<Neighborhood> neighborhood_; 
 
  Covariance<Location> covar_;
  Covariance<Location>*  rhs_covar_;
//Block_covariance<Location>*  rhs_covar_;
  KrigingCombiner* combiner_; 
  KrigingConstraints* Kconstraints_; 
 
  std::vector<double> kriging_weights_;

  int min_neigh_;
  GsTLVector<int> nblock_pts_;

  Temporary_gridRegion_Selector gridTempRegionSelector_;
  Temporary_gridRegion_Selector hdgridTempRegionSelector_;

  bool do_block_kriging_;


/* 
 protected: 
  bool set_kriging_parameters( const std::string& kriging_type, 
			       const Parameters_handler* parameters, 
			       Error_messages_handler* errors ); 
*/
}; 

 
#endif 
