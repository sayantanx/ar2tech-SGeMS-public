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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
** Modeling Software (GEMS)
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

#include <GsTLAppli/grid/grid_model/grid_property_manager.h>
#include <GsTLAppli/utils/string_manipulation.h> 
#include <GsTLAppli/appli/manager_repository.h>

#include <stdlib.h>
#include <GsTLAppli/grid/grid_model/grid_property_manager.h>


const std::string MultiRealization_property::separator( "__real" );

MultiRealization_property::MultiRealization_property()
  : size_( 0 ),
    prop_manager_( 0 ),
    group_(0){
	SmartPtr<Named_interface> ni =
			Root::instance()->interface( categoricalDefinition_manager+"/Default"  );
  definition_=
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
}

MultiRealization_property::MultiRealization_property( const std::string& name,
				  Grid_property_manager* manager )
  : name_( name ),
    prop_manager_( manager ),
    group_(0){
  size_ = 0;

  SmartPtr<Named_interface> ni =
  		Root::instance()->interface( categoricalDefinition_manager+"/Default"  );
  definition_=
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());

}

MultiRealization_property::MultiRealization_property( const std::string& name,
				  Grid_property_manager* manager,
          CategoricalPropertyDefinition* cat_definition)
  : name_( name ),
    prop_manager_( manager ),
    group_(0){
  size_ = 0;
  if(cat_definition == 0) {
    SmartPtr<Named_interface> ni =
    		Root::instance()->interface( categoricalDefinition_manager+"/Default"  );
    definition_=
      dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  }
  else
  	definition_ = cat_definition;
}

MultiRealization_property::
MultiRealization_property( const MultiRealization_property& rhs ) {
  name_ = rhs.name_;
  size_ = rhs.size_;
  prop_manager_ = rhs.prop_manager_;
  definition_ = rhs.definition_;
  group_ = rhs.group_;
}

MultiRealization_property& 
MultiRealization_property::operator = ( const MultiRealization_property& rhs ) {
  name_ = rhs.name_;
  size_ = rhs.size_;
  prop_manager_ = rhs.prop_manager_;
  definition_ = rhs.definition_;
  group_ = rhs.group_;

  return *this;
}


GsTLGridProperty* MultiRealization_property::new_realization() {
  // if there was already a realization, don't keep it loaded in memory
  // and swap it to disk
  if( size_ > 0 ) {
    GsTLGridProperty* previous_real = 
      prop_manager_->get_property( name_ + separator + 
				   String_Op::to_string( size_-1 ) );
    previous_real->swap_to_disk();
  }
  
  GsTLGridProperty* new_real = 
    prop_manager_->add_property( name_ + separator +
				 String_Op::to_string( size_ ) );
  if( new_real )
    size_++;

  if(group_) group_->add_property( new_real );
  
  return new_real;
}

GsTLGridProperty* MultiRealization_property::realization( int id ) {
  if( id >= size_ )
    return 0;

  return prop_manager_->get_property( name_ + MultiRealization_property::separator +
                                      String_Op::to_string( id ) );
}

const GsTLGridProperty* MultiRealization_property::realization( int id ) const {
  if( id >= size_ )
    return 0;

  return prop_manager_->get_property( name_ + MultiRealization_property::separator +
                                      String_Op::to_string( id ) );

}


void MultiRealization_property::
set_category_definition(CategoricalPropertyDefinition* definition) {
  definition_= definition;
}


GsTLGridCategoricalProperty* MultiRealization_property::new_categorical_realization() {
  // if there was already a realization, don't keep it loaded in memory
  // and swap it to disk
  if( size_ > 0 ) {
    GsTLGridCategoricalProperty* previous_real = 
      prop_manager_->get_categorical_property( name_ + separator + 
				   String_Op::to_string( size_-1 ) );
    previous_real->swap_to_disk();
  }
  
  GsTLGridCategoricalProperty* new_real = 
    prop_manager_->add_categorical_property( name_ + separator +
				 String_Op::to_string( size_ ), definition_->name() );
  if( new_real )
    size_++;
  
  if(group_) group_->add_property( new_real );
  return new_real;
}

GsTLGridCategoricalProperty* MultiRealization_property::categorical_realization( int id ) {
  if( id >= size_ )
    return 0;

  return prop_manager_->get_categorical_property( name_ + MultiRealization_property::separator +
                                      String_Op::to_string( id ) );
}

