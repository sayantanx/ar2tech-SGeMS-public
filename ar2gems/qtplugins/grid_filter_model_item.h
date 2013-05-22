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



#ifndef GRID_FILTER_MODEL_ITEM_H
#define GRID_FILTER_MODEL_ITEM_H

#include <GsTLAppli/qtplugins/common.h>
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/grid/grid_filter.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QAbstractProxyModel>
#include <QAbstractTableModel>
#include <QColor>

class QTPLUGINS_DECL Grid_filter_item {
public :
  Grid_filter_item( const QString& filter_name, Grid_filter_item* parent=0): filter_name_(filter_name), is_selected_(false),grid_(0), parent_(parent){}
  virtual ~Grid_filter_item(){}

  QString filter_name() const {return filter_name_;}

  virtual Grid_filter* create_filter() const = 0;

  virtual void set_selected( bool ok ) {is_selected_ = ok;}
  virtual bool is_selected() const {return is_selected_;}

  virtual void set_grid(const Geostat_grid* grid){ grid_ = grid; }
  virtual const Geostat_grid* grid(){return grid_;}

  virtual QVariant parameter() const =0;
  virtual void set_parameter(const QVariant& parameters) =0;

  virtual int child_count() const {return 0;}
  virtual Grid_filter_item* parent() const {return parent_;}
  virtual Grid_filter_item* child(int row) const {return 0;}

  virtual int row() const {return row_;}
  virtual void set_row(int row_number) { row_ = row_number;}

  

protected :
  Grid_filter_item* parent_;
  const QString filter_name_;
  bool is_selected_;
  int row_;
  const Geostat_grid* grid_;
  QString grid_name_;

};


class QTPLUGINS_DECL Grid_filter_union_item : public Grid_filter_item {
public :
  Grid_filter_union_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_union().item_type(),parent){}
  virtual ~Grid_filter_union_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {return "";}
  virtual void set_parameter(const QVariant& parameters) {}

  virtual void set_grid(const Geostat_grid* grid){
    grid_ = grid;
    std::vector<Grid_filter_item*>::iterator it = child_.begin();
    for( ; it!= child_.end(); ++it) {
      (*it)->set_grid(grid_);
    }

  }


  virtual int child_count() const {return this->is_selected()?child_.size():0;}
  virtual Grid_filter_item* child(int row) {return child_[row];}

protected :

  std::vector<Grid_filter_item*> child_;

};


class QTPLUGINS_DECL Grid_filter_intersection_item : public Grid_filter_item {
public :
  Grid_filter_intersection_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_intersection().item_type(),parent){}
  virtual ~Grid_filter_intersection_item(){}

  virtual Grid_filter* create_filter() const;

  virtual void set_grid(const Geostat_grid* grid){
    grid_ = grid;
    std::vector<Grid_filter_item*>::iterator it = child_.begin();
    for( ; it!= child_.end(); ++it) {
      (*it)->set_grid(grid_);
    }

  }

  virtual QVariant parameter() const {return "";};
  virtual void set_parameter(const QVariant& parameters) {};

  virtual int child_count() const {return this->is_selected()?child_.size():0;}
  virtual Grid_filter_item* child(int row) {return child_[row];}

protected :
  std::vector<Grid_filter_item*> child_;

};


class QTPLUGINS_DECL Grid_filter_region_item : public Grid_filter_item {
public :
  Grid_filter_region_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_region().item_type(),parent), region_name_("No Region Selected"){}
  virtual ~Grid_filter_region_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {return QString("%1").arg(region_name_);}
  virtual void set_parameter(const QVariant& parameters) {
    QString param = parameters.toString();
    if(grid_ == 0 ) return;
    //QStringList params = parameters.split("::");
    //grid_name_ = params[0];
    const Grid_region* region = grid_->region(param.toStdString());
    if( region == 0 ) {
      region_name_ = "No Region Selected";
    }
    else {
      region_name_ = param;
    }
  }

  virtual int child_count() const { return 0;}
  virtual Grid_filter_item* child(int row) {return 0;}

protected :

  QString region_name_;

};


class QTPLUGINS_DECL Grid_filter_category_item : public Grid_filter_item {
public :
  Grid_filter_category_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_category().item_type(),parent){}
  virtual ~Grid_filter_category_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {

    QString param = QString("%1 : %2").arg(property_name_).arg(category_name_);
    return param;
/*
    QStringList params;
    params<<property_name_<<category_name_;
    return params;
    */
  }
  virtual void set_parameter(const QVariant& parameters) {
    QStringList params = parameters.toStringList();
    property_name_ = params[0];
    category_name_ = params[1];
  }

  virtual int child_count() const { return 0;}
  virtual Grid_filter_item* child(int row) {return 0;}

protected :

  QString property_name_;
  QString category_name_;

};

