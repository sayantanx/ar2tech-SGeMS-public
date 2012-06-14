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
 * param_viewer.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/gui/viewer/visualization_parameters.h>

//Param_viewer::Param_viewer() :QWidget(),Named_interface() {
Visualization_parameters::Visualization_parameters() : Named_interface(), vtk_prop_(0) {
	// TODO Auto-generated constructor stub

}

Visualization_parameters::~Visualization_parameters() {
	// TODO Auto-generated destructor stub
}

GsTL_vtkProp* Visualization_parameters::vtk_prop(){
	return vtk_prop_;
}

Geostat_grid* Visualization_parameters::grid(){
  return vtk_prop_->grid();
}
