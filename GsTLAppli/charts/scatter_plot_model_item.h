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



#ifndef SCATTER_PLOT_MODEL_ITEM_H
#define SCATTER_PLOT_MODEL_ITEM_H

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/charts/common.h>
#include <GsTLAppli/charts/chart_model_item.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>

#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QAbstractProxyModel>
#include <QWidget>
#include <QHash>
#include <QPair>

class CHARTS_DECL Scatter_plot_item : public Chart_item{

public :
 enum  Marker_style	{  NONE , CROSS , PLUS , SQUARE , CIRCLE , DIAMOND };

public :
  Scatter_plot_item(int& id):id_(id),marker_size_(10), marker_style_(Scatter_plot_item::CIRCLE){
    marker_styles_string_ = QStringList() << "Cross" << "Plus" << "Square" << "Circle" << "Diamond" ;  }
  virtual ~Scatter_plot_item(){}

  virtual int children_count() const =0;
  virtual Scatter_plot_item* children(int row) =0;  
  virtual Scatter_plot_item* parent() =0;  

  virtual QString type() const =0;

 // void id(int id) const {id_ = id;}
  int id() const {return id_;}

  QString grid_name() const {return grid_name_;}
  QString item_name_x() const {return item_name_x_;}
  QString item_name_y() const {return item_name_y_;}
  virtual GsTL_object_item* object_item_x() = 0;
  virtual GsTL_object_item* object_item_y() = 0;

  virtual Geostat_grid* grid() const {return grid_;}

  virtual GsTLGridWeightProperty* weights() {return 0;}
  virtual GsTLGridRegion* region() {return 0;}
  virtual GsTLGridCategoricalProperty* categorical_property() {return 0;}
  
  virtual void weights(GsTLGridWeightProperty* weights) {}
  virtual void categorical_property(GsTLGridCategoricalProperty* cprop) {}
  virtual void region(GsTLGridRegion* region) {}

  virtual void weights(QString weights_name) {}
  virtual void categorical_property(QString cprop_name) {}
  virtual void region(QString region_name) {}


  virtual bool operator<(const Scatter_plot_item& rhs ) const {

    if(this->grid_name_ < rhs.grid_name()) return true;
    else if(this->grid_name() > rhs.grid_name()) return false;
    else if( this->item_name_x_ <  rhs.item_name_x() ) return true;
    else this->item_name_y_ <  rhs.item_name_y();

  }

  virtual bool operator==(const Scatter_plot_item& rhs ) const {
    return item_name_x_ == rhs.item_name_x() && item_name_y_ == rhs.item_name_y() && rhs.grid_name() == grid_name_;
  }

  virtual bool operator!=(const Scatter_plot_item& rhs ) const {
    return !this->operator==(rhs);
  }

  QColor color() const {return color_;}
  QString display_format() const {return display_format_;}
  //QString marker_style() const {return marker_style_;}
  float marker_size() const {return marker_size_;}
  bool is_visible() const {return is_visible_;}
  Scatter_plot_item::Marker_style marker_style() const {return marker_style_;} 
  QString marker_style_string() const {return marker_styles_string_.at(marker_style_-1);} 


  void marker_size(float marker_size){marker_size_ = marker_size;}
  void marker_style(Scatter_plot_item::Marker_style marker_style){marker_style_ = marker_style;}
  void marker_style(QString marker_style){marker_style_ = (Scatter_plot_item::Marker_style)(marker_styles_string_.indexOf(marker_style)+1);}
  virtual void color(const QColor &color) {color_ = color;}

  virtual void display_format(QString display_format) {display_format_ = display_format;}
  virtual void set_visibility( bool on ) {is_visible_ = on;}

protected :

  const int id_;
  QStringList marker_styles_string_;
  QString grid_name_;
  QString item_name_x_;  // Either the name of a property of a group
  QString item_name_y_;  // Either the name of a property of a group

  Geostat_grid* grid_;

  QColor color_;
  QString display_format_;
  bool is_visible_;
  Scatter_plot_item::Marker_style marker_style_;
  float marker_size_;

};


class CHARTS_DECL Scatter_plot_property_item :public Scatter_plot_item {
public :
  Scatter_plot_property_item( GsTLGridProperty* prop_x, GsTLGridProperty* prop_y, int& id, Scatter_plot_item* parent=0);
  virtual ~Scatter_plot_property_item(){}

  virtual QString type() const {return "Property";}

  
  GsTLGridProperty* prop_x() {return prop_x_;}
  GsTLGridProperty* prop_y() {return prop_y_;}
  virtual GsTL_object_item* object_item_x() {return prop_x_;}
  virtual GsTL_object_item* object_item_y() {return prop_y_;}

  virtual GsTLGridWeightProperty* weights() {return weights_;}
  virtual GsTLGridRegion* region() {return region_;}
  virtual GsTLGridCategoricalProperty* categorical_property() {return cprop_;}

  virtual void weights(GsTLGridWeightProperty* weights) {weights_ = weights;}
  virtual void region(GsTLGridRegion* region) {region_ = region;}
  virtual void categorical_property(GsTLGridCategoricalProperty* cprop) {cprop_ = cprop;}

