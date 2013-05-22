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



/*
 * group_param_viewer.cpp
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/gui/viewer/group_param_viewer.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>


//Named_interface* Group_param_viewer::create_new_interface(std::string&){
//	return new Group_param_viewer;
//}

Group_param_viewer::Group_param_viewer()
 : QWidget(0),vtk_prop_(0), grid_(0){

	this->setLayout(new QVBoxLayout(this));

}

Group_param_viewer::Group_param_viewer(GsTLGridPropertyGroup* group, GsTL_vtkProp* vtk_prop)
 : QWidget(0), vtk_prop_(0), grid_(0){

	this->setLayout(new QVBoxLayout(this));
	this->initialize(group,vtk_prop);

}

Group_param_viewer::~Group_param_viewer() {
	// TODO Auto-generated destructor stub
}

void Group_param_viewer::initialize(GsTLGridPropertyGroup* group, GsTL_vtkProp* vtk_prop){
	vtk_prop_ = vtk_prop;
	grid_ = vtk_prop_->grid();
	group_ = group;

	QString type = QString("Group Type: %1").arg(group_->type().c_str());
	this->layout()->addWidget(new QLabel(type, this));
}


void Group_param_viewer::update(std::string obj){
	// If this object had been modified, check to see if one property had been deleted
	if( obj != grid_->name() ) return;

// I believe that there is nothing to be done. since it should be deleted by an external manager.
// cannot self-delete (unfortunately!)
}

void Group_param_viewer::new_object(std::string obj){
	//Nothing to be done.
}
void Group_param_viewer::deleted_object(std::string obj){
	//Nothing to be done.
}


/*
 *   Group_viz_parameters
 */
Named_interface* Group_viz_parameters::create_new_interface(std::string&){
	return new Group_viz_parameters;

}

Group_viz_parameters::Group_viz_parameters() : Visualization_parameters(), viewer_(0){

}

Group_viz_parameters::~Group_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
}


void Group_viz_parameters::initialize( GsTLGridPropertyGroup* group, GsTL_vtkProp* vtk_prop){
	group_ = group;
	vtk_prop_=vtk_prop;
	viewer_ = new Group_param_viewer(group, vtk_prop);
}
QWidget* Group_viz_parameters::interface(){
	return viewer_;
}
