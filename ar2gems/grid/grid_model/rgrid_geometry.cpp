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

#include <grid/grid_model/rgrid_geometry.h>

#include <GsTL/math/math_functions.h>

RGrid_geometry::~RGrid_geometry() {
}


//***********************************************


Simple_RGrid_geometry::Simple_RGrid_geometry() 
  : RGrid_geometry(), 
    origin_( 0,0,0 ),
    z_rot_(0.0),
    volume_(1){
}


Simple_RGrid_geometry::~Simple_RGrid_geometry() {
}

RGrid_geometry* Simple_RGrid_geometry::clone() const {
  Simple_RGrid_geometry* geom = new Simple_RGrid_geometry;
  geom->set_size( 0, n_[0] );
  geom->set_size( 1, n_[1] );
  geom->set_size( 2, n_[2] );
  geom->set_origin( origin_ );
  geom->set_cell_dims( cell_dims_ );
  geom->set_rotation_z(z_rot_);


  return geom;
}


GsTLPoint Simple_RGrid_geometry::coordinates(GsTLInt i, GsTLInt j, GsTLInt k) const {
  
  GsTLCoord dx = GsTLCoord(i)*cell_dims_.x() ;
  GsTLCoord dy = GsTLCoord(j)*cell_dims_.y() ;
  
  GsTLCoord x = origin_.x() + dx*z_cos_angle_ - dy*z_sin_angle_; 
  GsTLCoord y = origin_.y() + dx*z_sin_angle_ + dy*z_cos_angle_ ; 
  GsTLCoord z = origin_.z() + GsTLCoord(k)*cell_dims_.z() ; 

  /*
  GsTLCoord x = origin_.x() + GsTLCoord(i)*cell_dims_.x() ; 
  GsTLCoord y = origin_.y() + GsTLCoord(j)*cell_dims_.y() ; 
  GsTLCoord z = origin_.z() + GsTLCoord(k)*cell_dims_.z() ; 
  */
  return GsTLPoint(x, y, z);
}


bool Simple_RGrid_geometry::
grid_coordinates( GsTLGridNode& ijk, const GsTLPoint& p ) const {
  GsTLPoint::difference_type translated = p - origin_;

  GsTLCoord dx  = translated.x()*z_cos_angle_ + translated.y()*z_sin_angle_;
  GsTLCoord dy  = -1.0*translated.x()*z_sin_angle_ + translated.y()*z_cos_angle_;

  ijk[0] = GsTL::floor( dx / float( cell_dims_.x() ) );
  ijk[1] = GsTL::floor( dy / float( cell_dims_.y() ) );
  ijk[2] = GsTL::floor( translated.z() / float( cell_dims_.z() ) );

  /*
  ijk[0] = GsTL::floor( translated.x() / float( cell_dims_.x() ) );
  ijk[1] = GsTL::floor( translated.y() / float( cell_dims_.y() ) );
  ijk[2] = GsTL::floor( translated.z() / float( cell_dims_.z() ) );
  */
  return true;
}
 
void Simple_RGrid_geometry::set_rotation_z(double z_angle) {
  z_rot_ = z_angle; 
  double z_rad = GsTL::PI/180.0*z_rot_;
  z_cos_angle_ = std::cos(-1*z_rad);
  z_sin_angle_ = std::sin(-1*z_rad);
//  z_cos_back_angle_ = std::cos(z_rad);
//  z_sin_back_angle_ = std::sin(z_rad);
}