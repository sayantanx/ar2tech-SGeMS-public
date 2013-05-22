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




#include <actions/distribution_action.h>
#include <actions/defines.h>
#include <utils/string_manipulation.h>
#include <math/continuous_distribution.h>
#include <utils/manager.h>
#include <utils/named_interface.h>
#include <appli/manager_repository.h>
#include <filters/distribution_filter.h>

Named_interface* New_distribution_action::create_new_interface( std::string& ){
  return new New_distribution_action;
}


New_distribution_action::New_distribution_action(void)
{
}


New_distribution_action::~New_distribution_action(void)
{
}


bool New_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if(params.size() <  3) {
    errors->report("Need at least three parameters (type, name, parameters)");
    return false;
  }
  dist_type_ = params[0];
  dist_name_ = params[1];
  Named_interface* ni = Root::instance()->new_interface_raw(dist_type_, continuous_distributions_manager+"/"+dist_name_ );
  if(ni != 0) {
    Parametric_continuous_distribution* dist = dynamic_cast<Parametric_continuous_distribution*>(ni);
    std::vector<std::string> pnames = dist->parameter_names();
    if(params.size()-2 != pnames.size()) {
      std::stringstream message;
      message<<"Distribution of type "<<dist_type_<<" needs "<<pnames.size()<< " parameters : ";
      for(int i=0; i<pnames.size(); ++i) {
        message<<pnames[i];
        if(i < pnames.size()-1)
          message<<",";
      }
      message<<".  Only "<<params.size()-2<<" were provided.";
      
      errors->report(message.str());
      return false;
    }
    for(int i=0; i<pnames.size(); ++i) {
      params_.push_back(String_Op::to_number<float>(params[i+2]));
    }
    is_continuous_ = true;
    return true;
  }
  ni = Root::instance()->new_interface_raw(dist_type_, categorical_distributions_manager+"/"+dist_name_ );

  return false;


}

bool New_distribution_action::exec() {

  if(is_continuous_) {
    Named_interface* ni = Root::instance()->new_interface(dist_type_, continuous_distributions_manager+"/"+dist_name_ ).raw_ptr();

    Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>(ni);
    if(dist->is_parametric()) {
      dynamic_cast<Parametric_continuous_distribution*>(dist)->initialize( params_ );
      return true;
    }
    else {

    }

    
  }
  else {   //Categorical

  }
    
  return false;
}

/*
  ---------------------
  parameters: filename
*/

Named_interface* Load_distribution_action::create_new_interface( std::string& ){
  return new Load_distribution_action;
}

Load_distribution_action::Load_distribution_action(void){

}
Load_distribution_action::~Load_distribution_action(void){

}

bool Load_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) 
{
  SmartPtr<Named_interface> ni = Root::instance()->new_interface("Distribution", topLevelInputFilters_manager+"/" );
  
  Distribution_input_filter* input_filter = dynamic_cast<Distribution_input_filter*>(ni.raw_ptr());

  std::string error_str;
  input_filter->read(parameters,&error_str);

  if(!error_str.empty())
    errors->report(error_str);

  return error_str.empty();

}

bool Load_distribution_action::exec(){
  return true;
}



/*
  ---------------------
  parameters: filename[::dist1::dist2]
  default all the distributions are saved
  
*/

Named_interface* Save_distribution_action::create_new_interface( std::string& ){
  return new Save_distribution_action;
}

Save_distribution_action::Save_distribution_action(void){

}
Save_distribution_action::~Save_distribution_action(void){

}

bool Save_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) 
{
  
  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if(params.size() <  1) {
    errors->report("Need at least one parameter (filename)");
    return false;
  }


  SmartPtr<Named_interface> ni = Root::instance()->new_interface("Distribution", outfilters_manager+"/" );
  Distribution_output_filter* out_filter = dynamic_cast<Distribution_output_filter*>(ni.raw_ptr());
  if(out_filter == 0) {
    errors->report("Could not find the filter to write continuous distribution to disk");
    return false;
  }
  

  std::string error_str;
  if(params.size() == 1) {
    return out_filter->write(params[0],&error_str);
  }
  else if(params.size() == 2) {
    Named_interface* ni_dist = Root::instance()->interface( continuous_distributions_manager+"/"+params[1] ).raw_ptr();
    if(ni_dist==0) return false;
    return out_filter->write(params[0],ni_dist,&error_str);
  }
  else {
    std::vector<const Named_interface*> dists_ni;
    for(int i=1; i<params.size(); ++i) {
      Named_interface* ni_dist = Root::instance()->interface( continuous_distributions_manager+"/"+params[i] ).raw_ptr();
      dists_ni.push_back(ni_dist);
    }  
    return out_filter->write(params[0],dists_ni, &error_str);
  }

  return false;

}

bool Save_distribution_action::exec(){
  return true;
}

/*
----------------------------------------
*/

Named_interface* Delete_distribution_action::create_new_interface( std::string& ){
  return new Delete_distribution_action;
}

Delete_distribution_action::Delete_distribution_action(void){

}
Delete_distribution_action::~Delete_distribution_action(void){

}

bool Delete_distribution_action::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if(params.empty() <  1) {
    errors->report("Need at least one parameter (dsitribution name)");
    return false;
  }

  Manager* dir = dynamic_cast<Manager*>(Root::instance()->interface( continuous_distributions_manager).raw_ptr());

  if(dir == 0) return false;

  for(int i=0; i<params.size(); ++i) {
    dir->delete_interface( params[i] );
  }

  return true;


}
bool Delete_distribution_action::exec(){
  return true;
}