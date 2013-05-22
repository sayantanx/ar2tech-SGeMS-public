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
 * log_data_grid.h
 *
 *  Created on: Jun 21, 2011
 *      Author: aboucher
 */

#ifndef LOG_DATA_GRID_H_
#define LOG_DATA_GRID_H_

#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/utils/gstl_object_item.h>

class GRID_DECL Log_data : public GsTL_object_item {
public :

  struct Segment_geometry {
    int nodeid;
    GsTLPoint start; 
    GsTLPoint end;
    double from;
    double to;
    double length;

    Segment_geometry(){}

    Segment_geometry(int id,GsTLPoint startxyz, GsTLPoint endxyz, double from_length, double to_length) :
    nodeid(id),start(startxyz), end(endxyz), from(from_length), to(to_length), length(to-from){}

  };

	typedef std::map<int, std::pair<GsTLPoint,GsTLPoint> >  nodeid_to_log_coords;

	Log_data(std::string name_id, int id, GsTL_object_item* parent);
	~Log_data();

	std::string name() const;
	int id() const;

	GsTLPoint collar_location() const;
	void collar_location(GsTLPoint collar_location);

  void add_segment_geometry(Segment_geometry& segment_geom);
	std::pair<GsTLPoint,GsTLPoint> get_log_segment(int node_id) const;
  const Segment_geometry& get_segment_geometry(int nodeid) const;

  int nodeid_from_segmentid(int segmentid) const;
  int segmentid_from_nodeid(int nodeid) const;

	std::map<int, Segment_geometry>::const_iterator segment_begin();
	std::map<int, Segment_geometry>::const_iterator segment_end();

	std::map<int, Segment_geometry>::const_iterator segment_begin() const;
	std::map<int, Segment_geometry>::const_iterator segment_end() const;


	int number_of_segments() const;
	double min_segment_length() const;
	double max_segment_length() const;
	double average_segment_length();
  double segment_length(int nodeid) const;
  double total_length() const;
  double total_distance_from_collar() const;
  double distance_of_first_sample_from_collar() const;
  std::pair<double,double> get_from_to(int nodeid) const;

  bool are_segments_continuous(int start_segmentid, int segment_length ) const;
  bool are_segments_equal_length() const {return are_segments_equal_length_;}

  void update_geomety();


	// GsTL_object_item implementation
	 virtual QString item_type() const;
	 virtual GsTL_object_item *child(int row);
	 virtual int childCount() const;
	 virtual int columnCount() const;
	 virtual QVariant item_data(int column) const;
//	 virtual int row() const;


protected :
	int id_;
	std::string name_id_;
	GsTLPoint collar_location_;
  double total_length_;
  bool are_segments_equal_length_;

  std::map<int, Segment_geometry> log_geometry_;

	double min_segment_length_;
	double max_segment_length_;
	double average_segment_length_;


};



class GRID_DECL Log_data_manager : public GsTL_object_item {
public :
	Log_data_manager(GsTL_object_item *parent);
	~Log_data_manager();

	int number_of_logs() const;

	Log_data* add_log(std::string name); // return the code

	Log_data* get_log_data(std::string name_id);
	Log_data* get_log_data(int id);

	const Log_data* get_log_data(std::string name_id) const;
	const Log_data* get_log_data(int id) const;
 
	 std::string get_log_name(int index) const;
	 int get_log_id(std::string log_name) const;

	// GsTL_object_item implementation
	 virtual QString item_type() const;
	 virtual GsTL_object_item *child(int row);
	 virtual int childCount() const;
	 virtual int columnCount() const;
	 virtual QVariant item_data(int column) const;
	 virtual int row() const;


private :
	 std::map<int, Log_data* > log_data_;
	 std::map<std::string, int> log_name_to_id_;

};


class GRID_DECL Log_data_grid: public Point_set {
public:

	static Named_interface* create_new_interface( std::string& name_and_size_str);

	Log_data_grid();

	Log_data_grid(std::string name, int size);
	virtual ~Log_data_grid();
  
  void set_log_geometry( std::map<std::string, std::vector< Log_data::Segment_geometry>>&  log_geometry );

 // Returns the most specific name of the current class
 virtual std::string classname() const { return "Log_data_grid"; }

 int number_of_segment_inside_region(std::string name_id, const Grid_region* region=0) const;
 int number_of_segment_inside_region(int id, const Grid_region* region=0) const;

 std::string get_log_name(int index) const;
 std::string get_log_name_from_nodeid(int nodeid) const;
 int get_log_id(std::string log_name) const;
 int get_log_id_from_nodeid(int nodeid) const;

 int number_of_logs() const;
 Log_data& get_log_data(std::string name_id);
 Log_data& get_log_data(int id);
 
 const Log_data::Segment_geometry& get_segment_geometry(int nodeid) const;

 const Log_data& get_log_data(std::string name_id) const;
 const Log_data& get_log_data(int id) const;

 bool are_segments_equal_length() const {return are_segments_equal_length_;}

 virtual double get_support(int nodeid) const;


 virtual GsTLInt closest_node( const location_type& P ) {
   appli_warning( "NOT YET IMPLEMENTED");
   return -1;
 }


// GsTL_object_item implementation
 virtual QString item_type() const;
 virtual GsTL_object_item *child(int row);
 virtual const GsTL_object_item *child(int row) const;
 virtual int childCount() const;
 virtual int columnCount() const;
 virtual QVariant item_data(int column) const;

 virtual GsTL_object_item* properties_item();


protected:
// defined in Point_set.h

 Log_data_manager* log_manager_;
 std::vector<int>  log_id_;


private:
 Log_data_grid( const Log_data_grid& );
 Log_data_grid& operator = ( const Log_data_grid& );

 bool are_segments_equal_length_;
};



#endif /* LOG_DATA_GRID_H_ */
