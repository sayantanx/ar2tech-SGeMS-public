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
** Author: Alexandre Boucher
** Copyright (C) 2010 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of the "gui" module of the Stanford Geostatistical Earth
** Modeling Software (GEMS)
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Library General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*
**********************************************************************/

#ifndef __GSTLAPPLI_GUI_APPLI_VTKQTGSTLVIEWER_H__
#define __GSTLAPPLI_GUI_APPLI_VTKQTGSTLVIEWER_H__
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/math/gstlvector.h> 


#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkLegendScaleActor.h>
#include <vtkCellPicker.h>
#include <vtkDistanceWidget.h>
#include <vtkCompassWidget.h>
#include <vtkCellCentersPointPlacer.h>

class GUI_DECL QvtkGsTLViewer : public QVTKWidget {

	Q_OBJECT
 public: 
	QvtkGsTLViewer( QWidget* parent = 0, const char* name =0,
		  bool embed = true ) ;
 
  virtual ~QvtkGsTLViewer() {};
  
  void rotate_camera( GsTLVector<float> dir, float ang );
  void rotate_camera( const GsTLVector<float>& focal_point,
                              const GsTLVector<float>& position,
                              const GsTLVector<float>& view_u);

  vtkRenderer*  get_renderer( );

  void render();


public slots :
	 void set_orientiation_axis_visible(bool on);
	 void set_scale_legend_visible(bool on);
	 void set_background_color(float r, float g, float b);
	 void enable_distance_ruler(bool on);



 protected:
  vtkSmartPointer<vtkOrientationMarkerWidget> orientation_widget_;
  vtkSmartPointer<vtkLegendScaleActor> scale_actor_;
  vtkSmartPointer<vtkCellPicker> picker_;
  vtkSmartPointer<vtkDistanceWidget> distance_;
  vtkSmartPointer<vtkCellCentersPointPlacer> point_placer_;
  vtkSmartPointer<vtkRenderer> renderer_;
 // vtkSmartPointer<vtkCompassWidget> compass_;



}; 
 
 
#endif 
