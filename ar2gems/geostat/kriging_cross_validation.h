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

#ifndef __GSTLAPPLI_GEOSTAT_ALGO_KRIGING_XVALIDATION_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_KRIGING_XVALIDATION_H__ 
 
 
#include <geostat/common.h>
#include <geostat/geostat_algo.h> 
#include <geostat/utilities.h> 
#include <grid/grid_model/grid_region_temp_selector.h> 

#include <GsTL/geometry/covariance.h> 
#include <GsTL/geometry/Block_covariance.h>
 
#include <string> 
 
class Neighborhood; 
class RGrid;
 
 
class GEOSTAT_DECL Kriging_x_validation : public Geostat_algo { 
 public: 
  Kriging_x_validation(); 
  ~Kriging_x_validation(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			   Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "kriging-x-validation"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 protected:
   void clean( const std::string& prop ); 

 protected: 
  typedef Geostat_grid::location_type Location; 
  typedef std::vector<double>::const_iterator weight_iterator; 
  typedef Kriging_combiner< weight_iterator, Neighborhood > KrigingCombiner; 
  typedef Kriging_constraints< Neighborhood, Geovalue > KrigingConstraints;
 
  std::string property_name_; 
   
  Geostat_grid* harddata_grid_; 
  Grid_continuous_property* hdata_prop_;
  Grid_continuous_property* blk_hdata_prop_;
  std::string harddata_property_name_;
   
  SmartPtr<Neighborhood> neighborhood_; 
 
  Covariance<Location> covar_;
  Covariance<Location>*  rhs_covar_;

  KrigingCombiner* combiner_; 
  KrigingConstraints* Kconstraints_; 
 
  std::vector<double> kriging_weights_;

  int min_neigh_;
 Temporary_gridRegion_Selector hdgridTempRegionSelector_;

/* 
 protected: 
  bool set_kriging_parameters( const std::string& kriging_type, 
			       const Parameters_handler* parameters, 
			       Error_messages_handler* errors ); 
*/
}; 

 
#endif 
