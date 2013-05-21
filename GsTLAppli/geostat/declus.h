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



//a. boucher
// @ar2tech 2011

#ifndef __GSTLAPPLI_DECLUS_H__ 
#define __GSTLAPPLI_DECLUS_H__ 
 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/geostat_algo.h> 
#include <GsTLAppli/geostat/utilities.h> 


#include <GsTL/geometry/covariance.h> 
#include <GsTL/geometry/Block_covariance.h>
 
#include <string> 
 
class Neighborhood; 
class RGrid;
 
 
class GEOSTAT_DECL Declus : public Geostat_algo { 
 public: 
  Declus(); 
  ~Declus(); 
   
  virtual bool initialize( const Parameters_handler* parameters, 
			   Error_messages_handler* errors ); 
  virtual int execute( GsTL_project* proj=0 ); 
  virtual std::string name() const { return "declus"; } 
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 protected:
   void clean( const std::string& prop ); 

 protected: 
  typedef Geostat_grid::location_type Location; 

  Grid_continuous_property* prop_;
  Grid_region* region_;
  Geostat_grid* grid_;

  std::string weight_name_;
  float tmin_, tmax_;  // Cutoff value
  float min_dimx_, min_dimy_, min_dimz_; 
  float max_dimx_, max_dimy_, max_dimz_; 
  int ncell_;
  int noff_;

   
  SmartPtr<Neighborhood> neighborhood_; 

}; 

 
#endif 
