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




#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/grid/grid_model/grid_property.h> 
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h> 
#include <GsTLAppli/appli/manager_repository.h> 
//#include <QtXml/QDomElement>

GsTLGridPropertyGroup*
Create_new_property_group(const std::string& name, const std::string& type) {
	if(type == "" || type == "General") return new GsTLGridPropertyGroup(name);
	else if(type == "Simulation") return new SimulationPropertyGroup(name);
  else if(type == "Categorical") return new CategoricalPropertyGroup(name);
	else if(type == "CategoricalIndicator") return new IndicatorCategoricalPropertyGroup(name);
	else if(type == "CategoricalProbability") return new CategoricalProbabilityPropertyGroup(name);
  else if(type == "ContinuousIndicator") return new IndicatorContinuousPropertyGroup(name);
	else return 0;

}

/*
Named_interface* GsTLGridPropertyGroup::create_new_interface( std::string& name) {
  return new GsTLGridPropertyGroup(name);
}
*/

GsTLGridPropertyGroup::GsTLGridPropertyGroup(){
    item_name_ = "";
  item_type_ = "Group";
}

GsTLGridPropertyGroup::GsTLGridPropertyGroup(std::string name){
  name_ = name;
  type_ = "General";
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());

  item_name_ = name_.c_str();
  item_type_ = "Group";
//  SmartPtr<Named_interface> ni = Root::instance()->interface(qitem_model_manager+"/Root");
//  model_ = dynamic_cast<Root_model*>(ni.raw_ptr());
//  root_ = meta_data_.createElement(type_.c_str());
//  meta_data_.appendChild(root_);
}


bool GsTLGridPropertyGroup::is_member_property(std::string prop_name) const {
  return( properties_.find( prop_name) != properties_.end() );
}

Grid_continuous_property* GsTLGridPropertyGroup::get_property(std::string prop_name) {
  property_map::iterator it = properties_.find( prop_name);
  if( it == properties_.end() ) return 0;
  return it->second;
}

const Grid_continuous_property* GsTLGridPropertyGroup::get_property(std::string prop_name) const {
  property_map::const_iterator it = properties_.find( prop_name);
  if( it == properties_.end() ) return 0;
  return it->second;
}

Grid_continuous_property* GsTLGridPropertyGroup::get_property(int id) {
  if( id >= properties_.size() ) return 0;
  property_map::iterator it = properties_.begin();
  std::advance(it,id);
  if( it == properties_.end() ) return 0;
  return it->second;
}


const Grid_continuous_property* GsTLGridPropertyGroup::get_property(int id) const {
  if( id >= properties_.size() ) return 0;
  property_map::const_iterator it = properties_.begin();
  std::advance(it,id);
  if( it == properties_.end() ) return 0;
  return it->second;
}

bool GsTLGridPropertyGroup::add_property(Grid_continuous_property* prop) {
  if(prop == 0) return false;
  if(model_) {
	  property_map::iterator it =properties_.lower_bound(prop->name());
	  int row = std::distance(properties_.begin(),it);
	  model_->begin_insert_item(this,row);
  }
  properties_[prop->name()] = prop;
  property_item_map_.insert(std::pair<std::string,GsTL_object_property_item>(prop->name(),GsTL_object_property_item(prop, this)));

  if(model_) model_->end_insert_item();

//  property_item_map_[prop->name()] = GsTL_object_property_item(prop, this);
  prop->add_group_membership( this );
  return true;
}

bool GsTLGridPropertyGroup::remove_property(Grid_continuous_property* prop){
  unsigned int ok = properties_.erase( prop->name() );
  if(ok) {
	  if(model_) {
		  int row = std::distance(property_item_map_.begin(),property_item_map_.find(prop->name()));
		  model_->begin_remove_item(this,row);
	  }
	  prop->remove_group_membership( this );
	  property_item_map_.erase(prop->name());
	  if(model_) model_->end_remove_item();
  }
  return ok != 0;
}

std::vector<std::string> GsTLGridPropertyGroup::property_names() const{
  std::vector<std::string> names;
  names.reserve(properties_.size());
  property_map::const_iterator  it = properties_.begin();
  for(; it != properties_.end(); ++it) {
    names.push_back(it->first);
  }
  return names;
}

