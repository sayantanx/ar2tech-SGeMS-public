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

//Modified A.Boucher April 2012



#ifndef __GSTLAPPLI_GEOSTAT_ALGO_COKRIGING_XVALIDATION_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_COKRIGING_XVALIDATION_H__ 
 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/geostat_algo.h> 
#include <GsTLAppli/geostat/utilities.h> 
#include <GsTLAppli/grid/grid_model/neighborhood.h> 
 
#include <GsTL/geometry/covariance.h> 
#include <GsTL/kriging/covariance_set.h> 
#include <GsTL/kriging/cokriging_combiner.h> 
#include <GsTL/kriging/cokriging_constraints.h> 

#include <GsTLAppli/grid/grid_model/grid_region_temp_selector.h> 

#include <string> 
 
class Neighborhood; 
class Parameters_handler; 
class Error_messages_handler; 
 
 
class GEOSTAT_DECL Cokriging_x_validation : public Geostat_algo { 
 public: 
  Cokriging_x_validation(); 
  ~Cokriging_x_validation(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			   Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "cokriging-x-validation"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 
 
 protected: 
  typedef Geostat_grid::location_type Location; 
  typedef Covariance_set< Covariance<Location> > CovarianceSet; 
 
  typedef GsTL_neighborhood< SmartPtr<Neighborhood> > NeighborhoodHandle; 
 
  typedef std::vector< NeighborhoodHandle >::const_iterator NeighIterator; 
  typedef std::vector<double>::const_iterator WeightIterator; 
  typedef CoKriging_combiner< WeightIterator, NeighIterator > CoKrigingCombiner; 
  typedef CoKriging_constraints< NeighIterator, Geovalue > KrigingConstraints;
 
 
 protected:   
  std::string property_name_; 
   
  Geostat_grid* prim_harddata_grid_;  
  Geostat_grid* sec_harddata_grid_;  
  std::string primary_variable_; 
  std::string secondary_variable_; 
   
   
  std::vector< NeighborhoodHandle > neighborhood_vector_; 
   
  CovarianceSet* covar_; 
  CoKrigingCombiner* combiner_; 
  KrigingConstraints* Kconstraints_; 
 
  std::vector<double> kriging_weights_; 

  int min_neigh_1_, min_neigh_2_;

  Temporary_gridRegion_Selector prim_hd_grid_region_;
  Temporary_gridRegion_Selector sec_hd_grid_region_;
 
 protected: 
/*  virtual bool finish_covset_and_neighborhood( Covariance<Location>& C11, 
					       geostat_utils::Cokriging_type type, 
					       const Parameters_handler* parameters, 
					       Error_messages_handler* errors ); */
}; 
 
 
 
 
 
 
 
#endif 
