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





#include <GsTLAppli/gui/vtk_prop/colormap_actor.h>
#include <GsTLAppli/utils/string_manipulation.h>


#include <qstring.h>
  

Colormap_actor::Colormap_actor()
  : cmap_( 0 ) {
  cbar_actor_ = vtkScalarBarActor::New();
  color_table_ = vtkLookupTableWithEnabling::New();

}



Colormap_actor::Colormap_actor( const Colormap_continuous& cmap, int nlabels, bool is_horizontal, bool is_log_scale)
  : cmap_( 0 ) {

  cbar_actor_ = vtkScalarBarActor::New();
  color_table_ = vtkLookupTableWithEnabling::New();


  if(nlabels>0)
    cbar_actor_->SetNumberOfLabels(nlabels);

  if(is_horizontal)
    cbar_actor_->SetOrientationToHorizontal();

  if(is_log_scale)
	color_table_->SetScaleToLog10();

  nb_colors_ = 0;

  Colormap_actor::colormap( cmap );
}


Colormap_actor::~Colormap_actor() {
  cbar_actor_->Delete();
  color_table_->Delete();
}


void Colormap_actor::colormap( const Colormap_continuous& cmap) {
  
  if( cmap_ == cmap ) return;

  cmap_ = cmap;
  
  color_table_->SetNumberOfColors(cmap.color_scale()->colors_count());

  for( int i=0; i < nb_colors_ ; i++ ) {
    float r,g,b;
    cmap.color_scale()->color(i, r,g,b);
    color_table_->SetTableValue(i,r,g,b);
  }
}

