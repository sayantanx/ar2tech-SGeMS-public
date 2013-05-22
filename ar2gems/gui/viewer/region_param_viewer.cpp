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
 * region_param_viewer.cpp
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */


#include <gui/viewer/region_param_viewer.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>


//Named_interface* Region_param_viewer::create_new_interface(std::string&){
//	return new Region_param_viewer;
//}

Region_param_viewer::Region_param_viewer() : QWidget() {

}

Region_param_viewer::
Region_param_viewer(Grid_region* region, GsTL_vtkProp* vtk_prop) : QWidget(){
	this->initialize(region, vtk_prop);

}

Region_param_viewer::~Region_param_viewer() {
	if( vtk_prop_->current_region() == region_ ) {
		vtk_prop_->set_region("");
	}
	// TODO Auto-generated destructor stub
}

void Region_param_viewer::initialize(Grid_region* region, GsTL_vtkProp* vtk_prop)
{
	vtk_prop_ = vtk_prop;
	grid_ = vtk_prop_->grid();
	region_ = region;

	int active_cell = region_->active_size();
	QString info_abs = QString("# of active : %1").arg(active_cell);
	QString info_per = QString("%% of active : %1").arg((float)active_cell/region_->size()*100 );

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel(info_abs, this));
	layout->addWidget(new QLabel(info_per, this));
	layout->addStretch();
	this->setLayout(layout);

	//this->layout()->addWidget(new QLabel(info_abs, this));
	//this->layout()->addWidget(new QLabel(info_per, this));
}


void Region_param_viewer::update(std::string obj){
	// If this object had been modified, check to see if one property had been deleted
	if( obj != grid_->name() ) return;

// I believe that there is nothing to be done. since it should be deleted by an external manager.
// cannot self-delete (unfortunately!)
}

void Region_param_viewer::new_object(std::string obj){
	//Nothing to be done.
}
void Region_param_viewer::deleted_object(std::string obj){
	//Nothing to be done.
}


/*
 *   Region_viz_parameters
 */
Named_interface* Region_viz_parameters::create_new_interface(std::string&){
	return new Region_viz_parameters;

}

Region_viz_parameters::Region_viz_parameters() : Visualization_parameters(), viewer_(0){

}

Region_viz_parameters::~Region_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
}


void Region_viz_parameters::initialize( Grid_region* region, GsTL_vtkProp* vtk_prop){
	region_ = region;
	vtk_prop_ = vtk_prop;
	viewer_ = new Region_param_viewer(region, vtk_prop);
}
QWidget* Region_viz_parameters::interface(){
	return viewer_;
}

Grid_region* Region_viz_parameters::region(){
	return region_;

}


void Region_viz_parameters::display(){
  vtk_prop_->set_visibility(true);
	vtk_prop_->set_region(region_->name());
}

void Region_viz_parameters::undisplay(){
	vtk_prop_->set_region("");
}
