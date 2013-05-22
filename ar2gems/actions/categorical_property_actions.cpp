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



#include <actions/categorical_property_actions.h> 
#include <actions/defines.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <appli/manager_repository.h>
#include <appli/project.h>
#include <grid/grid_model/geostat_grid.h> 
#include <grid/grid_model/grid_property.h>
#include <grid/grid_model/grid_categorical_property.h> 



//================================================
/* Convert_continuous_to_categorical_property grid_name::prop1::[prop2::]
* will copy convert the property from continuous to categorical
*/
bool Convert_continuous_to_categorical_property::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() <2 ) {
    errors->report( "some parameters are missing, Needs at least two parameters (grid + property)" );
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() );
    return false;
  }

  for(int i=1; i<params.size(); ++i) {
    Grid_categorical_property* cprop = grid->categorical_property(params[i]);
    if(cprop) continue;
    Grid_continuous_property* prop = grid->property(params[i]);
    if(prop == 0) continue;
    std::set<int> cat_codes;
    std::string prop_name = prop->name()+" - categorical";
    cprop = grid->add_categorical_property(prop_name);
    while(!cprop) {
      prop_name.append("_0");
      cprop = grid->add_categorical_property(prop_name);
    }
    for(int nodeid=0; nodeid < prop->size(); ++nodeid) {
      if( prop->is_informed(nodeid)) {
        int code = static_cast<int>(prop->get_value(nodeid));
        cprop->set_value(code,nodeid);
        cat_codes.insert(code);
      }
    }
    // Check is sequential coding
    bool is_sequential_coding = false;
    std::set<int>::iterator it = cat_codes.begin();
    if( *it == 0  ) {
      std::advance(it, cat_codes.size()-1);
      if( *it == cat_codes.size()-1 ) {
        is_sequential_coding = true;
      }
      
    }

    if( !is_sequential_coding  ) {  // Need to build a categorical definition
      CategoricalPropertyDefinitionName* cat_def = 0;
      std::string catdef_name = grid->name()+"-"+prop->name();
      while(!cat_def) {
        SmartPtr<Named_interface> ni = Root::instance()->new_interface( "categoricaldefinition://"+catdef_name,categoricalDefinition_manager +"/"+catdef_name );
        cat_def = dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
          if(!cat_def) catdef_name.append("_0");
      }

      std::set<int>::iterator it_cat_codes = cat_codes.begin();
      for( int code; it_cat_codes != cat_codes.end(); ++code, ++it_cat_codes) {
        cat_def->add_category(*it_cat_codes, QString("Code %1").arg(*it_cat_codes).toStdString() );
      }
      cprop->set_category_definition( cat_def );
    }

  }

  proj->update( params[0] );
  return true;
}


bool Convert_continuous_to_categorical_property::exec() {
  return true;
}


Named_interface*
Convert_continuous_to_categorical_property::create_new_interface( std::string& ) {
  return new Convert_continuous_to_categorical_property;
}




//================================================
/* Convert_continuous_to_categorical_property grid_name::prop1::[prop2::]
* will copy convert the property from continuous to categorical
*/
bool Convert_categorical_property_to_sequential_coding::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 2 ) {
    errors->report( "Some parameters are missing, needs two paramters (grid and categorical property)" );
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() );
    return false;
  }

  Grid_categorical_property* cprop = grid->categorical_property(params[1]);
  if(cprop->get_category_definition()->is_sequential_coding()) return true;

  const CategoricalPropertyDefinitionName* cdef = dynamic_cast<const CategoricalPropertyDefinitionName*>(cprop->get_category_definition());
  if(!cdef) {
    errors->report( "Error with the categorical definition" );
    return false;
  }
  std::vector<int> current_codes = cdef->category_codes();

  int ncat = cdef->number_of_category();
  std::string sequential_catdef_name = cdef->name() + " - Sequential";

// Check if the categorical definition exist
  SmartPtr<Named_interface> ni = Root::instance()->interface( categoricalDefinition_manager+"/"+sequential_catdef_name );

  CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

  while(!cat_def) {
    sequential_catdef_name.append(" "+cprop->name());
    SmartPtr<Named_interface> ni = Root::instance()->new_interface( "categoricaldefinition://"+sequential_catdef_name,categoricalDefinition_manager +"/"+sequential_catdef_name );
    cat_def = dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

    if(cat_def) {
 

      for(int i=0; i< ncat; ++i) {
        QString new_cat_name = QString("%1 (code:%2)").arg(cdef->get_category_name(current_codes[i]).c_str()).arg(current_codes[i]);
        cat_def->add_category(i, new_cat_name.toStdString() );
      }
    }
  }

  //Get the new value
  std::map<int,int> map_codes;
  for(int i=0; i< current_codes.size(); ++i ) {
    map_codes[current_codes[i]] = i;
  }

  std::string prop_name = cprop->name()+" sequential";
  Grid_categorical_property* seq_cprop = grid->add_categorical_property(cprop->name()+" sequential",cdef->name());
  while(!seq_cprop) {
    prop_name.append("_0");
    seq_cprop = grid->add_categorical_property(prop_name);
  }

  
  for(int i=0; i < cprop->size(); ++i) {
    if( !cprop->is_informed(i) ) continue;

    int code = cprop->get_value(i);
    std::map<int,int>::const_iterator it = map_codes.find(code);
    if( it == map_codes.end()  ) continue;
    seq_cprop->set_value(static_cast<Grid_continuous_property::property_type>(it->second),i);
  }
  seq_cprop->set_category_definition(cat_def);
  proj->update( params[0] );
  return true;
}


bool Convert_categorical_property_to_sequential_coding::exec() {
  return true;
}


Named_interface*
Convert_categorical_property_to_sequential_coding::create_new_interface( std::string& ) {
  return new Convert_categorical_property_to_sequential_coding;
}




//================================================
/* Convert_continuous_to_categorical_property grid_name::prop1::[prop2::]
* will copy convert the property from continuous to categorical
*/
bool Convert_definition_to_sequential_coding::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 1 ) {
    errors->report( "Only needs one parameter (categorical definition to be set to sequential)" );
    return false;
  }

  std::string current_cat_def_name = params[0];

  // Check if the categorical definition exist
  SmartPtr<Named_interface> ni = Root::instance()->interface( categoricalDefinition_manager+"/"+current_cat_def_name );

  CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

  if(cat_def == 0) {    
    errors->report( "No definition called "+current_cat_def_name+" was found" );
    return false;
  }

  //call the action  Convert_categorical_property_to_sequential_coding
  Convert_categorical_property_to_sequential_coding sequential_code;

  CategoricalPropertyDefinition::property_set::const_iterator it = cat_def->begin_property();

  for( ; it != cat_def->end_property(); ++it ) {
    QString parameters = QString("%1::%2").arg((*it)->grid_name().c_str()).arg((*it)->name().c_str());
    std::string parameter_str = parameters.toStdString();
    bool ok = sequential_code.init( parameter_str, proj, errors );
    if (!ok) return false;
  }

  proj->update( params[0] );
  return true;
}


bool Convert_definition_to_sequential_coding::exec() {
  return true;
}


Named_interface*
Convert_definition_to_sequential_coding::create_new_interface( std::string& ) {
  return new Convert_definition_to_sequential_coding;
}
