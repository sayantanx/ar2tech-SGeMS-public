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
 * Grid_categorical_property.cpp
 *
 *  Created on: Mar 29, 2010
 *      Author: aboucher
 */

#include <grid/grid_model/grid_categorical_property.h>
#include <appli/manager_repository.h> 
#include <sstream>


Named_interface* create_new_categorical_definition( std::string& name){
if(name == "Default") return new CategoricalPropertyDefinitionDefault;
else 
  return new CategoricalPropertyDefinitionName(name);
}

QColor CategoricalPropertyDefinition::color(std::string name) const{

  int cat_id = this->category_id( name );
  return this->color( cat_id );

}

void CategoricalPropertyDefinition::register_property(const Grid_continuous_property* prop) {
  props_.insert(prop);
}

void CategoricalPropertyDefinition::unregister_property(const Grid_continuous_property* prop) {
  props_.erase(prop);
}

CategoricalPropertyDefinition::property_set::const_iterator 
CategoricalPropertyDefinition::begin_property() const {
  return props_.begin();
}

CategoricalPropertyDefinition::property_set::const_iterator 
CategoricalPropertyDefinition::end_property() const {
  return props_.end();
}

std::string CategoricalPropertyDefinition::get_category_name_from_index(unsigned int index) const{
  int code = this->category_id_from_index(index);
  return this->get_category_name( this->category_id_from_index(index));
}

/*
  ----------------------------------------------
*/

Categorical_color_coding::Categorical_color_coding(std::string name,int code, GsTL_object_item* parent) 
 : GsTL_object_item(parent), code_(code){
   this->item_type("Category coding");
   this->item_name(name.c_str());
//  item_name_ = QString::fromStdString(name);
  color_ = CategoricalPropertyDefinitionDefault::default_color(code);
}

Categorical_color_coding::~Categorical_color_coding(){

}

Qt::ItemFlags Categorical_color_coding::flags(int column) const{
  if(column == 0 ) return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
  else if(column == 0 ) return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled ;

}

bool Categorical_color_coding::set_item_data(QVariant value, int column ){

  if(column==0) {
    item_name_ = value.toString();
    return true;
  }
  QColor color = value.value<QColor>();
  if(color.isValid()) {
    color_ = color;
    return true;
  }
  return false;
  

}

// 
// ------------------------------------------------------------------
//

CategoricalPropertyDefinitionName::CategoricalPropertyDefinitionName() {
	// TODO Auto-generated constructor stub

}

CategoricalPropertyDefinitionName::~CategoricalPropertyDefinitionName() {
	for( int i=0; i< cat_coding_.size(); ++i ) {
    delete cat_coding_[i];
  }
}

CategoricalPropertyDefinitionName::CategoricalPropertyDefinitionName(
		const std::string& name ){
  name_ = name;
}


void CategoricalPropertyDefinitionName::load_from_file(const std::string& filename){
  std::ifstream infile( filename.c_str() );
  if( !infile ) {
	appli_warning( "Cannot open file " << filename );
	return;
  }

  name_ = filename;

  while(!infile.eof()) {
	  std::string name;
	  std::getline(infile, name);
	  if( name.empty() ) break;
    int code = cat_coding_.size();

    cat_coding_[code] = new Categorical_color_coding(name,code, this);

	  //cat_coding_.push_back(new Categorical_color_coding(name,code, this));
  }
  infile.close();
}

std::string CategoricalPropertyDefinitionName::get_category_name(unsigned int id) const{

  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( id );

  if(it == cat_coding_.end()) return "";
  if(it->second == 0) {
    return "";
  }

  return it->second->name().toStdString();

/*
	if(id < cat_coding_.size()  )
    return cat_coding_[id]->name().toStdString();
	else
		return "";
    */
}

std::string CategoricalPropertyDefinitionName::name() const {
  return name_;
}

void CategoricalPropertyDefinitionName::set_definiton_name(std::string name){
  name_ = name;
}


