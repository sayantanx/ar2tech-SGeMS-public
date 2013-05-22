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




#include <GsTLAppli/math/library_math_init.h>
#include <GsTLAppli/math/continuous_distribution.h>
#include <GsTLAppli/math/categorical_distribution.h>
#include <GsTLAppli/math/non_parametric_distribution.h>
#include <GsTLAppli/utils/gstl_messages.h>


int library_math_init::references_ = 0;


int library_math_init::init_lib() {

//  std::cout << "number of subscriber to GsTLlog: "  
//    << GsTLlog.subscribers() << "  second opinion: " 
//    << GsTLAppli_logging::instance()->subscribers() << std::endl;
  references_++;
  if( references_ != 1 ) {
    GsTLlog << "math library already registered \n";
    return 2;
  }
     
  GsTLlog << "\n\n registering math library \n"; 
  
  //Registering distribution
  // /Distributions
  // /Distributions/Continuous
  // /Distributions/Categorical

  SmartPtr<Named_interface> ni = 
    Root::instance()->new_interface("directory",
				    "/Distributions" );
  Manager* dist_dir = dynamic_cast<Manager*>( ni.raw_ptr() );
  
  dist_dir->factory("directory", create_directory);

  //Create directory for continuous distribution
  ni =  dist_dir->new_interface("directory",
				    "/Continuous" );
      
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << "/Distributions/Continuous" << "\n";
    return 1;
  }
  
  bind_continuous_distribution_factories( dir );
  GsTLlog << "Registration of continuous distributions completed\n\n" ;

  
//Create directory for continuous distribution
  ni =  dist_dir->new_interface("directory",
				    "/Categorical" );
      
  dir = dynamic_cast<Manager*>( ni.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << "/Distributions/Categorical" << "\n";
    return 1;
  }
  
  bind_categorical_distribution_factories( dir );
  GsTLlog << "Registration of continuous distributions completed\n\n" ;



  Root::instance()->list_all( GsTLlog );
  return 0;
}


int library_math_init::release_lib() {
  references_--;
  if(references_==0) {
    Root::instance()->delete_interface( "/Distributions/Continuous" );
    Root::instance()->delete_interface( "/Distributions/Categorical" );
    Root::instance()->delete_interface( "/Distributions" );
  }
  return 0;
}



bool library_math_init::bind_continuous_distribution_factories(Manager* dir) {
  dir->factory( Beta_distribution().type(), Beta_distribution::create_new_interface );
  dir->factory( Exponential_distribution().type(), Exponential_distribution::create_new_interface );
  dir->factory( Extreme_value_distribution().type(), Extreme_value_distribution::create_new_interface );
  dir->factory( Gamma_distribution().type(), Gamma_distribution::create_new_interface );
  dir->factory( Logistic_distribution().type(), Logistic_distribution::create_new_interface );
  dir->factory( LogNormal_distribution().type(), LogNormal_distribution::create_new_interface );
  dir->factory( Gaussian_distribution().type(), Gaussian_distribution::create_new_interface );
  dir->factory( Pareto_distribution().type(), Pareto_distribution::create_new_interface );
  dir->factory( Rayleigh_distribution().type(), Rayleigh_distribution::create_new_interface );
  dir->factory( Triangular_distribution().type(), Triangular_distribution::create_new_interface );
  dir->factory( Uniform_distribution().type(), Uniform_distribution::create_new_interface );
  dir->factory( Weibull_distribution().type(), Weibull_distribution::create_new_interface );
  dir->factory( Non_parametric_distribution().type(), Non_parametric_distribution::create_new_interface );

  return true;
}

bool library_math_init::bind_categorical_distribution_factories(Manager* dir) {
  dir->factory( Categorical_distribution().type(), Categorical_distribution::create_new_interface );
  return true;

}

extern "C" {
  int libGsTLAppli_math_init() {
    return library_math_init::init_lib();
  }
  int libGsTLAppli_math_release(){
    return library_math_init::release_lib();
  }
}
