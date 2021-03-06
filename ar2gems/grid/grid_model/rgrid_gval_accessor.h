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

#ifndef __GSTLAPPLI_GRID_GVAL_ACCESSOR_H__ 
#define __GSTLAPPLI_GRID_GVAL_ACCESSOR_H__ 
 
 
#include <grid/common.h>
#include <grid/grid_model/sgrid_cursor.h> 
#include <grid/grid_model/grid_property.h> 
#include <math/gstlpoint.h> 
#include <utils/gstl_types.h> 
 
#include <grid/grid_model/geovalue.h> 
 
class RGrid; 
 
 
class GRID_DECL RGrid_gval_accessor { 
public: 
  typedef Geovalue geovalue_type;  
   
  RGrid_gval_accessor() {} 
   
  RGrid_gval_accessor( RGrid* rgrid, 
		       const std::string & prop_name ); 
  RGrid_gval_accessor( RGrid* rgrid, Grid_continuous_property* property ); 
 
  RGrid_gval_accessor(const RGrid_gval_accessor& a); 
 
     
  ~RGrid_gval_accessor() { } 
 
     
  RGrid_gval_accessor& operator = (const RGrid_gval_accessor& a) { 
    if (this != &a ) { 
      current_geovalue_ = a.current_geovalue_; 
    } 
    return *this; 
  } 
 
  inline void set_geovalue( int node_id, const Geovalue::location_type& loc ) { 
    current_geovalue_.node_id() = node_id; 
    current_geovalue_.set_cached_location( loc ); 
  } 
     
  inline Geovalue* node() { 
    return &current_geovalue_; 
  } 
 
 
 protected: 
  Geovalue current_geovalue_; 
 
}; 
 
 
 
 
#endif //__GSTL_APPLI_GRID_GRID_GVAL_ACCESSOR_H__ 
 
