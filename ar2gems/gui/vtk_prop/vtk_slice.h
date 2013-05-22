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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GUI_VTK_SLICE_H__
#define __GSTLAPPLI_GUI_VTK_SLICE_H__

/*
// There is a problem, even if defined in preprocessor
// SIMVOLEON_DLL is not recognized from there
// This should be fixed
#ifndef SIMVOLEON_DLL
  #define SIMVOLEON_DLL
#endif
*/ 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_common.h>
#include <GsTLAppli/math/gstlvector.h> 
 
 
#include <vector> 
#include <utility> 
 
class SGrid_cursor; 
class GsTL_vtkProp;
class Colormap;
 
 
 /*
void slice_translation_callback( void* data, SoDragger* ); 
 */
 
class GUI_DECL Vtk_slice {
   
 // friend void slice_translation_callback( void* data, SoDragger* );
 
 public: 
 
  Vtk_slice(  );
  virtual ~Vtk_slice() ;
 
  void property_display_mode( VTK::Property_display_mode mode );
  virtual GsTL_vtkProp* vtk_prop() { return vtk_prop_; }
  virtual void update() = 0; 
  virtual void translate( int step ) {};
  virtual void position( int new_pos ) {};
 
  int position() const { return pos_; } 
  int max_position() const { return max_pos_; } 
 
 protected: 
  GsTLVector<int> grid_dimensions_;
  const VTK::Axis axis_;
  int pos_, initial_pos_; 
  int max_pos_; 

  GsTLVector<int> slice_normal_;
  SGrid_cursor* cursor_;


  const bool* initialized_;
  Colormap*& cmap_;

  GsTL_vtkProp* vtk_prop_;

 
}; 
 
 
#endif 
