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


#include <qtplugins/grid_filter_model_item.h>
#include <appli/manager_repository.h>


Grid_filter* Grid_filter_region_item::create_filter() const{
  if(grid_ == 0) return 0;

  std::string region_name = region_name_.toStdString();
  const Grid_region* region = grid_->region(region_name);
  if(region ==0) return 0;

  return new Grid_filter_region( region );

}

Grid_filter* Grid_filter_category_item::create_filter() const{
  if(grid_ == 0) return 0;

  if(property_name_.isEmpty() || category_name_.isEmpty()) return 0;

  std::string cprop_str = property_name_.toStdString();
  const Grid_categorical_property* cprop = grid_->categorical_property(cprop_str);
  if(cprop ==0) return 0;

  std::string category_str = category_name_.toStdString();
  if(! cprop->get_category_definition()->is_category_exist(category_str) ) return 0;

  return new Grid_filter_category( cprop, category_str );

}

Grid_filter* Grid_filter_less_than_item::create_filter() const{
  if(grid_ == 0) return 0;

  if(prop_name_.isEmpty() ) return 0;

  std::string prop_str = prop_name_.toStdString();
  const Grid_continuous_property* prop = grid_->property(prop_str);
  if(prop ==0) return 0;

  return new Grid_filter_less_than( prop, upper_bound_);

}


Grid_filter* Grid_filter_greater_than_item::create_filter() const{
  if(grid_ == 0) return 0;

  if(prop_name_.isEmpty() ) return 0;

  std::string prop_str = prop_name_.toStdString();
  const Grid_continuous_property* prop = grid_->property(prop_str);
  if(prop ==0) return 0;

  return new Grid_filter_greater_than( prop, lower_bound_);

}

Grid_filter* Grid_filter_x_coord_bounded_item::create_filter() const{
  if(grid_ == 0) return 0;
  if(x_lower_bound_ > x_upper_bound_) return 0;

  return new Grid_filter_x_coord_bounded(grid_, x_lower_bound_, x_upper_bound_);

}

Grid_filter* Grid_filter_y_coord_bounded_item::create_filter() const{
  if(grid_ == 0) return 0;
  if(y_lower_bound_ > y_upper_bound_) return 0;

  return new Grid_filter_y_coord_bounded(grid_, y_lower_bound_, y_upper_bound_);

}

Grid_filter* Grid_filter_z_coord_bounded_item::create_filter() const{
  if(grid_ == 0) return 0;
  if(z_lower_bound_ > z_upper_bound_) return 0;

  return new Grid_filter_z_coord_bounded(grid_, z_lower_bound_, z_upper_bound_);

}

Grid_filter* Grid_filter_intersection_item::create_filter() const {

  if(child_.empty()) return 0;


  int n_filters_active = 0;
  std::vector<Grid_filter_item*> active_filters_;

  for(int i=0; i< child_.size(); ++i) {
    if( child_[i]->is_selected()  ) {
      active_filters_.push_back(child_[i]);
    }
  }

  if(active_filters_.size() == 0) return 0;
  else if(active_filters_.size() == 1) {
    return active_filters_[0]->create_filter();
  }
  else { // We assume an intersection
    Grid_filter_intersection* filter = new Grid_filter_intersection();
    for(int i=0; i< active_filters_.size(); ++i) {
      filter->add_filter(active_filters_[i]->create_filter());
    }
    return filter;
  }
}


Grid_filter* Grid_filter_union_item::create_filter() const {

  if(child_.empty()) return 0;


  int n_filters_active = 0;
  std::vector<Grid_filter_item*> active_filters_;

  for(int i=0; i< child_.size(); ++i) {
    if( child_[i]->is_selected()  ) {
      active_filters_.push_back(child_[i]);
    }
  }

  if(active_filters_.size() == 0) return 0;
  else if(active_filters_.size() == 1) {
    return active_filters_[0]->create_filter();
  }
  else { // We assume an intersection
    Grid_filter_union* filter = new Grid_filter_union();
    for(int i=0; i< active_filters_.size(); ++i) {
      filter->add_filter(active_filters_[i]->create_filter());
    }
    return filter;
  }
}