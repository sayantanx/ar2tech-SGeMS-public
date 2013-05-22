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

#include <gui/viewer/qvtkGsTLViewer.h>
#include <utils/gstl_messages.h>

#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkAxesActor.h>
#include <vtkAxisActor2D.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkProperty2D.h>
#include <vtkCaptionActor2D.h>
#include <vtkSmartPointer.h>
#include <vtkDistanceRepresentation.h>
#include <vtkDistanceRepresentation2D.h>
#include <vtkCamera.h>
#include <vtkGeoCamera.h>
#include <vtkRendererCollection.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkPointHandleRepresentation3D.h>
#include <vtkActorCollection.h>

#define MSTEP 0.02
#define SSTEP 0.02

QvtkGsTLViewer::QvtkGsTLViewer( QWidget* parent, const char* name, bool embed   )
  : QVTKWidget( parent  )
{
 // this->setSizePolicy(QSizePolicy::Expanding);
  renderer_ = vtkSmartPointer<vtkRenderer>::New();
  this->GetRenderWindow()->AddRenderer(renderer_);


// Create the axes marker
  vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
  axes->SetShaftTypeToLine();
  axes->SetXAxisLabelText("East");
  axes->SetYAxisLabelText("North");
  axes->SetZAxisLabelText("Up");
  axes->SetTotalLength(0.5,0.5,0.5);

  orientation_widget_ = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  orientation_widget_->SetOrientationMarker(axes);
  orientation_widget_->SetOutlineColor(0.93,0.57,0.13);
  orientation_widget_->SetInteractor(this->GetInteractor());
  orientation_widget_->SetViewport(0.0,0.0,0.25,0.25);
  orientation_widget_->SetEnabled(1);


  // Create the legend actor
  scale_actor_ = vtkSmartPointer<vtkLegendScaleActor>::New();
  scale_actor_->SetLabelModeToXYCoordinates ();
 // scale_actor_->GetRightAxis()->SetLabelFormat( );
 // scale_actor_->GetRightAxis()->SetAdjustLabels(0);
 // scale_actor_->GetLeftAxis()->SetAdjustLabels(1);
 // scale_actor_->GetTopAxis()->SetAdjustLabels(2);
 // scale_actor_->GetBottomAxis()->SetAdjustLabels(3);
  scale_actor_->VisibilityOff();
  renderer_->AddViewProp(scale_actor_);

  picker_ = vtkSmartPointer<vtkCellPicker>::New();
  picker_->SetTolerance(0.01);

  this->GetInteractor()->SetPicker(picker_);


  distance_ = vtkSmartPointer<vtkDistanceWidget>::New();
  distance_->SetInteractor( this->GetInteractor());
  distance_->CreateDefaultRepresentation();
//  static_cast<vtkDistanceRepresentation *>(distanceWidget->GetRepresentation())
//    ->SetLabelFormat("%-#6.3g mm");


distance_->CreateDefaultRepresentation();
vtkSmartPointer< vtkDistanceRepresentation2D > rep =
  vtkSmartPointer< vtkDistanceRepresentation2D >::New();
rep->GetAxis()->GetProperty()->SetColor( 1.0, 0.0, 0.0 );

  distance_->On();
  point_placer_ = vtkSmartPointer<vtkCellCentersPointPlacer>::New();
// Create a 3D handle reprensentation template for this distance
// widget
  vtkSmartPointer< vtkPointHandleRepresentation3D > handle_rep_3D =
    vtkSmartPointer< vtkPointHandleRepresentation3D >::New();

  handle_rep_3D->GetProperty()->SetLineWidth(4.0);
  rep->SetHandleRepresentation( handle_rep_3D );
  handle_rep_3D->GetProperty()->SetColor( 0.8, 0.2, 0 );
  distance_->SetRepresentation(rep);

// Instantiate the handles and have them be constrained by the
// placer.
  rep->InstantiateHandleRepresentation();
  rep->GetPoint1Representation()->SetPointPlacer(point_placer_);
  rep->GetPoint2Representation()->SetPointPlacer(point_placer_);

// With a "snap" constraint, we can't have a smooth motion anymore, so
// turn it off.
  static_cast< vtkPointHandleRepresentation3D * >(
  rep->GetPoint1Representation())->SmoothMotionOff();
  static_cast< vtkPointHandleRepresentation3D * >(
  rep->GetPoint2Representation())->SmoothMotionOff();

  distance_->SetEnabled(false);
/*

  compass_ = vtkSmartPointer<vtkCompassWidget>::New();
  compass_->CreateDefaultRepresentation();

  vtkGeoCamera* geo_camera = vtkGeoCamera::New();

  this->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->SetActiveCamera(geo_camera->GetVTKCamera());
  geo_camera->SetHeading(compass_->GetHeading()*360.0);
  geo_camera->SetTilt(compass_->GetTilt());
  geo_camera->SetDistance(compass_->GetDistance());
*/




}
vtkRenderer*  QvtkGsTLViewer::get_renderer( ) {
	return renderer_.GetPointer();
}

