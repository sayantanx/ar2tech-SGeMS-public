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
 * property_filter_view.h
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#ifndef PROPERTY_SECTION_VIEW_H_
#define PROPERTY_SECTION_VIEW_H_



#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>


#include <QWidget>
#include <QCheckBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <qComboBox>


class Single_section_widget: public QWidget {

	Q_OBJECT
public:
	Single_section_widget(int id, QString orientation, GsTL_vtkProp* vtk_prop,  QWidget *parent = 0);
	virtual ~Single_section_widget();

	bool is_enabled();
	int section_step();
	
	int id(){return id_;}

public slots :
	void set_enabled(bool on);
	void set_section_step(int step);

  void enable_section(bool on);
	
	void forward_remove();

signals:
	void remove(Single_section_widget*);
  void rendering_modified();

protected :
	int id_;
	QCheckBox* enabled_;
  QSpinBox *section_spin_;
	QPushButton *remove_;

  GsTL_vtkProp* vtk_prop_;
};



class Property_section_view: public QScrollArea {

	Q_OBJECT
public:
	Property_section_view( const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop,QWidget *parent = 0);
	virtual ~Property_section_view();

	virtual int number_of_enabled_sections(void);
	virtual int number_of_sections(void);
	virtual bool is_section_enabled();

	virtual void display();
	virtual void undisplay();

public slots :
	void add_section();
	void remove_section(Single_section_widget* section);

signals :
  void rendering_modified();

protected slots:
  void select_all_sections();
  void unselect_all_sections();


protected :
	const GsTLGridProperty* prop_;
	GsTL_vtkProp* vtk_prop_;


	int section_counter_;
  QComboBox* orientation_selector_;

	QList<Single_section_widget*> sections_;
	QVBoxLayout *section_layout_;



};



#endif /* PROPERTY_FILTER_VIEW_H_ */