std::vector<Grid_continuous_property::property_type> 
GsTLGridPropertyGroup::get_vector_data( int node_id ) const{
  property_map::const_iterator it = properties_.begin();
  std::vector<Grid_continuous_property::property_type> values;
  values.reserve(properties_.size() );
  for( ; it!= properties_.end(); ++it) {
    if( it->second->is_informed(node_id) ) values.push_back( it->second->get_value(node_id) );
  }
  return values;
}


void GsTLGridPropertyGroup::set_group_info(const std::string& info_str) {
  info_["Info"] = info_str;
//  QDomElement ele = meta_data_.createElement("Info");
//  root_.appendChild(ele);
//  ele.setAttribute("Info",info_str.c_str());
}


std::string GsTLGridPropertyGroup::get_group_info() const{
  std::map<std::string, std::string>::const_iterator it = info_.find("Info");
  if(it == info_.end()) return "";
  return it->second;
//  QDomElement ele = root_.firstChildElement("Info");
//  if(ele.isNull()) return "";
//  return ele.attribute("text").toStdString();
}

QString GsTLGridPropertyGroup::item_name() const{
	return QString(name_.c_str());
}

QString GsTLGridPropertyGroup::item_type() const{
	return QString("Group:%1").arg(type_.c_str());
}

GsTL_object_item *GsTLGridPropertyGroup::child(int row){
	 std::map<std::string, GsTL_object_property_item>::iterator it = property_item_map_.begin();
	for(int i=0; i< row; ++i, ++it ){}
	return &(it->second);
}
int GsTLGridPropertyGroup::childCount() const {
	return property_item_map_.size();
}

int GsTLGridPropertyGroup::columnCount() const {
	return 2;
}
QVariant GsTLGridPropertyGroup::item_data(int column) const{
	if(column == 0) return QString(name_.c_str());
	else if (column == 1) return item_type();

	else return QVariant();
}




/***
*  ------------------------
*/

Grid_property_group_manager::Grid_property_group_manager()
 :parent_(0){
	model_ = dynamic_cast<Root_model*>(Root::instance()->model());
}


GsTLGridPropertyGroup* Grid_property_group_manager::add_group(const std::string& name, const std::string& type) {
  group_map::iterator it_group = groups_.find(name);
  if(it_group != groups_.end()) return 0;
  // The group does not already exist
  GsTLGridPropertyGroup* group = Create_new_property_group(name, type);
 // GsTLGridPropertyGroup* group = new GsTLGridPropertyGroup(name);  // Need to get it from the manager
  if(group == 0) return 0; //failed to initialize maybe unknown type
  group->set_parent_item(parent_);
  if(model_) {
	  group_map::iterator it_row = groups_.lower_bound(name);
	  model_->begin_insert_item(parent_,std::distance(groups_.begin(), it_row)+1);
  }
  groups_[name] = group;
  if(model_) model_->end_insert_item();
// Add the group type to the list (or to the count is type already present)
  group_type_map::iterator it_type = group_type_.find(type);
  if(it_type == group_type_.end()) 
    group_type_[type] = 1;
  else
    it_type->second++;

  return group;
}

void Grid_property_group_manager::remove_group(const std::string& name) {
  group_map::iterator it_group = groups_.find(name);
  if(it_group == groups_.end()) return;
  GsTLGridPropertyGroup* group = it_group->second;

// remove the group type to the list (or to the decrease the count is type already present)
  std::string type = group->type();
  int count = group_type_[type];
  if(count == 1) 
    group_type_.erase(type);
  else
    group_type_[type]--;

// remove property membership from group
  GsTLGridPropertyGroup::property_map::iterator it = group->begin_property();
  std::vector<Grid_continuous_property*> props;
  for( ; it != group->end_property(); ++it) {
  	props.push_back(it->second);
  }
  std::vector<Grid_continuous_property*>::iterator it_props = props.begin();
  for( ; it_props != props.end(); ++it_props) {
  	group->remove_property(*it_props);
  }

// Remove the group from the manager
  if(model_) {
	  model_->begin_remove_item(parent_,std::distance(groups_.begin(), it_group)+1);
  }
  delete (group);
  groups_.erase(it_group);
  if(model_) model_->end_remove_item();


}

