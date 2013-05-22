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



#include <utils/gstl_messages.h>
#include <appli/manager_repository.h>
#include <filters/csv_filter.h>
#include <filters/csv_filter_qt_dialogs.h>
#include <grid/grid_model/cartesian_grid.h>
#include <grid/grid_model/point_set.h>
#include <utils/string_manipulation.h>
#include <grid/grid_model/reduced_grid.h>
#include <grid/grid_model/log_data_grid.h>
#include <grid/grid_model/structured_grid.h>

#include <qdialog.h>
#include <qapplication.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qcursor.h>

#include <string>
#include <cctype>


Named_interface* Csv_infilter::create_new_interface( std::string& ){
  return new Csv_infilter;
}


Csv_infilter::Csv_infilter() 
  : wizard_( new Csv_infilter_dialog( qApp->activeWindow() ) ) {
}

Csv_infilter::~Csv_infilter(void)
{
}


bool Csv_infilter::can_handle( const std::string& filename ){
  // To figure out if a file is a csv formatted file we do:
  //   - read the first line, and store the nuber of comma delimieted entries
  //   - Read the next 10 lines to ensure that the same number of entries than the first line

  QString qfilename(filename.c_str());
  if( !qfilename.endsWith(".csv",Qt::CaseInsensitive) )return false;
  QFile file( filename.c_str() );
  if( !file.open( QIODevice::ReadOnly ) ) return false;

  QTextStream stream( &file );
  stream.readLine();
  if( stream.atEnd() ) return false;

  QString str;
  stream >> str;
  stream.readLine();
  int nProperties = str.split(",").size();
//  int nPropertiesSpace = str.split(" ").size();
  bool ok;
  int test = str.toInt(&ok);
  if( ok ) return false;
 
  for( int i= 0; i < 10; i++ ) {
    if(stream.atEnd()) return true;
    stream.readLine();
    if( nProperties != str.split(",").size() ) return false;
  }
  return true;
}


Named_interface* Csv_infilter::read( const std::string& filename, 
                                 std::string* errors ){
  std::ifstream infile( filename.c_str() );
  if( !infile ) {
    if( errors ) 
      errors->append( "can't open file: " + filename );
  }

// This is where extra check could be done to automatically load the file
// based on user preferences stored in a (XML) file

  if( this->is_log_file(infile) ) {
    QStringList path = QString(filename.c_str()).split("/");
    QString grid_name = path.last().split(".").at(0);
	  Csv_logdata_infilter reader;
    return reader.read_no_gui(infile, grid_name.toStdString());
  }

  if( this->is_structured_grid(infile) ) {
    QStringList path = QString(filename.c_str()).split("/");
    QString grid_name = path.last().split(".").at(0);
	  Csv_structured_infilter reader;
	  return reader.read_no_gui(infile, grid_name.toStdString());
  }

  wizard_->set_file( infile );

  QApplication::setOverrideCursor( QCursor(Qt::ArrowCursor) );
    
  if( wizard_->exec() == QDialog::Rejected ) {
    QApplication::restoreOverrideCursor();
    return 0;
  }

  QApplication::restoreOverrideCursor();

  Csv_specialized_infilter* filter = wizard_->filter();
  return filter->read( infile ) ;
}


bool Csv_infilter::is_log_file( std::ifstream& infile){
	std::streampos mark = infile.tellg();
	std::string str;
	std::getline(infile, str);
	infile.seekg(mark);
	QString qstr(str.c_str());
	QStringList column_names = qstr.split(",");

	int dh_id = column_names.indexOf("dhid");
	if(dh_id == -1 ) return false;
	int xstart_id = column_names.indexOf("topx");
	if(xstart_id == -1 ) return false;
	int ystart_id = column_names.indexOf("topy");
	if(ystart_id == -1 ) return false;
	int zstart_id = column_names.indexOf("topz");
	if(zstart_id == -1 ) return false;
	int xend_id = column_names.indexOf("botx");
	if(xend_id == -1 ) return false;
	int yend_id = column_names.indexOf("boty");
	if(yend_id == -1 ) return false;
	int zend_id = column_names.indexOf("botz");
	if(zend_id == -1 ) return false;


	return true;
}


bool Csv_infilter::is_structured_grid( std::ifstream& infile){
	std::streampos mark = infile.tellg();
	std::string str;
	std::getline(infile, str);
	infile.seekg(mark);
	QString qstr(str.c_str());
	QStringList column_names = qstr.split(",");

  if(column_names.size() !=3) return false;

	int x_id = column_names.indexOf("xcorners");
	if(x_id == -1 ) return false;
	int y_id = column_names.indexOf("ycorners");
	if(y_id == -1 ) return false;
	int z_id = column_names.indexOf("zcorners");
	if(z_id == -1 ) return false;



	return true;
}

//========================================================


Named_interface* Csv_specialized_infilter::read( const std::string& filename, 
                                                std::string* errors ) {
  std::ifstream in( filename.c_str() );
  if( !in ) {
    if( errors )
      errors->append( "can't open file: " + filename );
    return 0;
  }

  return this->read( in );
}

//==============================================================
Named_interface* Csv_poinset_infilter::create_new_interface( std::string& ) {
  return new Csv_poinset_infilter();
}

Csv_poinset_infilter::Csv_poinset_infilter() 
 : dialog_( new Csv_input_pointset_dialog( qApp->activeWindow() ) ) {
}

Csv_poinset_infilter::~Csv_poinset_infilter() {
  dialog_ = 0;
}

QWidget* Csv_poinset_infilter::init_dialog( std::ifstream& infile ) {
  dialog_ = new Csv_input_pointset_dialog( infile, qApp->activeWindow() );
  return dialog_;
}

bool Csv_poinset_infilter::has_valid_parameters() const {
  return !dialog_->name().isEmpty();
}