const GsTLGridCategoricalProperty* MultiRealization_property::categorical_realization( int id ) const {
  if( id >= size_ )
    return 0;

  return prop_manager_->get_categorical_property( name_ + MultiRealization_property::separator +
                                      String_Op::to_string( id ) );

}

void MultiRealization_property::set_group( GsTLGridPropertyGroup* group){
  group_ = group;
}


//=================================================

bool Grid_property_manager::reNameProperty(std::string & oldName, std::string & newName)
{
	if( oldName.empty() ) return false; 

	Property_map::iterator it_new = properties_map_.find(newName); 	
	Property_map::iterator it = properties_map_.find(oldName); 

	if (it_new != properties_map_.end()) return false;

	if( it != properties_map_.end() )  {
		int id = it->second;
		properties_map_.erase(it);
		properties_map_[newName] = id;
		return true; 
	}
	else 
		return false; 
}

Grid_property_manager::Grid_property_manager( GsTLInt size ) {
  size_ = size;
  selected_property_ = -1;

  //SmartPtr<Named_interface> ni = Root::instance()->interface(qitem_model_manager+"/Root");
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
}

Grid_property_manager::~Grid_property_manager() {

  while(!properties_map_.empty()) {
    std::map< std::string, int >::iterator it = properties_map_.begin();
    int ind = it->second;


 //   The removal of the membership is done by the destructor.
    if(model_) {
    	model_->begin_remove_item(this,0);
    }

    delete properties_[ind];
    properties_[ind] = 0;
    properties_map_.erase( it );
    if(model_) model_->end_remove_item();
  }

/*
  std::vector< GsTLGridProperty* >::iterator it = properties_.begin();
  for( ; it != properties_.end() ; ++it )
    if( *it != 0 ) {
      if(model_) {
    	  model_->begin_remove_item(this,0);
      }
      delete *it;
      if(model_) model_->end_remove_item();
    }
*/
}


GsTLGridProperty* 
Grid_property_manager::add_property( const std::string& name ) {

  appli_assert( size_ != 0 );
  Property_map::iterator it = properties_map_.find( name );
  if( it == properties_map_.end() ) {
    int new_prop_id = properties_.size();
    GsTLGridProperty* prop = new GsTLGridProperty( size_, name );
    prop->set_parent_item(this);
    // Check if the property has been correctly created
    if(prop->size() == 0 ) {
        delete prop;
        GsTLcerr << "The property "<<name<<" could not be created,\n"<<
          "probably running out of memory" << gstlIO::end;
        return 0;

    }


// Inform the model that a new property is added
    if(model_) {
    	// Need to find this node in the tree: First go up to the root then down
    	std::map< std::string, int >::iterator it = properties_map_.lower_bound(name);
    	int row = std::distance( properties_map_.begin(), it);
    	model_->begin_insert_item(this,row);

    }
    properties_map_[name] = new_prop_id;
    properties_.push_back( prop );

    if(model_) model_->end_insert_item();

    // if no property was selected before, select the first one in the list
    if( selected_property_ == -1 )
      selected_property_ = new_prop_id;

    return properties_[ new_prop_id ];
  }
  else
    return 0 ; 
}


GsTLGridProperty*
Grid_property_manager::add_property_from_disk( const std::string& name, const std::string& filename ) {

  appli_assert( size_ != 0 );
  Property_map::iterator it = properties_map_.find( name );
  if( it == properties_map_.end() ) {
    int new_prop_id = properties_.size();
    GsTLGridProperty* prop = new GsTLGridProperty( size_, name, filename );
    prop->set_parent_item(this);
    // Check if the property has been correctly created
    if(prop->size() == 0 ) {
        delete prop;
        GsTLcerr << "The property "<<name<<" could not be created,\n"<<
          "probably running out of memory" << gstlIO::end;
        return 0;

    }

    if(model_) {
    	// Need to find this node in the tree: First go up to the root then down
    	std::map< std::string, int >::iterator it = properties_map_.lower_bound(name);
    	int row = std::distance( properties_map_.begin(), it);
    	model_->begin_insert_item(this,row);
    }

    properties_map_[name] = new_prop_id;
    properties_.push_back( prop );

    if(model_) model_->end_insert_item();

    // if no property was selected before, select the first one in the list
    if( selected_property_ == -1 )
      selected_property_ = new_prop_id;

    return properties_[ new_prop_id ];
  }
  else
    return 0 ;
}