  virtual void weights(QString weights_name) {weights_ = grid_->weight_property(weights_name.toStdString());}
  virtual void region(QString region_name) {region_ = grid_->region(region_name.toStdString());}
  virtual void categorical_property(QString cprop_name) {cprop_ = grid_->categorical_property(cprop_name.toStdString());}

  virtual int children_count() const {return 0;}
  virtual Scatter_plot_item* children(int row) {return 0;}
  virtual Scatter_plot_item* parent() {return parent_;};  

  /*
  virtual bool operator==(const Histogram_property_item& rhs ) const {
    return this->prop_ == rhs.prop();
  }

  virtual bool operator!=(const Histogram_property_item& rhs ) const {
    return this->prop_ != rhs.prop();
  }
  */
private :
  
  Scatter_plot_item* parent_;
  GsTLGridProperty* prop_x_;
  GsTLGridProperty* prop_y_;
  GsTLGridRegion* region_;
  GsTLGridWeightProperty* weights_;
  GsTLGridCategoricalProperty* cprop_;


};


class CHARTS_DECL Scatter_plot_property_group_item :public Scatter_plot_item {
public :
  Scatter_plot_property_group_item(GsTLGridProperty* prop_x, GsTLGridPropertyGroup* group_y, int& id);
  virtual ~Scatter_plot_property_group_item();

  virtual QString type() const {return "Property-Group";}
  GsTLGridProperty* prop() {return prop_x_;}
  GsTLGridPropertyGroup* group() {return group_y_;}
  virtual GsTL_object_item* object_item_x() {return prop_x_;}
  virtual GsTL_object_item* object_item_y() {return group_y_;}

  virtual GsTLGridWeightProperty* weights(){return weights_;}
  virtual GsTLGridRegion* region(){return region_;}
  virtual GsTLGridCategoricalProperty* categorical_property() {return cprop_;}

  virtual void weights(GsTLGridWeightProperty* weights);
  virtual void region(GsTLGridRegion* region);
  virtual void categorical_property(GsTLGridCategoricalProperty* cprop);

  virtual void weights(QString weights_name);
  virtual void region(QString region_name) ;
  virtual void categorical_property(QString cprop_name);



  virtual int children_count() const {return prop_items_.size();}
  virtual Scatter_plot_item* children(int row);
  virtual Scatter_plot_item* parent() {return 0;}  
  int row_for_children(Scatter_plot_item*) const;

  //Need to propagate the change to underlying properties
  virtual void color(const QColor &color);
  virtual void display_format(QString line_format);
  virtual void set_visibility( bool on );


  /*
  inline bool operator==(const Histogram_group_item& rhs ) const {
    return this->group_ == rhs.group();
  }

  inline bool operator!=(const Histogram_group_item& rhs ) const {
    return this->group_ != rhs.group();
  }
  */
private :

  GsTLGridProperty*prop_x_;
  GsTLGridPropertyGroup* group_y_;
  GsTLGridRegion* region_;
  GsTLGridWeightProperty* weights_;
  GsTLGridCategoricalProperty* cprop_;

  std::set< Scatter_plot_property_item*> prop_items_;

};



class CHARTS_DECL Scatter_plot_group_item :public Scatter_plot_item {
public :
  Scatter_plot_group_item( GsTLGridPropertyGroup* group_x, GsTLGridPropertyGroup* group_y, int& id);
  virtual ~Scatter_plot_group_item();

  virtual QString type() const {return "Group";}
  GsTLGridPropertyGroup* group_x() {return group_x_;}
  GsTLGridPropertyGroup* group_y() {return group_y_;}
  virtual GsTL_object_item* object_item_x() {return group_x_;}
  virtual GsTL_object_item* object_item_y() {return group_y_;}

  virtual GsTLGridWeightProperty* weights(){return weights_;}
  virtual GsTLGridRegion* region(){return region_;}
  virtual GsTLGridCategoricalProperty* categorical_property(){return cprop_;}

  virtual void weights(GsTLGridWeightProperty* weights);
  virtual void region(GsTLGridRegion* region);
  virtual void categorical_property(GsTLGridCategoricalProperty* cprop);

  virtual void weights(QString weights_name);
  virtual void region(QString region_name) ;
  virtual void categorical_property(QString cprop_name);



  virtual int children_count() const {return prop_items_.size();}
  virtual Scatter_plot_item* children(int row);
  virtual Scatter_plot_item* parent() {return 0;}  
  int row_for_children(Scatter_plot_item*) const;

  //Need to propagate the change to underlying properties
  virtual void color(const QColor &color);
  virtual void display_format(QString line_format);
  virtual void set_visibility( bool on );


  /*
  inline bool operator==(const Histogram_group_item& rhs ) const {
    return this->group_ == rhs.group();
  }

  inline bool operator!=(const Histogram_group_item& rhs ) const {
    return this->group_ != rhs.group();
  }
  */
private :

  GsTLGridPropertyGroup* group_x_;
  GsTLGridPropertyGroup* group_y_;
  GsTLGridRegion* region_;
  GsTLGridWeightProperty* weights_;
  GsTLGridCategoricalProperty* cprop_;

  std::set< Scatter_plot_property_item*> prop_items_;

};



#endif // HISTOGRAM_MODEL_H