Geostat_grid* Csv_poinset_infilter::read( std::ifstream& infile ) {

	QByteArray tmp = dialog_->name().simplified().toLatin1();
	std::string name( tmp.constData() );

  int X_col_id = dialog_->X_column_index();
  int Y_col_id = dialog_->Y_column_index();
  int Z_col_id = dialog_->Z_column_index();

  if( X_col_id == Y_col_id || X_col_id == Z_col_id || Y_col_id == Z_col_id ) {
    GsTLcerr << "The same column was selected for multiple coordinates \n" << gstlIO::end;
    return 0;
  }

  bool use_no_data_value = dialog_->use_no_data_value();
  float no_data_value = Grid_continuous_property::no_data_value;
  if( dialog_->use_no_data_value() ) {
    no_data_value = dialog_->no_data_value();
  }


  std::string str;
  std::getline(infile, str);
  QString qstr(str.c_str());
  QStringList property_names = qstr.split(",");

  bool is_x_provided = dialog_->X_column_name() != "None";
  bool is_y_provided = dialog_->Y_column_name() != "None";
  bool is_z_provided = dialog_->Z_column_name() != "None";
  if(is_x_provided) property_names.removeOne(dialog_->X_column_name());
  if(is_y_provided) property_names.removeOne(dialog_->Y_column_name());
  if(is_z_provided) property_names.removeOne(dialog_->Z_column_name());


  std::vector< std::vector< QString > > property_values( property_names.size() );
  std::vector< Point_set::location_type > point_locations;

//  For a csv file no data value is indicated by an empty field e.g. {34,,5.5}
  while( std::getline(infile, str) ) {
    qstr = str.c_str();
    QStringList fields = qstr.split(",");

    Point_set::location_type loc;
    if(is_x_provided) loc[0] = fields[X_col_id].toDouble();
    if(is_y_provided) loc[1] = fields[Y_col_id].toDouble();
    if(is_z_provided) loc[2] = fields[Z_col_id].toDouble();
    point_locations.push_back(loc);

    unsigned int i=0;
   for(unsigned int j=0;j<fields.size();j++) {
      if(j==0) i=0;
      if(j != X_col_id && j != Y_col_id && j != Z_col_id) {
        property_values[i].push_back(fields[j]);
        i++;
      }
    }
  }
  
//   done reading file
//----------------------------

  int point_set_size = point_locations.size();
  appli_message( "read " << point_set_size << " points" );

  // We now have a vector containing all the locations and another one with
  // all the property values.
  // Create a pointset, initialize it with the data we collected, and we're done
 
  // ask manager to get a new pointset and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  std::string name_size_str = name+"::"+String_Op::to_string( point_set_size );
  ni = Root::instance()->new_interface( "point_set://" + name_size_str,
				  gridModels_manager + "/" + name );
  Point_set* pset = dynamic_cast<Point_set*>( ni.raw_ptr() );
  appli_assert( pset != 0 );

  pset->point_locations( point_locations );


  for( unsigned int k= 0; k < property_names.size(); k++ ) {
  // Need to find out if property is categorical
    unsigned int check_size = std::min(30,static_cast<int>(property_values[k].size()));
    bool is_categ = false;
    for(unsigned int i=0; i<check_size ; i++ ) {
      bool ok;
      property_values[k][i].toFloat(&ok);
      if(!ok)  {
        is_categ = true;
        break;
      }
    }

    if(!is_categ) {
      Grid_continuous_property* prop = pset->add_property( property_names[k].toStdString() );
      for( int l=0; l < point_set_size; l++ ) {
        float val = property_values[k][l].toFloat();
        if(use_no_data_value && val == no_data_value) 
          val = Grid_continuous_property::no_data_value;
        prop->set_value( val, l );
      }
    }
    else {
      //Create categorical definition
      // by default it is named: grid-propertyName
      std::string catdef_name = name+"-"+property_names[k].toStdString();
       ni = Root::instance()->new_interface( "categoricaldefinition://"+catdef_name,categoricalDefinition_manager +"/"+catdef_name );
        CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

      std::set<QString> cat_names;
      for( int i=0; i < point_set_size; i++ ) {
        cat_names.insert(property_values[k][i]);
        //cat_def->add_category(property_values[k][i].toStdString());
      }
      std::set<QString>::iterator  it_cat_names = cat_names.begin();
      for( int code; it_cat_names != cat_names.end(); ++code, ++it_cat_names) {
        cat_def->add_category(code, it_cat_names->toStdString() );
      }
      Grid_categorical_property* prop = pset->add_categorical_property( property_names[k].toStdString(),cat_def->name() );
//      prop->set_category_definition(cat_def->name());
      QString no_data_value_str = QString().arg( no_data_value);
      for( int i=0; i < point_set_size; i++ ) {
        QString val =  property_values[k][i];
        if(use_no_data_value && val == no_data_value_str)  {
          prop->set_value( Grid_continuous_property::no_data_value, i );
        }
        else 
          prop->set_value( val.toStdString(), i );
      }
    }
  }


  return pset;
}

//================================================================================



//==============================================================
Named_interface* Csv_logdata_infilter::create_new_interface( std::string& ) {
  return new Csv_logdata_infilter();
}

Csv_logdata_infilter::Csv_logdata_infilter() {
// : dialog_( new Csv_input_pointset_dialog( qApp->activeWindow() ) ) {
}

Csv_logdata_infilter::~Csv_logdata_infilter() {
//  dialog_ = 0;
}

QWidget* Csv_logdata_infilter::init_dialog( std::ifstream& infile ) {
//  dialog_ = new Csv_input_pointset_dialog( infile, qApp->activeWindow() );
//  return dialog_;
	return new QWidget();
}

bool Csv_logdata_infilter::has_valid_parameters() const {
//  return !dialog_->name().isEmpty();
	return true;
}

Geostat_grid* Csv_logdata_infilter::read( std::ifstream& infile ) {
  return 0;
}


Geostat_grid* Csv_logdata_infilter::read_no_gui(ifstream& infile, std::string name) {
	std::string str;
	std::getline(infile, str);
	QString qstr(str.c_str());
	QStringList column_names = qstr.split(",");

	int dh_id = column_names.indexOf("dhid");
	if(dh_id == -1 ) return 0;
	int xstart_id = column_names.indexOf("topx");
	if(xstart_id == -1 ) return 0;
	int ystart_id = column_names.indexOf("topy");
	if(ystart_id == -1 ) return 0;
	int zstart_id = column_names.indexOf("topz");
	if(zstart_id == -1 ) return 0;
	int xend_id = column_names.indexOf("botx");
	if(xend_id == -1 ) return 0;
	int yend_id = column_names.indexOf("boty");
	if(yend_id == -1 ) return 0;
	int zend_id = column_names.indexOf("botz");
	if(zend_id == -1 ) return 0;

  //Check for if from-to is present
	int from_id = column_names.indexOf("from");
	if(from_id == -1 ) return 0;
	int to_id = column_names.indexOf("to");
	if(to_id == -1 ) return 0;

	infile.seekg(0);

  if(name.empty()) name = "test-dh";

	return this->read(infile, name,dh_id,
					  xstart_id, ystart_id, zstart_id,
					  xend_id, yend_id, zend_id, from_id, to_id, -99);


}

