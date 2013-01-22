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

#ifndef __GRID_FILTER_H__ 
#define __GRID_FILTER_H__ 
 
#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/grid/grid_model/grid_property.h> 
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h> 
#include <GsTLAppli/grid/grid_model/grid_region.h> 
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 


class GRID_DECL Grid_filter
{
public:
  Grid_filter(void){}
  virtual ~Grid_filter(void){}

  virtual bool is_valid_nodeid(int nodeid) const = 0;
};


class GRID_DECL Grid_filter_union  : public Grid_filter 
{
public:
  Grid_filter_union(void){}
  virtual ~Grid_filter_union(void){
    for(int i=0; i< filters_.size(); ++i) {
      delete filters_[i];
    }
  
  }

  int number_of_filters() const {return filters_.size();}
  const Grid_filter* filter(int index)  const {return filters_[index];}
  
  const void add_filter(Grid_filter* filter )  {filters_.push_back(filter);}
  const void remove_filter(int index)  {
    if( filters_.size() >= index ) return;

    std::vector<const Grid_filter*>::iterator it = filters_.begin() + index;
    delete filters_[index];
    filters_.erase( it );
  }

  virtual bool is_valid_nodeid(int nodeid) const{

  for( int i=0; filters_.size(); ++i  ) {
    if(filters_[i]->is_valid_nodeid(nodeid)) return true;
  }

  return false;

  }

private:
  std::vector<const Grid_filter*> filters_;

};


class GRID_DECL Grid_filter_intersection  : public Grid_filter 
{
public:
  Grid_filter_intersection( ){}

  virtual ~Grid_filter_intersection( ){
    for(int i=0; i< filters_.size(); ++i) {
      delete filters_[i];
    }
  
  }

  int number_of_filters() const {return filters_.size();}
  const Grid_filter* filter(int index)  const {return filters_[index];}
  
  const void add_filter(Grid_filter* filter )  {filters_.push_back(filter);}
  const void remove_filter(int index)  {
    if( filters_.size() >= index ) return;

    std::vector<const Grid_filter*>::iterator it = filters_.begin() + index;
    delete filters_[index];
    filters_.erase( it );
  }

  virtual bool is_valid_nodeid(int nodeid) const{

  for( int i=0; filters_.size(); ++i  ) {
    if(!filters_[i]->is_valid_nodeid(nodeid)) return false;
  }

  return true;

  }

private:
  std::vector<const Grid_filter*> filters_;

};


class GRID_DECL Grid_filter_region  : public Grid_filter
{
public:
  Grid_filter_region(const GsTLGridRegion* region) : region_(region){}
  virtual ~Grid_filter_region(void){}

  virtual bool is_valid_nodeid(int nodeid) const {
    return region_->is_inside_region(nodeid);
  }

private :
  const GsTLGridRegion* region_;
};


class GRID_DECL Grid_filter_category  : public Grid_filter
{
public:
  Grid_filter_category(const GsTLGridCategoricalProperty* cprop, int active_category) 
    : cprop_(cprop), category_(active_category){}

  virtual ~Grid_filter_category(void){}

  virtual bool is_valid_nodeid(int nodeid) const {
    if( !cprop_->is_informed(nodeid) ) return false;

    return cprop_->get_value(nodeid) == category_;
  }

private :
  const GsTLGridCategoricalProperty* cprop_;
  int category_;
};

class GRID_DECL Grid_filter_less_than  : public Grid_filter
{
public:
  Grid_filter_less_than(const GsTLGridProperty* prop, float upper_bound) 
    : prop_(prop), upper_bound_(upper_bound){}

  virtual ~Grid_filter_less_than(void){}

  virtual bool is_valid_nodeid(int nodeid) const {
    if( !prop_->is_informed(nodeid) ) return false;

    return prop_->get_value(nodeid) < upper_bound_;
  }

private :
  const GsTLGridProperty* prop_;
  float upper_bound_;
};

class GRID_DECL Grid_filter_lessor_or_equal_than  : public Grid_filter
{
public:
  Grid_filter_lessor_or_equal_than(const GsTLGridProperty* prop, float upper_bound) 
    : prop_(prop), upper_bound_(upper_bound){}

  virtual ~Grid_filter_lessor_or_equal_than(void){}

  virtual bool is_valid_nodeid(int nodeid) const {
    if( !prop_->is_informed(nodeid) ) return false;

    return prop_->get_value(nodeid) <= upper_bound_;
  }

private :
  const GsTLGridProperty* prop_;
  float upper_bound_;
};

class GRID_DECL Grid_filter_greater_than  : public Grid_filter
{
public:
  Grid_filter_greater_than(const GsTLGridProperty* prop, float lower_bound) 
    : prop_(prop), lower_bound_(lower_bound){}

  virtual ~Grid_filter_greater_than(void){}

  virtual bool is_valid_nodeid(int nodeid) const {
    if( !prop_->is_informed(nodeid) ) return false;

    return prop_->get_value(nodeid) > lower_bound_;
  }

private :
  const GsTLGridProperty* prop_;
  float lower_bound_;
};

class GRID_DECL Grid_filter_greater_or_equal_than  : public Grid_filter
{
public:
  Grid_filter_greater_or_equal_than(const GsTLGridProperty* prop, float lower_bound) 
    : prop_(prop), lower_bound_(lower_bound){}

  virtual ~Grid_filter_greater_or_equal_than(void){}

  virtual bool is_valid_nodeid(int nodeid) const {
    if( !prop_->is_informed(nodeid) ) return false;

    return prop_->get_value(nodeid) >= lower_bound_;
  }

private :
  const GsTLGridProperty* prop_;
  float lower_bound_;
};

class GRID_DECL Grid_filter_bounded  : public Grid_filter
{
public:
  Grid_filter_bounded(const GsTLGridProperty* prop, float lower_bound,float upper_bound) 
    : prop_(prop), lower_bound_(lower_bound), upper_bound_(upper_bound){}

  virtual ~Grid_filter_bounded(void){}

  virtual bool is_valid_nodeid(int nodeid) const {
    if( !prop_->is_informed(nodeid) ) return false;

    float v = prop_->get_value(nodeid);
    return v > lower_bound_ && v < upper_bound_;
  }

private :
  const GsTLGridProperty* prop_;
  float lower_bound_;
  float upper_bound_;
};

#endif