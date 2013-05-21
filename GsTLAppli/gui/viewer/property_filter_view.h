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
 * property_filter_view.h
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#ifndef PROPERTY_FILTER_VIEW_H_
#define PROPERTY_FILTER_VIEW_H_



#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
#include <GsTLAppli/qtplugins/categorical_selectors.h>

#include <QWidget>
#include <QCheckBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>


class Property_filter_threshold_continuous: public QWidget {

	Q_OBJECT
public:
	Property_filter_threshold_continuous(int id, QWidget *parent = 0);
	virtual ~Property_filter_threshold_continuous();

	bool is_enabled();
	float min_threshold();
	float max_threshold();

	int id(){return id_;}

public slots :
	void set_enabled(bool on);
	void set_min_threshold(float val);
	void set_max_threshold(float val);
	void forward_remove();

signals:
	void remove(Property_filter_threshold_continuous*);

protected :
	int id_;
	QCheckBox* enabled_;
	QLineEdit* min_threshold_;
	QLineEdit* max_threshold_;
	QPushButton *remove_;
};



class Property_filter_view: public QScrollArea {

	Q_OBJECT
public:
	Property_filter_view( const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop,QWidget *parent = 0);
	virtual ~Property_filter_view();

	virtual int number_of_enabled_threshold(void)=0;
	virtual int number_of_threshold(void)=0;
	virtual bool is_thresholding_enabled()=0;

	virtual void display()=0;
	virtual void undisplay()=0;

signals :
  void rendering_modified();

protected :
	const GsTLGridProperty* prop_;
	GsTL_vtkProp* vtk_prop_;

};



class Property_thresholds_continuous_control: public Property_filter_view {

	Q_OBJECT
public:
	Property_thresholds_continuous_control(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop, QWidget *parent = 0);
	virtual ~Property_thresholds_continuous_control();

	virtual int number_of_enabled_threshold(void);
	virtual int number_of_threshold(void);
	virtual bool is_thresholding_enabled();

	virtual void display();
	virtual void undisplay();

public slots :
	void add_threshold();
	void remove_threshold(Property_filter_threshold_continuous* threshold);


protected slots :
	void execute();
protected slots:
	void select_all_categories();
	void unselect_all_categories();

protected :

	int threshold_counter_;

	QList<Property_filter_threshold_continuous*> thresholds_;
	QVBoxLayout *thresholds_layout_;

	/*
	QPushButton* compute_;
	QPushButton* add_;
	QPushButton* select_all_;
	QPushButton* unselect_all_;
	*/

};



class Property_thresholds_categorical_control: public Property_filter_view {

	Q_OBJECT
public:
	Property_thresholds_categorical_control(const GsTLGridCategoricalProperty* prop, GsTL_vtkProp* vtk_prop, QWidget *parent = 0);
	virtual ~Property_thresholds_categorical_control();

	virtual int number_of_enabled_threshold(void);
	virtual int number_of_threshold(void);
	virtual bool is_thresholding_enabled();

public slots:
	virtual void display();
	virtual void undisplay();
	virtual void execute();

protected slots:
	void select_all_categories();
	void unselect_all_categories();

protected :

	const GsTLGridCategoricalProperty* cprop_;
  CategoricalDefinitionTable* cdef_table_;
	QList<QCheckBox*> thresholds_;
/*
	QPushButton* compute_;
	QPushButton* select_all_;
	QPushButton* unselect_all_;
*/
};


#endif /* PROPERTY_FILTER_VIEW_H_ */