Geostat_grid* Csv_logdata_infilter::read( std::ifstream& infile, std::string name, int dh_id,
							  int xstart_id, int ystart_id, int zstart_id,
							  int xend_id, int yend_id, int zend_id, int from_id, int to_id,
							  float nan  ){


  std::string str;
  std::getline(infile, str);
  QString qstr(str.c_str());
  QStringList property_names = qstr.split(",");

  QString nan_str = QString("%1").arg(nan);

  QStringList coord_names;
  coord_names.append(property_names.at(xstart_id));
  coord_names.append(property_names.at(ystart_id));
  coord_names.append(property_names.at(zstart_id));
  coord_names.append(property_names.at(xend_id));
  coord_names.append(property_names.at(yend_id));
  coord_names.append(property_names.at(zend_id));
  coord_names.append(property_names.at(dh_id));


  for(int i=0; i<coord_names.size(); ++i  ) {
	  property_names.removeOne(coord_names.at(i));
  }




  std::vector< std::vector< QString > > property_values( property_names.size() );

//  typedef std::vector< std::pair< int, std::pair<GsTLPoint ,GsTLPoint > > > segment_geometry_vectorT;
//  segment_geometry_vectorT  log_segments;

//  std::map<std::string, segment_geometry_vectorT>  dh_log_segments;

  std::vector<Log_data::Segment_geometry> log_geometry;
  std::map<std::string, std::vector<Log_data::Segment_geometry>>  grid_log_geometry;

  std::vector< Point_set::location_type > start_point_locations;
  std::vector< Point_set::location_type > end_point_locations;
  std::vector< std::string > dh_id_values;

//  For a csv file no data value is indicated by an empty field e.g. {34,,5.5}
  int grid_size=0;
  while( std::getline(infile, str) ) {
    qstr = str.c_str();
    QStringList fields = qstr.split(",");

    GsTLPoint start_loc, end_loc;

    start_loc[0] = fields[xstart_id].toDouble();
    start_loc[1] = fields[ystart_id].toDouble();
    start_loc[2] = fields[zstart_id].toDouble();
    start_point_locations.push_back(start_loc);

    end_loc[0] = fields[xend_id].toDouble();
    end_loc[1] = fields[yend_id].toDouble();
    end_loc[2] = fields[zend_id].toDouble();
    end_point_locations.push_back(end_loc);


    double from = fields[from_id].toDouble();
    double to = fields[to_id].toDouble();

 //   log_segments.push_back(std::make_pair( grid_size, std::make_pair(start_loc,end_loc)));
 //   log_geometry.push_back( Log_data::Segment_geometry( grid_size, start_loc,end_loc, from, to) );

    std::string dh_name = fields[dh_id].toStdString();
/*
    std::map<std::string, segment_geometry_vectorT>::iterator it = dh_log_segments.find(dh_name);
    if( it  == dh_log_segments.end()) {
    	segment_geometry_vectorT vector;
    	dh_log_segments[dh_name] = vector;
    	it = dh_log_segments.find(dh_name);
    }
    it->second.push_back(std::make_pair( grid_size, std::make_pair(start_loc,end_loc)));
*/
    std::map<std::string, std::vector<Log_data::Segment_geometry>>::iterator it_grid = grid_log_geometry.find(dh_name);
    if( it_grid  == grid_log_geometry.end()) {
    	std::vector<Log_data::Segment_geometry> vector_segments;
    	grid_log_geometry[dh_name] = vector_segments;
    	it_grid = grid_log_geometry.find(dh_name);
    }
    it_grid->second.push_back(Log_data::Segment_geometry( grid_size, start_loc,end_loc, from, to));


    dh_id_values.push_back(fields[dh_id].toStdString());


    unsigned int i=0;
   for(unsigned int j=0;j<fields.size();j++) {
      if(j==0) i=0;
      if(j != xstart_id && j != ystart_id && j != zstart_id  &&
    	 j != xend_id && j != yend_id && j != zend_id 	&& j != dh_id  ) {
        property_values[i].push_back(fields[j]);
        i++;
      }
    }
   grid_size++;

  }

//   done reading file
//----------------------------


  appli_message( "read " << grid_size << " points" );

  // We now have a vector containing all the locations and another one with
  // all the property values.
  // Create a pointset, initialize it with the data we collected, and we're done

  // ask manager to get a new pointset and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  std::string name_size_str = name+"::"+String_Op::to_string( grid_size );
  ni = Root::instance()->new_interface( "log_grid://" + name_size_str,
				  gridModels_manager + "/" + name );
  Log_data_grid* log_grid = dynamic_cast<Log_data_grid*>( ni.raw_ptr() );
  appli_assert( log_grid != 0 );

  //log_grid->set_log_geometry(dh_log_segments);
  log_grid->set_log_geometry(grid_log_geometry);



  for( unsigned int k= 0; k < property_names.size(); k++ ) {
  // Need to find out if property is categorical
    unsigned int check_size = std::min(30,static_cast<int>(property_values[k].size()));
    bool is_categ = false;
    for(unsigned int i=0; i<check_size ; i++ ) {
      bool ok;
      property_values[k][i].toFloat(&ok);
      if(!ok)  {
        is_categ = true;
        break;
      }
    }

    if(!is_categ) {
      Grid_continuous_property* prop = log_grid->add_property( property_names[k].toStdString() );
      for( int l=0; l < grid_size; l++ ) {
    	  if( property_values[k][l] != nan_str ) {
    		  prop->set_value( property_values[k][l].toFloat(), l );
    	  }
      }
    }
    else {
      //Create categorical definition
      // by default it is named: grid-propertyName
      std::string catdef_name = name+"-"+property_names[k].toStdString();
       ni = Root::instance()->new_interface( "categoricaldefinition://"+catdef_name,categoricalDefinition_manager +"/"+catdef_name );
        CategoricalPropertyDefinitionName* cat_def =
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

      std::set<QString> cat_names;
      for( int i=0; i < grid_size; i++ ) {
        cat_names.insert(property_values[k][i]);
        //cat_def->add_category(property_values[k][i].toStdString());
      }
      std::set<QString>::iterator it_cat_names = cat_names.begin();
      for(int code = 0; it_cat_names != cat_names.end(); ++it_cat_names, ++code) {
        cat_def->add_category( code, it_cat_names->toStdString() );
      }

      Grid_categorical_property* prop = log_grid->add_categorical_property( property_names[k].toStdString(),cat_def->name() );
//      prop->set_category_definition(cat_def->name());
      for( int i=0; i < grid_size; i++ ) {
    	  if( property_values[k][i] != nan_str ) {
    		  prop->set_value( property_values[k][i].toStdString(), i );
    	  }
      }
    }
  }


  return log_grid;


}

