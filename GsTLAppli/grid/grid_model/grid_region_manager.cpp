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
** Author: Alexandre Boucher modified from Nicolas Remy
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
Modified from grid_property_manager.cpp
**********************************************************************/

#include <GsTLAppli/grid/grid_model/grid_region_manager.h>
#include <GsTLAppli/utils/string_manipulation.h> 
#include <GsTLAppli/appli/manager_repository.h>

#include <stdlib.h>



//=================================================

bool Grid_region_manager::reNameRegion(std::string & oldName, std::string & newName)
{
	if( oldName.empty() ) return false; 

	Region_map::iterator it_new = regions_map_.find(newName); 	
	Region_map::iterator it = regions_map_.find(oldName); 

	if (it_new != regions_map_.end()) return false;

	if( it != regions_map_.end() )  {
		int id = it->second;
		regions_map_.erase(it);
		regions_map_[newName] = id;
		return true; 
	}
	else 
		return false; 
}

Grid_region_manager::Grid_region_manager( GsTLInt size ) {
  size_ = size;
  selected_region_ = -1;
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
  //SmartPtr<Named_interface> ni = Root::instance()->interface(qitem_model_manager+"/Root");
  //model_ = dynamic_cast<Root_model*>(ni.raw_ptr());
}

Grid_region_manager::~Grid_region_manager() {
  std::vector< GsTLGridRegion* >::iterator it = regions_.begin();
  for( ; it != regions_.end() ; ++it )
    if( *it != 0 )
      delete *it;
}


GsTLGridRegion* 
Grid_region_manager::add_region( const std::string& name ) {

  appli_assert( size_ != 0 );
  Region_map::iterator it = regions_map_.find( name );
  if( it == regions_map_.end() ) {

	if(model_) {
		// Need to find this node in the tree: First go up to the root then down
		std::map< std::string, int >::iterator it = regions_map_.lower_bound(name);
		int row = std::distance( regions_map_.begin(), it);
		model_->begin_insert_item(this,row);
	}
    int new_prop_id = regions_.size();
    regions_map_[name] = new_prop_id;
    GsTLGridRegion *region = new GsTLGridRegion( size_, name );
    region->set_parent_item(this);
    regions_.push_back( region );

    if(model_) model_->end_insert_item();

    return regions_[ new_prop_id ];
  }
  else
    return 0 ; 
}



bool 
Grid_region_manager::remove_region( const std::string& name ) {
  Region_map::iterator it = regions_map_.find( name );
  if( it != regions_map_.end() ) {
    // delete the propery but don't modify the vector of region*
    // ex: if region 2 is deleted, region 3 does not become region 2,
    // but remains region 3.
	if(model_) {
		int row = std::distance( regions_map_.begin(), it);
		model_->begin_remove_item(this,row);
	}
    int ind = it->second;
    delete regions_[ind];
    regions_[ind] = 0;
    regions_map_.erase( it );
    if(model_) model_->end_remove_item();
    return true;
  }
  else
    return false;
}


bool Grid_region_manager::could_conflict( const std::string& name ) {
  // loop through all the string keys and check if they contain "name" as
  // a sub-string
  std::map< std::string, int >::const_iterator it = regions_map_.begin();
  for( ; it != regions_map_.end() ; ++it ) {
    if( it->first.find( name ) != std::string::npos ) return true;
  }

  return false;
}



QString Grid_region_manager::item_type() const{
	return QString("Regions");
}
GsTL_object_item* Grid_region_manager::child(int row){
	Region_map::const_iterator it = regions_map_.begin();
	for(int i=0; i<row; ++i,++it){}
	return regions_[it->second];
}

const GsTL_object_item* Grid_region_manager::child(int row) const{
	Region_map::const_iterator it = regions_map_.begin();
	for(int i=0; i<row; ++i,++it){}
	return regions_[it->second];
}

int Grid_region_manager::childCount() const{
	return regions_map_.size();
}

int Grid_region_manager::columnCount() const{
	return 2;
}

QVariant Grid_region_manager::item_data(int column) const{
	if( column == 0 ) {
		return QString("Regions");
	}
	return QVariant();
}
/*
int Grid_property_manager::row() const{
	if(parent == 0) return 0;

}
*/
