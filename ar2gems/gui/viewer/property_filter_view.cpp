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
 * property_filter_view.cpp
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */


#include <gui/viewer/property_filter_view.h>
#include <appli/manager_repository.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>



Property_filter_threshold_continuous::Property_filter_threshold_continuous(int id, QWidget *parent)
 : QWidget(parent), id_(id)
{

	QHBoxLayout* layout = new QHBoxLayout(this);
	enabled_ = new QCheckBox(this);
	min_threshold_ = new QLineEdit("Min",this);
	max_threshold_ = new QLineEdit("Max",this);
//	QString min_default = QString("<span style= color:#ff0000;>%1</span>").arg("Min Included");
//	min_threshold_->setText(min_default);
//	//QString min_default("<font color=gray>Min Included</FONT>");
//	QString max_default("<font color=#ff0000;>Max Included</font>");

	remove_ = new QPushButton(this);
//	remove_->setIcon(QIcon::fromTheme("list-remove"));

  QIcon icon_delete;
  icon_delete.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
  remove_->setIcon(icon_delete);

	layout->addWidget(enabled_);
	layout->addWidget(min_threshold_);
	layout->addWidget(max_threshold_);
	layout->addWidget(remove_);

	QObject::connect(remove_,SIGNAL(clicked()), this,SLOT(forward_remove()));
	//QObject::connect(remove_,SIGNAL(clicked), this,SLOT(deleteLater()));


}
Property_filter_threshold_continuous::~Property_filter_threshold_continuous(){}

bool Property_filter_threshold_continuous::is_enabled(void){
	return enabled_->isChecked();

}

void Property_filter_threshold_continuous::set_enabled(bool on){
	enabled_->setChecked(on);
}


float Property_filter_threshold_continuous::min_threshold(){
	return min_threshold_->text().toFloat();
}
float Property_filter_threshold_continuous::max_threshold(){
	return max_threshold_->text().toFloat();
}


void Property_filter_threshold_continuous::set_min_threshold(float val){
	min_threshold_->setText(QString("%1").arg(val));
}
void Property_filter_threshold_continuous::set_max_threshold(float val){
	max_threshold_->setText(QString("%1").arg(val));
}

void Property_filter_threshold_continuous::forward_remove(){
	emit remove(this);
	emit deleteLater();
}

/*
 *
 */


Property_thresholds_continuous_control::
Property_thresholds_continuous_control(const Grid_continuous_property* prop,GsTL_vtkProp* vtk_prop,QWidget *parent)
 : Property_filter_view(prop, vtk_prop, parent),  threshold_counter_(0){

	QVBoxLayout *layout = new QVBoxLayout(this);

	QPushButton* add = new QPushButton(this);
//	add->setIcon(QIcon::fromTheme("edit-add"));
  QIcon icon_add;
  icon_add.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/add.png"), QSize(), QIcon::Normal, QIcon::Off);
  add->setIcon(icon_add);
	add->setToolTip("Add a new set of threshold");

	QPushButton* select_all = new QPushButton("Select All",this);
//	select_all->setIcon(QIcon::fromTheme("edit-select-all"));
  QIcon icon_select_all;
  icon_select_all.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/edit-select-all.png"), QSize(), QIcon::Normal, QIcon::Off);
  select_all->setIcon(icon_select_all);
	select_all->setToolTip("Select all thresholds");
	QPushButton* unselect_all= new QPushButton("Unselect All",this);
	unselect_all->setToolTip("Unselect all thresholds");

	QHBoxLayout *top_layout = new QHBoxLayout(this);
	top_layout->addWidget(add);
	top_layout->addStretch(2);
	top_layout->addWidget(select_all);
	top_layout->addWidget(unselect_all);

	layout->addLayout(top_layout);

	thresholds_layout_ = new QVBoxLayout(this);
	layout->addLayout(thresholds_layout_);
	//Create a default entry
	this->add_threshold();
//	Property_filter_threshold_continuous *threshold = new Property_filter_threshold_continuous(threshold_counter_,this);
//	threshold_counter_++;
//	thresholds_.append(threshold);
//	thresholds_layout_->addWidget(threshold);
//

	QPushButton* compute = new QPushButton("Display/Refresh",this);
	compute->setToolTip("Update the visualization with the selected thresolds");
	layout->addWidget(compute);

	QObject::connect(compute, SIGNAL(clicked()), this, SLOT(execute()));
	QObject::connect(add, SIGNAL(clicked()), this, SLOT(add_threshold()));
	QObject::connect(select_all, SIGNAL(clicked()), this, SLOT(select_all_categories()));
	QObject::connect(unselect_all, SIGNAL(clicked()), this, SLOT(unselect_all_categories()));


}
Property_thresholds_continuous_control::~Property_thresholds_continuous_control(){}

