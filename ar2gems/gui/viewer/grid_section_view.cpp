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


#include <GsTLAppli/gui/viewer/grid_section_view.h>
#include <GsTLAppli/appli/manager_repository.h>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QSlider>
#include <QComboBox>



Single_section_widget::Single_section_widget(int id, QString orientation, GsTL_vtkProp* vtk_prop, QWidget *parent)
 : QWidget(parent), id_(id), vtk_prop_(vtk_prop)
{
  this->setMinimumHeight(3);

  int max_extent = vtk_prop->add_section( id, orientation, true );

	enabled_ = new QCheckBox(this);
  enabled_->setChecked(true);
  QLabel* section_orientation = new QLabel(orientation, this);

	section_spin_ = new QSpinBox(this);
	section_spin_->setRange(1,max_extent);
	section_spin_->setValue(1);
	QSlider* section_slider = new QSlider(Qt::Horizontal, this);
	section_slider->setRange(1,max_extent);
	section_slider->setValue(1);
	section_slider->setTracking(false);

	remove_ = new QPushButton(this);

  QIcon icon_delete;
  icon_delete.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
  remove_->setIcon(icon_delete);

  QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(enabled_);
  layout->addWidget(section_orientation);
	layout->addWidget(section_slider);
	layout->addWidget(section_spin_);
	layout->addWidget(remove_);
  this->setLayout(layout);


  QObject::connect(enabled_, SIGNAL(toggled(bool)),
			this, SLOT(enable_section(bool)));

	QObject::connect(section_spin_, SIGNAL(valueChanged(int)),
			section_slider, SLOT(setValue(int)));
	QObject::connect(section_slider, SIGNAL(valueChanged(int)),
			section_spin_, SLOT(setValue(int)));

	QObject::connect(section_slider,SIGNAL(valueChanged(int)), this, SLOT(set_section_step(int)));

	QObject::connect(remove_,SIGNAL(clicked()), this,SLOT(forward_remove()));


}
Single_section_widget::~Single_section_widget(){}

bool Single_section_widget::is_enabled(void){
	return enabled_->isChecked();

}

void Single_section_widget::set_enabled(bool on){
	enabled_->setChecked(on);
  
}

void Single_section_widget::enable_section(bool on){
	if(on) vtk_prop_->enable_section(id_);
  else vtk_prop_->disable_section(id_);
  emit this->rendering_modified();
  
}




int Single_section_widget::section_step(){
	return section_spin_->value();
}

void Single_section_widget::set_section_step(int step){
	section_spin_->setValue(step);

  if(this->is_enabled()) {
    vtk_prop_->update_section(id_, step, this->is_enabled());
    emit this->rendering_modified();
  }

}


void Single_section_widget::forward_remove(){
	emit remove(this);
	emit deleteLater();
}

/*
 *
 */


Grid_section_view::
Grid_section_view(GsTL_vtkProp* vtk_prop,QWidget *parent)
 : QScrollArea(parent), vtk_prop_(vtk_prop), section_counter_(0){

	QVBoxLayout *layout = new QVBoxLayout(this);

	QPushButton* add = new QPushButton(this);
//	add->setIcon(QIcon::fromTheme("edit-add"));
  QIcon icon_add;
  icon_add.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/add.png"), QSize(), QIcon::Normal, QIcon::Off);
  add->setIcon(icon_add);
	add->setToolTip("Add a new section");
  orientation_selector_ = new QComboBox(this);
  orientation_selector_->addItem("X");
  orientation_selector_->addItem("Y");
  orientation_selector_->addItem("Z");



	QPushButton* select_all = new QPushButton("Select All",this);
//	select_all->setIcon(QIcon::fromTheme("edit-select-all"));
  QIcon icon_select_all;
  icon_select_all.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/edit-select-all.png"), QSize(), QIcon::Normal, QIcon::Off);
  select_all->setIcon(icon_select_all);
	select_all->setToolTip("Select all thresholds");
	QPushButton* unselect_all= new QPushButton("Unselect All",this);
	unselect_all->setToolTip("Unselect all thresholds");

	QHBoxLayout *top_layout = new QHBoxLayout(this);
	top_layout->addWidget(orientation_selector_);
  top_layout->addWidget(add);
	top_layout->addStretch(2);
	top_layout->addWidget(select_all);
	top_layout->addWidget(unselect_all);

	layout->addLayout(top_layout);

	section_layout_ = new QVBoxLayout(this);
  section_layout_->setSizeConstraint(QLayout::SetMinimumSize);


  QFrame* frame = new QFrame(this);
  frame->setLayout(section_layout_);
  frame->setMinimumHeight(1);
  QScrollArea* scroll_area = new QScrollArea(this);
  scroll_area->setWidget(frame);
  scroll_area->setWidgetResizable(true);
  frame->show();
  scroll_area->show();
  scroll_area->setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded );

  layout->addWidget(scroll_area);

//  layout->addLayout(section_layout_);
  this->setLayout(layout);

	


	QObject::connect(add, SIGNAL(clicked()), this, SLOT(add_section()));
	QObject::connect(select_all, SIGNAL(clicked()), this, SLOT(select_all_sections()));
	QObject::connect(unselect_all, SIGNAL(clicked()), this, SLOT(unselect_all_sections()));


}
Grid_section_view::~Grid_section_view(){}

int Grid_section_view::number_of_enabled_sections(void){
	int n = 0;
	for(int i=0;i<sections_.size(); ++i) {
		if(sections_.at(i)->is_enabled()) n++;
	}
	return n;
}
int Grid_section_view::number_of_sections(void){
	return sections_.size();
}


bool Grid_section_view::is_section_enabled(){
	for(int i=0;i<sections_.size(); ++i) {
		if(sections_.at(i)->is_enabled()) return true;
	}
	return false;
}


void Grid_section_view::add_section(){

  QString orientation = orientation_selector_->currentText();
	Single_section_widget *section = new Single_section_widget(section_counter_, orientation,vtk_prop_ ,this);
	section_counter_++;
	sections_.append(section);
	section_layout_->addWidget(section);

  QObject::connect(section, SIGNAL( rendering_modified() ),
					 this, SIGNAL( rendering_modified() ) );

	QObject::connect(section, SIGNAL( remove(Single_section_widget*) ),
					 this, SLOT(remove_section(Single_section_widget*) ) );

  emit this->rendering_modified();

}

void Grid_section_view::remove_section(Single_section_widget* section){

	section_layout_->removeWidget(section);
	sections_.removeOne(section);

	vtk_prop_->remove_section(section->id());

	QObject::disconnect(section, SIGNAL( void remove(Single_section_widget*) ),
					 this, SLOT(remove_threshold(Single_section_widget*) ) );

  emit this->rendering_modified();

}


void Grid_section_view::display(){


  emit this->rendering_modified();
}
void Grid_section_view::undisplay(){
  emit this->rendering_modified();
	//vtk_prop_->clear_thresholding();
}


void Grid_section_view::select_all_sections(){
	for(int i=0; i < sections_.size() ; ++i ) {
		sections_.at(i)->set_enabled(true);
	}
  emit this->rendering_modified();
}
void Grid_section_view::unselect_all_sections(){
	for(int i=0; i < sections_.size() ; ++i ) {
		sections_.at(i)->set_enabled(false);
	}
  emit this->rendering_modified();

}


