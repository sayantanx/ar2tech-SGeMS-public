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



/**********************************************************************
** Author: Alexandre Bocuher
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Stanford Geostatistical Earth
** Modeling Software (SGeMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#ifndef __GSTLAPPLI_GRID_PROPERTY_SET_H__ 
#define __GSTLAPPLI_GRID_PROPERTY_SET_H__ 
 
#include <utils/gstl_object_item.h>
#include <utils/gstl_types.h> 
#include <utils/gstl_messages.h>
#include <appli/root_model.h>
#include <grid/common.h>
#include <grid/grid_model/grid_property.h> 

#include <GsTL/cdf/categ_non_param_cdf.h>
#include <GsTL/cdf/non_param_cdf.h>

#include <string> 
#include <map>
#include <set>
//#include <QtXml/QDomDocument>


 
class GRID_DECL GsTLGridPropertyGroup :public GsTL_object_item {
public:

//  static Named_interface* create_new_interface( std::string& name);

  typedef std::map<std::string, Grid_continuous_property*> property_map;
  GsTLGridPropertyGroup();
  GsTLGridPropertyGroup(std::string name);
  virtual ~GsTLGridPropertyGroup(){}

  virtual std::string name() const {return name_;}
  virtual std::string type() const {return type_;}
  

  bool is_member_property(std::string prop_name) const;
  Grid_continuous_property* get_property(std::string prop_name);
  Grid_continuous_property* get_property(int id);
  const Grid_continuous_property* get_property(std::string prop_name) const;
  const Grid_continuous_property* get_property(int id) const;
  int size() const {return properties_.size();}
  bool empty() const {return properties_.empty();}

  bool add_property(Grid_continuous_property*);
  bool remove_property(Grid_continuous_property*);

  std::vector<Grid_continuous_property::property_type> get_vector_data( int node_id ) const;

  property_map::iterator begin_property(){ return properties_.begin(); }
  property_map::iterator end_property() {  return properties_.end(); }
  property_map::const_iterator begin_property() const { return properties_.begin(); }
  property_map::const_iterator end_property() const {  return properties_.end(); }
  std::vector<std::string> property_names() const;

  std::string get_group_info() const;
  void set_group_info(const std::string& info_str);

  // GsTL_object_item
  virtual QString item_name() const;
  virtual QString item_type() const;
  virtual GsTL_object_item *child(int row);
  virtual int childCount() const;
  virtual int columnCount() const;
  virtual QVariant item_data(int column) const;
//    virtual int row() const;

protected :
  property_map properties_;
  std::string name_;
  std::string type_;
  
  std::map<std::string, GsTL_object_property_item> property_item_map_;

  Root_model* model_;

 // QDomElement root_;
//  QDomDocument meta_data_;

  std::map<std::string, std::string> info_;
};


GsTLGridPropertyGroup*
Create_new_property_group(const std::string& name,const std::string& type="");

class GRID_DECL Grid_property_group_manager {
public :
  Grid_property_group_manager();
  virtual ~Grid_property_group_manager(){}

  GsTLGridPropertyGroup* add_group(const std::string& name, const std::string& type);

  void remove_group(const std::string& name);

  GsTLGridPropertyGroup* get_group(const std::string& name);
  const GsTLGridPropertyGroup* get_group(const std::string& name) const;

  bool add_property_to_group(Grid_continuous_property* prop, const std::string& group_name);

  bool remove_property_from_group(Grid_continuous_property* prop, const std::string& group_name);

  std::list<std::string> group_types() const;

  std::list<std::string> group_names(const std::string& type="") const;

  std::map<std::string, GsTLGridPropertyGroup*>::iterator begin_group();
  std::map<std::string, GsTLGridPropertyGroup*>::iterator end_group();
  std::map<std::string, GsTLGridPropertyGroup*>::const_iterator begin_group() const;
  std::map<std::string, GsTLGridPropertyGroup*>::const_iterator end_group() const;

  unsigned int size() const;

  void set_parent_item( GsTL_object_item* parent){parent_=parent;}

protected :
  typedef std::map<std::string, GsTLGridPropertyGroup*> group_map;
  typedef std::map<std::string, int> group_type_map;
  group_map groups_;
  group_type_map group_type_;

  GsTL_object_item* parent_;

  Root_model* model_;



};




class GRID_DECL SimulationPropertyGroup : public GsTLGridPropertyGroup {
public:

 // static Named_interface* create_new_interface( std::string& name);

  SimulationPropertyGroup();
  SimulationPropertyGroup(std::string name);
  virtual ~SimulationPropertyGroup(){}

  bool is_categorical();

   
  void set_algo_command(const std::string& algo_str);
  std::string get_algo_command();

protected :

  std::string simulation_base_name_;
  std::string grid_name_;

};


class CategoricalPropertyDefinition;


class GRID_DECL CategoricalPropertyGroup : public GsTLGridPropertyGroup {
public:

 // static Named_interface* create_new_interface( std::string& name);

  CategoricalPropertyGroup();
  CategoricalPropertyGroup(std::string name);
  virtual ~CategoricalPropertyGroup (){}

  virtual void set_categorical_definition(std::string cat_def_name);
  virtual void set_categorical_definition(const CategoricalPropertyDefinition* cat_def);
  virtual std::string get_categorical_definition_name() const;
  virtual  const CategoricalPropertyDefinition* get_categorical_definition() const;


protected :
  const CategoricalPropertyDefinition* cat_def_;

};


class GRID_DECL CategoricalProbabilityPropertyGroup : public CategoricalPropertyGroup {
public:

 // static Named_interface* create_new_interface( std::string& name);

  CategoricalProbabilityPropertyGroup();
  CategoricalProbabilityPropertyGroup(std::string name);
  virtual ~CategoricalProbabilityPropertyGroup (){}


  Categ_non_param_cdf<int>  get_distribution(int node_id) const;

};


//indicator for categorical attribute
// note: There is only one non-zero indicator per node-id
class GRID_DECL IndicatorCategoricalPropertyGroup : public CategoricalPropertyGroup {
public:

//  static Named_interface* create_new_interface( std::string& name);

  IndicatorCategoricalPropertyGroup();
  IndicatorCategoricalPropertyGroup(std::string name);
  virtual ~IndicatorCategoricalPropertyGroup (){}

  int get_category(int node_id) const;
  std::string get_category_name(int node_id) const;


};


//indicator for categorical attribute
// note: the indicator property are continuous values modeling
// a categorical attributes
class GRID_DECL IndicatorContinuousPropertyGroup : public GsTLGridPropertyGroup {
public:

 // static Named_interface* create_new_interface( std::string& name);

  IndicatorContinuousPropertyGroup();
  IndicatorContinuousPropertyGroup(std::string name);
  virtual ~IndicatorContinuousPropertyGroup (){}

  void set_thresholds(std::vector<float>);
  std::vector<float> get_thresholds() const;

  Non_param_cdf<>  get_distribution(int node_id) const;


protected :
  std::vector<float> thresholds_;

};


#endif