//================================================================================



//========================================================

Named_interface* Csv_grid_infilter::create_new_interface( std::string& ) {
  return new Csv_grid_infilter();
}

Csv_grid_infilter::Csv_grid_infilter() 
: dialog_( new Csv_input_grid_dialog( qApp->activeWindow() ) ) {
}

Csv_grid_infilter::~Csv_grid_infilter() {
  // deleting dialog_ causes a segfault. See why.
  //  delete dialog_;
  dialog_ = 0;
}

QWidget* Csv_grid_infilter::init_dialog( std::ifstream& ) {
  dialog_ = new Csv_input_grid_dialog( qApp->activeWindow() );
  return dialog_;
}


Geostat_grid* Csv_grid_infilter::read( std::ifstream& infile ) {

  int nx = dialog_->nx();
  int ny = dialog_->ny();
  int nz = dialog_->nz();
  float x_size = dialog_->x_size();
  float y_size = dialog_->y_size();
  float z_size = dialog_->z_size();
  float Ox = dialog_->Ox();
  float Oy = dialog_->Oy();
  float Oz = dialog_->Oz();
  float rotation_z_angle = dialog_->rotation_z();

  bool use_no_data_value = dialog_->use_no_data_value();
  float no_data_value = Grid_continuous_property::no_data_value;
  QString no_data_value_str = QString().arg(no_data_value);
  if( dialog_->use_no_data_value() ) {
    no_data_value = dialog_->no_data_value();
    no_data_value_str = QString::number(no_data_value);

  }

  QByteArray tmp = dialog_->name().simplified().toLatin1();
  std::string name( tmp.constData() );

  // ask manager to get a new grid and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  appli_message( "creating new grid '" << name << "'" 
		             << " of dimensions: " << nx << "x" << ny << "x" << nz);

  ni = Root::instance()->new_interface( "cgrid://"+name,
                                        gridModels_manager + "/" + name );
  Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  appli_assert( grid != 0 );

  grid->set_dimensions( nx, ny, nz,
			x_size, y_size, z_size);
  grid->origin( GsTLPoint( Ox,Oy,Oz) );
  grid->set_rotation_z(rotation_z_angle);
  appli_message( "grid resized to " << nx << "x" << ny << "x" << nz
		<< "  total=: " << grid->size() );

  std::string buffer;
  
  //-------------------------
  //   now, read the file

  std::getline( infile, buffer, '\n');
  QStringList property_names = QString(buffer.c_str()).split(",");

//Read one column at a time
  std::streampos start_data = infile.tellg();
  for(unsigned int j = 0; j< property_names.size(); j++) {
    infile.clear();
    infile.seekg( start_data );
    // Check if property j is categorical
    bool is_categ = false;
    for(unsigned int i=0; i<30 ; i++ ) {
      bool ok;
      if( std::getline(infile, buffer) ) break;
      QString qstr(buffer.c_str());
      QStringList values_str = qstr.split(",");
      values_str[j].toFloat(&ok);
      if(!ok)  {
        is_categ = true;
        break;
      }
    }
    infile.clear();
    infile.seekg( start_data );

    if(is_categ) {
      Grid_categorical_property* prop = 
        grid->add_categorical_property(property_names[j].toStdString());
      
      ni = Root::instance()->new_interface( categoricalDefinition_manager, name+"-"+property_names[j].toStdString());
      CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

      std::set<QString> cat_names;
      while( std::getline(infile, buffer) ) {
        QString qstr(buffer.c_str());
        QStringList values_qstr = qstr.split(",");
        cat_names.insert(values_qstr[j]);
        //cat_def->add_category(values_qstr[j].toStdString());
      }

      std::set<QString>::iterator it_cat_names = cat_names.begin();
      for(int code=0; it_cat_names != cat_names.end(); ++it_cat_names ) {
        cat_def->add_category(code,it_cat_names->toStdString() );
      }

      prop->set_category_definition(cat_def->name());
      infile.clear();
      infile.seekg( start_data );

      int node_id=0;
      while( std::getline(infile, buffer) ) {
        QString qstr(buffer.c_str());
        QStringList values_qstr = qstr.split(",");
        QString val = values_qstr[j];
        if(use_no_data_value && val == no_data_value_str)  {
          prop->set_value( Grid_continuous_property::no_data_value, node_id );
        }
        else {
          prop->set_value( val.toStdString(), node_id );
        }
        node_id++;
      }
    }
    else {
      Grid_continuous_property* prop = 
        grid->add_property(property_names[j].toStdString());
      int node_id=0;
      while( std::getline(infile, buffer) ) {
        QString qstr(buffer.c_str());
        QStringList values_qstr = qstr.split(",");
        if(!values_qstr[j].isEmpty()) {
          bool ok;
          float val = values_qstr[j].toFloat(&ok);
           if(ok  && val != no_data_value) prop->set_value(val,node_id);
        }
        node_id++;
      }
    }

  }
  return grid;
}



bool Csv_grid_infilter::has_valid_parameters() const {
  return !dialog_->name().isEmpty();
}





//========================================================

Named_interface* Csv_mgrid_infilter::create_new_interface( std::string& ) {
  return new Csv_mgrid_infilter();
}

Csv_mgrid_infilter::Csv_mgrid_infilter() : 
	dialog_(new Csv_input_mgrid_dialog( qApp->activeWindow() ) )
{}

Csv_mgrid_infilter::~Csv_mgrid_infilter() {
  dialog_ = 0;
}

QWidget* Csv_mgrid_infilter::init_dialog( std::ifstream& infile) {
  dialog_ = new Csv_input_mgrid_dialog(infile, qApp->activeWindow() );
  return dialog_;
}


