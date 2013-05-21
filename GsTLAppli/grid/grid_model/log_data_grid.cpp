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



/*
 * log_data_grid.cpp
 *
 *  Created on: Jun 21, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/grid/grid_model/log_data_grid.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <cmath>


Log_data::Log_data(std::string name_id, int id, GsTL_object_item* parent)
: GsTL_object_item(parent), name_id_(name_id), id_(id), total_length_(0.0) {
		average_segment_length_ = -1;
		min_segment_length_ = 9e10;
		max_segment_length_ = -9e10;

}

Log_data::~Log_data(){}

std::string Log_data::name() const{
	return name_id_;
}

int Log_data::id() const{
	return id_;
}



GsTLPoint Log_data::collar_location() const{
	return collar_location_;
}

void Log_data::collar_location(GsTLPoint collar_location){
	collar_location_=collar_location;
}

void Log_data::add_segment_geometry(Log_data::Segment_geometry& segment_geom){
  std::map<int, Segment_geometry>::const_iterator it = log_geometry_.find(segment_geom.nodeid);
	if(it == log_geometry_.end()) {
    log_geometry_[segment_geom.nodeid] = segment_geom;
//    log_coords_[segment_geom.nodeid] = std::make_pair(segment_geom.start,segment_geom.end);
		GsTLPoint d = segment_geom.start-segment_geom.end;
		double length = sqrt(d.x()*d.x() + d.y()*d.y() + d.z()*d.z());
		if( length < min_segment_length_ ) min_segment_length_ = length;
		if( length > max_segment_length_ ) max_segment_length_ = length;
	//	lengths_[segment_geom.nodeid] = length;
    total_length_ +=  length;
 //   from_to_[segment_geom.nodeid] = std::make_pair(segment_geom.from,segment_geom.to);
	}
}


std::pair<GsTLPoint,GsTLPoint> Log_data::get_log_segment(int node_id) const{

  std::map<int, Segment_geometry>::const_iterator it = log_geometry_.find(node_id);

	//nodeid_to_log_coords::const_iterator it = log_coords_.find(node_id);
	if(it == log_geometry_.end()) {
		GsTLPoint nan_pt(GsTLGridProperty::no_data_value,GsTLGridProperty::no_data_value,GsTLGridProperty::no_data_value);
		return std::make_pair(nan_pt,nan_pt);
	}
	//return it->second;
  return std::make_pair( it->second.start, it->second.end);

}

const Log_data::Segment_geometry& Log_data::get_segment_geometry(int nodeid) const{
  std::map<int, Segment_geometry>::const_iterator it = log_geometry_.find(nodeid);
  return it->second;
}

int Log_data::nodeid_from_segmentid(int segmentid) const {
  if(segmentid >= log_geometry_.size()) return -1;
  std::map<int, Segment_geometry>::const_iterator it = log_geometry_.begin();
  std::advance(it,segmentid);
  return it->first;
}


int Log_data::segmentid_from_nodeid(int nodeid) const {
  std::map<int, Segment_geometry>::const_iterator it = log_geometry_.find(nodeid);
  if(it == log_geometry_.end()) return -1;
  return distance( log_geometry_.begin(), it);
}

	std::map<int, Log_data::Segment_geometry>::const_iterator Log_data::segment_begin(){
    return log_geometry_.begin();
  }
	std::map<int, Log_data::Segment_geometry>::const_iterator Log_data::segment_end(){
    return log_geometry_.end();
  }


	std::map<int, Log_data::Segment_geometry>::const_iterator Log_data::segment_begin() const{
    return log_geometry_.begin();
  }
	std::map<int, Log_data::Segment_geometry>::const_iterator Log_data::segment_end() const{
    return log_geometry_.end();
  }

 

int Log_data::number_of_segments() const{
	return log_geometry_.size();
}

double Log_data::min_segment_length() const{
	return min_segment_length_;
}
double Log_data::max_segment_length() const{
	return max_segment_length_;
}

double Log_data::average_segment_length() {
	if( average_segment_length_ < 0 ) {
		float sum = 0.0;
    std::map<int, Log_data::Segment_geometry>::const_iterator it = log_geometry_.begin();
		//std::map<int, float>::const_iterator it = lengths_.begin();
		for( ; it!= log_geometry_.end(); ++it) {
			sum += it->second.length;
		}
		average_segment_length_ = sum/log_geometry_.size();
	}
	return average_segment_length_;

}

double Log_data::segment_length(int nodeid) const {
  //std::map<int, float>::const_iterator it = lengths_.find( nodeid );
  std::map<int, Log_data::Segment_geometry>::const_iterator it = log_geometry_.find(nodeid);
  if( it == log_geometry_.end() ) return -1;
  return it->second.length;
}


double Log_data::total_length() const{
  return total_length_;
}

double Log_data::total_distance_from_collar() const{
  if(log_geometry_.empty()) return 0.0;
  std::map<int, Segment_geometry>::const_iterator it = (--log_geometry_.end());
  return it->second.to;
}

double Log_data::distance_of_first_sample_from_collar() const{
  if(log_geometry_.empty()) return 0.0;
  std::map<int, Segment_geometry>::const_iterator it = log_geometry_.begin();
  return it->second.from;
}


std::pair<double,double> Log_data::get_from_to(int nodeid) const{
  std::map<int, Log_data::Segment_geometry>::const_iterator it = log_geometry_.find(nodeid);
  if(it == log_geometry_.end()) return std::make_pair((double)-1.0,(double)-1.0);
  return std::make_pair(it->second.from,it->second.to);
}

bool Log_data::are_segments_continuous(int start_nodeid, int segment_length ) const {

  int start_segmentid = this->segmentid_from_nodeid(start_nodeid);
  if( start_segmentid < 0 || start_segmentid+segment_length > log_geometry_.size() ) return false;
 

  std::map<int, Log_data::Segment_geometry>::const_iterator it_current = log_geometry_.find(start_nodeid);
  std::map<int, Log_data::Segment_geometry>::const_iterator it_next = log_geometry_.find(start_nodeid);
  for(int i=0; i<segment_length-1; ++i ) {
    std::advance(it_next,1);
    if( it_current->second.to !=  it_next->second.from ) return false;
    it_current = it_next;
  }
 
  return true;

}

void Log_data::update_geomety(){
  // Check is all segements are equal length
  if(min_segment_length_ == max_segment_length_) {
    are_segments_equal_length_ = true;
    return;
  }
  //Check if the difference is not due to the last composite
  int n_segments = this->number_of_segments();
  for(int i=0;i<n_segments-1; ++i) {
    if( this->segment_length( this->nodeid_from_segmentid(i)) < max_segment_length_ ) {
      are_segments_equal_length_ = false;
      return;
    }
  }
  are_segments_equal_length_ = true;
}

	// GsTL_object_item implementation
QString Log_data::item_type() const {
	return "Log data";
}
GsTL_object_item *Log_data::child(int row) {
	return 0;
}
int Log_data::childCount() const {
	return 0;
}
int Log_data::columnCount() const {
	return 2;
}
QVariant Log_data::item_data(int column) const{
	if(column ==0)
		return QString(name_id_.c_str());
	else if(column ==1) {
		return this->item_type();
	}

	return QVariant();

}
/*
int Log_data::row() const{
	std::distance()
}*/


