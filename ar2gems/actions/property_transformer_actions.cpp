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



#include <actions/property_transformer_actions.h>
#include <grid/property_transformer.h>
#include <actions/defines.h>
#include <utils/string_manipulation.h>
#include <utils/error_messages_handler.h>
#include <appli/manager_repository.h>
#include <appli/project.h>
#include <grid/grid_model/geostat_grid.h> 
#include <grid/grid_model/grid_property.h>
#include <grid/grid_model/grid_property_set.h>


/**
* New_pca_transformer
*/

Named_interface* New_pca_transformer::create_new_interface( std::string& ){
  return new New_pca_transformer;
}
   
//format name::grid::region::prop1::prop2::
// if no region is needed then either enter 0 or blank ::::
bool New_pca_transformer::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   true );

  if( params.size() < 5 ) {
    errors->report( "Must have at least 5 parameters, name of the transformer, name of the grid, region (if any) and at elast two properties" );
    return false;
  }

  //Check if there is already a transformer with that name
  SmartPtr<Named_interface> ni = Root::instance()->interface( transformer_manager + "/" + params[0]);
  if(ni.raw_ptr() != 0) {
    errors->report( "The transformer "+params[0]+" already exist" );
    return false;
  }

  // Get the grid
  std::string grid_name = params[1];
   ni = Root::instance()->interface( gridModels_manager + "/" + grid_name);
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( ni.raw_ptr() );
  if(!grid)  {
    errors->report( "The grid "+params[1]+" does not exist" );
    return false;
  }

  Grid_region* region = 0;
  if( !params[2].empty() )  {
    region = grid->region(params[2]);
  }
  std::vector<const Grid_continuous_property*> props;
  for(int i=3; i<params.size(); ++i) {
    Grid_continuous_property* prop = grid->property(params[i]);
    if(prop==0) {
      errors->report( "The property "+params[i]+" does not exist" );
      return false;
    }
    props.push_back(prop);
  }

  ni = Root::instance()->new_interface( "PCA_transformer://",
				  transformer_manager + "/" + params[0] );
  PCA_transformer* pc_trans = dynamic_cast<PCA_transformer*>(ni.raw_ptr());
  if(pc_trans==0) {
     errors->report( "The transformer could not be find" );
     return false;
  }
  bool ok = pc_trans->initialize(props, region);

  if(!ok) {
    Root::instance()->delete_interface(transformer_manager + "/" + params[0]);
    //Need to remove the pc_trans from the manager
    return false;
  }
  else 
    return true;

}

bool New_pca_transformer::exec(){
  return true;
}
 

/*
-------------------------
*/

Named_interface* Multivariate_forward_transform::create_new_interface( std::string& ){
  return new Multivariate_forward_transform;
}
   
//format name::grid::region::prop1::prop2::
// if no region is needed then either enter 0 or blank ::::
bool Multivariate_forward_transform::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   true );

  if( params.size() < 5 ) {
    errors->report( "Must have at least 5 parameters, name of the transformer, name of the grid, region (if any) and at least two properties" );
    return false;
  }

  //Check if there is already a transformer with that name
  SmartPtr<Named_interface> ni = Root::instance()->interface( transformer_manager + "/" + params[0]);
  if(ni.raw_ptr() == 0) {
    errors->report( "The transformer "+params[0]+" does not exist" );
    return false;
  }
  Multi_property_transformer* mvariate_trans = dynamic_cast<Multi_property_transformer*>(ni.raw_ptr());
  if(mvariate_trans == 0) {
    errors->report( "The transformer "+params[0]+" is not a multivariate transformer" );
    return false;
  }

  // Get the grid
  std::string grid_name = params[1];
   ni = Root::instance()->interface( gridModels_manager + "/" + grid_name);
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( ni.raw_ptr() );
  if(!grid)  {
    errors->report( "The grid "+params[1]+" does not exist" );
    return false;
  }

  Grid_region* region = 0;
  if( !params[2].empty() )  {
    region = grid->region(params[2]);
  }
  std::vector<const Grid_continuous_property*> props;
  for(int i=3; i<params.size(); ++i) {
    Grid_continuous_property* prop = grid->property(params[i]);
    if(prop==0) {
      errors->report( "The property "+params[i]+" does not exist" );
      return false;
    }
    props.push_back(prop);
  }


  mvariate_trans->forward_transform(grid, props, region);
  return true;



}

bool Multivariate_forward_transform::exec(){
  return true;
}
 


/*
-------------------------
*/

Named_interface* Multivariate_back_transform::create_new_interface( std::string& ){
  return new Multivariate_back_transform;
}
   
//format name::grid::region::prop1::prop2::
// if no region is needed then either enter 0 or blank ::::
bool Multivariate_back_transform::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   true );

  if( params.size() < 5 ) {
    errors->report( "Must have at least 5 parameters, name of the transformer, name of the grid, region (if any) and at elast two properties" );
    return false;
  }

  //Check if there is already a transformer with that name
  SmartPtr<Named_interface> ni = Root::instance()->interface( transformer_manager + "/" + params[0]);
  if(ni.raw_ptr() == 0) {
    errors->report( "The transformer "+params[0]+" does not exist" );
    return false;
  }
  Multi_property_transformer* mvariate_trans = dynamic_cast<Multi_property_transformer*>(ni.raw_ptr());
  if(mvariate_trans == 0) {
    errors->report( "The transformer "+params[0]+" is not a multivariate transformer" );
    return false;
  }

  // Get the grid
  std::string grid_name = params[1];
   ni = Root::instance()->interface( gridModels_manager + "/" + grid_name);
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( ni.raw_ptr() );
  if(!grid)  {
    errors->report( "The grid "+params[1]+" does not exist" );
    return false;
  }

  Grid_region* region = 0;
  if( !params[2].empty() )  {
    region = grid->region(params[2]);
  }
  std::vector<const Grid_continuous_property*> props;
  for(int i=3; i<params.size();++i) {
    Grid_continuous_property* prop = grid->property(params[i]);
    if(prop==0) {
      errors->report( "The property "+params[i]+" does not exist" );
      return false;
    }
    props.push_back(prop);
  }


  mvariate_trans->back_transform(grid, props, region);
  return true;



}

bool Multivariate_back_transform::exec(){
  return true;
}
 




/*
-------------------------
*/

Named_interface* Delete_multivariate_transforms::create_new_interface( std::string& ){
  return new Delete_multivariate_transforms;
}
   
//format name::grid::region::prop1::prop2::
// if no region is needed then either enter 0 or blank ::::
bool Delete_multivariate_transforms::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   false );

  for(int i=0; i<params.size();++i ) {
    Root::instance()->delete_interface( transformer_manager + "/" + params[i]);
  }

  return true;



}

bool Delete_multivariate_transforms::exec(){
  return true;
}
 




