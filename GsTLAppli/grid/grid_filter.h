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
#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/grid/grid_model/grid_property.h> 
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h> 
#include <GsTLAppli/grid/grid_model/grid_region.h> 
#include <GsTLAppli/grid/grid_model/geostat_grid.h> 
#include <GsTLAppli/grid/grid_model/log_data_grid.h> 


class GRID_DECL Grid_filter : public Named_interface
{
public:
  Grid_filter(){}
  virtual ~Grid_filter(){}

  virtual bool is_valid_nodeid(int nodeid) const = 0;

  virtual Grid_filter* clone() const =0;
};

class GRID_DECL Grid_filter_wrapper : public Grid_filter
{
public:
  Grid_filter_wrapper(){}
  virtual ~Grid_filter_wrapper(){}

  void initialize(Grid_filter* filter) {
    if(filter_) delete filter_;
    filter_ = filter->clone();
  }

  virtual bool is_valid_nodeid(int nodeid) const {return filter_->is_valid_nodeid(nodeid);}

  virtual Grid_filter* clone() const {
    if(filter_ == 0) return 0;
    return filter_->clone();
  }

private:
  Grid_filter* filter_;
};




class GRID_DECL Grid_filter_union  : public Grid_filter 
{
public:
  Grid_filter_union(){
    item_type_ = "Union";
  }

  virtual ~Grid_filter_union(){
    for(int i=0; i< filters_.size(); ++i) {
      delete filters_[i];
    }
  
  }

  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_union();
  }

  virtual Grid_filter* clone() const {
    Grid_filter_union* union_filter = new Grid_filter_union();
    for( int i=0; filters_.size(); ++i  ) {
      union_filter->add_filter( this->filter(i)->clone() );
    }
    return union_filter;
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

  for( int i=0; i<filters_.size(); ++i  ) {
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
  Grid_filter_intersection( ){
    item_type_ = "Intersection";
  }

  virtual ~Grid_filter_intersection( ){
    for(int i=0; i< filters_.size(); ++i) {
      delete filters_[i];
    }
  
  }

  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_intersection();
  }

  virtual Grid_filter* clone() const {
    Grid_filter_intersection* intersection_filter = new Grid_filter_intersection();
    for( int i=0; filters_.size(); ++i  ) {
      intersection_filter->add_filter( this->filter(i)->clone() );
    }
    return intersection_filter;
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

  for( int i=0; i<filters_.size(); ++i  ) {
    if(!filters_[i]->is_valid_nodeid(nodeid)) return false;
  }

  return true;

  }

private:
  std::vector<const Grid_filter*> filters_;

};


class GRID_DECL Grid_filter_region  : public Grid_filter
{

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_region();
  }

public:
  Grid_filter_region() : region_(0){
    item_type_ = "Region";
  }
  Grid_filter_region(const GsTLGridRegion* region) : region_(region){
    item_type_ = "Region";
  }
  virtual ~Grid_filter_region(void){}

  void set_region(const GsTLGridRegion* region){region_ = region;}

  virtual Grid_filter* clone() const {
    return new Grid_filter_region(this->region_);
  }

  virtual bool is_valid_nodeid(int nodeid) const {
    return region_->is_inside_region(nodeid);
  }


private :
  const GsTLGridRegion* region_;
};


class GRID_DECL Grid_filter_category  : public Grid_filter
{
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_category();
  }

public:
  Grid_filter_category() 
    : cprop_(0), category_(-1){
    item_type_ = "Category";
  }

  Grid_filter_category(const GsTLGridCategoricalProperty* cprop, int active_category) 
    : cprop_(cprop), category_(active_category){
    item_type_ = "Category";
  }

  Grid_filter_category(const GsTLGridCategoricalProperty* cprop, std::string category_name) 
    : cprop_(cprop){

    category_ = cprop_->get_category_definition()->category_id(category_name);
    item_type_ = "Category";
  }

  virtual ~Grid_filter_category(void){}

  void set_category(const GsTLGridCategoricalProperty* cprop, int active_category) {
    cprop_ = cprop;
    category_ = active_category;
  }

  void set_category(const GsTLGridCategoricalProperty* cprop, std::string category_name) {
    cprop_ = cprop;
    category_ = cprop_->get_category_definition()->category_id(category_name);
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_category(this->cprop_, this->category_);
  }

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

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_less_than();
  }

public:
  Grid_filter_less_than() 
    : prop_(0), upper_bound_(-9e20){
    item_type_ = "Less than";
  }

  Grid_filter_less_than(const GsTLGridProperty* prop, float upper_bound) 
    : prop_(prop), upper_bound_(upper_bound){
    item_type_ = "Less than";
  }

  virtual ~Grid_filter_less_than(void){}

  void set_upper_bound(const GsTLGridProperty* prop, float upper_bound)  {
    prop_ = prop;
    upper_bound_ =upper_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_less_than(this->prop_, this->upper_bound_);
  }

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

