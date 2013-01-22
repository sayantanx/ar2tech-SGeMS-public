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
 * region_param_viewer.h
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#ifndef REGION_PARAM_VIEWER_H_
#define REGION_PARAM_VIEWER_H_

#include <Qt>
#include <QWidget>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
#include <GsTLAppli/gui/viewer/visualization_parameters.h>

class GUI_DECL Region_param_viewer : public QWidget {
public:

//	static Named_interface* create_new_interface(std::string&);

	Region_param_viewer();
	Region_param_viewer(GsTLGridRegion* region, GsTL_vtkProp* vtk_prop);
	virtual ~Region_param_viewer();

	void initialize(GsTLGridRegion* region, GsTL_vtkProp* vtk_prop);

// Project View interface
	virtual void update(std::string obj);
	virtual void new_object(std::string obj);
	virtual void deleted_object(std::string obj);

protected :
	const Geostat_grid* grid_;
	GsTLGridRegion* region_;
	GsTL_vtkProp* vtk_prop_;
};

/*
 *
 */

class GUI_DECL Region_viz_parameters : public Visualization_parameters {
public :

	static Named_interface* create_new_interface(std::string&);

	Region_viz_parameters();
	~Region_viz_parameters();
	void initialize( GsTLGridRegion* region, GsTL_vtkProp* vtk_prop);

	GsTLGridRegion* region();

	virtual void display();
	virtual void undisplay();
	virtual QWidget* interface();

protected :
	GsTLGridRegion* region_;
	Region_param_viewer* viewer_;
};

#endif /* REGION_PARAM_VIEWER_H_ */