GsTLGridWeightProperty* 
Grid_property_manager::add_weight_property( const std::string& name ) {

  appli_assert( size_ != 0 );
  Property_map::iterator it = properties_map_.find( name );
  if( it == properties_map_.end() ) {
    int new_prop_id = properties_.size();
    GsTLGridWeightProperty* prop = new GsTLGridWeightProperty( size_, name );
    prop->set_parent_item(this);
    // Check if the property has been correctly created
    if(prop->size() == 0 ) {
        delete prop;
        GsTLcerr << "The property "<<name<<" could not be created,\n"<<
          "probably running out of memory" << gstlIO::end;
        return 0;

    }


// Inform the model that a new property is added
    if(model_) {
    	// Need to find this node in the tree: First go up to the root then down
    	std::map< std::string, int >::iterator it = properties_map_.lower_bound(name);
    	int row = std::distance( properties_map_.begin(), it);
    	model_->begin_insert_item(this,row);

    }
    properties_map_[name] = new_prop_id;
    properties_.push_back( prop );

    if(model_) model_->end_insert_item();

    // if no property was selected before, select the first one in the list
    if( selected_property_ == -1 )
      selected_property_ = new_prop_id;

    return dynamic_cast<GsTLGridWeightProperty*>( properties_[ new_prop_id ]);
  }
  else
    return 0 ; 
}


GsTLGridWeightProperty*
Grid_property_manager::add_weight_property_from_disk( const std::string& name, const std::string& filename ) {

  appli_assert( size_ != 0 );
  Property_map::iterator it = properties_map_.find( name );
  if( it == properties_map_.end() ) {
    int new_prop_id = properties_.size();
    GsTLGridWeightProperty* prop = new GsTLGridWeightProperty( size_, name, filename );
    prop->set_parent_item(this);
    // Check if the property has been correctly created
    if(prop->size() == 0 ) {
        delete prop;
        GsTLcerr << "The property "<<name<<" could not be created,\n"<<
          "probably running out of memory" << gstlIO::end;
        return 0;

    }

    if(model_) {
    	// Need to find this node in the tree: First go up to the root then down
    	std::map< std::string, int >::iterator it = properties_map_.lower_bound(name);
    	int row = std::distance( properties_map_.begin(), it);
    	model_->begin_insert_item(this,row);
    }

    properties_map_[name] = new_prop_id;
    properties_.push_back( prop );

    if(model_) model_->end_insert_item();

    // if no property was selected before, select the first one in the list
    if( selected_property_ == -1 )
      selected_property_ = new_prop_id;

    return dynamic_cast<GsTLGridWeightProperty*>( properties_[ new_prop_id ] );
  }
  else
    return 0 ;
}



GsTLGridCategoricalProperty*
Grid_property_manager::add_categorical_property( const std::string& name,
                                                const std::string definition_name) {

  appli_assert( size_ != 0 );
  Property_map::iterator it = properties_map_.find( name );
  if( it == properties_map_.end() ) {
    int new_prop_id = properties_.size();
    GsTLGridProperty* prop = new GsTLGridCategoricalProperty( size_, name, definition_name);
    prop->set_parent_item(this);

    // Check if the property has been correctly created
    if(prop->size() == 0 ) {
        delete prop;
        GsTLcerr << "The property "<<name<<" could not be created,\n"<<
          "probably running out of memory" << gstlIO::end;
        return 0;

    }

    if(model_) {
    	// Need to find this node in the tree: First go up to the root then down
    	std::map< std::string, int >::iterator it = properties_map_.lower_bound(name);
    	int row = std::distance( properties_map_.begin(), it);
    	model_->begin_insert_item(this,row);
    }
    properties_map_[name] = new_prop_id;
    properties_.push_back( prop );
    if(model_) model_->end_insert_item();
    //properties_.push_back( new GsTLGridProperty( size_, name ) );

    // if no property was selected before, select the first one in the list
    if( selected_property_ == -1 )
      selected_property_ = new_prop_id;

    return dynamic_cast<GsTLGridCategoricalProperty*>(properties_[ new_prop_id ]);
  }
  else
    return 0 ;
}