int Property_thresholds_continuous_control::number_of_enabled_threshold(void){
	int n = 0;
	for(int i=0;i<thresholds_.size(); ++i) {
		if(thresholds_.at(i)->is_enabled()) n++;
	}
	return n;
}
int Property_thresholds_continuous_control::number_of_threshold(void){
	return thresholds_.size();
}


bool Property_thresholds_continuous_control::is_thresholding_enabled(){
	for(int i=0;i<thresholds_.size(); ++i) {
		if(thresholds_.at(i)->is_enabled()) return true;
	}
	return false;
}


void Property_thresholds_continuous_control::add_threshold(){
	Property_filter_threshold_continuous *threshold = new Property_filter_threshold_continuous(threshold_counter_,this);
	threshold_counter_++;
	thresholds_.append(threshold);
	thresholds_layout_->addWidget(threshold);

	QObject::connect(threshold, SIGNAL( remove(Property_filter_threshold_continuous*) ),
					 this, SLOT(remove_threshold(Property_filter_threshold_continuous*) ) );

}
void Property_thresholds_continuous_control::remove_threshold(Property_filter_threshold_continuous* threshold){

	thresholds_layout_->removeWidget(threshold);
	thresholds_.removeOne(threshold);

	vtk_prop_->remove_thresholding(threshold->id());

	QObject::disconnect(threshold, SIGNAL( void remove(Property_filter_threshold_continuous*) ),
					 this, SLOT(remove_threshold(Property_filter_threshold_continuous*) ) );

}

void Property_thresholds_continuous_control::execute(){

	for(int i=0; i < thresholds_.size() ; ++i ) {
		Property_filter_threshold_continuous* threshold = thresholds_.at(i);
		vtk_prop_->update_thresholding(threshold->id(), threshold->min_threshold(),
									   threshold->max_threshold(), threshold->is_enabled());
	}
  emit this->rendering_modified();
}


void Property_thresholds_continuous_control::display(){

	for(int i=0; i < thresholds_.size() ; ++i ) {
		Property_filter_threshold_continuous* threshold = thresholds_.at(i);
		vtk_prop_->update_thresholding(threshold->id(), threshold->min_threshold(),
									   threshold->max_threshold(), threshold->is_enabled());
	}
  emit this->rendering_modified();
}
void Property_thresholds_continuous_control::undisplay(){
  emit this->rendering_modified();
	//vtk_prop_->clear_thresholding();
}


void Property_thresholds_continuous_control::select_all_categories(){
	for(int i=0; i < thresholds_.size() ; ++i ) {
		thresholds_.at(i)->set_enabled(true);
	}
}
void Property_thresholds_continuous_control::unselect_all_categories(){
	for(int i=0; i < thresholds_.size() ; ++i ) {
		thresholds_.at(i)->set_enabled(false);
	}

}

/*
 *   Property_filter_view
 */


Property_filter_view::Property_filter_view(const Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop,QWidget *parent)
 : prop_(prop), vtk_prop_(vtk_prop), QScrollArea(parent)
{
}

Property_filter_view::~Property_filter_view() {
	// TODO Auto-generated destructor stub
}