void CategoricalPropertyDefinitionName::set_category_names(std::vector<std::string> names) {

  for(int i=0; i< names.size(); ++i) {
    cat_coding_[i] = new Categorical_color_coding(names[i], i,this);
    //cat_coding_.push_back(new Categorical_color_coding(names[i], i,this));
  }

	//cat_names_ = names;
}
/*
int CategoricalPropertyDefinitionName::add_category(std::string name){
	bool ok = is_category_exist( name );
	if(ok) return -1;
  int code = cat_coding_.size();
  cat_coding_[code] = new Categorical_color_coding(name, code,this);
  //cat_coding_.push_back(new Categorical_color_coding(name, code,this));
	
  //cat_names_.push_back(name);
  //colors_.push_back(CategoricalPropertyDefinitionDefault::default_color(cat_names_.size()-1));
	return code;
}
*/

int CategoricalPropertyDefinitionName::add_category(int code, std::string name){
	bool ok = is_category_exist( code );
	if(ok) return -1;
  cat_coding_[code] = new Categorical_color_coding(name, code,this);

	return code;
}


int CategoricalPropertyDefinitionName::category_id(std::string name) const{
  QString name_cat = name.c_str();

  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.begin();
//	std::vector<Categorical_color_coding*>::const_iterator it = cat_coding_.begin();
	for( ; it != cat_coding_.end(); ++it) {
    if(it->second->name() == name_cat) return it->second->code();
  //  if((*it)->name() == name_cat) return (*it)->code();
	}
  
	return -1;
}

bool CategoricalPropertyDefinitionName::is_sequential_coding() const {
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.begin();

  if(it->first != 0) return false;

  std::advance(it,cat_coding_.size()-1 );
  if(it->first != cat_coding_.size()-1) return false;
  
  return true;

}

int CategoricalPropertyDefinitionName::category_id_from_index(int index ) const{
  if( index >= cat_coding_.size() || index < 0) return -1;

  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.begin();
  std::advance(it,index);
  int code = it->first;
  return it->first;
 
}

int CategoricalPropertyDefinitionName::index_from_category_id(int cat_id ) const{
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find(cat_id);
  if( it == cat_coding_.end() ) return -1; 
  return std::distance(cat_coding_.begin(),it);
}


bool CategoricalPropertyDefinitionName::is_category_exist(std::string name) const {

  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( this->category_id(name) );
  return it != cat_coding_.end();

	//return category_id(name) >= 0;
}

bool CategoricalPropertyDefinitionName::is_category_exist(int id) const {
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( id );
  return it != cat_coding_.end();
}

int CategoricalPropertyDefinitionName::number_of_category() const{
	return cat_coding_.size();
}

bool CategoricalPropertyDefinitionName::rename(int id, std::string new_name){

  std::map<int,Categorical_color_coding*>::iterator it = cat_coding_.find( id );
  if( it == cat_coding_.end() ) return false;;
	
  it->second->name(new_name.c_str());

  return true;
  /*
  bool ok = false;
	if(id < cat_coding_.size()  ) {
    cat_coding_[id]->name(new_name.c_str());
		ok = true;
	}
	return ok;
  */
}

bool CategoricalPropertyDefinitionName::rename_from_index(int index, std::string name){
  return this->rename( this->category_id_from_index(index), name );
}



std::vector<std::string> CategoricalPropertyDefinitionName::category_names() const{
  std::vector<std::string> names;
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.begin();
	//std::vector<Categorical_color_coding*>::const_iterator it = cat_coding_.begin();
	for( ; it != cat_coding_.end(); ++it) {
    names.push_back( it->second->name().toStdString() );
    //names.push_back( (*it)->name().toStdString() );
	}
  return names;
}

std::vector<int> CategoricalPropertyDefinitionName::category_codes() const{
  std::vector<int> codes;
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.begin();
	for( ; it != cat_coding_.end(); ++it) {
    codes.push_back( it->first );
	}
  return codes;
}


QColor CategoricalPropertyDefinitionName::color(unsigned int cat_id) const { 
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( cat_id );
  if(it == cat_coding_.end()) return QColor();
  return it->second->color();
}

float CategoricalPropertyDefinitionName::red(unsigned int cat_id) const {
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( cat_id );
  if(it == cat_coding_.end()) return QColor().redF();
  return it->second->color().redF();
}

