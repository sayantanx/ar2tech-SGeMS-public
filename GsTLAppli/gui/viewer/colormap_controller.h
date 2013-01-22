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
 * colormap_controller.h
 *
 *  Created on: May 19, 2011
 *      Author: aboucher
 */

#ifndef COLORMAP_CONTROLLER_H_
#define COLORMAP_CONTROLLER_H_

#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>
#include <GsTLAppli/extra/qtplugins/categorical_selectors.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>

#include <Qt>
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSlider>

class Colormap_controller : public QWidget{

	Q_OBJECT
public:
	Colormap_controller(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop,QWidget *parent=0);
	virtual ~Colormap_controller();

	virtual Colormap* colormap() = 0;

public slots:


signals:
	void display_modified();
  void colormap_changed( const Colormap* );

protected :
	const GsTLGridProperty* prop_;
	GsTL_vtkProp* vtk_prop_;

};

class Colormap_continuous_controller : public Colormap_controller{

	Q_OBJECT
public:
	Colormap_continuous_controller(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop,QWidget *parent=0);
	virtual ~Colormap_continuous_controller();
  virtual Colormap* colormap(){return cmap_;}
	bool is_log_scale();

public slots:

	void set_colormap_min_value();
	void set_colormap_max_value();
//	void recompute_cmap_bounds();
	void enable_log_scale();

	void set_log_scale(bool on);

signals :
    void colormap_changed( const Colormap* );

protected slots:
	virtual void create_colormap();
	virtual void set_colorscale( const QString& colorscale_name );
	virtual void set_alpha( int alpha );
  bool reset_min_max();

protected :
	
  void init_colormap_selector();

protected :

	QComboBox *colormap_selector_;
	QSlider* alpha_slider_;  
  QLineEdit* min_cmap_value_edit_;
	QLineEdit* max_cmap_value_edit_;
	QPushButton *reset_cmap_bounds_button_;
	QCheckBox *log_scale_checkbox_;

  Colormap_continuous* cmap_;

};

class Colormap_categorical_controller : public Colormap_controller{

	Q_OBJECT
public:
	Colormap_categorical_controller(const GsTLGridCategoricalProperty* prop, GsTL_vtkProp* vtk_prop,QWidget *parent=0);
	virtual ~Colormap_categorical_controller();

  virtual Colormap* colormap() {return cmap_;}

protected slots:
//	virtual void create_colormap();
  virtual void update_colormap();

signals :
    void colormap_changed( const Colormap* );

protected :
	const GsTLGridCategoricalProperty* cprop_;
  CategoricalDefinitionTable* cdef_table_;
  QPushButton* display_button_;

  Colormap_categorical* cmap_;

};

#endif /* COLORMAP_CONTROLLER_H_ */