bool Csv_mgrid_infilter::get_mgrid_xyz_dimensions(
    std::ifstream& infile, Reduced_grid* grid, 
    int X_col_id, int Y_col_id, int Z_col_id,
    float x_size, float y_size, float z_size,
    float rotation_z_angle)
{

  // Read the first line with property name
  std::string buffer;
  std::getline( infile, buffer, '\n');

  // need to back rotate the coordinate into ijk
  double sin_rot_z = std::sin((double)rotation_z_angle*GsTL::PI/180.00);
  double cos_rot_z = std::cos((double)rotation_z_angle*GsTL::PI/180.00);

  std::vector<Geostat_grid::location_type> xyz;
  Geostat_grid::location_type max_xyz(-1e20,-1e20,-1e20);
  Geostat_grid::location_type max_xyz_rotated(-1e20,-1e20,-1e20);
  Geostat_grid::location_type origin(9e20,9e20,9e20);
  Geostat_grid::location_type origin_rotated(9e20,9e20,9e20);
  while( infile ) {
		std::getline( infile, buffer, '\n');
    if (buffer.empty()) break;
		std::vector<std::string> buf = String_Op::decompose_string( buffer, ",", false );
		if (buf.empty()) break;

		Geostat_grid::location_type loc;
    Geostat_grid::location_type loc_rotated;

		loc[0] = String_Op::to_number<float>(buf[X_col_id]);
		loc[1] = String_Op::to_number<float>(buf[Y_col_id]);
		loc[2] = String_Op::to_number<float>(buf[Z_col_id]);
    loc_rotated[0] = cos_rot_z*loc[0] - sin_rot_z*loc[1];
    loc_rotated[1] = sin_rot_z*loc[0] + cos_rot_z*loc[1];
    xyz.push_back(loc);

    //need to find the lower left corner given the rotated space
    if(loc_rotated[0] > max_xyz_rotated[0]) {
      //max_xyz[0] = loc[0];
      max_xyz_rotated[0] = loc_rotated[0];
    }
    if(loc_rotated[1] > max_xyz_rotated[1]){
      //max_xyz[1] = loc[1];
      max_xyz_rotated[1] = loc_rotated[1];
    }
    if(loc[2] > max_xyz_rotated[2]){
      max_xyz_rotated[2] = loc[2];
    }

    if(loc_rotated[0] < origin_rotated[0]){
//      origin[0] = loc[0];
      origin_rotated[0] = loc_rotated[0];
    }
    if(loc_rotated[1] < origin_rotated[1]){
  //    origin[1] = loc[1];
      origin_rotated[1] = loc_rotated[1];
    }
    if(loc[2] < origin_rotated[2]){
      origin_rotated[2] = loc[2];
    }

  }
  GsTLGridNode nxyz(int((max_xyz_rotated[0]-origin_rotated[0])/x_size)+1,
                    int((max_xyz_rotated[1]-origin_rotated[1])/y_size)+1,
                    int((max_xyz_rotated[2]-origin_rotated[2])/z_size)+1);

  // Get the origin in the real space
    origin[0] = cos_rot_z*origin_rotated[0] + sin_rot_z*origin_rotated[1];
    origin[1] = -1*sin_rot_z*origin_rotated[0] + cos_rot_z*origin_rotated[1];
    origin[2] = origin_rotated[2];


  grid->set_dimensions(nxyz[0],nxyz[1],nxyz[2],
      x_size, y_size, z_size, xyz, origin, rotation_z_angle);
  


  return true;
}


Geostat_grid* Csv_mgrid_infilter::readRegularGridFormat(std::ifstream& infile,Reduced_grid * grid)
{

  infile.clear();
  infile.seekg(0, ios::beg);
	std::vector<std::string> nums;

	int nx = dialog_->nx();
	int ny = dialog_->ny();
	int nz = dialog_->nz();
	float x_size = dialog_->x_size();
	float y_size = dialog_->y_size();
	float z_size = dialog_->z_size();
	float Ox = dialog_->Ox();
	float Oy = dialog_->Oy();
	float Oz = dialog_->Oz();
  float rotation_z_angle = dialog_->rotation_z();

	grid->set_dimensions( nx, ny, nz,
		x_size, y_size, z_size);
	grid->origin( GsTLPoint( Ox,Oy,Oz) );
  grid->set_rotation_z(rotation_z_angle);

	std::string buffer;

  int maskColNumber = dialog_->mask_column_index();
  int X_col_id = dialog_->X_column_index();
  int Y_col_id = dialog_->Y_column_index();
  int Z_col_id = dialog_->Z_column_index();

  bool use_no_data_value = dialog_->use_no_data_value();
  float no_data_value = Grid_continuous_property::no_data_value;
  QString no_data_value_str = QString().arg(no_data_value);
  if( dialog_->use_no_data_value() ) {
    no_data_value = dialog_->no_data_value();
    no_data_value_str = QString::number(no_data_value);
  }
	//-------------------------
	//   now, read the file
	std::getline( infile, buffer, '\n');
	std::vector<std::string> property_names = String_Op::decompose_string( buffer, ",", false );


  int beg_data_pos = infile.tellg();
  std::vector<bool> mask;
  mask.reserve(grid->rgrid_size());
  int size_active=0;

// Read Mask
  for( int i=0; i < grid->rgrid_size() ; i++ ) {
		std::getline( infile, buffer, '\n');
		std::vector<std::string> buf = String_Op::decompose_string(buffer, ",", false);
    if(buf.size() < property_names.size()) {
	    GsTLcerr << "Invalid file format\n Line " <<i<<" does not have " <<static_cast<int>(property_names.size())<<" columns"<< gstlIO::end;
		  return NULL;
    }
    mask.push_back( buf[maskColNumber] == "1" );
	}
  grid->mask( mask );


	// reposition the stream
	infile.clear();
	infile.seekg( 0 );
	std::getline( infile, buffer, '\n');

//Read one column at a time
  std::streampos start_data = infile.tellg();
  for(unsigned int j = 0; j< property_names.size(); j++) {
    infile.clear();
    infile.seekg( start_data );
    // Check if property j is categorical
    bool is_categ = false;
    for(unsigned int i=0; i<30 ; i++ ) {
      if( std::getline(infile, buffer) ) break;
      std::vector<std::string> values_str = String_Op::decompose_string(buffer, ",", false);
      is_categ = !String_Op::is_number(values_str[j]);
      if(is_categ) break;
    }
    infile.clear();
    infile.seekg( start_data );

    if(is_categ) {
      Grid_categorical_property* prop = 
        grid->add_categorical_property(property_names[j]);

      // Set the category definition
      SmartPtr<Named_interface> ni = 
        Root::instance()->new_interface( categoricalDefinition_manager, 
                                         dialog_->name().toStdString()+"-"+property_names[j]);
      CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());


      std::set<QString> cat_names;
      while( std::getline(infile, buffer) ) {
        QString qstr(buffer.c_str());
        QStringList values_qstr = qstr.split(",");
        cat_names.insert(values_qstr[j]);
        //cat_def->add_category(values_qstr[j].toStdString());
      }

      std::set<QString>::iterator it_cat_names = cat_names.begin();
      for(int code=0; it_cat_names != cat_names.end(); ++it_cat_names ) {
        cat_def->add_category(code,it_cat_names->toStdString() );
      }

      prop->set_category_definition(cat_def->name());
      infile.clear();
      infile.seekg( start_data );

      // Read the data
      int node_id=0;
      int index = 0;
      while( std::getline(infile, buffer) ) {
        if( mask[index] ) {
          QString qstr(buffer.c_str());
          QStringList values_qstr = qstr.split(",");
          QString val = values_qstr[j];
          if(!val.isEmpty() && val != no_data_value_str ) {
            prop->set_value(values_qstr[j].toStdString(),node_id);
          }
          node_id++;
        }
        index++;
      }
    }
    else {
      Grid_continuous_property* prop = 
        grid->add_property(property_names[j]);
      int node_id=0;
      int index = 0;
      while( std::getline(infile, buffer) ) {
        if(mask[index]) {
          QString qstr(buffer.c_str());
          QStringList values_qstr = qstr.split(",");
          if(!values_qstr[j].isEmpty()) {
            bool ok;
            float val = values_qstr[j].toFloat(&ok);
            if(ok && val != no_data_value ) prop->set_value(val,node_id);
          }
          node_id++;
        }
        index++;
      }
      
    }
  }
	return grid;
}


