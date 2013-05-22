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

#ifndef __COORDINATE_MAPPER_H__ 
#define __COORDINATE_MAPPER_H__ 
 
#include <grid/common.h>
#include <utils/gstl_types.h> 
#include <math/gstlpoint.h> 

class Geostat_grid;

class GRID_DECL Coordinate_mapper { 
public: 
  Coordinate_mapper(Geostat_grid* grid);
  virtual ~Coordinate_mapper(){}

  // Coordinate transformation from real world to grid coordinate
  virtual GsTLPoint xyz_coords(int node_id) const;
  virtual GsTLPoint uvw_coords(int node_id)  const;
  virtual GsTLPoint uvw_coords(GsTLPoint xyz)   {return xyz; }
  virtual GsTLPoint xyz_coords(GsTLPoint uvw)   {return uvw; }

protected :
  Geostat_grid* grid_;

};


#endif