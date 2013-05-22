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



/*
 * colorbar_controller.cpp
 *
 *  Created on: Aug 9, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/gui/viewer/colorbar_controller.h>
#include <GsTLAppli/gui/utils/colormap.h>

#include <vtkTextProperty.h>

Colorbar_controller::Colorbar_controller(vtkRenderer* renderer, QWidget* parent)
	:renderer_(renderer), QWidget(parent){

}

Colorbar_controller::~Colorbar_controller(){
	prop_to_colorbarsT::iterator it = colorbars_.begin();
	for( ; it!= colorbars_.end(); ++it) {
		it->second.second->Delete();
	}

}

vtkScalarBarActor* Colorbar_controller::color_bar(GsTL_vtkProp* prop){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return 0;
	return it->second.second;

}

bool Colorbar_controller::is_colorbar_visible(GsTL_vtkProp* prop){

	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return false;
	return it->second.first;
}


void Colorbar_controller::update_colorbars(){
	prop_to_colorbarsT::iterator it = colorbars_.begin();
	for( ; it!= colorbars_.end(); ++it) {
		GsTL_vtkProp* prop = it->first;
		vtkScalarBarActor* cbar_actor = it->second.second;
		cbar_actor->SetLookupTable(prop->get_colormap()->color_table());
	}
}

void Colorbar_controller::update_colorbars(GsTL_vtkProp* prop){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return;
	vtkScalarBarActor* cbar_actor = it->second.second;
	cbar_actor->SetLookupTable(prop->get_colormap()->color_table());

}

void Colorbar_controller::remove_color_bar(GsTL_vtkProp* prop){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it != colorbars_.end()) {
		it->second.second->Delete();
	}
	colorbars_.erase(it);
}

void Colorbar_controller::add_color_bar(GsTL_vtkProp* prop, bool is_visible){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it != colorbars_.end()) return;
	vtkScalarBarActor* cbar_actor = vtkScalarBarActor::New();
	cbar_actor->GetLabelTextProperty()->BoldOff();
	cbar_actor->GetLabelTextProperty()->ShadowOff();
	cbar_actor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	colorbars_[prop] = std::make_pair(is_visible,cbar_actor);

}

void Colorbar_controller::set_colorbar_visible(GsTL_vtkProp* prop, bool visibility){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return;
	it->second.second->SetVisibility(visibility);
}


void Colorbar_controller::set_colorbar_orientation(GsTL_vtkProp* prop, bool is_vertical){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return;
	vtkScalarBarActor* cbar_actor = it->second.second;
	if(is_vertical) {
		cbar_actor->GetPositionCoordinate()->SetValue(0.9,0.25);
		cbar_actor->SetOrientationToVertical();
		cbar_actor->SetWidth(0.12);
		cbar_actor->SetHeight(0.5);
		cbar_actor->SetTextPositionToSucceedScalarBar();
	}
	else {
		cbar_actor->SetOrientationToHorizontal();
		cbar_actor->GetPositionCoordinate()->SetValue(0.25,0.08);
		cbar_actor->SetWidth(0.5);
		cbar_actor->SetHeight(0.12);
		cbar_actor->SetTextPositionToPrecedeScalarBar();
	}
}

void Colorbar_controller::set_colorbar_width(GsTL_vtkProp* prop, float width){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return;
	it->second.second->SetWidth(width);
}

void Colorbar_controller::set_colorbar_height(GsTL_vtkProp* prop, float height){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return;
	it->second.second->SetHeight(height);
}

void Colorbar_controller::set_colorbars_text_color(bool is_background_white){
	prop_to_colorbarsT::iterator it = colorbars_.begin();
	for( ; it!= colorbars_.end(); ++it) {
		vtkScalarBarActor* cbar_actor = it->second.second;
		if(is_background_white)
			cbar_actor->GetLabelTextProperty()->SetColor(0,0,0);
		else
			cbar_actor->GetLabelTextProperty()->SetColor(1,1,1);
	}
}


void Colorbar_controller::set_number_of_ticks(GsTL_vtkProp* prop, int n_ticks){
	prop_to_colorbarsT::iterator it = colorbars_.find(prop);
	if(it == colorbars_.end()) return;
	vtkScalarBarActor* cbar_actor = it->second.second;
	cbar_actor->GetNumberOfLabels() != n_ticks;
	cbar_actor->SetNumberOfLabels(n_ticks);
}