Geostat_grid* Csv_mgrid_infilter::readPointsetFormat(std::ifstream& infile, Reduced_grid * grid)
{
  
	int X_col_id = dialog_->X_column_index();
	int Y_col_id = dialog_->Y_column_index();
	int Z_col_id = dialog_->Z_column_index();	
	if( X_col_id == Y_col_id || X_col_id == Z_col_id || Y_col_id == Z_col_id ) {
		GsTLcerr << "The same column number was selected for multiple coordinates \n" << gstlIO::end;
		return 0;
	}
	float x_size = dialog_->x_size();
	float y_size = dialog_->y_size();
	float z_size = dialog_->z_size();
  float rotation_z_angle = dialog_->rotation_z();

  if(x_size< 0. || y_size< 0. || z_size< 0. ) {
    GsTLcerr << "Pixel size must be positive "<< gstlIO::end;
    return NULL;
  }

  bool use_no_data_value = dialog_->use_no_data_value();
  float no_data_value = Grid_continuous_property::no_data_value;
  QString no_data_value_str = QString().arg(no_data_value);
  if( dialog_->use_no_data_value() ) {
    no_data_value = dialog_->no_data_value();
    no_data_value_str = QString::number(no_data_value);
  }

  get_mgrid_xyz_dimensions(infile,grid,X_col_id,Y_col_id,Z_col_id,
                             x_size,y_size,z_size,rotation_z_angle);

  infile.clear();
  infile.seekg(0, ios::beg);
  std::string buffer;
  std::getline(infile, buffer);
  QString qstr(buffer.c_str());
  QStringList property_names = qstr.split(",");


//Read one column at a time
  std::streampos start_data = infile.tellg();
  for(unsigned int j = 0; j< property_names.size(); j++) {
    if(j == X_col_id || j == Y_col_id || j == Z_col_id) continue;
    infile.clear();
    infile.seekg( start_data );
    // Check if property j is categorical
    bool is_categ = false;
    for(unsigned int i=0; i<30 ; i++ ) {
      std::getline(infile, buffer);
      if( buffer.empty() ) break;
      std::vector<std::string> values_str = String_Op::decompose_string(buffer, ",", false);
      is_categ = !String_Op::is_number(values_str[j]);
      if(is_categ) break;
    }
    infile.clear();
    infile.seekg( start_data );

    if(is_categ) {
      Grid_categorical_property* prop = 
        grid->add_categorical_property(property_names[j].toStdString());

            // Set the category definition
    std::string cdef_name = dialog_->name().toStdString()+"-"+property_names[j].toStdString();
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( categoricalDefinition_manager+"/"+cdef_name );
    CategoricalPropertyDefinitionName* cat_def;
    if( ni.raw_ptr() == 0 ) {
      ni = Root::instance()->new_interface( "categoricaldefinition://"+cdef_name,
                                           categoricalDefinition_manager +"/"+cdef_name );
    }

    cat_def = dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
    if(cat_def == 0) continue;
/*
      SmartPtr<Named_interface> ni = 
        Root::instance()->new_interface( categoricalDefinition_manager, 
                                         dialog_->name().toStdString()+"-"+property_names[j].toStdString());
      CategoricalPropertyDefinitionName* cat_def = 
            dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
*/
      std::set<QString> cat_names;
      while( std::getline(infile, buffer) ) {
        QString qstr(buffer.c_str());
        QStringList values_qstr = qstr.split(",");
        cat_names.insert(values_qstr[j]);
        //cat_def->add_category(values_qstr[j].toStdString());
      }

      std::set<QString>::iterator it_cat_names = cat_names.begin();
      for(int code=0; it_cat_names != cat_names.end(); ++it_cat_names ) {
        cat_def->add_category(code,it_cat_names->toStdString() );
      }

      prop->set_category_definition(cat_def->name());
      infile.clear();
      infile.seekg( start_data );

      int node_id=0;
      while( std::getline(infile, buffer) ) {
        QString qstr(buffer.c_str());
        QStringList values_qstr = qstr.split(",");
        if(!values_qstr[j].isEmpty()) {
          GsTLGridNode ijk;
          Geostat_grid::location_type loc;
		      loc[0] = values_qstr[X_col_id].toDouble();
		      loc[1] = values_qstr[Y_col_id].toDouble();
		      loc[2] = values_qstr[Z_col_id].toDouble();
          grid->geometry()->grid_coordinates(ijk,loc);
          int node_id = grid->cursor()->node_id(ijk[0],ijk[1],ijk[2]);
          if( node_id < 0 ) continue;
          QString val = values_qstr[j];
          if( !val.isEmpty() && val != no_data_value_str)
            prop->set_value(values_qstr[j].toStdString(),node_id);
        }
        node_id++;
      }
    }
    else {
      Grid_continuous_property* prop = 
        grid->add_property(property_names[j].toStdString());
      int node_id=0;
      while( std::getline(infile, buffer) ) {
        QString qstr(buffer.c_str());
        QStringList values_qstr = qstr.split(",");
        if(!values_qstr[j].isEmpty()) {
          GsTLGridNode ijk;
          Geostat_grid::location_type loc;
		      loc[0] = values_qstr[X_col_id].toDouble();
		      loc[1] = values_qstr[Y_col_id].toDouble();
		      loc[2] = values_qstr[Z_col_id].toDouble();
          grid->geometry()->grid_coordinates(ijk,loc);
          int node_id = grid->cursor()->node_id(ijk[0],ijk[1],ijk[2]);
          if( node_id < 0 ) continue;
          bool ok;
          float val = values_qstr[j].toFloat(&ok);
          if(ok && val != no_data_value) prop->set_value(val,node_id);
        }
        node_id++;
      }
    }
  }

	return grid;
}

