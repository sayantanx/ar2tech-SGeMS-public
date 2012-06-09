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





#ifndef __GSTLAPPLI_VTK_COLORMAP_NODE_H__
#define __GSTLAPPLI_VTK_COLORMAP_NODE_H__
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/colormap.h> 
 
#include <vtkScalarBarActor.h>
#include <vtkLookupTableWithEnabling.h>

 
 
class GUI_DECL Colormap_actor {
 public: 
  Colormap_actor();
  Colormap_actor( const Colormap_continuous& cmap ,int nlabels, bool is_horizontal  = false, bool is_log_scale = false);
  ~Colormap_actor();
  void colormap( const Colormap_continuous& cmap ); 
 
  vtkScalarBarActor* vtk_actor() { return cbar_actor_; }
 
 private: 
  Colormap_continuous cmap_; 
  int nb_colors_; 

  vtkScalarBarActor* cbar_actor_;
  vtkLookupTableWithEnabling* color_table_;
 

};  
 
#endif 
