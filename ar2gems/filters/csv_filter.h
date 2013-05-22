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



#ifndef __GSTLAPPLI_CSV_FILTERS_H__ 
#define __GSTLAPPLI_CSV_FILTERS_H__ 
 
 
#include <filters/common.h>
#include <filters/filter.h> 
#include <filters/csv_filter_qt_dialogs.h>
#include <grid/grid_model/grid_property.h>
#include <vector>


class Gslib_infilter_dialog; 
class Gslib_input_grid_dialog; 
class Gslib_input_pointset_dialog; 
class QWidget; 
class Reduced_grid;
class Log_data_grid;
class Csv_input_pointset_dialog;


class FILTERS_DECL Csv_infilter :
  public Input_filter
{
 public: 
  static Named_interface* create_new_interface( std::string& ); 
  
public:
  Csv_infilter(void);
  virtual ~Csv_infilter(void);

  virtual std::string filter_name() const { return "csv" ; }
  virtual std::string file_extensions() const { return "*.csv"; }

  virtual bool can_handle( const std::string& filename );
  virtual Named_interface* read( const std::string& filename, 
                              std::string* errors = 0 ); 

protected :
  bool is_log_file( std::ifstream& infile);
  bool is_structured_grid( std::ifstream& infile);

 protected: 
  Csv_infilter_dialog* wizard_; 
};


/** This class defines the common interface of all gslib filters capable 
 * or parsing a given type of file. 
 * The "file" can be either read from an actual file or a string 
 */ 
class FILTERS_DECL Csv_specialized_infilter : public Input_filter { 
 public: 
  virtual ~Csv_specialized_infilter() {}; 
 
  virtual QWidget* init_dialog( std::ifstream& infile ) = 0; 
  virtual std::string filter_name() const { return "csv specialized" ; } 
  virtual Named_interface* read( const std::string& filename, 
                              std::string* errors = 0 ); 
  virtual Geostat_grid* read( std::ifstream& infile ) = 0; 

  virtual std::string object_filtered() { return ""; } 
 
  virtual bool has_valid_parameters() const = 0;
}; 

 
/** This class defines a filter capable of parsing a csv point-set file. 
 */ 

class FILTERS_DECL Csv_poinset_infilter : public Csv_specialized_infilter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Csv_poinset_infilter(); 
  virtual ~Csv_poinset_infilter(); 
   
  virtual QWidget* init_dialog( std::ifstream& infile ); 
  virtual std::string object_filtered() { return "point set" ; } 
  virtual Geostat_grid* read( std::ifstream& infile ); 
 
  virtual bool has_valid_parameters() const;

 protected: 
  Csv_input_pointset_dialog* dialog_; 

 private: 
  Csv_poinset_infilter( const Csv_poinset_infilter& ); 
  Csv_poinset_infilter& operator=( const Csv_poinset_infilter& ); 
}; 



/** This class defines a filter capable of parsing a csv log-set file.
 */

class FILTERS_DECL Csv_logdata_infilter : public Csv_specialized_infilter {
 public:
  static Named_interface* create_new_interface( std::string& );

 public:
  Csv_logdata_infilter();
  virtual ~Csv_logdata_infilter();

  virtual QWidget* init_dialog( std::ifstream& infile );
  virtual std::string object_filtered() { return "log data" ; }

  virtual Geostat_grid* read( std::ifstream& infile );

  virtual Geostat_grid* read_no_gui( std::ifstream& infile, std::string name = "" );

  virtual bool has_valid_parameters() const;

 protected:



//  TODO
 // Csv_input_logdata_dialog* dialog_;

 private:

  virtual Geostat_grid* read( std::ifstream& infile, std::string name, int dh_id,
							  int xstart_id, int ystart_id, int zstart_id,
							  int xend_id, int yend_id, int zend_id, int from_id, int to_id,
							  float nan = Grid_continuous_property::no_data_value );

  Csv_logdata_infilter( const Csv_logdata_infilter& );
  Csv_logdata_infilter& operator=( const Csv_logdata_infilter& );
};