/*
 *   Log_data_manager
 */

Log_data_manager::Log_data_manager(GsTL_object_item *parent)
 : GsTL_object_item(parent) {


}
Log_data_manager::~Log_data_manager(){

}

int Log_data_manager::number_of_logs() const{
	return log_data_.size();
}

Log_data* Log_data_manager::add_log(std::string name){

	std::map<std::string, int>::iterator it = log_name_to_id_.find(name);
	if(it != log_name_to_id_.end() ) {
//		int i= log_name_to_id_[name];
		return log_data_[it->second];
	}
	int pos = log_name_to_id_.size();
	log_name_to_id_[name] = pos;

	log_data_[pos] = new Log_data(name, pos, this);

	return log_data_[pos];
	//return pos;

}

std::string Log_data_manager::get_log_name(int index) const{
	std::map<int, Log_data* >::const_iterator it = log_data_.find(index);
	if(it == log_data_.end()) return "";
	return it->second->name();
}

int Log_data_manager::get_log_id(std::string log_name) const{
	std::map<std::string, int >::const_iterator it = log_name_to_id_.find(log_name);
	if(it == log_name_to_id_.end()) return -1;
	else return it->second;
}


Log_data* Log_data_manager::get_log_data(std::string name_id){
	std::map<std::string, int >::iterator it = log_name_to_id_.find(name_id);
//	if(it == log_name_to_id.end());

	return log_data_[it->second];

}
Log_data* Log_data_manager::get_log_data(int id){
	std::map<int, Log_data* >::iterator it = log_data_.find(id);
//	if(it == log_data_.end()) return 0;

	return it->second;
}