Geostat_grid* Csv_mgrid_infilter::read( std::ifstream& infile ) 
{
//	QByteArray tmp  =dialog_->name().simplified().toLatin1();
	//std::string name( tmp.constData() );
  std::string name = dialog_->name().toStdString();

  // ask manager to get a new grid and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  ni = Root::instance()->new_interface( "reduced_grid://"+name,
                                        gridModels_manager + "/" + name );
  Reduced_grid* grid = dynamic_cast<Reduced_grid*>( ni.raw_ptr() );
  appli_assert( grid != 0 );

  if (dialog_->is_xyz_file())
    return readPointsetFormat(infile, grid);
  else
	  return readRegularGridFormat(infile, grid);	  
}

bool Csv_mgrid_infilter::has_valid_parameters() const{
  bool ok = true;
  ok = ok && dialog_->x_size() > 0.;
  ok = ok && dialog_->y_size() > 0.;
  ok = ok && dialog_->z_size() > 0.;
  ok = ok && !dialog_->name().isEmpty();
  return ok;
}




//==============================================================
Named_interface* Csv_structured_infilter::create_new_interface( std::string& ) {
  return new Csv_structured_infilter();
}

Csv_structured_infilter::Csv_structured_infilter() {
// : dialog_( new Csv_input_pointset_dialog( qApp->activeWindow() ) ) {
}

Csv_structured_infilter::~Csv_structured_infilter() {
//  dialog_ = 0;
}

QWidget* Csv_structured_infilter::init_dialog( std::ifstream& infile ) {
//  dialog_ = new Csv_input_pointset_dialog( infile, qApp->activeWindow() );
//  return dialog_;
	return new QWidget();
}

bool Csv_structured_infilter::has_valid_parameters() const {
//  return !dialog_->name().isEmpty();
	return true;
}

Geostat_grid* Csv_structured_infilter::read( std::ifstream& infile ) {
  return 0;
}


Geostat_grid* Csv_structured_infilter::read_no_gui(ifstream& infile, std::string name) {
	std::string str;
	std::getline(infile, str);
	QString qstr(str.c_str());
	QStringList column_names = qstr.split(",");

	int x_id = column_names.indexOf("xcorners");
	if(x_id == -1 ) return 0;
	int y_id = column_names.indexOf("ycorners");
	if(y_id == -1 ) return 0;
	int z_id = column_names.indexOf("zcorners");
	if(z_id == -1 ) return 0;
// Hack for now
//  QString name = column_names[3];

	infile.seekg(0);

  if(name.empty()) name = "test-structured-grid";

  return this->read(infile, name,x_id,y_id,z_id);


}

Geostat_grid* Csv_structured_infilter::read( std::ifstream& infile, std::string name, 
                int x_id, int y_id, int z_id, float nan  ){

  std::string str;
  std::getline(infile, str);
  QString qstr(str.c_str());
  QStringList property_names = qstr.split(",");

  QString nan_str = QString("%1").arg(nan);

  QStringList coord_names;
  coord_names.append(property_names.at(x_id));
  coord_names.append(property_names.at(y_id));
  coord_names.append(property_names.at(z_id));
  
  std::getline(infile, str);
  QString dim_str(str.c_str());
  QStringList dim_str_list = dim_str.split(",");
  int nx = dim_str_list.at(0).toInt();
  int ny = dim_str_list.at(1).toInt();
  int nz = dim_str_list.at(2).toInt();



  for(int i=0; i<coord_names.size(); ++i  ) {
	  property_names.removeOne(coord_names.at(i));
  }

  std::vector<GsTLPoint> corners;

//  For a csv file no data value is indicated by an empty field e.g. {34,,5.5}
  int grid_size=0;
  while( std::getline(infile, str) ) {
    qstr = str.c_str();
    QStringList fields = qstr.split(",");

    GsTLPoint  pts(fields[x_id].toDouble(),fields[y_id].toDouble(),fields[z_id].toDouble() );
    corners.push_back(pts);

    grid_size++;

  }

//   done reading file
//----------------------------


  appli_message( "read " << grid_size << " points" );

  // We now have a vector containing all the locations and another one with
  // all the property values.
  // Create a pointset, initialize it with the data we collected, and we're done

  // ask manager to get a new pointset and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return 0;
  }

  ni = Root::instance()->new_interface( "Structured_grid://" + name,
				  gridModels_manager + "/" + name );
  Structured_grid* sgrid = dynamic_cast<Structured_grid*>( ni.raw_ptr() );
  appli_assert( sgrid != 0 );

  sgrid->set_dimensions(nx-1,ny-1,nz-1);
  sgrid->set_structured_points(corners);


  return sgrid;


}




/*
 * csv outfilter
 */

Named_interface* Csv_outfilter::create_new_interface( std::string& ) {
	return new Csv_outfilter;
}

Csv_outfilter::Csv_outfilter() {}

Csv_outfilter::~Csv_outfilter() {}