GsTLGridPropertyGroup* Grid_property_group_manager::get_group(const std::string& name) {
  group_map::iterator it = groups_.find(name);
  if(it != groups_.end()) 
    return it->second;
  else
    return 0;
}


const GsTLGridPropertyGroup* 
Grid_property_group_manager::get_group(const std::string& name) const{
  group_map::const_iterator it = groups_.find(name);
  if(it != groups_.end()) 
    return it->second;
  else
    return 0;
}

bool Grid_property_group_manager::
add_property_to_group(Grid_continuous_property* prop, const std::string& name) {
  group_map::iterator it = groups_.find(name);
  if(it != groups_.end()) return false;
  it->second->add_property(prop);
  return true;
}

bool Grid_property_group_manager::
remove_property_from_group(Grid_continuous_property* prop, const std::string& name) {
  group_map::iterator it = groups_.find(name);
  if(it != groups_.end()) return false;
  it->second->remove_property(prop);
  return true;
}

std::list<std::string> Grid_property_group_manager::group_types() const{
  group_type_map::const_iterator it = group_type_.begin();
  std::list<std::string> group_type;
  for(; it!= group_type_.begin(); ++it) 
    group_type.push_back(it->first);
  return group_type;
}

std::list<std::string> 
Grid_property_group_manager::group_names(const std::string& type) const{
  group_map::const_iterator it = groups_.begin();
  std::list<std::string> group_name;
  for(; it!= groups_.end(); ++it) {
    if( type == "") group_name.push_back(it->first);
    else if( it->second->type() == type) group_name.push_back(it->first);
  }
  return group_name;
}


unsigned int Grid_property_group_manager::size() const{
  return groups_.size();
}


std::map<std::string, GsTLGridPropertyGroup*>::iterator  Grid_property_group_manager::begin_group(){
	return groups_.begin();
}

std::map<std::string, GsTLGridPropertyGroup*>::iterator Grid_property_group_manager::end_group(){
	return groups_.end();
}


std::map<std::string, GsTLGridPropertyGroup*>::const_iterator  Grid_property_group_manager::begin_group() const{
	return groups_.begin();
}

std::map<std::string, GsTLGridPropertyGroup*>::const_iterator Grid_property_group_manager::end_group() const{
	return groups_.end();
}


/* 
*         -----------------------
*/

//Named_interface* SimulationPropertyGroup::create_new_interface( std::string& name) {
//  return new SimulationPropertyGroup(name);
//}

SimulationPropertyGroup::SimulationPropertyGroup() {
  type_ = "Simulation";
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  root_ = meta_data_.createElement(type_.c_str());
//  meta_data_.appendChild(root_);

}

SimulationPropertyGroup::SimulationPropertyGroup(std::string name) {
  name_ = name;
  type_ = "Simulation";
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  root_ = meta_data_.createElement(type_.c_str());
//  meta_data_.appendChild(root_);
}


void SimulationPropertyGroup::set_algo_command(const std::string& algo_str){
  info_["Algo"] = algo_str;
//  QDomDocument doc_algo;
//  doc_algo.setContent( QString(algo_str.c_str() ) );
//  root_.appendChild( doc_algo.firstChild() );

}

std::string SimulationPropertyGroup::get_algo_command(){
  std::map<std::string, std::string>::const_iterator it = info_.find("Algo");
  if(it == info_.end()) return "";
  return it->second;
 //QDomDocument doc_algo;
 //doc_algo.appendChild(root_.firstChildElement("parameters"));
 //return doc_algo.toString().toStdString();

}

/* Categorical Property group 
*
*/
/*
Named_interface* CategoricalPropertyGroup::create_new_interface( std::string& name) {
  return new CategoricalPropertyGroup(name);
}
*/
CategoricalPropertyGroup::CategoricalPropertyGroup()
{
  type_ = "Categorical";
  cat_def_ = 0;
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  root_ = meta_data_.createElement(type_.c_str());
//  meta_data_.appendChild(root_);

}

CategoricalPropertyGroup::CategoricalPropertyGroup(std::string name)
{
  name_ = name;
  cat_def_ = 0;
  type_ = "Categorical";
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  root_ = meta_data_.createElement("type_.c_str()");
//  meta_data_.appendChild(root_);
}


void 
CategoricalPropertyGroup::set_categorical_definition(std::string cat_def_name){
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_def_name  );

  if(ni.raw_ptr() == 0) 
    cat_def_=0;
	else 
    cat_def_ = dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr()); 
}