const Log_data* Log_data_manager::get_log_data(std::string name_id) const{
	std::map<std::string, int >::const_iterator it_id = log_name_to_id_.find(name_id);
	std::map<int, Log_data* >::const_iterator it = log_data_.find(it_id->second);
//	if(it == log_name_to_id.end());

	return it->second;

}
const Log_data* Log_data_manager::get_log_data(int id) const{
	std::map<int, Log_data* >::const_iterator it = log_data_.find(id);
//	if(it == log_data_.end()) return 0;

	return it->second;
}


QString Log_data_manager::item_type() const {
	return "Logs";
}

GsTL_object_item *Log_data_manager::child(int row) {
	std::map<std::string, int >::const_iterator it = log_name_to_id_.begin();
	for(int i=0; i< row; ++i,++it){}

	return log_data_[it->second];
}

int Log_data_manager::childCount() const {
	return log_data_.size();
}

int Log_data_manager::columnCount() const {
	return 2;
}

int Log_data_manager::row() const{

	return parent_->childCount()-1;
}


QVariant Log_data_manager::item_data(int column) const{
	if(column ==0)
		return QString("Logs");


	return QVariant();

}



/*
 *
 */


Named_interface* Log_data_grid::create_new_interface( std::string& name_and_size_str){
	if( name_and_size_str.empty() )
		return 0;

	String_Op::string_pair params = String_Op::split_string(name_and_size_str, "::",true);
	int size = String_Op::to_number<int>( params.second );
	appli_assert( size >= 0 );
	return new Log_data_grid( params.first,size );
}

Log_data_grid::Log_data_grid()
   : Point_set()
{
	log_manager_ = new Log_data_manager(this);

}


Log_data_grid::Log_data_grid(std::string name, int size)
   : Point_set(name, size)
{
	log_manager_ = new Log_data_manager(this);

}

Log_data_grid::~Log_data_grid() {
	delete log_manager_;
}

void Log_data_grid::set_log_geometry( std::map<std::string, std::vector< Log_data::Segment_geometry>>&  log_geometry ){

	std::map<std::string, std::vector< Log_data::Segment_geometry> >::iterator it_log = log_geometry.begin();
	int number_of_values=0;
	for( ; it_log != log_geometry.end(); ++it_log) {
		number_of_values+= it_log->second.size();
	}

  are_segments_equal_length_ = true;

	std::vector< Point_set::location_type > point_locations(number_of_values);
  log_id_.insert(log_id_.begin(),number_of_values,-1);

	for(it_log = log_geometry.begin() ; it_log != log_geometry.end(); ++it_log) {
		Log_data* log_data = log_manager_->add_log(it_log->first);
    int log_id = log_data->id();
//		Log_data* log_data = log_manager_->get_log_data(id);
		std::vector< Log_data::Segment_geometry>::iterator it_segments = it_log->second.begin();
		for( ; it_segments != it_log->second.end(); ++it_segments) {
			log_data->add_segment_geometry(*it_segments);

			GsTLPoint loc;
			loc[0] = (it_segments->end.x()  + it_segments->start.x())/2.0;
			loc[1] = (it_segments->end.y()  + it_segments->start.y())/2.0;
			loc[2] = (it_segments->end.z()  + it_segments->start.z())/2.0;
			point_locations[it_segments->nodeid] = loc;
      log_id_[it_segments->nodeid] = log_id;
//			point_locations.push_back(loc);
		}
    log_data->update_geomety();
    if(  !log_data->are_segments_equal_length() ) are_segments_equal_length_=false;
	}

	this->point_locations( point_locations );
 


}