float CategoricalPropertyDefinitionName::green(unsigned int cat_id) const {
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( cat_id );
  if(it == cat_coding_.end()) return QColor().greenF();
  return it->second->color().greenF();
}

float CategoricalPropertyDefinitionName::blue(unsigned int cat_id) const {
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( cat_id );
  if(it == cat_coding_.end()) return QColor().blueF();
  return it->second->color().blueF();
}

float CategoricalPropertyDefinitionName::alpha(unsigned int cat_id) const {
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.find( cat_id );
  if(it == cat_coding_.end()) return QColor().alphaF();
  return it->second->color().alphaF();
}


const GsTL_object_item* CategoricalPropertyDefinitionName::child(int row) const {
  std::map<int,Categorical_color_coding*>::const_iterator it = cat_coding_.begin( );
  std::advance(it,row);
  return it->second;
}

/*
std::vector<std::string>::const_iterator 
CategoricalPropertyDefinitionName::begin_category_name() const{
  return cat_names_.begin();
}

std::vector<std::string>::const_iterator 
CategoricalPropertyDefinitionName::end_category_name()const{
  return cat_names_.end();
}
*/
/*
void CategoricalPropertyDefinitionName::color(unsigned int cat_id, QColor color){
  if( colors_.size() < cat_id  ) {
    colors_[cat_id] = color;
  }
}
*/

/*
 * Default Categorical definition: no user-defined name
 * The name are of type "category_0"
 */

CategoricalPropertyDefinitionDefault::CategoricalPropertyDefinitionDefault():current_n_categ_in_use_(10){
  // Build the vector of color
  /*   it would be a better idea to take the list from the colorscale manager but the color are only loaded with the gui library
  SmartPtr<Named_interface> ni =
	  Root::instance()->interface( colorscale_manager+"/default_categorical" );
  Color_scale* colors = dynamic_cast<Color_scale*>( ni.raw_ptr() );
  */
  
  colors_.push_back(QColor(141,211,199));
  colors_.push_back(QColor(255, 255, 179));
  colors_.push_back(QColor(190, 186, 218));
  colors_.push_back(QColor(251, 128, 114));
  colors_.push_back(QColor(128, 177, 211));
  colors_.push_back(QColor(253, 180, 98));
  colors_.push_back(QColor(179, 222, 105));
  colors_.push_back(QColor(252, 205, 229));
  colors_.push_back(QColor(217, 217, 217));
  colors_.push_back(QColor(188, 128, 189));
  colors_.push_back(QColor(204, 235, 197));
  colors_.push_back(QColor(255, 237, 111));
  for( int i=0; i< 100; ++i) {
    QString name = QString("Category %1").arg(i);
    cat_coding_.push_back(new Categorical_color_coding(name.toStdString(),i,this));
  }


}

CategoricalPropertyDefinitionDefault::~CategoricalPropertyDefinitionDefault(){	
  for( int i=0; i< cat_coding_.size(); ++i ) {
    delete cat_coding_[i];
  }
}

//Useful to ensure that all classes use the same default
QColor CategoricalPropertyDefinitionDefault::default_color(int cat_id){
  switch (cat_id%12) {
  case 0: return QColor(141,211,199);
  case 1: return QColor(255, 255, 179);
  case 2: return QColor(190, 186, 218);
  case 3: return QColor(251, 128, 114);
  case 4: return QColor(128, 177, 211);
  case 5: return QColor(253, 180, 98);
  case 6: return QColor(179, 222, 105);
  case 7: return QColor(252, 205, 229);
  case 8: return QColor(217, 217, 217);
  case 9: return QColor(188, 128, 189);
  case 10: return QColor(204, 235, 197);
  case 11: return QColor(255, 237, 111);
  default : return QColor();
  }
}


std::string CategoricalPropertyDefinitionDefault::get_category_name(unsigned int id) const{
  if(id < cat_coding_.size()) {
    return cat_coding_[id]->name().toStdString();
  }
  else {
	  std::stringstream name;
	  name <<"Category "<<id;
	  return name.str();
  }
}