class QTPLUGINS_DECL Grid_filter_less_than_item : public Grid_filter_item {
public :
  Grid_filter_less_than_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_less_than().item_type(),parent), prop_name_("None"),upper_bound_(Grid_continuous_property::no_data_value){}
  virtual ~Grid_filter_less_than_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {return QString("%1 < %2").arg(prop_name_).arg(upper_bound_);}
  virtual void set_parameter(const QVariant& parameters) {
    QStringList params = parameters.toStringList();
    prop_name_ = params[0];
    upper_bound_ = params[1].toFloat();
  }

  virtual int child_count() const { return 0;}
  virtual Grid_filter_item* child(int row) {return 0;}

protected :

  QString prop_name_;
  float upper_bound_;

};


class QTPLUGINS_DECL Grid_filter_greater_than_item : public Grid_filter_item {
public :
  Grid_filter_greater_than_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_greater_than().item_type(),parent), prop_name_("None"),lower_bound_(Grid_continuous_property::no_data_value){}
  virtual ~Grid_filter_greater_than_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {return QString(" %1 > %2").arg(prop_name_).arg(lower_bound_);}
  virtual void set_parameter(const QVariant& parameters) {
    QStringList params = parameters.toStringList();
    prop_name_ = params[0];
    lower_bound_ = params[1].toFloat();
  }

  virtual int child_count() const { return 0;}
  virtual Grid_filter_item* child(int row) {return 0;}

protected :

  QString prop_name_;
  float lower_bound_;
};


class QTPLUGINS_DECL Grid_filter_x_coord_bounded_item : public Grid_filter_item {
public :
  Grid_filter_x_coord_bounded_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_x_coord_bounded().item_type(),parent),
    x_lower_bound_(Grid_continuous_property::no_data_value),x_upper_bound_(Grid_continuous_property::no_data_value){}
  virtual ~Grid_filter_x_coord_bounded_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {return QString(" %1 < X < %2").arg(x_lower_bound_).arg(x_upper_bound_);}
  virtual void set_parameter(const QVariant& parameters) {
    QStringList params = parameters.toStringList();
    x_lower_bound_ = params[0].toDouble();
    x_upper_bound_ = params[1].toDouble();
  }

  virtual int child_count() const { return 0;}
  virtual Grid_filter_item* child(int row) {return 0;}

protected :

  double x_lower_bound_;
  double x_upper_bound_;
};

class QTPLUGINS_DECL Grid_filter_y_coord_bounded_item : public Grid_filter_item {
public :
  Grid_filter_y_coord_bounded_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_y_coord_bounded().item_type(),parent),
  y_lower_bound_(Grid_continuous_property::no_data_value),y_upper_bound_(Grid_continuous_property::no_data_value){}
  virtual ~Grid_filter_y_coord_bounded_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {return QString(" %1 < Y < %2").arg(y_lower_bound_).arg(y_upper_bound_);}
  virtual void set_parameter(const QVariant& parameters) {
    QStringList params = parameters.toStringList();
    y_lower_bound_ = params[0].toDouble();
    y_upper_bound_ = params[1].toDouble();
  }

  virtual int child_count() const { return 0;}
  virtual Grid_filter_item* child(int row) {return 0;}

protected :

  double y_lower_bound_;
  double y_upper_bound_;
};

class QTPLUGINS_DECL Grid_filter_z_coord_bounded_item : public Grid_filter_item {
public :
  Grid_filter_z_coord_bounded_item(Grid_filter_item* parent=0): Grid_filter_item(Grid_filter_z_coord_bounded().item_type(),parent),
  z_lower_bound_(Grid_continuous_property::no_data_value),z_upper_bound_(Grid_continuous_property::no_data_value){}
  virtual ~Grid_filter_z_coord_bounded_item(){}

  virtual Grid_filter* create_filter() const;

  virtual QVariant parameter() const {return QString(" %1 < Z < %2").arg(z_lower_bound_).arg(z_upper_bound_);}
  virtual void set_parameter(const QVariant& parameters) {
    QStringList params = parameters.toStringList();
    z_lower_bound_ = params[0].toDouble();
    z_upper_bound_ = params[1].toDouble();
  }

  virtual int child_count() const { return 0;}
  virtual Grid_filter_item* child(int row) {return 0;}

protected :

  double z_lower_bound_;
  double z_upper_bound_;
};


/*
  dir->factory( Grid_filter_lessor_or_equal_than().item_type().toStdString(), Grid_filter_lessor_or_equal_than::create_new_interface );
  dir->factory( Grid_filter_greater_or_equal_than().item_type().toStdString(), Grid_filter_greater_or_equal_than::create_new_interface );
  dir->factory( Grid_filter_bounded().item_type().toStdString(), Grid_filter_bounded::create_new_interface );
  dir->factory( Grid_filter_x_coord_bounded().item_type().toStdString(), Grid_filter_x_coord_bounded::create_new_interface );
  dir->factory( Grid_filter_y_coord_bounded().item_type().toStdString(), Grid_filter_y_coord_bounded::create_new_interface );
  dir->factory( Grid_filter_z_coord_bounded().item_type().toStdString(), Grid_filter_z_coord_bounded::create_new_interface );
  dir->factory( Grid_filter_log_names().item_type().toStdString(), Grid_filter_log_names::create_new_interface );
  */


#endif // CATEGORICAL_DEFINITION_PROXY_MODEL_H
