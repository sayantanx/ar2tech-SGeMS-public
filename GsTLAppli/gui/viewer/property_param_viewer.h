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
 * property_param_viewer.h
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#ifndef PROPERTY_PARAM_VIEWER_H_
#define PROPERTY_PARAM_VIEWER_H_

#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
#include <GsTLAppli/gui/viewer/object_property_viewer.h>
#include <GsTLAppli/gui/viewer/colormap_controller.h>
#include <GsTLAppli/gui/viewer/property_filter_view.h>
#include <GsTLAppli/gui/viewer/property_stats_view.h>
//#include <GsTLAppli/gui/viewer/property_section_view.h>
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/viewer/visualization_parameters.h>
#include <QWidget>
#include <QTabWidget>
 //#include <QGroupBox>

class GUI_DECL Property_param_viewer : public QWidget {

	Q_OBJECT
public:

//	static Named_interface* create_new_interface(std::string&);

	Property_param_viewer();
	Property_param_viewer(GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop);
	virtual ~Property_param_viewer();

	virtual void initialize(GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop);

	const Geostat_grid* grid();
	QString grid_name() const;

// Project View interface
	virtual void update(std::string obj);
	virtual void new_object(std::string obj);
	virtual void deleted_object(std::string obj);

public slots :
	void display();
	void undisplay();

  void update_colormap_display();

//	QString get_colormap

signals :
	void colormap_changed( const Colormap* );
  void rendering_modified();

protected :
	virtual void init_color_page();
	virtual void init_filtering_page();
	virtual void init_stats_page();
//  virtual void init_section_page();

protected :
	const Geostat_grid* grid_;
	const GsTLGridProperty *prop_;
	const GsTLGridCategoricalProperty *cprop_;

	GsTL_vtkProp* vtk_prop_;

	Colormap_controller *color_;
	Property_filter_view *threshold_;
//  Property_section_view* section_;

protected :
	QString current_prop_name_;
	QTabWidget *tab_;
	bool is_displayed_;


};


class GUI_DECL Property_viz_parameters : public Visualization_parameters {
public :

	static Named_interface* create_new_interface(std::string&);

	Property_viz_parameters();
	~Property_viz_parameters();
	void initialize( GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop);

	virtual void display();
	virtual void undisplay();

	GsTLGridProperty* property();

	virtual QWidget* interface();


protected :
	GsTLGridProperty* prop_;
	Property_param_viewer* viewer_;
};


#endif /* PROPERTY_PARAM_VIEWER_H_ */
