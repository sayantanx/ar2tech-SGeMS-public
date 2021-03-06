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



#include "moving_window.h"
#include <geostat/parameters_handler.h>
#include <utils/gstl_plugins.h>
#include <utils/error_messages_handler.h>
#include <utils/string_manipulation.h>
#include <grid/grid_model/geostat_grid.h>
#include <grid/grid_model/rgrid_neighborhood.h>
#include <grid/grid_model/gval_iterator.h>
#include <math/gstlpoint.h>
#include <geostat/utilities.h>
#include <algorithm>




Moving_window::Moving_window() {
	filters_ = NULL;
	neigh_ = NULL;
  catdef_ = NULL;
}

Moving_window::~Moving_window( void) {
  if(filters_) delete filters_;
  if(neigh_) delete neigh_;
}

bool Moving_window::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {

	std::cout << "initializing algorithm Moving_window \n";
  this->extract_parameters(parameters);

	std::string grid_name = parameters->value( "Input_data.grid" );
	errors->report( grid_name.empty(),"Input_data", "No grid specified" ); 

	if( !grid_name.empty() ) {
  	bool ok = geostat_utils::create( grid_, grid_name,
				 "Grid_Name", errors );
		if( !ok || !grid_ ) return false;
	}
	else 
		return false;


  bool is_neigh_rect = parameters->value( "is_neigh_rect.value" )=="1";
  std::string type = parameters->value( "filter_type.value" );

  if(type == "User defined" ||  type == "Default Filtersim filters"  || type == "Sobel" ) {
    if( !dynamic_cast<RGrid*>( grid_ ) ) 
      errors->report( "Input_data", "The selected filter requires a regular grid" );
  }

  if( is_neigh_rect && !dynamic_cast<RGrid*>( grid_ ) ) 
    errors->report( "Input_data", "A rectangular neighborhood requires a regular grid" );

  if(!errors->empty()) return false;



  std::string prop_name = parameters->value("Input_data.property");
  Grid_continuous_property* prop_input = grid_->property( prop_name );
  Grid_categorical_property* cprop = dynamic_cast<Grid_categorical_property*>(prop_input);
  nCategory_ = 0;
  if(cprop) {
    nCategory_ = cprop->get_number_of_category();
    catdef_ = cprop->get_category_definition();
  }
    

  grid_->select_property( prop_name );

  if( !prop_input ) {

    errors->report("Input_data","No input property selected");

    return false;

  }



	if( type == "User defined" ) 

	{

    std::string filename = parameters->value( "filter_filename.value");

    Filtersim_filters user_def_filters(filename);

    Grid_template* window_tpl = user_def_filters.get_geometry();

    Strati_grid* sgrid = dynamic_cast<Strati_grid*>(grid_);
    neigh_ = sgrid->window_neighborhood(*window_tpl);
    neigh_->select_property(prop_input->name());

 //   neigh_ = new Rgrid_window_neighborhood (*window_tpl, dynamic_cast<RGrid*>( grid_ ),grid_->selected_property());

    filters_ = new Rasterized_filter<Smart_Neighborhood,Filtersim_filters>( user_def_filters );

	}

  else if( type == "Default Filtersim filters" ) 

  {

    int nx = String_Op::to_number<int>(parameters->value("Filtsim_def_size_x.value"));

    int ny = String_Op::to_number<int>(parameters->value("Filtsim_def_size_y.value"));

    int nz = String_Op::to_number<int>(parameters->value("Filtsim_def_size_z.value"));

    Filtersim_filters default_Filtersim_filters(nx, ny, nz );

    Grid_template* window_tpl = default_Filtersim_filters.get_geometry();

    Strati_grid* sgrid = dynamic_cast<Strati_grid*>(grid_);
    neigh_ = sgrid->window_neighborhood(*window_tpl);
    neigh_->select_property(prop_input->name());

 //   neigh_ = new Rgrid_window_neighborhood (*window_tpl, dynamic_cast<RGrid*>( grid_ ),grid_->selected_property());

    filters_ = new Rasterized_filter<Smart_Neighborhood,Filtersim_filters>( default_Filtersim_filters );

  }

  else if( type == "Sobel" ) {

    filters_ = new Rasterized_filter<Smart_Neighborhood,Sobel_weights>( Sobel_weights() );

    Grid_template tpl;

    if( parameters->value("Sobel_orientation.value")=="XY") tpl = create_neigh_template(1,1,0);

    else if( parameters->value("Sobel_orientation.value")=="XZ" ) tpl = create_neigh_template(1,0,1);

    else if( parameters->value("Sobel_orientation.value")=="YZ" ) tpl = create_neigh_template(0,1,1);

    else errors->report("Sobel_orientation","An orientation must be selected");

    Strati_grid* sgrid = dynamic_cast<Strati_grid*>(grid_);
    neigh_ = sgrid->window_neighborhood(tpl);
    neigh_->select_property(prop_input->name());

 //   neigh_ = new Rgrid_window_neighborhood (tpl, dynamic_cast<RGrid*>( grid_ ),prop_input);

  }

  else {

    if( !is_neigh_rect ) {

      GsTLTriplet ranges, angles;

      geostat_utils::extract_ellipsoid_definition( ranges, 

					angles,"neigh_ellipsoid.value",parameters,errors);  



      neigh_ = grid_->neighborhood( ranges, angles );

    } else {

      int nx = String_Op::to_number<int>(parameters->value("size_x.value"));

      int ny = String_Op::to_number<int>(parameters->value("size_y.value"));

      int nz = String_Op::to_number<int>(parameters->value("size_z.value"));

      Grid_template tpl = create_neigh_template(nx,ny,nz);

      Strati_grid* sgrid = dynamic_cast<Strati_grid*>(grid_);
      neigh_ = sgrid->window_neighborhood(tpl);
      neigh_->select_property(prop_input->name());

//      neigh_ = new Rgrid_window_neighborhood (tpl, dynamic_cast<RGrid*>( grid_ ),prop_input );

    }



    neigh_->max_size(1000000);

    if( type == "Gaussian low pass" ) {

	  std::string sigma_str = parameters->value("sigma.value");

		if ( sigma_str.empty() ) {

			errors->report("sigma","A scaling value is needed");

			return false;

		}

      float sigma = String_Op::to_number<float>( sigma_str );

	  	if ( sigma <= 0 ) {

			errors->report("sigma","Sigma must be positive");

			return false;

		}

      Gaussian_kernel g_kernel(sigma);

      filters_ = new Functional_filter<Smart_Neighborhood,Gaussian_kernel>( g_kernel  );

    }

    else if( type == "Moving Average" ) filters_ = new Moving_average<Smart_Neighborhood>();

    else if( type == "Moving Variance" ) filters_ = new Moving_variance<Smart_Neighborhood>();

  }

  if(!errors->empty()) return false;





   std::string prefix = parameters->value("prefix_out.value");

  GsTLGridPropertyGroup* group = geostat_utils::add_group_to_grid( grid_, prefix,"General");

  
 if(nCategory_ > 0) {
    for( int c=0; c<nCategory_ ; c++ ) {
      for(int i=0; i<filters_->number_filters(); i++ ) {
        std::string filter_name = prefix + " "+filters_->name(i) + " "+catdef_->get_category_name(c);
        props_.push_back( geostat_utils::add_property_to_grid( grid_, filter_name ) );
        props_.back()->set_parameters(parameters_);
        group->add_property(props_.back());
      }
    }
 }
 else {
   for(int i=0; i<filters_->number_filters(); i++ ) {
      std::string filter_name = prefix + " "+filters_->name(i);
      Grid_continuous_property* prop = geostat_utils::add_property_to_grid( grid_, filter_name );
      prop->set_parameters(parameters_);
      props_.push_back( prop );
      group->add_property(prop);
   }
}



  grid_->select_property( prop_input->name() );

  neigh_->select_property( prop_input->name() );
 // neigh_->includes_center( true );

	return true;

}





