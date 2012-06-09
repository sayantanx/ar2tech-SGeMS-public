/* -----------------------------------------------------------------------------
** Copyright© 2012 Advanced Resources and Risk Technology, LLC
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
: GsTL_object_item(parent), name_id_(name_id), id_(id) {
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

void Log_data::add_log_segment(int node_id, GsTLPoint begin, GsTLPoint end){
	nodeid_to_log_coords::iterator it = log_coords_.find(node_id);
	if(it == log_coords_.end()) {
		log_coords_[node_id] = std::make_pair(begin,end);
		GsTLPoint d = end-begin;
		float length = sqrt(d.x()*d.x() + d.y()*d.y() + d.z()*d.z());
		if( length < min_segment_length_ ) min_segment_length_ = length;
		if( length > max_segment_length_ ) max_segment_length_ = length;
		lengths_[node_id] = length;

	}

}


std::pair<GsTLPoint,GsTLPoint> Log_data::get_log_segment(int node_id) const{

	nodeid_to_log_coords::const_iterator it = log_coords_.find(node_id);
	if(it == log_coords_.end()) {
		GsTLPoint nan_pt(GsTLGridProperty::no_data_value,GsTLGridProperty::no_data_value,GsTLGridProperty::no_data_value);
		return std::make_pair(nan_pt,nan_pt);
	}
	return it->second;

}

Log_data::nodeid_to_log_coords::const_iterator Log_data::position_begin(){
	return log_coords_.begin();
}

Log_data::nodeid_to_log_coords::const_iterator Log_data::position_end(){
	return log_coords_.end();
}


Log_data::nodeid_to_log_coords::const_iterator Log_data::position_begin() const {
	return log_coords_.begin();
}

Log_data::nodeid_to_log_coords::const_iterator Log_data::position_end() const{
	return log_coords_.end();
}

int Log_data::number_of_segments() const{
	return log_coords_.size();
}

float Log_data::min_segment_length() const{
	return min_segment_length_;
}
float Log_data::max_segment_length() const{
	return max_segment_length_;
}

float Log_data::average_segment_length() {
	if( average_segment_length_ < 0 ) {
		float sum = 0.0;
		std::map<int, float>::const_iterator it = lengths_.begin();
		for( ; it!= lengths_.end(); ++it) {
			sum += it->second;
		}
		average_segment_length_ = sum/lengths_.size();
	}
	return average_segment_length_;

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


void Log_data_grid::set_log_geometry( std::map<std::string, std::vector< std::pair<int, std::pair<GsTLPoint,GsTLPoint> > > >& log_segments)
{


	std::map<std::string, std::vector<  std::pair<int,std::pair<GsTLPoint,GsTLPoint> > > >::iterator it_log = log_segments.begin();
	int number_of_values=0;
	for( ; it_log != log_segments.end(); ++it_log) {
		number_of_values+= it_log->second.size();
	}

	std::vector< Point_set::location_type > point_locations(number_of_values);

	for(it_log = log_segments.begin() ; it_log != log_segments.end(); ++it_log) {
		Log_data* log_data = log_manager_->add_log(it_log->first);
//		Log_data* log_data = log_manager_->get_log_data(id);
		std::vector< std::pair<int,std::pair<GsTLPoint,GsTLPoint> > >::iterator it_segments = it_log->second.begin();
		for( ; it_segments != it_log->second.end(); ++it_segments) {
			log_data->add_log_segment(it_segments->first,it_segments->second.first,it_segments->second.second);

			GsTLPoint loc;
			loc[0] = (it_segments->second.second.x()  + it_segments->second.first.x())/2.0;
			loc[1] = (it_segments->second.second.y()  + it_segments->second.first.y())/2.0;
			loc[2] = (it_segments->second.second.z()  + it_segments->second.first.z())/2.0;
			point_locations[it_segments->first] = loc;
//			point_locations.push_back(loc);
		}
	}

	this->point_locations( point_locations );
}



int Log_data_grid::number_of_logs() const{
	return log_manager_->number_of_logs();
}

std::string Log_data_grid::get_log_name(int index) const{
	return log_manager_->get_log_name(index);
}

int Log_data_grid::get_log_id(std::string log_name) const{
	return log_manager_->get_log_id(log_name);
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


int Log_data_grid::number_of_segment_inside_region(std::string name_id, const GsTLGridRegion* region) const{
	int id = this->get_log_id(name_id);
	if(id < 0 ) return -1;
	return this->number_of_segment_inside_region(id, region);
}

int Log_data_grid::number_of_segment_inside_region(int id, const GsTLGridRegion* region) const{
	const Log_data* ldata = log_manager_->get_log_data(id);
	if(region==0) region = this->selected_region();
	if( region == 0 ) return ldata->number_of_segments();

	int n = 0;
	Log_data::nodeid_to_log_coords::const_iterator it = ldata->position_begin();
	for( ;it!=ldata->position_end(); ++ it) {
		if( region->is_inside_region(it->first)) n++;
	}
	return n;
}


QString Log_data_grid::item_type() const{
	return QString(this->classname().c_str());
}

GsTL_object_item *Log_data_grid::child(int row){
	if(row == 0) {
		return log_manager_;
	}
	else if(row == 1) {
		return &point_prop_;
	}
	else if(row < group_manager_.size() +2) {
		std::map<std::string, GsTLGridPropertyGroup*>::iterator  it = group_manager_.begin_group();
		for(int i=1; i<row; ++i, ++it){}
		return it->second;
	}
	else return &region_manager_;

}

const GsTL_object_item *Log_data_grid::child(int row) const{
	if(row == 0) {
		return log_manager_;
	}
	else if(row == 1) {
		return &point_prop_;
	}
	else if(row < group_manager_.size() +2) {
		std::map<std::string, GsTLGridPropertyGroup*>::const_iterator  it = group_manager_.begin_group();
		for(int i=1; i<row; ++i, ++it){}
		return it->second;
	}
	else return &region_manager_;

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


