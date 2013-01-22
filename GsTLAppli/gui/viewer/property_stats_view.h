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
 * property_stats_view.h
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#ifndef PROPERTY_STATS_VIEW_H_
#define PROPERTY_STATS_VIEW_H_

#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>

#include <QWidget>

class Property_stats_view : public QWidget {
public:
	Property_stats_view(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop, QWidget *parent=0);
	virtual ~Property_stats_view();

protected :
	const GsTLGridProperty* prop_;
	GsTL_vtkProp* vtk_prop_;

};

class Continuous_property_stats_view : public Property_stats_view {
public:
	Continuous_property_stats_view(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop, QWidget *parent=0);
	virtual ~Continuous_property_stats_view();

};

class Categorical_property_stats_view : public Property_stats_view {
public:
	Categorical_property_stats_view(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop, QWidget *parent=0);
	virtual ~Categorical_property_stats_view();

};

Property_stats_view* get_stats_view(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop, QWidget *parent=0);

#endif /* PROPERTY_STATS_VIEW_H_ */
