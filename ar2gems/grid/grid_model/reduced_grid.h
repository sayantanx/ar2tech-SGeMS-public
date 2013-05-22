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
** Author: Ting Li
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

#ifndef REDUCED_GRID_H
#define REDUCED_GRID_H


#include <grid/grid_model/cartesian_grid.h>
#include <grid/grid_model/rgrid.h>
#include <grid/maskedgridcursor.h>

#include <math/gstlvector.h>
#include <grid/grid_model/neighborhood.h>
#include <utils/string_manipulation.h>
#include <grid/grid_model/point_set.h>
#include <math/gstlpoint.h>

#include <qdatastream.h>

#include <string>
#include <map>
#include <vector>

#define DIV 5.0  /* neighbor search radius = dim/DIV */


Named_interface* create_reduced_grid( std::string& name_and_size_str);

class GRID_DECL Reduced_grid : public Cartesian_grid
{
public:
	Reduced_grid(std::string name);
	Reduced_grid(std::string name, int size);
	Reduced_grid();   
  virtual ~Reduced_grid();

	virtual std::string classname() const { return "Masked_grid"; } 

	//====================================
	// new methods
	// this is based on point set read function


	void insertLocation(GsTLGridNode loc){
		active_coords_.push_back(loc);
	}

	void copyStructure(const Reduced_grid *);


	// returns nx*ny*nz
	GsTLInt rgrid_size() const;

	inline bool is_inside_mask(int idInFullGrid) const ;

	// given a node id in full grid, return the corresponding id in masked grid
	// returns -1 if not applicable.
	const int full2reduced(int idInFullGrid) const ;
	const int reduced2full(int idInReducedGrid) const;


	virtual Neighborhood* neighborhood( double x, double y, double z, 
		double ang1, double ang2, double ang3, 
		const Covariance<location_type>* cov=0, 
		bool only_harddata = false,
    const Grid_region* region = 0,
    Coordinate_mapper* coord_mapper=0); 

	virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
		const GsTLTripletTmpl<double>& angles, 
		const Covariance<location_type>* cov=0, 
		bool only_harddata = false,
    const Grid_region* region = 0,
    Coordinate_mapper* coord_mapper=0); 

	virtual Window_neighborhood* window_neighborhood( const Grid_template& templ ); 

  inline location_type location( int node_id ) const ;

  bool add_location(int i, int j, int k);
  bool add_location(GsTLCoord x, GsTLCoord y, GsTLCoord z);
  bool add_location(int CartesianGridNodeId);

  GsTLInt closest_node( const location_type& P ) const;

  const std::vector<bool>& mask() const;
  void mask(const std::vector<bool>& mask);
  void mask(const std::vector<GsTLGridNode>& ijkCoords);
  void mask(const std::vector<location_type>& xyzCoords);


//  void set_dimensions( int nx, int ny, int nz);

  void set_dimensions( int nx, int ny, int nz, 
		       double xsize, double ysize, double zsize,
           float rotation_angle_z=0);

  void set_dimensions( int nx, int ny, int nz, 
		       double xsize, double ysize, double zsize, 
           std::vector<bool> mask, 
           float rotation_angle_z=0 ); 

  void set_dimensions( int nx, int ny, int nz,
    double xsize, double ysize, double zsize,
    const std::vector<GsTLGridNode>& ijkCoords,
    float rotation_angle_z=0);

  void set_dimensions( int nx, int ny, int nz, 
    double xsize, double ysize, double zsize,
    const std::vector<location_type>& xyzCoords,
    Geostat_grid::location_type origin,
    float rotation_angle_z=0);


	//==============================================
	// The following override the functions inherited from rgrid
	void set_geometry(RGrid_geometry* geom) ;
	GsTLInt size() const ;

  virtual const SGrid_cursor* cursor() const; 
  virtual SGrid_cursor* cursor(); 


	//===============================================

protected:
	
	// translates from an id in reduced grid to an id in the full grid
//	std::map<int,int> reduced2original_;
	std::vector<int> reduced2original_;
	// the other direction
//	std::map<int,int> original2reduced_;
	std::vector<int> original2reduced_;

	// stores coordinates of active cells
	std::vector<GsTLGridNode> active_coords_;

	// mask for active cells
	std::vector<bool> mask_;

	// number of active cells
	GsTLInt active_size_;
	GsTLInt rgrid_size_;

  MaskedGridCursor* mgrid_cursor_;

  protected :

  void build_ijkmap_from_mask();
  void build_mask_from_ijk(
     const std::vector<GsTLGridNode>& iCoords);


  void build_mask_from_xyz(
    const std::vector<location_type>& xyzCoords);

};


/*
inline GsTLGridNode Reduced_grid::ijkValue(int node_id) const {
	std::map<int,int>::const_iterator itr = reduced2original_.find(node_id);
	node_id = itr->second;
	GsTLInt max_nxy = geom_->dim(0)*geom_->dim(1);
	GsTLInt inxy = node_id % max_nxy; 
	GsTLInt k = (node_id - inxy)/max_nxy; 
	GsTLInt j = (inxy - node_id%geom_->dim(0))/geom_->dim(0); 
	GsTLInt i = inxy%geom_->dim(0);
	return GsTLGridNode(i,j,k);
}
*/
inline bool Reduced_grid::is_inside_mask(int idInFullGrid) const {
  return mask_[idInFullGrid];
}


#endif