Property_thresholds_categorical_control::
Property_thresholds_categorical_control(const Grid_categorical_property* prop,GsTL_vtkProp* vtk_prop,QWidget *parent)
 : Property_filter_view(prop, vtk_prop, parent), cprop_(prop){

	QVBoxLayout *layout = new QVBoxLayout(this);


	QPushButton* select_all= new QPushButton("Select All",this);
	select_all->setIcon(QIcon::fromTheme("edit-select-all"));
	select_all->setToolTip("Select all thresholds");
	QPushButton* unselect_all= new QPushButton("Unselect All",this);
	unselect_all->setToolTip("Unselect all thresholds");

	QHBoxLayout *top_layout = new QHBoxLayout(this);
	top_layout->addStretch(2);
	top_layout->addWidget(select_all);
	top_layout->addWidget(unselect_all);

	layout->addLayout(top_layout);

  cdef_table_ = new CategoricalDefinitionTable(this);
  cdef_table_->initialize();
  cdef_table_->set_flags_name(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
  cdef_table_->set_flags_color(Qt::ItemIsEnabled );

  const CategoricalPropertyDefinition* cdef_const = cprop_->get_category_definition();

  //Need to a non-const cat definition
  Named_interface* ni = Root::instance()->interface(categoricalDefinition_manager+"/"+cdef_const->name()).raw_ptr();
  CategoricalPropertyDefinition* cdef = dynamic_cast<CategoricalPropertyDefinition*>(ni);
  
  CategoricalPropertyDefinitionName* cdef_named = dynamic_cast<CategoricalPropertyDefinitionName*>(cdef);
  if(cdef_named) {
    cdef_table_->show_definition(cdef_named);
  }
  else {
    cdef_table_->show_definition(cprop_->get_number_of_category());
  }
  layout->addWidget(cdef_table_);




  /*
	QVBoxLayout *thresholds_layout = new QVBoxLayout(this);
	layout->addLayout(thresholds_layout);

	int ncat = cprop_->get_number_of_category();
	for( int i=0; i< ncat; ++i ) {
		QCheckBox* thresold = new QCheckBox(cprop_->get_category_definition()->get_category_name(i).c_str(), this);
		thresholds_.append(thresold);
		thresholds_layout->addWidget(thresold);
	}
	thresholds_layout->addStretch();
  */


	QPushButton* compute = new QPushButton("Display/Refresh",this);
	compute->setToolTip("Update the visualization with the selected thresholds");
	layout->addWidget(compute);

	QObject::connect(compute, SIGNAL(clicked()), this, SLOT(execute()));
	QObject::connect(select_all, SIGNAL(clicked()), this, SLOT(select_all_categories()));
	QObject::connect(unselect_all, SIGNAL(clicked()), this, SLOT(unselect_all_categories()));

}

Property_thresholds_categorical_control::~Property_thresholds_categorical_control(){}

int Property_thresholds_categorical_control::number_of_enabled_threshold(void){
	int n = 0;
	for(int i=0;i<thresholds_.size(); ++i) {
		if(thresholds_.at(i)->isChecked()) n++;
	}
	return n;
}
int Property_thresholds_categorical_control::number_of_threshold(void){
  return  cdef_table_->model()->rowCount();
}


bool Property_thresholds_categorical_control::is_thresholding_enabled(){
  QAbstractItemModel* model = cdef_table_->model();

  int nrows = model->rowCount();
  for(int i=0; i<nrows; ++i) {
    QModelIndex index = model->index(i,0);
    if ( model->data(index,Qt::CheckStateRole).toBool() ) return true;
  }

//	for(int i=0;i<thresholds_.size(); ++i) {
//		if(thresholds_.at(i)->isChecked()) return true;
//	}
	return false;
}


void Property_thresholds_categorical_control::execute(){


  // May need to update the displays
  if( cdef_table_->get_definition_name().toStdString() != cprop_->get_category_definition()->name()) {

    cdef_table_->show_definition(cprop_->get_category_definition()->name().c_str());


  }


  QAbstractItemModel* model = cdef_table_->model();

  const CategoricalPropertyDefinition* cdef = cprop_->get_category_definition();

  int nrows = model->rowCount();
  for(int i=0; i<nrows; ++i) {
    QModelIndex index = model->index(i,0);
    int cat_id = cdef->category_id_from_index( i );
    vtk_prop_->update_thresholding(i, cat_id, cat_id, model->data(index,Qt::CheckStateRole).toBool() );
  }

//	for(int i=0; i < thresholds_.size() ; ++i ) {
//		vtk_prop_->update_thresholding(i, i, i, thresholds_.at(i)->isChecked() );
//	}
  emit this->rendering_modified();
}


void Property_thresholds_categorical_control::display(){

  QAbstractItemModel* model = cdef_table_->model();
  const CategoricalPropertyDefinition* cdef = cprop_->get_category_definition();

  int nrows = model->rowCount();
  for(int i=0; i<nrows; ++i) {
    QModelIndex index = model->index(i,0);
    int cat_id = cdef->category_id_from_index( i );
    vtk_prop_->update_thresholding(i, cat_id, cat_id, model->data(index,Qt::CheckStateRole).toBool() );
  }

//	for(int i=0; i < thresholds_.size() ; ++i ) {
//		vtk_prop_->update_thresholding(i, i, i, thresholds_.at(i)->isChecked() );
//	}
  emit this->rendering_modified();

}
void Property_thresholds_categorical_control::undisplay(){
  emit this->rendering_modified();
	//vtk_prop_->clear_thresholding();
}

void Property_thresholds_categorical_control::select_all_categories(){

  QAbstractItemModel* model = cdef_table_->model();

  int nrows = model->rowCount();
  for(int i=0; i<nrows; ++i) {
    QModelIndex index = model->index(i,0);
    model->setData(index, QVariant(true) ,Qt::CheckStateRole);
    cdef_table_->update(index);
  }

	//for(int i=0; i < thresholds_.size() ; ++i ) {
	//	thresholds_.at(i)->setChecked(true);
	//}
}
void Property_thresholds_categorical_control::unselect_all_categories(){

  QAbstractItemModel* model = cdef_table_->model();

  int nrows = model->rowCount();
  for(int i=0; i<nrows; ++i) {
    QModelIndex index = model->index(i,0);
    model->setData(index, QVariant(false) ,Qt::CheckStateRole);
    cdef_table_->update(index);
  }

//	for(int i=0; i < thresholds_.size() ; ++i ) {
//		thresholds_.at(i)->setChecked(false);
//	}
}
