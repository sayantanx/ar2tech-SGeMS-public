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



#include <GsTLAppli/actions/categorical_definition_actions.h> 
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h> 

Named_interface* New_categorical_definition::create_new_interface( std::string& ) {
  return new New_categorical_definition;
}

bool New_categorical_definition::init( std::string& parameters, GsTL_project* proj,
                                      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 3 ) {
    errors->report( "Must have at least 3 parameters, name of the definition and at least two categories" );
    return false;
  }

  std::string def_name = params[0];
  int n_cat = params.size()-1;
// Check if the definition already exist
   SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+def_name );
  if( ni ) {
    errors->report("There is already a categorical definition named "+def_name );
    return false;
  }
  ni = Root::instance()->new_interface( "categoricaldefinition://"+def_name,
                                         categoricalDefinition_manager +"/"+def_name );
  CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

  for(int i=1; i<params.size(); i++ ) {
    cat_def->add_category(i-1, params[i]);
  }
  
  return true;

}

bool New_categorical_definition::exec()  {
  return true;
}


/*
        -------------------------------
*/


Named_interface* New_alpha_numerical_categorical_definition::create_new_interface( std::string& ) {
  return new New_alpha_numerical_categorical_definition;
}

bool New_alpha_numerical_categorical_definition::init( std::string& parameters, GsTL_project* proj,
                                      Error_messages_handler* errors ) {

  
  QString param_string =   parameters.c_str();                                      
  QStringList params = param_string.split( Actions::separator.c_str() );

  if( params.size() < 6 ) {
    errors->report( "Must have at least 6 parameters, name of the definition, number of categories (>=2) ans at least two categorie names and code" );
    return false;
  }

  bool ok;
  int ncat = params.at(1).toInt( &ok );
  if( !ok ) {
    errors->report( "The second parameter must be an integer" );
    return false;
  }

  if(params.size() != ncat*2 +2 ) {
    errors->report( "The number of category entered and the categories listed does not match" );
    return false;
  }

  // get the list of numercial code
  std::vector<int> codes;
  for(int i=ncat+2; i<params.size(); i++ ) {
    int code = params.at(i).toInt( &ok );
    if( !ok ) {
      QString error_mess = QString("The %1th code could not be converted to an integer").arg(i);
      errors->report( error_mess.toStdString() );
      return false;
    }
    codes.push_back(code);
  }



  std::string def_name = params.at(0).toStdString();

   SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+def_name );
  if( ni ) {
    errors->report("There is already a categorical definition named "+def_name );
    return false;
  }
  ni = Root::instance()->new_interface( "categoricaldefinition://"+def_name,
                                         categoricalDefinition_manager +"/"+def_name );
  CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

  for(int i=0; i<ncat; i++ ) {
    cat_def->add_category(codes[i],params.at(i+2).toStdString());
  }
  
  return true;

}

bool New_alpha_numerical_categorical_definition::exec()  {
  return true;
}


/*
        -------------------------------
*/


Named_interface* Assign_categorical_definition::create_new_interface( std::string& ) {
  return new Assign_categorical_definition;
}

bool Assign_categorical_definition::init( std::string& parameters, GsTL_project* proj,
                                      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 2 ) {
    errors->report( "Must have at least 3 parameters, name of the definition, the grid  and at least one property" );
    return false;
  }

// Check if the categorical definition exist
  SmartPtr<Named_interface> ni = Root::instance()->interface( categoricalDefinition_manager+"/"+params[0] );

  CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
  if( !cat_def ) {
    errors->report("No definition named "+params[0] );
    return false;
  }

// Check if the grid exist
  ni = Root::instance()->interface( gridModels_manager + "/" + params[1] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( ni.raw_ptr() );
  if( !grid ) {
    errors->report("No grid named "+params[1] );
    return false;
  }

  for(int i=2;i<params.size(); ++i) {
    Grid_categorical_property* prop = grid->categorical_property(params[i]);
    if(!prop) continue;
    prop->set_category_definition( cat_def );
  }
  return true;
}
 

bool Assign_categorical_definition::exec()  {
  return true;
}