public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_lessor_or_equal_than();
  }

public:

  Grid_filter_lessor_or_equal_than() 
    : prop_(0), upper_bound_(-9e20){
    item_type_ = "Less or equal than";
  }

  Grid_filter_lessor_or_equal_than(const GsTLGridProperty* prop, float upper_bound) 
    : prop_(prop), upper_bound_(upper_bound){
    item_type_ = "Less or equal than";
  }

  virtual ~Grid_filter_lessor_or_equal_than(void){}

  void set_upper_bound(const GsTLGridProperty* prop, float upper_bound)  {
    prop_ = prop;
    upper_bound_= upper_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_lessor_or_equal_than(this->prop_, this->upper_bound_);
  }

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
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_greater_than();
  }


public:
  Grid_filter_greater_than() 
    : prop_(0), lower_bound_(9e20){
    item_type_ = "Greater than";
  }

  Grid_filter_greater_than(const GsTLGridProperty* prop, float lower_bound) 
    : prop_(prop), lower_bound_(lower_bound){
    item_type_ = "Greater than";
  }

  virtual ~Grid_filter_greater_than(void){}

  void set_lower_bound(const GsTLGridProperty* prop, float lower_bound)  {
    prop_ = prop;
    lower_bound_= lower_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_greater_than(this->prop_, this->lower_bound_);
  }

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
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_greater_or_equal_than();
  }

public:
  Grid_filter_greater_or_equal_than() 
    : prop_(0), lower_bound_(-9e20){
    item_type_ = "Greater or equal than";
  }

  Grid_filter_greater_or_equal_than(const GsTLGridProperty* prop, float lower_bound) 
    : prop_(prop), lower_bound_(lower_bound){
    item_type_ = "Greater or equal than";
  }

  virtual ~Grid_filter_greater_or_equal_than(void){}

  void set_lower_bound(const GsTLGridProperty* prop, float lower_bound)  {
    prop_ = prop;
    lower_bound_= lower_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_greater_or_equal_than(this->prop_, this->lower_bound_);
  }

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
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_greater_or_equal_than();
  }

public:

  Grid_filter_bounded() 
    : prop_(0), lower_bound_(-9e20), upper_bound_(9e20){
    item_type_ = "Bounded by";
  }

  Grid_filter_bounded(const GsTLGridProperty* prop, float lower_bound,float upper_bound) 
    : prop_(prop), lower_bound_(lower_bound), upper_bound_(upper_bound){
    item_type_ = "Bounded by";
  }

  virtual ~Grid_filter_bounded(void){}

  void set_bounds(const GsTLGridProperty* prop, float lower_bound,float upper_bound)  {
    prop_ = prop;
    lower_bound_= lower_bound;
    upper_bound_= upper_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_bounded(this->prop_, this->lower_bound_,  this->upper_bound_);
  }

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


class GRID_DECL Grid_filter_x_coord_bounded  : public Grid_filter
{
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_x_coord_bounded();
  }
public:

  Grid_filter_x_coord_bounded() 
    : grid_(0), x_coord_lower_bound_(-9e50), x_coord_upper_bound_(9e50){
    item_type_ = "Bounded by x coords";
  }

  Grid_filter_x_coord_bounded(const Geostat_grid* grid, double x_coord_lower_bound,double x_coord_upper_bound) 
    : grid_(grid), x_coord_lower_bound_(x_coord_lower_bound), x_coord_upper_bound_(x_coord_upper_bound){
    item_type_ = "Bounded by x coords";
  }

  virtual ~Grid_filter_x_coord_bounded(void){}

  void set_bounds(const Geostat_grid* grid, double x_coord_lower_bound,double x_coord_upper_bound)  {
    grid_= grid; 
    x_coord_lower_bound_ = x_coord_lower_bound;
    x_coord_upper_bound_= x_coord_upper_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_x_coord_bounded(this->grid_, this->x_coord_lower_bound_,  this->x_coord_upper_bound_);
  }

  virtual bool is_valid_nodeid(int nodeid) const {

    Geostat_grid::location_type loc = grid_->location(nodeid);

    double x = loc.x();
    return x > x_coord_lower_bound_ && x < x_coord_upper_bound_;
  }

private :
  const Geostat_grid* grid_;
  double x_coord_lower_bound_;
  double x_coord_upper_bound_;
};


class GRID_DECL Grid_filter_y_coord_bounded  : public Grid_filter
{
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_y_coord_bounded();
  }

