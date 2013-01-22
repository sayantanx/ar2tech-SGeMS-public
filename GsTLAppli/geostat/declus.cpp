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



//a. boucher
// @ar2tech 2011

#include <GsTLAppli/geostat/declus.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/point_set_neighborhood.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/appli/utilities.h>
#include <GsTLAppli/grid/grid_model/grid_region_temp_selector.h> 

Named_interface* Declus::create_new_interface( std::string& ) {
  return new Declus;
}


Declus::Declus()
  :  prop_(0), region_(0), grid_(0)
{
}
 

Declus::~Declus() {

}


// This is the gslib implementation 
int Declus::execute( GsTL_project* ) {

  SmartPtr<Progress_notifier> progress_notifier = 
    utils::create_notifier( "Running Declus", 
			    ncell_, 1 );

  Temporary_propRegion_Selector prop_region_setter(region_, prop_);

  //Get the bounding box
  std::pair<Geostat_grid::location_type,Geostat_grid::location_type> bbox = dynamic_cast<Point_set*>(grid_)->bounding_box();
  GsTLCoord xmin = bbox.first.x();  
  GsTLCoord ymin = bbox.first.y();
  GsTLCoord zmin = bbox.first.z();
  GsTLCoord xmax = bbox.second.x();  
  GsTLCoord ymax = bbox.second.y();
  GsTLCoord zmax = bbox.second.z();

//
// initialize the "best" weight values:
//

//
//define a "lower" origin to use for the cell sizes:
//
      GsTLCoord xo1 = xmin - 0.01;
      GsTLCoord yo1 = ymin - 0.01;
      GsTLCoord zo1 = zmin - 0.01;
//
//define the increment for the cell size:
//
      GsTLCoord xinc = (max_dimx_-min_dimx_) / ncell_;
      GsTLCoord yinc = (max_dimy_-min_dimy_) / ncell_;
      GsTLCoord zinc = (max_dimz_-min_dimz_) / ncell_;
//
//loop over "ncell+1" cell sizes in the grid network:
//
      int ncellx = int((xmax-(xo1-min_dimx_))/min_dimx_)+1;
      int ncelly = int((ymax-(yo1-min_dimy_))/min_dimy_)+1;
      int ncellz = int((zmax-(zo1-min_dimz_))/min_dimz_)+1;

      int ncellt = ncellx*ncelly*ncellz;

      GsTLCoord xcs =  min_dimx_ - xinc;
      GsTLCoord ycs = min_dimy_ - yinc;
      GsTLCoord zcs = min_dimz_ - zinc;
//
//MAIN LOOP over cell sizes:
//
      for(int nc = 0; nc<= ncell_; ++nc) {
        progress_notifier->notify();

        xcs = xcs + xinc;
        ycs = ycs + yinc;
        zcs = zcs + zinc;

        std::stringstream stream_name;
        stream_name <<weight_name_<<" ( "<<xcs<<","<<ycs<<","<<zcs<<" )";
        GsTLGridWeightProperty* wprop = grid_->add_weight_property(stream_name.str());
        while(wprop == 0 ) {
          stream_name<<"_0";
          wprop = grid_->add_weight_property(stream_name.str());
        }
        wprop->set_parameters(parameters_);
        for(int i=0; i< wprop->size(); ++i) {
          if( !region_ || region_->is_inside_region(i)) wprop->set_value(0,i);
        }

//
//determine the maximum number of grid cells in the network:
//
            int ncellx = int((xmax-(xo1-xcs))/xcs)+1;
            int ncelly = int((ymax-(yo1-ycs))/ycs)+1;
            int ncellz = int((zmax-(zo1-zcs))/zcs)+1;
            int ncellt = ncellx*ncelly*ncellz;
            std::vector<std::vector<int> > cellwt(ncellt);
//
//loop over all the origin offsets selected:
//
            GsTLCoord xfac= std::min((xcs/noff_),(0.5*(xmax-xmin)));
            GsTLCoord yfac= std::min((ycs/noff_),(0.5*(ymax-ymin)));
            GsTLCoord zfac= std::min((zcs/noff_),(0.5*(zmax-zmin)));
            for(int kp=0; kp<noff_; ++kp) {
 //           do kp=1,noff
                  
                  GsTLCoord xo = xo1 - (static_cast<GsTLCoord>(kp))*xfac;  //Remove the -1 given the c++/fortran indices
                  GsTLCoord yo = yo1 - (static_cast<GsTLCoord>(kp))*yfac;
                  GsTLCoord zo = zo1 - (static_cast<GsTLCoord>(kp))*zfac;

//
//determine which cell each datum is in:
//
                  int npoints = 0;
                  Geostat_grid::iterator it = grid_->begin();
                  for( ; it!= grid_->end(); ++it) {
                    if(region_ && !region_->is_inside_region(it->node_id()) ) continue;
//                  do i=1,nd
                    Geostat_grid::location_type loc = it->location();
                    int icellx =  (loc.x() - xo)/xcs;
                    int icelly = (loc.y() - yo)/ycs;
                    int icellz = (loc.z() - zo)/zcs;
                    int icell  = icellx + (icelly)*ncellx + (icellz)*ncelly*ncellx;
                    cellwt[icell].push_back(it->node_id());
                    npoints++;
//                    int index(i)      = icell
//                    cellwt(icell) = cellwt(icell) + 1.0
                  }
//
//The weight assigned to each datum is inversely proportional to the
//number of data in the cell.  We first need to get the sum of weights
//so that we can normalize the weights to sum to one:
//

                  int non_empty_cell=0;
                  for(int j= 0; j< cellwt.size(); ++j) {
                    if(cellwt[j].empty()) continue;
                    non_empty_cell++;
                  }

                  double max_value_factor = (double)npoints/(npoints+1);

                  for(int j= 0; j< cellwt.size(); ++j) {
                    int n_pts_in_cell = cellwt[j].size();
                    if( n_pts_in_cell == 0 ) continue;
                    std::vector<int>::const_iterator it_id = cellwt[j].begin();
                    for( ; it_id != cellwt[j].end(); ++it_id ) {
                      float w = wprop->get_value( *it_id) + max_value_factor/n_pts_in_cell/non_empty_cell/noff_;
                      wprop->set_value(w, *it_id ); // need to be initialized
                    }
                  }

            }

//Currently the weight sum to one.  Must be set to sum to n/(n+1) to ensure the maximum value 
//  can be set with an extrapolation tail

//
//END MAIN LOOP over all cell sizes:
      }

   return 0;
}


