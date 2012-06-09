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

#ifndef __GSTLAPPLI_GUI_VTK_STRATI_GRID_H__
#define __GSTLAPPLI_GUI_VTK_STRATI_GRID_H__
 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
 
#include <qstring.h>

#include <vector> 
#include <list> 
 
 
class GUI_DECL vtkProp_strati_grid : public GsTL_vtkProp {
 public: 
  enum Display_mode { RENDERING=0, FULL=1, SLICES=2, BBOX=3 }; 
 
 public: 
  vtkProp_strati_grid() : GsTL_vtkProp() {}
  virtual ~vtkProp_strati_grid() {}
  virtual int add_slice( VTK::Axis axis, int position, bool visible ) = 0;
  virtual void remove_slice( int slice_id ) = 0; 
   
  virtual void set_region(const std::string& region_name )=0;

  virtual void display_mode( Display_mode mode ) = 0; 
  virtual void property_display_mode( VTK::Property_display_mode mode ) = 0;
 
  virtual void show_bounding_box( bool on ) = 0;
  virtual void hide_volume( bool on ) = 0;

}; 
 
#endif 