void
CategoricalPropertyGroup::set_categorical_definition(const CategoricalPropertyDefinition* cat_def){
  cat_def_  = cat_def;
}


const CategoricalPropertyDefinition* 
CategoricalPropertyGroup::get_categorical_definition() const{
  return cat_def_;
}

std::string CategoricalPropertyGroup::get_categorical_definition_name() const {
  return cat_def_->name();
}



/* Categorical Probability Property group 
*
*/
/*
Named_interface* CategoricalProbabilityPropertyGroup::create_new_interface( std::string& name) {
  return new CategoricalProbabilityPropertyGroup(name);
}
*/
CategoricalProbabilityPropertyGroup::CategoricalProbabilityPropertyGroup()
:CategoricalPropertyGroup(){
  type_ = "CategoricalProbability";
  cat_def_ = 0;
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  root_ = meta_data_.createElement(type_.c_str());
//  meta_data_.appendChild(root_);

}

CategoricalProbabilityPropertyGroup::
CategoricalProbabilityPropertyGroup(std::string name)
:CategoricalPropertyGroup(name)
{
  name_ = name;
  cat_def_ = 0;
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  type_ = "CategoricalIndicator";
//  root_ = meta_data_.createElement("type_.c_str()");
//  meta_data_.appendChild(root_);
}


Categ_non_param_cdf<int>  
CategoricalProbabilityPropertyGroup::get_distribution(int node_id) const{
  std::vector<Grid_continuous_property::property_type> prob = get_vector_data( node_id );
  return Categ_non_param_cdf<int>(prob.size(), prob.begin());
}



/* Indicator Probability  group
*
*/

/*
Named_interface* IndicatorCategoricalPropertyGroup::create_new_interface( std::string& name) {
  return new IndicatorCategoricalPropertyGroup(name);
}
*/
IndicatorCategoricalPropertyGroup::IndicatorCategoricalPropertyGroup()
{
  type_ = "CategoricalIndicator";
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  root_ = meta_data_.createElement(type_.c_str());
//  meta_data_.appendChild(root_);

}

IndicatorCategoricalPropertyGroup::IndicatorCategoricalPropertyGroup(std::string name)
{
  type_ = "CategoricalIndicator";
  name_ = name;
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  type_ = "CategoricalIndicator";
//  root_ = meta_data_.createElement("type_.c_str()");
//  meta_data_.appendChild(root_);
}




int IndicatorCategoricalPropertyGroup::get_category(int node_id) const{
  float cat;
  property_map::const_iterator it = properties_.begin();
  for( ; it!= properties_.end(); ++it) {
    if( it->second->get_value(node_id) == 1 ) {
      return std::distance(properties_.begin(),it );
    }
  }
  return -1;
}

std::string IndicatorCategoricalPropertyGroup::get_category_name(int node_id) const{
  int cat = this->get_category(node_id);
  return cat_def_->get_category_name(cat);
}



/* Group for indicator of continuous property
*
*/

/*
Named_interface* IndicatorContinuousPropertyGroup::create_new_interface( std::string& name) {
  return new IndicatorContinuousPropertyGroup(name);
}
*/
IndicatorContinuousPropertyGroup::IndicatorContinuousPropertyGroup()
{
  type_ = "ContinuousIndicator";
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());

//  root_ = meta_data_.createElement(type_.c_str());
//  meta_data_.appendChild(root_);

}

IndicatorContinuousPropertyGroup::IndicatorContinuousPropertyGroup(std::string name)
{
  name_ = name;
  type_ = "ContinuousIndicator";
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//  root_ = meta_data_.createElement("type_.c_str()");
//  meta_data_.appendChild(root_);
}

void IndicatorContinuousPropertyGroup::set_thresholds(std::vector<float> thresholds){
  thresholds_ = thresholds;
}

std::vector<float> IndicatorContinuousPropertyGroup::get_thresholds() const {
  return thresholds_;
}

Non_param_cdf<>
IndicatorContinuousPropertyGroup::get_distribution(int node_id) const{
  std::vector<Grid_continuous_property::property_type> prob = get_vector_data( node_id );
  return Non_param_cdf<>(thresholds_.begin(),thresholds_.end(), prob.begin() );
}

