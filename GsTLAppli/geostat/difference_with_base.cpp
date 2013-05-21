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
** Author: Alexandre Boucher
** Copyright (C) 2010  Advanced Resources and Risk Technology
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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
**
**********************************************************************/
#include <GsTLAppli/geostat/difference_with_base.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/geostat/utilities.h>

#include <algorithm>


bool DiffProperties::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) 
{

	std::cout << "initializing algorithm DifferenceWithBase \n";

	std::string grid_name = parameters->value( "Grid.value" );
  if( grid_name.empty() ) {
		errors->report("Grid","No grid specified");
    return false;
  }

	grid_ = 0;
	bool ok = geostat_utils::create( grid_, grid_name,
			 "Grid", errors );
	if( !ok || !grid_ ) return false;


  grid_region_.set_temporary_region(
                parameters->value( "Grid.region" ), grid_);


  std::string prop_str = parameters->value( "props.value" );
  if(prop_str.empty()) {
    errors->report("props","No property specified");
    return false;
  }
	std::vector< std::string > prop_name = 
            String_Op::decompose_string(prop_str, ";" );


	for(std::vector< std::string >::iterator it_str = prop_name.begin(); 
    it_str != prop_name.end(); it_str++ ) {
      Grid_continuous_property* prop = grid_->property( *it_str );
      errors->report(!prop, "props","No property exist with that name");
      props_.push_back(prop);
  }
  if( !errors->empty() ) return false;

  prop_str = parameters->value( "prop_base.value" );
  if(prop_str.empty()) {
    errors->report("prop_base","No base property specified");
    return false;
  }
  prop_base_ = grid_->property( prop_str );
  if(  prop_base_ == 0 ) {
    errors->report("prop_base","The base property specified does not exist");
    return false;
  }

  this->extract_parameters(parameters);
  return true;
}


int DiffProperties::execute( GsTL_project* ) { 

  std::vector< Grid_continuous_property* >::const_iterator it_prop = props_.begin();

  GsTLGridPropertyGroup* group;
  std::string group_name = "Difference with "+prop_base_->name();
  group = grid_->get_group(group_name);
  if(group == 0)
    group = grid_->add_group(group_name , "General" );

  for(  ; it_prop!= props_.end(); ++it_prop) {
    std::string name = (*it_prop)->name()+" - " + prop_base_->name();
    Grid_continuous_property*  prop_diff = 
      geostat_utils::add_property_to_grid(grid_, name );
    prop_diff->set_parameters(parameters_);
    if(group) group->add_property(prop_diff);
    grid_->select_property( prop_diff->name() );
    
    Geostat_grid::iterator it = grid_->begin();
    for( ; it!= grid_->end(); ++it) {
      int id = it->node_id();
      if( (*it_prop)->is_informed(id) && prop_base_->is_informed(id) ) {
        it->set_property_value( (*it_prop)->get_value(id) -  prop_base_->get_value(id) );
      }
    }
  }
  return 0;

}

Named_interface* DiffProperties::create_new_interface( std::string& ) {
  return new DiffProperties;
}