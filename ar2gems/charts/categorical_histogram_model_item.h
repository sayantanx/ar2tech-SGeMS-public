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

#ifndef CATEGORICAL_HISTOGRAM_MODEL_ITEM_H
#define CATEGORICAL_HISTOGRAM_MODEL_ITEM_H

#include <utils/manager.h>
#include <appli/root_model.h>
#include <charts/common.h>
#include <charts/chart_model_item.h>
#include <grid/grid_model/geostat_grid.h>
#include <grid/grid_model/grid_property.h>
#include <grid/grid_model/grid_categorical_property.h>
#include <grid/grid_model/grid_weight_property.h>
#include <grid/grid_model/grid_property_set.h>
#include <math/categorical_distribution.h>

#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QAbstractProxyModel>
#include <QWidget>
#include <QHash>

class CHARTS_DECL Categorical_histogram_item : public Chart_item{
public :
  Categorical_histogram_item(int& id):id_(id){}
  virtual ~Categorical_histogram_item(){}

  virtual int children_count() const =0;
  virtual Categorical_histogram_item* children(int row) =0;  
  virtual Categorical_histogram_item* parent() =0;  

  virtual QString type() const =0;

 // void id(int id) const {id_ = id;}
  int id() const {return id_;}

  QString grid_name() const {return grid_name_;}
  QString item_name() const {return item_name_;}
  virtual GsTL_object_item* object_item() = 0;

  Geostat_grid* grid() const {return grid_;}

  virtual Grid_weight_property* weights() {return 0;}
  virtual Grid_region* region() {return 0;}
  virtual void weights(Grid_weight_property* weights) {}
  virtual void region(Grid_region* region) {}
  virtual void weights(QString weights_name) {}
  virtual void region(QString region_name) {}

  virtual bool operator<(const Categorical_histogram_item& rhs ) const {

    if(this->grid_name_ < rhs.grid_name()) return true;
    else if(this->grid_name() > rhs.grid_name()) return false;
    else return this->item_name_ <  rhs.item_name();

  }

  virtual bool operator==(const Categorical_histogram_item& rhs ) const {
    return item_name_ == rhs.item_name() &&  rhs.grid_name() == grid_name_;
  }

  virtual bool operator!=(const Categorical_histogram_item& rhs ) const {
    return !this->operator==(rhs);
  }
  /*
  float r_color() const {return color_.redF();}
  float g_color() const {return color_.greenF();}
  float b_color() const {return color_.blueF();}
  */
  QColor color() const {return color_;}
  bool is_visible() {return is_visible_;}

  virtual void color(const QColor &color) {color_ = color;}
  virtual void set_visibility( bool on ) {is_visible_ = on;}

protected :

  const int id_;

  QString grid_name_;
  QString item_name_;  // Either the name of a property of a group

  Geostat_grid* grid_;

  QColor color_;
  bool is_visible_;

};


class CHARTS_DECL Categorical_histogram_property_item :public Categorical_histogram_item {
public :
  Categorical_histogram_property_item( Grid_categorical_property* prop, int& id, Categorical_histogram_item* parent=0);
  virtual ~Categorical_histogram_property_item(){}

  virtual QString type() const {return "CategoricalProperty";}

  
  Grid_categorical_property* prop() {return prop_;}
  virtual GsTL_object_item* object_item() {return dynamic_cast<GsTL_object_item*>(prop_);}

  virtual Grid_weight_property* weights() {return weights_;}
  virtual Grid_region* region() {return region_;}
  virtual void weights(Grid_weight_property* weights) {weights_ = weights;}
  virtual void region(Grid_region* region) {region_ = region;}
  virtual void weights(QString weights_name) {weights_ = grid_->weight_property(weights_name.toStdString());}
  virtual void region(QString region_name) {region_ = grid_->region(region_name.toStdString());}

  virtual int children_count() const {return 0;}
  virtual Categorical_histogram_item* children(int row) {return 0;}
  virtual Categorical_histogram_item* parent() {return parent_;};  

  /*
  virtual bool operator==(const Categorical_histogram_property_item& rhs ) const {
    return this->prop_ == rhs.prop();
  }

  virtual bool operator!=(const Categorical_histogram_property_item& rhs ) const {
    return this->prop_ != rhs.prop();
  }
  */
private :
  
  Categorical_histogram_item* parent_;
  Grid_categorical_property* prop_;
  Grid_region* region_;
  Grid_weight_property* weights_;


};


class CHARTS_DECL Categorical_histogram_group_item :public Categorical_histogram_item {
public :
  Categorical_histogram_group_item( GsTLGridPropertyGroup* group, int& id);
  virtual ~Categorical_histogram_group_item();

  virtual QString type() const {return "Group";}
  GsTLGridPropertyGroup* group() {return group_;}
  virtual GsTL_object_item* object_item() {return group_;}

  virtual Grid_weight_property* weights(){return weights_;}
  virtual Grid_region* region(){return region_;}
  virtual void weights(Grid_weight_property* weights);
  virtual void region(Grid_region* region);
  virtual void weights(QString weights_name);
  virtual void region(QString region_name) ;



  virtual int children_count() const {return prop_items_.size();}
  virtual Categorical_histogram_item* children(int row);
  virtual Categorical_histogram_item* parent() {return 0;}  
  int row_for_children(Categorical_histogram_item*) const;

  //Need to propagate the change to underlying properties
  virtual void color(const QColor &color);
  virtual void set_visibility( bool on );

private :

  GsTLGridPropertyGroup* group_;
  Grid_region* region_;
  Grid_weight_property* weights_;

  std::set< Categorical_histogram_property_item*> prop_items_;

};




class CHARTS_DECL Categorical_histogram_distribution_item :public Categorical_histogram_item {
public :
  Categorical_histogram_distribution_item( Categorical_distribution* dist, int& id );
  virtual ~Categorical_histogram_distribution_item(){}

  virtual QString type() const {return "CategoricalDistribution";}

  
  Grid_categorical_property* prop() {return 0;}
  virtual GsTL_object_item* object_item() {return dist_;}

  virtual Grid_weight_property* weights() {return 0;}
  virtual Grid_region* region() {return 0;}
  virtual void weights(Grid_weight_property* weights) {}
  virtual void region(Grid_region* region) {}
  virtual void weights(QString weights_name) {}
  virtual void region(QString region_name) {}

  virtual int children_count() const {return 0;}
  virtual Categorical_histogram_item* children(int row) {return 0;}
  virtual Categorical_histogram_item* parent() {return 0;}  


private :
  
  Categorical_histogram_item* parent_;
  Categorical_distribution* dist_;
};

#endif // HISTOGRAM_MODEL_H
