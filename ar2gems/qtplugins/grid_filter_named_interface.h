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



#ifndef GRID_FILTER_NAMED_INTERFACE_H
#define GRID_FILTER_NAMED_INTERFACE_H

#include <qtplugins/common.h>
#include <utils/manager.h>
#include <grid/grid_filter.h>
#include <qtplugins/grid_filter_model_item.h>
#include <qtplugins/grid_filter_delegate.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QAbstractProxyModel>
#include <QAbstractTableModel>
#include <QColor>

class QTPLUGINS_DECL Grid_filter_named_interface : public  Named_interface {
public :
  Grid_filter_named_interface( ){}
  Grid_filter_named_interface( const QString& name):filter_name_(name){}
  virtual ~Grid_filter_named_interface(){}

  virtual QString filter_name() const {return filter_name_;}
  virtual Grid_filter_item* filter_item() const =0;
  virtual Grid_filter_abstract_delegate* filter_delegate() const = 0;

protected :
  const QString filter_name_;

};


class QTPLUGINS_DECL Grid_filter_union_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_union_item_ni;
  }

public :

  Grid_filter_union_item_ni(): Grid_filter_named_interface(Grid_filter_union().item_type()){}
  virtual ~Grid_filter_union_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_union_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_abstract_delegate(); }

};


class QTPLUGINS_DECL Grid_filter_intersection_item_ni : public Grid_filter_named_interface {
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_intersection_item_ni;
  }

public :
  Grid_filter_intersection_item_ni(): Grid_filter_named_interface(Grid_filter_intersection().item_type()){}
  virtual ~Grid_filter_intersection_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_intersection_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_abstract_delegate(); }

};


class QTPLUGINS_DECL Grid_filter_region_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_region_item_ni;
  }

public :
  Grid_filter_region_item_ni(): Grid_filter_named_interface(Grid_filter_region().item_type()){}
  virtual ~Grid_filter_region_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_region_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_region_delegate(); }

};


class QTPLUGINS_DECL Grid_filter_category_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_category_item_ni;
  }

public :
  Grid_filter_category_item_ni(): Grid_filter_named_interface(Grid_filter_category().item_type()){}
  virtual ~Grid_filter_category_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_category_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_cateorical_property_delegate(); }


};

class QTPLUGINS_DECL Grid_filter_less_than_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_less_than_item_ni;
  }

public :
  Grid_filter_less_than_item_ni(): Grid_filter_named_interface(Grid_filter_less_than().item_type()){}
  virtual ~Grid_filter_less_than_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_less_than_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_threshold_property_delegate(); }


};


class QTPLUGINS_DECL Grid_filter_greater_than_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_greater_than_item_ni;
  }

public :
  Grid_filter_greater_than_item_ni(): Grid_filter_named_interface(Grid_filter_greater_than().item_type()){}
  virtual ~Grid_filter_greater_than_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_greater_than_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_threshold_property_delegate(); }


};


class QTPLUGINS_DECL Grid_filter_x_coord_bounded_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_x_coord_bounded_item_ni;
  }

public :
  Grid_filter_x_coord_bounded_item_ni(): Grid_filter_named_interface(Grid_filter_x_coord_bounded().item_type()){}
  virtual ~Grid_filter_x_coord_bounded_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_x_coord_bounded_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_coord_bounded_delegate(); }


};


class QTPLUGINS_DECL Grid_filter_y_coord_bounded_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_y_coord_bounded_item_ni;
  }

public :
  Grid_filter_y_coord_bounded_item_ni(): Grid_filter_named_interface(Grid_filter_y_coord_bounded().item_type()){}
  virtual ~Grid_filter_y_coord_bounded_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_y_coord_bounded_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_coord_bounded_delegate(); }


};

class QTPLUGINS_DECL Grid_filter_z_coord_bounded_item_ni : public Grid_filter_named_interface {

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_z_coord_bounded_item_ni;
  }

public :
  Grid_filter_z_coord_bounded_item_ni(): Grid_filter_named_interface(Grid_filter_z_coord_bounded().item_type()){}
  virtual ~Grid_filter_z_coord_bounded_item_ni(){}

  virtual Grid_filter_item* filter_item() const {return new Grid_filter_z_coord_bounded_item();}
  virtual Grid_filter_abstract_delegate* filter_delegate() const {return new Grid_filter_coord_bounded_delegate(); }


};

#endif // CATEGORICAL_DEFINITION_PROXY_MODEL_H
