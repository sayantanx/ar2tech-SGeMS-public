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




#include <GsTLAppli/charts/histogram_model_item.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>
#include <QtAlgorithms>


 bool histogram_item_less_than(const Histogram_item *item1, const Histogram_item *item2)
 {
   return item1->operator<(*item2);
 }


Histogram_property_item::Histogram_property_item( GsTLGridProperty* prop, int& id, Histogram_item* parent)
  : Histogram_item(id), prop_(prop), parent_(parent), weights_(0), region_(0)
{
  grid_ = dynamic_cast< Geostat_grid*>(prop->parent()->parent());
  grid_name_ = grid_->item_name();
  item_name_  = QString::fromStdString(prop_->name());

  display_format_ = "Bars";
}


Histogram_distribution_item::Histogram_distribution_item( Continuous_distribution* dist, int& id)
  : Histogram_item(id), dist_(dist)
{
  item_name_  = QString::fromStdString(dist_->name());

  display_format_ = "Lines";
}


Histogram_group_item::Histogram_group_item( GsTLGridPropertyGroup* group, int& id)
  : Histogram_item(id), group_(group), weights_(0), region_(0) 
{
  grid_ = dynamic_cast< Geostat_grid*>(group_->parent());
  grid_name_ = grid_->item_name();
  item_name_  = group_->item_name();

  //Build the list of property
  GsTLGridPropertyGroup::property_map::const_iterator it=  group_->begin_property();
  for( ; it!= group_->end_property(); ++it) {
    ++id;
    prop_items_.insert( new Histogram_property_item(it->second, id, this)  );
  }

  this->display_format("Lines");
  
}

Histogram_group_item::~Histogram_group_item() {

  std::set<Histogram_property_item*>::iterator it = prop_items_.begin();

  for( ; it != prop_items_.end(); ++it ) {
    delete *it;
  }

}

Histogram_item* Histogram_group_item::children(int row) {
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for(int i=0 ; i<row ;++i,++it  ){}
  return *it;
}

int Histogram_group_item::row_for_children(Histogram_item* item) const{
  Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(item);
  if(prop_item==0) return -1;
  std::set< Histogram_property_item*>::const_iterator it = prop_items_.find(prop_item);
  if(it==prop_items_.end()) return -1;
  return std::distance(prop_items_.begin(), it);
}

void Histogram_group_item::color(const QColor &color) {
  color_ = color;
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->color(color);
  }
}

void Histogram_group_item::display_format(QString display_format) {
  display_format_ = display_format;
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->display_format(display_format);
  }
}

void Histogram_group_item::set_visibility( bool on ) {
  is_visible_ = on;
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->set_visibility(on);
  }
}


void Histogram_group_item::weights(GsTLGridWeightProperty* weights){
  weights_ = weights;
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->weights(weights);
  }
}

void Histogram_group_item::region(GsTLGridRegion* region){
  region_ = region;
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->region(region);
  }
}

void Histogram_group_item::weights(QString weights_name){
  weights_ = grid_->weight_property(weights_name.toStdString());
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->weights(weights_);
  }
}

void Histogram_group_item::region(QString region_name){
  region_ = grid_->region(region_name.toStdString());
  std::set< Histogram_property_item*>::iterator it = prop_items_.begin();
  for( ; it != prop_items_.end(); ++it) {
    (*it)->region(region_);
  }
}
