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

#include <GsTLAppli/gui/vtk_prop/vtk_slice.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/utils/gstl_messages.h>



#include <iostream>
#include <numeric>


/*
void slice_translation_callback( void* data, SoDragger* dragger ) {
  Oinv_slice* slice = (Oinv_slice*) data;
  float x,y,z;
  SoTranslate1Dragger *dragger1D = dynamic_cast<SoTranslate1Dragger *>(dragger);
  dragger1D->translation.getValue().getValue(x,y,z);

  if( x > slice->max_pos_ - slice->initial_pos_ )
    x = slice->max_pos_ - slice->initial_pos_;
  if( x < - slice->initial_pos_ )
    x= - slice->initial_pos_;

  slice->translate( static_cast<int>( x - slice->pos_ ) );
  dragger1D->translation.setValue( static_cast<int>(x), 0, 0);
}
*/
//==========================


Vtk_slice::Vtk_slice(  )
  :  grid_dimensions_(grid_dimensions),
     std_values_( std_values ), initialized_(initialized),
     cmap_(cmap),
     axis_( axis ), pos_( pos ),
     slice_normal_( 0, 0, 0 ) {

  slice_normal_[axis] = 1;
  max_pos_ = grid_dimensions_[axis]-1;

  // make sure 0 <= pos <= max_pos
  pos_ = std::min( pos_, max_pos_ );
  pos_ = std::max( pos_, 0 );
  initial_pos_ = pos_;

  cursor_ = new SGrid_cursor( grid_dimensions_[0],
                              grid_dimensions_[1],
                              grid_dimensions_[2] );

  // compute the bound indices of the back plane
  GsTLVector<int> vec(0,0,0);
  for( int i=0; i < 3 ; i++ ) {
    vec[i] = (1-slice_normal_[i]) * grid_dimensions_[i];
  }
  GsTLVector<int> start= pos*slice_normal_;
  GsTLVector<int> stop = vec + (pos+1)*slice_normal_;
/*
  texture_ = new Texture_node( std::make_pair( start, stop ),
                               std_values_, initialized_, cmap, *cursor_ );

  root_ = new GsTL_SoNode;
  root_->ref();

  slice_root_ = new SoSeparator;
  root_->addChild( slice_root_ );
  */
}
    


Vtk_slice::~Vtk_slice() {
 // root_->unref();
}



void Vtk_slice::property_display_mode( Oinv::Property_display_mode mode ) {
 //texture_->property_display_mode( mode );
}