bool Csv_outfilter::write( std::string outfile_name, const Named_interface* ni, std::string* errors ) {
  
  const Geostat_grid* grid = dynamic_cast<const Geostat_grid*>(ni);
  if(grid == 0) return false;

  const Log_data_grid* log_grid = dynamic_cast<const Log_data_grid*>(grid);
  if(log_grid) {
    return write_log_data_grid(outfile_name, log_grid, errors );
  }

  std::ofstream outfile( outfile_name.c_str() );
  if( !outfile ) {
    if( errors )
      errors->append( "can't write to file: " + outfile_name );
    return false;
  }

  typedef std::list<std::string>::const_iterator string_iterator;
  std::list<std::string> & property_names = _list_to_write;
  //std::list<std::string> property_names = grid->property_list();
  std::vector< const Grid_continuous_property* > properties;

  bool output_locations = false;
  int nb_properties = property_names.size();

  //TL modified
  if( dynamic_cast<const Point_set*>( grid ) || dynamic_cast<const Reduced_grid*>(grid)) {
    output_locations = true;
    nb_properties += 3;
  }


  // write property names
  if( output_locations )
    outfile << "X,Y,Z,";
  string_iterator it = property_names.begin();
  if(it != property_names.end()) {
  	outfile << *it;
    const Grid_continuous_property* prop = grid->property( *it );
  	if( prop != 0 )properties.push_back( prop );
  	it++;
  }
  for( ; it != property_names.end();
       ++ it ) {
    outfile <<","<< *it;
    const Grid_continuous_property* prop = grid->property( *it );
  	if( prop != 0 )properties.push_back( prop );
  }
  outfile<<std::endl;

  int grid_size;
  if( properties.empty() )
    grid_size = 0;
  else
    grid_size = properties[0]->size();

  // Write the property values
  for( int i=0; i < grid_size ; i++ ) {
    if( output_locations ) {

	  Geostat_grid::location_type loc = grid->location( i );
		std::stringstream ss_x, ss_y, ss_z;

    ss_x.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_y.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_z.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
		ss_x << loc.x();
    ss_y << loc.y();
    ss_z << loc.z();

    outfile << ss_x.str()<<","<<ss_y.str() << "," << ss_z.str()<< ",";

		}

		for( unsigned int j=0; j < property_names.size(); ++j ) {
			if( properties[j]->is_informed( i ) ) {
				const Grid_categorical_property* cprop =
						dynamic_cast<const Grid_categorical_property*>(properties[j]);
				if(cprop)
					outfile << cprop->get_category_name(i);
				else
					outfile << properties[j]->get_value( i );
			}


			else
				outfile << Grid_continuous_property::no_data_value;
			if( j < property_names.size()-1 ) outfile<<",";
		}

		outfile << std::endl;
  }
  return true;
}



bool Csv_outfilter::write_log_data_grid( std::string outfile_name,const Log_data_grid* grid, std::string* errors  ) {
  
  std::ofstream outfile( outfile_name.c_str() );
  if( !outfile ) {
    if( errors )
      errors->append( "can't write to file: " + outfile_name );
    return false;
  }

  typedef std::list<std::string>::const_iterator string_iterator;
  std::list<std::string> & property_names = _list_to_write;
  std::vector< const Grid_continuous_property* > properties;

  //Be sure to remove the from and to from the property_names list
  std::list<std::string>::iterator it_name = property_names.begin();
  while( it_name != property_names.end()) {
    std::string p_name = *it_name;
    if(p_name == "from" || p_name=="to" || p_name=="dhid" || 
       p_name=="topx" || p_name=="topy" || p_name=="topz" || 
       p_name=="midx" || p_name=="midy" || p_name=="midz" || 
       p_name=="botx" || p_name=="boty" || p_name=="botz" ) {
      std::list<std::string>::iterator it_to_be_deleted = it_name;
      ++it_name;
      property_names.erase(it_to_be_deleted);
    }
    else {
      ++it_name;
    }

  }

  int nb_properties = property_names.size();

  // write property names
  outfile << "dhid,topx,topy,topz,midx,midy,midz,botx,boty,botz,from,to";
  string_iterator it = property_names.begin();
//  if(it != property_names.end()) {
//  	outfile << *it;
//    const Grid_continuous_property* prop = grid->property( *it );
//  	if( prop != 0 )properties.push_back( prop );
//  	it++;
//  }
  for( ; it != property_names.end();
       ++ it ) {
    outfile <<","<< *it;
    const Grid_continuous_property* prop = grid->property( *it );
  	if( prop != 0 )properties.push_back( prop );
  }
  outfile<<std::endl;

  int grid_size;
  if( properties.empty() )
    grid_size = 0;
  else
    grid_size = properties[0]->size();

  // Write the property values
  for( int i=0; i < grid_size ; i++ ) {

	  Geostat_grid::location_type mid_loc = grid->location( i );
    const Log_data::Segment_geometry& seg_geom = grid->get_segment_geometry( i );

		std::stringstream ss_start_x, ss_start_y, ss_start_z;
    std::stringstream ss_mid_x, ss_mid_y, ss_mid_z;
    std::stringstream ss_end_x, ss_end_y, ss_end_z;
    std::stringstream ss_from, ss_to;

    ss_start_x.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_start_y.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_start_z.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_start_x << seg_geom.start.x();
    ss_start_y << seg_geom.start.y();
    ss_start_z << seg_geom.start.z();

    ss_mid_x.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_mid_y.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_mid_z.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
		ss_mid_x << mid_loc.x();
    ss_mid_y << mid_loc.y();
    ss_mid_z << mid_loc.z();

    ss_end_x.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_end_y.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_end_z.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
		ss_end_x << seg_geom.end.x();
    ss_end_y << seg_geom.end.y();
    ss_end_z << seg_geom.end.z();

    ss_from.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
    ss_to.precision(std::numeric_limits<GsTLCoord>::digits10);//override the default
		ss_from << seg_geom.from;
    ss_to << seg_geom.to;

    outfile<<grid->get_log_name_from_nodeid( i )<<",";
    outfile << ss_start_x.str()<<","<<ss_start_y.str() << "," << ss_start_z.str()<< ",";
    outfile << ss_mid_x.str()<<","<<ss_mid_y.str() << "," << ss_mid_z.str()<< ",";
    outfile << ss_end_x.str()<<","<<ss_end_y.str() << "," << ss_end_z.str()<< ",";
    outfile << ss_from.str()<<","<<ss_to.str() << ",";

		for( unsigned int j=0; j < property_names.size(); ++j ) {
			if( properties[j]->is_informed( i ) ) {
				const Grid_categorical_property* cprop =
						dynamic_cast<const Grid_categorical_property*>(properties[j]);
				if(cprop)
					outfile << cprop->get_category_name(i);
				else
					outfile << properties[j]->get_value( i );
			}


			else
				outfile << Grid_continuous_property::no_data_value;
			if( j < property_names.size()-1 ) outfile<<",";
		}

		outfile << std::endl;
  }
  return true;
}
