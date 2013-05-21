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
 * property_stats_view.cpp
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/gui/viewer/property_stats_view.h>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

Property_stats_view* get_stats_view(const Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop, QWidget *parent){
	if(prop->classname()=="Grid_categorical_property")
		return new Categorical_property_stats_view(prop,vtk_prop,parent);
	else if(prop->classname()=="Grid_continuous_property")
		return new Continuous_property_stats_view(prop,vtk_prop,parent);
	else
		return 0;
}

Property_stats_view::Property_stats_view(const Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop, QWidget *parent)
 : prop_(prop), vtk_prop_(vtk_prop), QWidget(parent)
{

}

Property_stats_view::~Property_stats_view() {
	// TODO Auto-generated destructor stub
}


Continuous_property_stats_view::
Continuous_property_stats_view(const Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop, QWidget *parent)
 : Property_stats_view(prop,vtk_prop,parent){

	Grid_continuous_property::const_iterator it1 = prop->begin(true);
	Grid_continuous_property::const_iterator it = prop_->begin(true);
	QVBoxLayout* layout = new QVBoxLayout(this);

	unsigned int n = 0;
	double s=0;
	double s2=0;
	for( ; it != prop_->end(); ++it, ++n) {
		float val = *it;
		s += val;
		s2 += val*val;
	}
	layout->addWidget(new QLabel(QString("Average : %1").arg(s/n),this));
	layout->addWidget(new QLabel(QString("Variance : %1").arg(s2/n - s*s/n/n),this));
	layout->addWidget(new QLabel(QString("Number of nan : %1").arg(prop_->size() - n),this));
	this->setLayout(layout);
}

Continuous_property_stats_view::~Continuous_property_stats_view(){}

Categorical_property_stats_view::
Categorical_property_stats_view(const Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop, QWidget *parent)
 : Property_stats_view(prop,vtk_prop,parent){

	const Grid_categorical_property* cprop_ = dynamic_cast<const Grid_categorical_property*>(prop_);
	Grid_continuous_property::const_iterator it = cprop_->begin(true);
	QVBoxLayout* layout = new QVBoxLayout(this);

  std::vector<int> codes = cprop_->get_category_id();
  int ncat = codes.size();
	//int ncat = cprop_->get_number_of_category();

  std::map<int, int> count;
  for(int i=0; i< ncat; ++i) {
    count[codes[i]] = 0;
  }

	//std::vector<int> count(ncat,0);
	unsigned int n = 0;
	for( ; it != prop_->end(); ++it, ++n) {
		int id = static_cast<int>(*it);
		if(id >= 0)  count[id]++;
	}
	QTableWidget* table = new QTableWidget(ncat,2,this);
	QStringList labels;
//	labels<<"Name"<<"%"<<"n";
	labels<<"%"<<"Number";
	table->setHorizontalHeaderLabels(labels);
	QStringList cat_names;
 
	for(int c=0; c<ncat; c++) {
		cat_names<<cprop_->get_category_definition()->get_category_name(codes[c]).c_str();
		table->setItem(c,0,new QTableWidgetItem(QString("%1").arg(static_cast<float>(count[codes[c]])/n)));
		table->setItem(c,1,new QTableWidgetItem(QString("%1").arg(count[codes[c]])));
	}
	table->setVerticalHeaderLabels(cat_names);
  table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	layout->addWidget(table);
  
	this->setLayout(layout);
}

Categorical_property_stats_view::~Categorical_property_stats_view(){}


