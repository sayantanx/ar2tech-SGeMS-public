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
** This file is part of the "grid" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GRID_MODEL_RGRID_NEIGHBORHOOD_H__ 
#define __GSTLAPPLI_GRID_MODEL_RGRID_NEIGHBORHOOD_H__ 
 
 
#include <grid/common.h>
#include <math/gstlpoint.h> 
#include <grid/grid_model/neighborhood.h> 
#include <grid/grid_model/sgrid_cursor.h> 
//#include <grid/maskgridcursor.h> 
 
#include <GsTL/geometry/geometry_algorithms.h> 
#include <GsTL/geometry/covariance.h> 
 
#include <vector> 
#include <stack> 
 
class RGrid; 
class Grid_continuous_property; 
 


//===================================== 
//    Window Neighborhood 
//===================================== 
 
class GRID_DECL Rgrid_window_neighborhood : public Window_neighborhood { 
public:
  typedef Geovalue value_type; 
  typedef Neighborhood::iterator iterator; 
  typedef Neighborhood::const_iterator const_iterator; 

 public: 
  Rgrid_window_neighborhood(); 
  Rgrid_window_neighborhood( const Grid_template& geom, RGrid* grid=0, 
			     Grid_continuous_property* prop = 0 ); 
  virtual ~Rgrid_window_neighborhood() {} 
 
  virtual void set_geometry( Grid_template::iterator begin,  
			     Grid_template::iterator end ); 
  virtual void add_geometry_vector( const Grid_template::Euclidean_vector& vec );
  virtual void clear_geometry() { geom_.clear(); }

  /** If the center geovalue is informed, it will be included at the begining
  * of the found neighbors. Note that if the template geometry already included
  * offset vector (0,0,0) before calling \c includes_center(true), then the
  * center will be included twice.
  */
  virtual void includes_center( bool on );

  void set_grid( RGrid* grid ); 
  virtual bool select_property( const std::string& prop_name );  
  virtual const Grid_continuous_property* selected_property() const { return property_; }

  virtual void find_neighbors( const Geovalue& center ) ; 
  virtual void find_all_neighbors( const Geovalue& center );

  virtual int size() const;
  virtual void max_size( int s ); 
  virtual int max_size() const { return geom_.max_size(); } 
  virtual Geovalue center() const { return center_; } 
 
  virtual void set_neighbors( const_iterator begin, const_iterator end );

 protected: 
  RGrid* grid_; 
  Grid_continuous_property* property_; 
//  Grid_template geom_; 
  Geovalue center_; 
  SGrid_cursor cursor_; 

  mutable int size_;
}; 
 
 
 
 
//===================================== 
//    Ellipsoid Neighborhood 
//===================================== 
 
class GRID_DECL Rgrid_ellips_neighborhood : public Neighborhood { 
public:
  typedef Geovalue value_type; 
  typedef Neighborhood::iterator iterator; 
  typedef Neighborhood::const_iterator const_iterator; 
 
 public: 
  /** The dimensions of the search ellipsoid are expressed in number of cells 
   * Angles are in radian. The 3 angles are the standard angles of a spherical
   * coordinate system (the third angle, z_angle, is rake). x_angle is not 
   * azimuth. It's pi/2-azimuth.
   */ 
  Rgrid_ellips_neighborhood( RGrid* grid, 
			     Grid_continuous_property* property, 
			     GsTLInt max_radius, GsTLInt mid_radius, GsTLInt min_radius, 
			     double x_angle, double y_angle, double z_angle, 
			     int max_neighbors = 20, 
			     const Covariance<GsTLPoint>* cov = 0,
           const Grid_region* region = 0); 
 
  virtual ~Rgrid_ellips_neighborhood() {}; 
  virtual bool select_property( const std::string& prop_name );  
  virtual const Grid_continuous_property* selected_property() const { return property_; }

  virtual void find_neighbors( const Geovalue& center ); 
  virtual void max_size( int s ); 
  virtual int max_size() const { return max_neighbors_; } 
  virtual Geovalue center() const { return center_; } 
 
  virtual void set_neighbors( const_iterator begin, const_iterator end );

  virtual Grid_template& geometry() { return geom_; } 
  virtual const Grid_template& geometry() const { return geom_; } 

 protected: 
  RGrid* grid_; 
  Grid_continuous_property* property_; 
  SGrid_cursor cursor_; 
  Grid_template geom_; 
  int max_neighbors_; 
  Geovalue center_; 
 
}; 
 

/** This neighborhood only retains nodes flagged as hard data. 
 */ 
class GRID_DECL Rgrid_ellips_neighborhood_hd : public Rgrid_ellips_neighborhood { 
public:
  typedef Geovalue value_type; 
  typedef Neighborhood::iterator iterator; 
  typedef Neighborhood::const_iterator const_iterator; 

 public: 
  Rgrid_ellips_neighborhood_hd( RGrid* grid, 
				Grid_continuous_property* property, 
				GsTLInt max_radius, GsTLInt mid_radius, GsTLInt min_radius, 
				double x_angle, double y_angle, double z_angle, 
				int max_neighbors = 20, 
				const Covariance<GsTLPoint>* cov = 0,
        const Grid_region* region = 0); 
  ~Rgrid_ellips_neighborhood_hd() {}; 
 
  virtual void find_neighbors( const Geovalue& center ); 
}; 
 
 
 
//===================================== 
//    Some utilities 
//===================================== 
 
class GRID_DECL Ellipsoid_rasterizer { 
 public: 
  typedef GsTLGridNode::difference_type EuclideanVector; 
 
  Ellipsoid_rasterizer( GsTLInt nx, GsTLInt ny, GsTLInt nz, 
			GsTLInt max_radius, GsTLInt mid_radius, GsTLInt min_radius, 
			double x_angle, double y_angle, double z_angle ); 
  ~Ellipsoid_rasterizer(); 
  std::vector<EuclideanVector>& rasterize() ; 
   
 
 private: 
  void check_node( const GsTLGridNode& loc ); 
 
 private: 
  std::vector<EuclideanVector> ellipsoid_nodes_; 
  std::vector<bool> already_visited_; 
  std::stack<GsTLInt> s_; 
  SGrid_cursor cursor_; 
  Anisotropic_norm_3d<EuclideanVector> norm_; 
  GsTLInt max_radius_; 
  GsTLGridNode center_; 
}; 
 
 
//=========================================== 
// Comparison of two euclidean vectors based on covariance value 
// v1 > v2  iif  cov(v1) > cov(v2) 
 
class GRID_DECL Evector_greater_than { 
 public: 
  typedef GsTLGridNode::difference_type EuclideanVector; 
 
  Evector_greater_than( const Covariance<GsTLPoint>& cov ) 
    : cov_( &cov ) { }; 
 
  bool operator()(const EuclideanVector& v1, const EuclideanVector& v2) { 
    GsTLVector<GsTLCoord> vv1(v1); 
    GsTLVector<GsTLCoord> vv2(v2); 
 
    return cov_->compute( vv1 ) > cov_->compute( vv2 ); 
  } 
 
 private: 
  const Covariance<GsTLPoint>* cov_; 
}; 
 
 
#endif 