int Moving_window::execute(GsTL_project *) { 

  Geostat_grid::iterator it_gval = grid_->begin();
  std::vector< float > scores;

  
  for(; it_gval != grid_->end(); ++it_gval ) {
    neigh_->find_neighbors ( *it_gval );
    
    if( nCategory_ > 0 ) {

      int index = 0;

      std::vector< Grid_continuous_property::property_type >  data;
      Neighborhood::iterator it_neigh = neigh_->begin();
      for( ; it_neigh != neigh_->end() ; ++it_neigh ) {
        if( it_neigh->is_informed() )  data.push_back( it_neigh->property_value() );
      }
      for(int c = 0; c< nCategory_; c++ ) {
        std::vector< Grid_continuous_property::property_type >  data_id;
        for( int i=0; i<data.size(); ++i )
          data_id.push_back( (data[i]==c)?1.0:0.0 );

        for(int i=0; i<filters_->number_filters(); i++ ) {
          props_[index]->set_value((*filters_)(*neigh_,data_id.begin(),data_id.end(), i ),it_gval->node_id() );
          index++;
        } 
      }
    }
    else {
      for(int i=0; i<props_.size(); i++ )  
        props_[i]->set_value((*filters_)(*neigh_, i ),it_gval->node_id() );
    }


 //   (*filters_)(*neigh_, i );
 //   if(scores.size() != props_.size() ) continue;
 //   for(int i=0; i<scores.size(); i++ ) 
 //     props_[i]->set_value(scores[i],it_gval->node_id() );
  }
  return 0;

}





Grid_template Moving_window::create_neigh_template( int nx, int ny, int nz ) 

{

  Grid_template tpl;
  for(int k =-nz; k<=nz; ++k ) {
    for(int j =-ny; j<=ny; ++j ) {
      for(int i =-nx; i<=nx; ++i ) tpl.add_vector(i,j,k);
    }
  }
  return tpl;

}





Named_interface* Moving_window::create_new_interface( std::string& ) {
  return new Moving_window;
}









//GEOSTAT_PLUGIN(Moving_window)