void Declus::clean( const std::string& prop ) {
  grid_->remove_property( prop );
}


bool Declus::initialize( const Parameters_handler* parameters,
			  Error_messages_handler* errors ) {
  
  std::string grid_name = parameters->value( "grid_name.value" );
  errors->report( grid_name.empty(), 
		  "grid_name", "No grid selected" );

  std::string property_name = parameters->value( "property_name.value" );
  errors->report( property_name.empty(), 
		  "property_name", "No property name specified" );

  // Get the simulation grid from the grid manager
  if( !grid_name.empty() ) {
    bool ok = geostat_utils::create( grid_, grid_name,
				 "grid_name", errors );
    if( !ok ) return false;
  }
  else 
    return false;

  prop_ = grid_->property(property_name);
  if(prop_ == 0) {
    errors->report( "property_name", "Property does not exist" );
    return false;
  }

  std::string region_name = parameters->value( "grid_name.region" );

  if(!region_name.empty()) {
    region_ = grid_->region(region_name);
    if(region_ == 0) {
      errors->report( "grid_name", "The specified region ("+region_name+") does not exist" );
      return false;
    }
  }

  weight_name_ = parameters->value( "weight_prefix.value" );
  if(weight_name_.empty()) {
    errors->report( "weight_prefix", "Name for output weights not specified" );
    return false;
  }

  ncell_ = String_Op::to_number<int>(parameters->value( "ncells.value" ));
  //hard coded
  noff_ = 8;

  //Get the minimum and maximum grid cell size

  min_dimx_ =  String_Op::to_number<float>(parameters->value( "min_dimx.value" ));
  min_dimy_ =  String_Op::to_number<float>(parameters->value( "min_dimy.value" ));
  min_dimz_ =  String_Op::to_number<float>(parameters->value( "min_dimx.value" ));

  max_dimx_ =  String_Op::to_number<float>(parameters->value( "max_dimx.value" ));
  max_dimy_ =  String_Op::to_number<float>(parameters->value( "max_dimy.value" ));
  max_dimz_ =  String_Op::to_number<float>(parameters->value( "max_dimx.value" ));

  //Check for inconsistencies
  errors->report( min_dimx_ > max_dimx_ , "min_dimx", "Minimum greater than maximum" );
  errors->report( min_dimy_ > max_dimy_ , "min_dimy", "Minimum greater than maximum" );
  errors->report( min_dimz_ > max_dimz_ , "min_dimz", "Minimum greater than maximum" );
  

  if( !errors->empty() )
    return false;
 
  this->extract_parameters(parameters);
  return true;
}