int Log_data_grid::number_of_logs() const{
	return log_manager_->number_of_logs();
}

std::string Log_data_grid::get_log_name(int index) const{
	return log_manager_->get_log_name(index);
}

std::string Log_data_grid::get_log_name_from_nodeid(int nodeid) const{
  return this->get_log_name( log_id_[nodeid]);
}

int Log_data_grid::get_log_id(std::string log_name) const{
	return log_manager_->get_log_id(log_name);
}

int Log_data_grid::get_log_id_from_nodeid(int nodeid) const {
  if(nodeid < 0 || nodeid>= this->size() ) return -1;
  return log_id_[nodeid];
}

const Log_data::Segment_geometry& Log_data_grid::get_segment_geometry(int nodeid) const{
  int log_id =  log_id_[nodeid];
  return log_manager_->get_log_data(log_id)->get_segment_geometry(nodeid);
}

Log_data& Log_data_grid::get_log_data(std::string name_id){
	return *log_manager_->get_log_data(name_id);
}

Log_data& Log_data_grid::get_log_data(int id){
	return *log_manager_->get_log_data(id);
}

const Log_data& Log_data_grid::get_log_data(std::string name_id) const{
	return *log_manager_->get_log_data(name_id);
}

const Log_data& Log_data_grid::get_log_data(int id) const{
	return *log_manager_->get_log_data(id);
}


int Log_data_grid::number_of_segment_inside_region(std::string name_id, const Grid_region* region) const{
	int id = this->get_log_id(name_id);
	if(id < 0 ) return -1;
	return this->number_of_segment_inside_region(id, region);
}

int Log_data_grid::number_of_segment_inside_region(int id, const Grid_region* region) const{
	const Log_data* ldata = log_manager_->get_log_data(id);
	if(region==0) region = this->selected_region();
	if( region == 0 ) return ldata->number_of_segments();

	int n = 0;
  std::map<int, Log_data::Segment_geometry>::const_iterator it = ldata->segment_begin();
 	for( ;it!=ldata->segment_end(); ++ it) {
		if( region->is_inside_region(it->first)) n++;
	}
	return n;
}


double Log_data_grid::get_support(int nodeid) const{
  const Log_data::Segment_geometry& geom = get_segment_geometry( nodeid);
  return geom.length;
}

QString Log_data_grid::item_type() const{
	return QString(this->classname().c_str());
}

GsTL_object_item *Log_data_grid::child(int row){
	if(row == 0) {
		return &point_prop_;
	}

	else if(row < group_manager_.size() +1) {
		std::map<std::string, GsTLGridPropertyGroup*>::iterator  it = group_manager_.begin_group();
    std::advance(it,row-1);
		//for(int i=1; i<row; ++i, ++it){}
		return it->second;
	}
	else if (row == group_manager_.size() + 1) {
    return &region_manager_;
  }
	else  {
		return log_manager_;
	}

}

const GsTL_object_item *Log_data_grid::child(int row) const{
	if(row == 0) {
		return &point_prop_;
	}
  	else if(row < group_manager_.size() +1) {
      std::map<std::string, GsTLGridPropertyGroup*>::const_iterator  it = group_manager_.begin_group();
      std::advance(it,row-1);
		  //for(int i=1; i<row; ++i, ++it){}
		  return it->second;
	}
	else if (row == group_manager_.size() + 1) {
    return &region_manager_;
  }
	else  {
		return log_manager_;
	}

}

int Log_data_grid::childCount() const{
	return group_manager_.size()+3;

}

int Log_data_grid::columnCount() const{
	return 2;

}

QVariant Log_data_grid::item_data(int column) const{
	if(column ==0)
		return QString(name_.c_str());
	if(column ==1)
		return this->item_type();

	return QVariant();

}

GsTL_object_item* Log_data_grid::properties_item(){
	return &point_prop_;
}


//=========================