void QvtkGsTLViewer::render(){
	this->GetRenderWindow()->Render();
  this->update();
}


void QvtkGsTLViewer::rotate_camera( GsTLVector<float> dir, float ang ) {
  vtkCamera* camera = renderer_->GetActiveCamera();
  //vtkCamera* camera;
  if( !camera ) return;
  
  camera->Azimuth(dir.x());
  camera->Yaw(dir.y());
  camera->Pitch(dir.z());

  this->update();


}

void QvtkGsTLViewer::set_orientiation_axis_visible(bool on){
	orientation_widget_->SetEnabled((int)on);
  this->update();
}
void QvtkGsTLViewer::set_scale_legend_visible(bool on){
	scale_actor_->SetVisibility((int)on);
  this->update();
}


void QvtkGsTLViewer::set_background_color(float r, float g, float b){
	renderer_->SetBackground( r,g,b);
	float r_scale = 1.0-r;
	float g_scale = 1.0-g;
	float b_scale = 1.0-b;
	scale_actor_->GetRightAxis()->GetProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetLeftAxis()->GetProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetTopAxis()->GetProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetBottomAxis()->GetProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetRightAxis()->GetLabelTextProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetLeftAxis()->GetLabelTextProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetTopAxis()->GetLabelTextProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetBottomAxis()->GetLabelTextProperty()->SetColor(r_scale, g_scale, b_scale);

	scale_actor_->GetLegendLabelProperty()->SetColor(r_scale, g_scale, b_scale);
	scale_actor_->GetLegendTitleProperty()->SetColor(r_scale, g_scale, b_scale);

	vtkAxesActor* orientation_axes = vtkAxesActor::SafeDownCast(orientation_widget_->GetOrientationMarker());
	orientation_axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(r_scale, g_scale, b_scale);
	orientation_axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(r_scale, g_scale, b_scale);
	orientation_axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(r_scale, g_scale, b_scale);

	vtkDistanceRepresentation2D* distance_rep = dynamic_cast<vtkDistanceRepresentation2D*>(distance_->GetDistanceRepresentation());
	if(distance_rep) {
		distance_rep->GetAxis()->GetTitleTextProperty()->SetColor(r_scale, g_scale, b_scale);
	}

  this->update();



}

void QvtkGsTLViewer::enable_distance_ruler(bool on){
	if(on == false) {
		distance_->SetWidgetStateToStart();
    point_placer_->RemoveAllProps();
	}
  else {
    vtkActorCollection * actors =	renderer_->GetActors();
    actors->InitTraversal(); 
    vtkActor* actor = actors->GetNextItem(); 
    while(actor!=0) 
      { 
        if( actor->IsA("vtkActor2D") ) continue;
        point_placer_->AddProp(actor);
         actor = actors->GetNextItem(); 
      } 
  }


	distance_->SetEnabled((int)on);

//	distance_->SetVisibility((int)on);

}