/** This class defines a filter capable of parsing a csv grid file. 
 * The grid it will create from the file will be a cartesian grid.  
 */ 
class FILTERS_DECL Csv_grid_infilter : public Csv_specialized_infilter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Csv_grid_infilter(); 
  virtual ~Csv_grid_infilter(); 
   
  virtual QWidget* init_dialog( std::ifstream& infile ); 
  virtual std::string object_filtered() { return "cartesian grid" ; } 
  virtual Geostat_grid* read( std::ifstream& infile ); 
  virtual bool has_valid_parameters() const;


 protected: 
  Csv_input_grid_dialog* dialog_; 

 private: 
  Csv_grid_infilter( const Csv_grid_infilter& ); 
  Csv_grid_infilter& operator=( const Csv_grid_infilter& ); 

}; 
 
class FILTERS_DECL Csv_mgrid_infilter : public Csv_specialized_infilter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Csv_mgrid_infilter(); 
  virtual ~Csv_mgrid_infilter(); 
   
  virtual QWidget* init_dialog( std::ifstream& infile ); 
  virtual std::string object_filtered() { return "masked grid" ; } 
  virtual Geostat_grid* read( std::ifstream& infile ); 
  virtual bool has_valid_parameters() const;

 protected: 
  Csv_input_mgrid_dialog* dialog_; 

 private: 
  Csv_mgrid_infilter( const Csv_mgrid_infilter& ); 
  Csv_mgrid_infilter& operator=( const Csv_mgrid_infilter& ); 

  Geostat_grid * readRegularGridFormat(std::ifstream& infile,Reduced_grid *);
  Geostat_grid * readPointsetFormat(std::ifstream& infile,Reduced_grid *);

  bool get_mgrid_xyz_dimensions(
    std::ifstream& infile, Reduced_grid* grid, 
    int X_col_id, int Y_col_id, int Z_col_id,
    float x_size, float y_size, float z_size,
    float rotation_z_angle = 0.0);

  bool get_mgrid_ijk_dimensions(
    std::ifstream& infile, Reduced_grid* grid, 
    int X_col_id, int Y_col_id, int Z_col_id,
    float x_size, float y_size, float z_size);
};  




/** This class defines a filter capable of parsing a csv log-set file.
 */

class FILTERS_DECL Csv_structured_infilter : public Csv_specialized_infilter {
 public:
  static Named_interface* create_new_interface( std::string& );

 public:
  Csv_structured_infilter();
  virtual ~Csv_structured_infilter();

  virtual QWidget* init_dialog( std::ifstream& infile );
  virtual std::string object_filtered() { return "structured grid" ; }

  virtual Geostat_grid* read( std::ifstream& infile );

  virtual Geostat_grid* read_no_gui( std::ifstream& infile , std::string name = "" );

  virtual bool has_valid_parameters() const;

 protected:



//  TODO
 // Csv_input_logdata_dialog* dialog_;

 private:

  virtual Geostat_grid* read( std::ifstream& infile, std::string name, int xcorners,
							  int ycorners, int zcorners,
							  float nan = Grid_continuous_property::no_data_value );

  Csv_structured_infilter( const Csv_structured_infilter& );
  Csv_structured_infilter& operator=( const Csv_structured_infilter& );
};



/*
 * -----------------------
 * Output filter
 */
class Csv_outfilter: public Output_filter {
public:
  static Named_interface* create_new_interface( std::string& );
public:
  Csv_outfilter();
  virtual ~Csv_outfilter();

  virtual std::string filter_name() const { return "csv" ; }
  virtual std::string file_extensions() const { return "*.csv"; }

  virtual bool write( std::string outfile_name, const Named_interface* ni,
                      std::string* errors = 0 );

  virtual std::string type_data() const {return "Grid";}

private:
  bool write_log_data_grid( std::string outfile_name,const Log_data_grid* grid, std::string* errors = 0 );
};

#endif