public:

  Grid_filter_y_coord_bounded() 
    : grid_(0), y_coord_lower_bound_(-9e50), y_coord_upper_bound_(9e50){
    item_type_ = "Bounded by y coords";
  }

  Grid_filter_y_coord_bounded(const Geostat_grid* grid, double y_coord_lower_bound,double y_coord_upper_bound) 
    : grid_(grid), y_coord_lower_bound_(y_coord_lower_bound), y_coord_upper_bound_(y_coord_upper_bound){
    item_type_ = "Bounded by y coords";
  }

  virtual ~Grid_filter_y_coord_bounded(void){}

  void set_bounds(const Geostat_grid* grid, double y_coord_lower_bound,double y_coord_upper_bound)  {
    grid_= grid; 
    y_coord_lower_bound_ = y_coord_lower_bound;
    y_coord_upper_bound_= y_coord_upper_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_y_coord_bounded(this->grid_, this->y_coord_lower_bound_,  this->y_coord_upper_bound_);
  }

  virtual bool is_valid_nodeid(int nodeid) const {

    Geostat_grid::location_type loc = grid_->location(nodeid);

    double y = loc.y();
    return y > y_coord_lower_bound_ && y < y_coord_upper_bound_;
  }

private :
  const Geostat_grid* grid_;
  double y_coord_lower_bound_;
  double y_coord_upper_bound_;
};


class GRID_DECL Grid_filter_z_coord_bounded  : public Grid_filter
{
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_z_coord_bounded();
  }

public:

  Grid_filter_z_coord_bounded() 
    : grid_(0), z_coord_lower_bound_(-9e50), z_coord_upper_bound_(9e50){
    item_type_ = "Bounded by z coords";
  }

  Grid_filter_z_coord_bounded(const Geostat_grid* grid, double z_coord_lower_bound,double z_coord_upper_bound) 
    : grid_(grid), z_coord_lower_bound_(z_coord_lower_bound), z_coord_upper_bound_(z_coord_upper_bound){
    item_type_ = "Bounded by z coords";
  }

  virtual ~Grid_filter_z_coord_bounded(void){}

  void set_bounds(const Geostat_grid* grid, double z_coord_lower_bound,double z_coord_upper_bound)  {
    grid_= grid; 
    z_coord_lower_bound_ = z_coord_lower_bound;
    z_coord_upper_bound_= z_coord_upper_bound;
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_z_coord_bounded(this->grid_, this->z_coord_lower_bound_,  this->z_coord_upper_bound_);
  }

  virtual bool is_valid_nodeid(int nodeid) const {

    Geostat_grid::location_type loc = grid_->location(nodeid);

    double z = loc.z();
    return z > z_coord_lower_bound_ && z < z_coord_upper_bound_;
  }

private :
  const Geostat_grid* grid_;
  double z_coord_lower_bound_;
  double z_coord_upper_bound_;
};


class GRID_DECL Grid_filter_log_names  : public Grid_filter
{
public :
  static Named_interface* create_new_interface(std::string& str) {
    return new Grid_filter_log_names();
  }

public:
  Grid_filter_log_names() 
    : grid_(0)
  {
    item_type_ = "Log name";
  }


  Grid_filter_log_names(const Log_data_grid* grid, std::vector<std::string> log_names) 
    : grid_(grid)
  {
    item_type_ = "Log name";
    log_ids_.reserve(log_names.size());
    std::vector<std::string>::const_iterator it = log_names.begin();
    for( ; it != log_names.end() ; ++it ) {
      int id = grid_->get_log_id(*it);
      if(id >=0 ) log_ids_.push_back( id   );
    }

  }


  Grid_filter_log_names(const Log_data_grid* grid, std::vector<int> log_id) 
    : grid_(grid), log_ids_(log_id)
  {
  }

  void set_log_names(const Log_data_grid* grid, std::vector<std::string> log_names) 
  {
    grid_ = grid;
    log_ids_.reserve(log_names.size());
    std::vector<std::string>::const_iterator it = log_names.begin();
    for( ; it != log_names.end() ; ++it ) {
      int id = grid_->get_log_id(*it);
      if(id >=0 ) log_ids_.push_back( id   );
    }
  }

  virtual Grid_filter* clone() const {
    return new Grid_filter_log_names(this->grid_, this->log_ids_);
  }

  virtual ~Grid_filter_log_names(void){}

  virtual bool is_valid_nodeid(int nodeid) const {

    int log_id = grid_->get_log_id_from_nodeid(nodeid);
    std::vector<int>::const_iterator it = std::find( log_ids_.begin(), log_ids_.end(), log_id );
    return it != log_ids_.end();

  }

private :
  const Log_data_grid* grid_;
  std::vector<int> log_ids_;

};

#endif
