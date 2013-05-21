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




#include <GsTLAppli/charts/scatter_plot_model_item.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>
#include <QtAlgorithms>


 bool scatter_plot_item_less_than(const Scatter_plot_item *item1, const Scatter_plot_item *item2)
 {
   return item1->operator<(*item2);
 }


Scatter_plot_property_item::Scatter_plot_property_item( GsTLGridProperty* prop_x,GsTLGridProperty* prop_y, int& id, Scatter_plot_item* parent)
  : Scatter_plot_item(id), prop_x_(prop_x), prop_y_(prop_y), parent_(parent), weights_(0), region_(0)
{
  grid_ = dynamic_cast< Geostat_grid*>(prop_x_->parent()->parent());
  grid_name_ = grid_->item_name();
  item_name_x_  = QString::fromStdString(prop_x_->name());
  item_name_y_  = QString::fromStdString(prop_y_->name());

  //display_format_ = "Bars";
}



Scatter_plot_property_group_item::Scatter_plot_property_group_item( GsTLGridProperty* prop_x,GsTLGridPropertyGroup* group_y, int& id)
  : Scatter_plot_item(id), prop_x_(prop_x), group_y_(group_y), weights_(0), region_(0) 
{

  if(group_y->empty())  return;
  if(prop_x->parent()->parent() !=  group_y->parent()  )  return;

  grid_ = dynamic_cast< Geostat_grid*>(group_y_->parent());
  grid_name_ = grid_->item_name();
  item_name_x_  = prop_x_->item_name();
  item_name_y_  = group_y_->item_name();



  //Build the list of property
  GsTLGridPropertyGroup::property_map::const_iterator it_y=  group_y_->begin_property();
  for( ; it_y!= group_y_->end_property(); ++it_y) {
    if( prop_x_ == it_y->second ) continue;
    ++id;
    prop_items_.insert( new Scatter_plot_property_item(prop_x_, it_y->second, id, this)  );
  }

  //this->display_format("Lines");
  
}

Scatter_plot_property_group_item::~Scatter_plot_property_group_item() {

  std::set<Scatter_plot_property_item*>::iterator it = prop_items_.begin();

  for( ; it != prop_items_.end(); ++it ) {
    delete *it;
  }

}

Scatter_plot_item* Scatter_plot_property_group_item::children(int row) {
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for(int i=0 ; i<row ;++i,++it  ){}
  return *it;
}

int Scatter_plot_property_group_item::row_for_children(Scatter_plot_item* item) const{
  Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(item);
  if(prop_item==0) return -1;
  std::set< Scatter_plot_property_item*>::const_iterator it = prop_items_.find(prop_item);
  if(it==prop_items_.end()) return -1;
  return std::distance(prop_items_.begin(), it);
}

void Scatter_plot_property_group_item::color(const QColor &color) {
  color_ = color;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->color(color);
  }
}

void Scatter_plot_property_group_item::display_format(QString display_format) {
  display_format_ = display_format;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->display_format(display_format);
  }
}

void Scatter_plot_property_group_item::set_visibility( bool on ) {
  is_visible_ = on;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->set_visibility(on);
  }
}


void Scatter_plot_property_group_item::weights(GsTLGridWeightProperty* weights){
  weights_ = weights;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->weights(weights);
  }
}

void Scatter_plot_property_group_item::categorical_property(GsTLGridCategoricalProperty* cprop){
  cprop_ = cprop;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->categorical_property(cprop_);
  }
}

void Scatter_plot_property_group_item::region(Grid_region* region){
  region_ = region;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->region(region);
  }
}

void Scatter_plot_property_group_item::weights(QString weights_name){
  weights_ = grid_->weight_property(weights_name.toStdString());
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->weights(weights_);
  }
}


void Scatter_plot_property_group_item::categorical_property(QString cprop_name){
  cprop_ = grid_->categorical_property(cprop_name.toStdString());
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->categorical_property(cprop_);
  }
}


void Scatter_plot_property_group_item::region(QString region_name){
  region_ = grid_->region(region_name.toStdString());
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->region(region_);
  }
}




/*
 ---------------------
 */

Scatter_plot_group_item::Scatter_plot_group_item( GsTLGridPropertyGroup* group_x,GsTLGridPropertyGroup* group_y, int& id)
  : Scatter_plot_item(id), group_x_(group_x), group_y_(group_y), weights_(0), region_(0) 
{

  if(group_x_->size() != group_y_->size())  return;

  grid_ = dynamic_cast< Geostat_grid*>(group_x_->parent());
  grid_name_ = grid_->item_name();
  item_name_x_  = group_x_->item_name();
  item_name_y_  = group_y_->item_name();



  //Build the list of property
  GsTLGridPropertyGroup::property_map::const_iterator it_x=  group_x_->begin_property();
  GsTLGridPropertyGroup::property_map::const_iterator it_y=  group_y_->begin_property();
  for( ; it_x!= group_x_->end_property(); ++it_x, ++it_y) {
    ++id;
    prop_items_.insert( new Scatter_plot_property_item(it_x->second, it_y->second, id, this)  );
  }

  //this->display_format("Lines");
  
}

Scatter_plot_group_item::~Scatter_plot_group_item() {

  std::set<Scatter_plot_property_item*>::iterator it = prop_items_.begin();

  for( ; it != prop_items_.end(); ++it ) {
    delete *it;
  }

}

Scatter_plot_item* Scatter_plot_group_item::children(int row) {
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for(int i=0 ; i<row ;++i,++it  ){}
  return *it;
}

int Scatter_plot_group_item::row_for_children(Scatter_plot_item* item) const{
  Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(item);
  if(prop_item==0) return -1;
  std::set< Scatter_plot_property_item*>::const_iterator it = prop_items_.find(prop_item);
  if(it==prop_items_.end()) return -1;
  return std::distance(prop_items_.begin(), it);
}

void Scatter_plot_group_item::color(const QColor &color) {
  color_ = color;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->color(color);
  }
}

void Scatter_plot_group_item::display_format(QString display_format) {
  display_format_ = display_format;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->display_format(display_format);
  }
}

void Scatter_plot_group_item::set_visibility( bool on ) {
  is_visible_ = on;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->set_visibility(on);
  }
}


void Scatter_plot_group_item::weights(GsTLGridWeightProperty* weights){
  weights_ = weights;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->weights(weights);
  }
}

void Scatter_plot_group_item::categorical_property(GsTLGridCategoricalProperty* cprop){
  cprop_ = cprop;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->categorical_property(cprop_);
  }
}

void Scatter_plot_group_item::region(Grid_region* region){
  region_ = region;
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->region(region);
  }
}

void Scatter_plot_group_item::weights(QString weights_name){
  weights_ = grid_->weight_property(weights_name.toStdString());
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->weights(weights_);
  }
}


void Scatter_plot_group_item::categorical_property(QString cprop_name){
  cprop_ = grid_->categorical_property(cprop_name.toStdString());
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->categorical_property(cprop_);
  }
}


void Scatter_plot_group_item::region(QString region_name){
  region_ = grid_->region(region_name.toStdString());
  std::set< Scatter_plot_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->region(region_);
  }
}