GsTLGridCategoricalProperty*
Grid_property_manager::add_categorical_property_from_disk( const std::string& name,
																								const std::string& filename,
                                                const std::string definition_name) {

  appli_assert( size_ != 0 );
  Property_map::iterator it = properties_map_.find( name );
  if( it == properties_map_.end() ) {
    int new_prop_id = properties_.size();
    GsTLGridProperty* prop = new GsTLGridCategoricalProperty(filename, size_, name, definition_name);
    prop->set_parent_item(this);

    // Check if the property has been correctly created
    if(prop->size() == 0 ) {
        delete prop;
        GsTLcerr << "The property "<<name<<" could not be created,\n"<<
          "probably running out of memory" << gstlIO::end;
        return 0;

    }

    if(model_) {
    	// Need to find this node in the tree: First go up to the root then down
    	std::map< std::string, int >::iterator it = properties_map_.lower_bound(name);
    	int row = std::distance( properties_map_.begin(), it);
    	model_->begin_insert_item(this,row);
    }
    properties_map_[name] = new_prop_id;
    properties_.push_back( prop );
    if(model_) model_->end_insert_item();
    //properties_.push_back( new GsTLGridProperty( size_, name ) );

    // if no property was selected before, select the first one in the list
    if( selected_property_ == -1 )
      selected_property_ = new_prop_id;

    return dynamic_cast<GsTLGridCategoricalProperty*>(properties_[ new_prop_id ]);
  }
  else
    return 0 ;
}

bool 
Grid_property_manager::remove_property( const std::string& name ) {
  Property_map::iterator it = properties_map_.find( name );
  if( it != properties_map_.end() ) {
    // delete the propery but don't modify the vector of property*
    // ex: if property 2 is deleted, property 3 does not become property 2,
    // but remains property 3.
    int ind = it->second;
 //   The removal of the membership is done by the destructor.
    if(model_) {
    	int row = std::distance(properties_map_.begin(),it);
    	model_->begin_remove_item(this,row);
    }

    delete properties_[ind];
    properties_[ind] = 0;
    properties_map_.erase( it );
    if(model_) model_->end_remove_item();

    // Check if the property was use as the selected selected_properties
    if( selected_property_ == ind ) {
      if(properties_map_.size() == 0) selected_property_=0;
      else selected_property_ = properties_map_.begin()->second;

    }

    return true;
  }
  else
    return false;
}


MultiRealization_property* 
Grid_property_manager::new_multireal_property( 
      const std::string& name,
      CategoricalPropertyDefinition* definition ) {
  // Make sure the requested name does not conflict with another one
  // If it does, append "_0" to the requested name
  const std::string suffix = MultiRealization_property::separator;

  std::string new_name = name;
  std::string conflict = new_name + suffix;
  while( could_conflict( conflict ) ) { //properties_map_.find( conflict ) != properties_map_.end() ) {
    new_name += "_0";
    conflict = new_name + suffix;
  }

  multireal_properties_[ new_name ] = MultiRealization_property( new_name, this, definition );
  MultirealProperty_map::iterator found = multireal_properties_.find( new_name );
  appli_assert( found != multireal_properties_.end() );
  
  return &found->second;
}


MultiRealization_property* 
Grid_property_manager::multireal_property( const std::string& name ) {
  MultirealProperty_map::iterator found = multireal_properties_.find( name );
  if( found != multireal_properties_.end() )
    return &found->second;
  else 
    return 0;
}
  


bool Grid_property_manager::could_conflict( const std::string& name ) {
  // loop through all the string keys and check if they contain "name" as
  // a sub-string
  std::map< std::string, int >::const_iterator it = properties_map_.begin();
  for( ; it != properties_map_.end() ; ++it ) {
    if( it->first.find( name ) != std::string::npos ) return true;
  }

  return false;
}


QString Grid_property_manager::item_type() const{
	return QString("Properties");
}
GsTL_object_item* Grid_property_manager::child(int row){
  if( properties_map_.empty() ) return 0;
	Property_map::const_iterator it = properties_map_.begin();
	for(int i=0; i<row; ++i,++it){}
	return properties_[it->second];
}

int Grid_property_manager::childCount() const{
	return properties_map_.size();
}

int Grid_property_manager::columnCount() const{
	return 2;
}

QVariant Grid_property_manager::item_data(int column) const{
	if( column == 0 ) {
		return QString("Properties");
	}
	return QVariant();
}

int Grid_property_manager::row() const{
	return 0;
}