int CategoricalPropertyDefinitionDefault::category_id(std::string name) const{
	if( name.substr(0,9) != "Category " ) return -1;
	int id;
  std::istringstream name_istr(name.substr(9,name.size()-9));

	if (name_istr>>id)
		return id;
	else return -1;



}

bool CategoricalPropertyDefinitionDefault::is_category_exist(std::string name) const {
	return category_id(name) >= 0;
}


std::string CategoricalPropertyDefinitionDefault::name() const {
  return "Default";
}


/*
 *   Grid_categorical_property
 */

Grid_categorical_property::Grid_categorical_property(
		GsTLInt size, const std::string& name,
    const std::string cat_definition_name,
		property_type default_value)
		: Grid_continuous_property(size,name,no_data_value),number_of_categories_(0)
{

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_definition_name  );
  appli_assert( ni );

  cat_definitions_ = 
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  if(cat_definitions_ == 0) {
    ni = Root::instance()->interface( categoricalDefinition_manager+"/Default" );
    cat_definitions_ = 
      dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  }
  cat_definitions_->register_property(this);

}


Grid_categorical_property::Grid_categorical_property(
		const std::string& in_filename, GsTLInt size, const std::string& name,
    const std::string cat_definition_name,
		property_type default_value)
		: Grid_continuous_property(size,name,in_filename, no_data_value),number_of_categories_(0) 
{

  SmartPtr<Named_interface> ni =
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_definition_name  );
//  appli_assert( ni );

  cat_definitions_ =
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  if(cat_definitions_ == 0) {
    ni = Root::instance()->interface( categoricalDefinition_manager+"/Default" );
    cat_definitions_ =
      dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  }
  cat_definitions_->register_property(this);

}



/*
Grid_categorical_property::Grid_categorical_property( 
        Grid_continuous_property* cont_prop, const std::string& name,
				const std::string cat_definition_name = "Default",
        property_type default_value = no_data_value ) 
        :Grid_continuous_property(cont_prop->size(), cont_prop->name(), no_data_value)
{

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_definition_name  );
  appli_assert( ni );

  cat_definitions_ = 
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  if(cat_definitions_ == 0) {
    ni = Root::instance()->interface( categoricalDefinition_manager+"/Default" );
    cat_definitions_ = 
      dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  }
  cat_definitions_->register_property(this);

}
*/



Grid_categorical_property::~Grid_categorical_property(){
    if(cat_definitions_)  cat_definitions_->unregister_property(this);
  }

bool Grid_categorical_property::set_category_definition( std::string cat_definition_name){

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_definition_name  );
//  Manager* cdef_manager = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( ni );

	CategoricalPropertyDefinition* cat_def = 
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  
  if(cat_definitions_ == cat_def) return true;
  if(cat_def == 0) return false;
  
  cat_definitions_->unregister_property(this);
  cat_definitions_ = cat_def;
  cat_definitions_->register_property(this);
  return true;
}

bool Grid_categorical_property
::set_category_definition( CategoricalPropertyDefinition* cat_definition){
  if(cat_definition == 0) return false;
  if(cat_definitions_ == cat_definition) return true;

  cat_definitions_->unregister_property(this);
  cat_definitions_ = cat_definition;
  cat_definitions_->register_property(this);
  return true;
}

/*
void Grid_categorical_property
::compute_number_of_category(){
	Grid_continuous_property::const_iterator it = this->begin();
	number_of_categories_ = 0;
	for( ; it!=this->end(); ++it) {
		if(*it > number_of_categories_) number_of_categories_ = *it;
	}
	number_of_categories_++;
}
*/
int Grid_categorical_property
::compute_number_of_category() const{
	Grid_continuous_property::const_iterator it = this->begin();

  std::set<int> category_codes;
	for( ; it!=this->end(); ++it) {
		category_codes.insert(*it );
	}
  return category_codes.size();

/*	int ncat = 0;
	for( ; it!=this->end(); ++it) {
		if(*it > ncat) ncat = *it;
	}
	return ncat+1;
  */
}

QString Grid_categorical_property::item_type() const{
	return "CategoricalProperty";
}
