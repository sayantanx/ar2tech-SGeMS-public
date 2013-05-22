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

#ifndef __GSTLAPPLI_GEOSTAT_COSISIM_H__ 
#define __GSTLAPPLI_GEOSTAT_COSISIM_H__  
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/geostat_algo.h> 
#include <GsTLAppli/geostat/utilities.h> 
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 
#include <GsTLAppli/grid/grid_model/neighborhood.h> 
#include <GsTLAppli/grid/grid_model/property_copier.h> 
 
#include <GsTL/geometry/covariance.h> 
#include <GsTL/kriging/markov_bayes_covariance.h> 
#include <GsTL/math/indicator_coding.h>
#include <GsTL/utils/smartptr.h> 
#include <GsTLAppli/grid/grid_model/grid_region_temp_selector.h> 
#include <string> 
#include <vector> 
 
 
//class Grid_initializer; 
template<class T> class Non_parametric_cdf; 
 
 
class GEOSTAT_DECL Cosisim : public Geostat_algo { 
public: 
  Cosisim(); 
  ~Cosisim(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			                     Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "cosisim"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 
protected:  
  typedef Geostat_grid::random_path_iterator RandomPath;
  typedef Non_parametric_cdf<float> Ccdf_type;
  typedef geostat_utils::NeighborhoodHandle NeighorhoodType;

  Geostat_grid* simul_grid_; 
  MultiRealization_property* multireal_property_; 
  std::vector< Grid_continuous_property* > indicators_;
  
  std::vector< Grid_continuous_property* > primary_indicators_;
  std::vector< Grid_continuous_property* > secondary_indicators_;


  long int seed_; 
  int nb_of_realizations_; 
 
  //Grid_initializer* initializer_; 
  SmartPtr<Property_copier> property_copier_; 
  
  Ccdf_type* ccdf_; 
  Ccdf_type* marginal_; 
    
  bool do_median_ik_; 

  geostat_utils::NeighborhoodVector hard_neighborhoods_;
  geostat_utils::NeighborhoodVector soft_neighborhoods_;

  typedef Covariance<geostat_utils::Location> CovarianceType;
  typedef MarkovBayes_covariance< CovarianceType > MarkovBayesCovariance;
  std::vector<MarkovBayesCovariance> covariances_;
  
  std::vector<geostat_utils::CoKrigingCombiner> combiners_; 
  geostat_utils::CoKrigingConstraints* kconstraints_; 
 
  Indicator<float> indicator_coder_;

  Temporary_gridRegion_Selector grid_region_;
  Temporary_gridRegion_Selector soft_grid_region_;

protected: 
  void init_neighborhoods( Geostat_grid* hard_grid, Geostat_grid* soft_grid, 
                           const GsTLTripletTmpl<double>& hard_ranges, 
                           const GsTLTripletTmpl<double>& hard_angles,
                           const GsTLTripletTmpl<double>& soft_ranges, 
                           const GsTLTripletTmpl<double>& soft_angles,
                           int hard_max_neighbors, int soft_max_neighbors,
                           std::vector< CovarianceType >& covariances,
                           Search_filter* prim_filter = NULL, 
                           Search_filter* soft_filter=NULL);

  void get_current_local_cdf( const Geovalue& g,
                         std::vector< unsigned int >& unestimated_indicators );

  int median_ik( RandomPath begin, RandomPath end, 
                 Progress_notifier* progress_notifier);
  geostat_utils::NeighborhoodVector median_ik_find_neighbors( const Geovalue& center,
                              std::vector< unsigned int >& unestimated_indicators );

  int full_ik( RandomPath begin, RandomPath end, 
               Progress_notifier* progress_notifier );
  geostat_utils::NeighborhoodVector full_ik_find_neighbors( const Geovalue& center,
                              std::vector< unsigned int >& unestimated_indicators );

  void code_into_indicators( Geovalue& g );
  void clear_ccdf();
  void reset_indicator_properties();
  void remove_temporary_properties();
  void clean( Grid_continuous_property* prop ); 
}; 
 
 
#endif 
