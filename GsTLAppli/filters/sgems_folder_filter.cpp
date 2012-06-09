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
 * sgems_input_filter.cpp
 *
 *  Created on: Jun 16, 2010
 *      Author: aboucher
 */

#include <GsTLAppli/filters/sgems_folder_filter.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>
#include <GsTLAppli/grid/grid_model/log_data_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/utils/error_messages_handler.h>


Named_interface* Sgems_folder_input_filter::create_new_interface( std::string& ) {
  return new Sgems_folder_input_filter();
}


Sgems_folder_input_filter::Sgems_folder_input_filter() {
	// TODO Auto-generated constructor stub

}

Sgems_folder_input_filter::~Sgems_folder_input_filter() {
	// TODO Auto-generated destructor stub
}

bool Sgems_folder_input_filter::can_handle( const std::string& filename ){
	QDir dir(filename.c_str());

	bool isDir = dir.exists();
	bool isFile = dir.exists(filename.c_str());
	if(!isDir  &&  !isFile) return false;

	QString outFilename;
	if(isDir) {
		outFilename = dir.absolutePath()+"/grid_geometry.xml";
	}
	else {
		outFilename = filename.c_str();
	}

	QFile file( outFilename );
  if( !file.open( QIODevice::ReadOnly ) ) {
  	return false;
  }

	QDomDocument doc("geometry");
	bool ok = doc.setContent(file.readAll());
	file.close();
	if(!ok) return false;
	QDomElement root =  doc.documentElement();
	return root.hasAttribute("type") && root.hasAttribute("name");

}
Named_interface* Sgems_folder_input_filter::read( const std::string& filename,
                            std::string* errors ) {

	QDir dir(filename.c_str());

	bool isDir = dir.exists();
	bool isFile = dir.exists(filename.c_str());
	if(!isDir  &&  !isFile) return false;

	QString outFilename;
	if(isDir) {
		outFilename = dir.absolutePath()+"/grid_geometry.xml";
	}
	else {
		outFilename = filename.c_str();
		dir.cdUp();
	}

	QFile file( outFilename );
  if( !file.open( QIODevice::ReadOnly ) ) {
  	errors->append("Unable to to open the file "+outFilename.toStdString());
  	return 0;
  }

	QDomDocument doc("geometry");
	bool ok = doc.setContent(file.readAll());
	file.close();
	if(!ok) {
		errors->append("Could not load the xml geometry file");
		return 0;
	}

	QDomElement root =  doc.documentElement();
	QString type = root.attribute("type");

	Geostat_grid* grid=0;
	QDomElement elemGeom = root.firstChildElement("Geometry");
	if(type == "Cgrid")
		grid =  read_cartesian_grid(dir,root, errors);
	else if(type == "Masked_grid")
		grid =  read_masked_grid(dir,root, errors);
	else if(type == "Point_set")
		grid = read_point_set(dir,root, errors);
	else if(type == "Log_data_grid")
		grid = read_log_data_grid(dir,root, errors);
	else {
		errors->append("Non matching grid type");
		return 0;
	}


  ok  = read_category_definition(root.firstChildElement("CategoricalDefinitions"), grid, errors);
  if(!ok) {
  	errors->append("Could not read the categorical definition");
  	return grid;
  }
  ok = read_properties(dir,root.firstChildElement("Properties"), grid, errors);
  if(!ok) {
  	errors->append("Could not read the properties");
  	return grid;
  }
  ok  = read_regions(dir, root.firstChildElement("Regions"), grid, errors);
  if(!ok) {
  	errors->append("Could not read the regions");
  	return grid;
  }
  ok  = read_group(root.firstChildElement("PropertyGroups"), grid, errors);
  if(!ok) {
  	errors->append("Could not read the groups");
  	return grid;
  }
  return grid;

}


Geostat_grid*
Sgems_folder_input_filter::read_cartesian_grid(QDir dir,const QDomElement& elem, std::string* errors){
	QString grid_name = elem.attribute("name");

	QDomElement elemGeom = elem.firstChildElement("Geometry");

  std::string final_grid_name;
  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( "cgrid://"+grid_name.toStdString(),
    		gridModels_manager + "/" + grid_name.toStdString());
  Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  if(grid == 0) return 0;

  int nx = elemGeom.attribute("nx").toInt();
  int ny = elemGeom.attribute("ny").toInt();
  int nz = elemGeom.attribute("nz").toInt();

  double xsize = elemGeom.attribute("dx").toDouble();
  double ysize = elemGeom.attribute("dy").toDouble();
  double zsize = elemGeom.attribute("dz").toDouble();

  double ox = elemGeom.attribute("ox").toDouble();
  double oy = elemGeom.attribute("oy").toDouble();
  double oz = elemGeom.attribute("oz").toDouble();
  float rotation_z_angle = elemGeom.attribute("rotation_z_angle").toFloat();

  grid->set_dimensions( nx, ny, nz, xsize, ysize, zsize );
  grid->origin( Cartesian_grid::location_type( ox,oy,oz ) );
  grid->set_rotation_z(rotation_z_angle);

  return grid;
}

Geostat_grid*
Sgems_folder_input_filter::read_masked_grid(QDir dir,const QDomElement& elem, std::string* errors){

	QString grid_name = elem.attribute("name");

	QDomElement elemGeom = elem.firstChildElement("Geometry");
	QString grid_size = elemGeom.attribute("nActiveCells");


	bool ok_size;
	grid_size.toInt(&ok_size);
	if(!ok_size) {
		errors->append("Failed to get the size of the active grid");
		return 0;
	}

  int nx = elemGeom.attribute("nx").toInt();
  int ny = elemGeom.attribute("ny").toInt();
  int nz = elemGeom.attribute("nz").toInt();

  double xsize = elemGeom.attribute("dx").toDouble();
  double ysize = elemGeom.attribute("dy").toDouble();
  double zsize = elemGeom.attribute("dz").toDouble();

  double ox = elemGeom.attribute("ox").toDouble();
  double oy = elemGeom.attribute("oy").toDouble();
  double oz = elemGeom.attribute("oz").toDouble();

  float rotation_z_angle = elemGeom.attribute("rotation_z_angle").toFloat();


// Read the mask
 // std::string maskfile = dir.absolutePath().toStdString()+"/gridmask.sgems";
 // std::fstream stream(maskfile.c_str());

  QFile file( dir.absolutePath()+"/gridmask.sgems" );
  if( !file.open( QIODevice::ReadOnly ) ) {
  	errors->append("Could not open the mask gridmask.sgems ");
  	return 0;
  }
	QDataStream stream( &file );
#if QT_VERSION >= 0x040600
	stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
#endif

	std::vector<bool> mask;
	mask.reserve(nx*ny*nz);
	int mask_size = 0;
	for( GsTLInt k = 0; k < nx*ny*nz ; k++ ) {
		bool val;
		stream >> val;
		mask.push_back( val );
		if(val) mask_size++;
	}

  std::string final_grid_name;
	SmartPtr<Named_interface> ni =
		Root::instance()->new_interface( "reduced_grid://" + grid_name.toStdString()+"::"+grid_size.toStdString(),
		gridModels_manager + "/" + grid_name.toStdString());
	Reduced_grid* grid = dynamic_cast<Reduced_grid*>( ni.raw_ptr() );
	if(grid==0) return 0;

	if (!grid) {
		errors->append("could not create the grid");
		return 0;
	}

  grid->set_dimensions( nx, ny, nz, xsize, ysize, zsize, mask, rotation_z_angle );
	grid->origin( Cartesian_grid::location_type( ox,oy,oz ) );

	return grid;

}

Geostat_grid*
Sgems_folder_input_filter::read_point_set(QDir dir,const QDomElement& elem, std::string* errors){
	std::string grid_name = elem.attribute("name").toStdString();

	QDomElement elemGeom = elem.firstChildElement("Geometry");
	QString grid_size_str = elemGeom.attribute("size");

	bool ok_size;
	int size = grid_size_str.toInt(&ok_size);
	if(!ok_size) {
		errors->append("Failed to get the size of the grid");
		return 0;
	}


  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( "point_set://" + grid_name+"::"+grid_size_str.toStdString(),
                                     gridModels_manager + "/" + grid_name);
  Point_set* grid = dynamic_cast<Point_set*>( ni.raw_ptr() );
  if(grid==0) return 0;

  QFile file( dir.absolutePath()+"/coordinates.sgems" );
  if( !file.open( QIODevice::ReadOnly ) ) {
  	errors->append("Could not open file coordinates.sgems");
  	return grid;
  }

	QDataStream stream( &file );
#if QT_VERSION >= 0x040600
	stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
#endif
 /*
  // read the coordinates of the points
  std::string coordsfile = dir.absolutePath().toStdString()+"/coordinates.sgems";
  std::fstream stream(coordsfile.c_str(),std::ios::in | std::ios::binary );
  if(stream.bad()) {
  	errors->append("Could not open file "+coordsfile);
  	return grid;
  }
*/
  std::vector<Point_set::location_type > coords;
  for( unsigned int k = 0; k < size; k ++ ) {
  	GsTLCoord x,y,z;
    stream >> x >> y >> z;
    coords.push_back( Point_set::location_type( x,y,z) );
  }
  grid->point_locations( coords );

  return grid;
}


Geostat_grid* Sgems_folder_input_filter::read_log_data_grid(QDir dir,const QDomElement& elem, std::string* errors){
	std::string grid_name = elem.attribute("name").toStdString();

	QDomElement elemGeom = elem.firstChildElement("Geometry");
	QString grid_size_str = elemGeom.attribute("size");

	bool ok_size;
	int size = grid_size_str.toInt(&ok_size);
	if(!ok_size) {
		errors->append("Failed to get the size of the grid");
		return 0;
	}


  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( "log_grid://" + grid_name+"::"+grid_size_str.toStdString(),
                                     gridModels_manager + "/" + grid_name);
  Log_data_grid* grid = dynamic_cast<Log_data_grid*>( ni.raw_ptr() );
  if(grid==0) return 0;

  QFile file( dir.absolutePath()+"/coordinates.sgems" );
  if( !file.open( QIODevice::ReadOnly ) ) {
  	errors->append("Could not open file coordinates.sgems");
  	return grid;
  }

	QDataStream stream( &file );
#if QT_VERSION >= 0x040600
	stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
#endif

  std::vector<Point_set::location_type > coords;
  for( unsigned int k = 0; k < size; k ++ ) {
  	GsTLCoord x,y,z;
    stream >> x >> y >> z;
    coords.push_back( Point_set::location_type( x,y,z) );
  }
  grid->point_locations( coords );

  //Read the geometry of each log
  typedef std::vector< std::pair< int, std::pair<GsTLPoint ,GsTLPoint > > > segment_geometry_vectorT;
  segment_geometry_vectorT  log_segments;
  std::map<std::string, segment_geometry_vectorT>  dh_log_segments;

  QDomElement elemLog = elemGeom.firstChildElement("Log_Geometry");
  for( ; !elemLog.isNull(); elemLog = elemLog.nextSiblingElement("Log_Geometry") ){
  	segment_geometry_vectorT vector;
  	std::string log_name = elemLog.attribute("name").toStdString();
  	dh_log_segments[log_name] = vector;
  	std::map<std::string, segment_geometry_vectorT>::iterator it = dh_log_segments.find(log_name);
  	QDomElement elemSegment = elemLog.firstChildElement("Segment");
  	for( ; !elemSegment.isNull(); elemSegment = elemSegment.nextSiblingElement("Segment") ){
  		int nodeid = elemSegment.attribute("nodeid").toInt();
  		QStringList from = elemSegment.attribute("from").split(",");
  		QStringList to = elemSegment.attribute("to").split(",");
  		GsTLPoint start_loc(from.at(0).toDouble(),from.at(1).toDouble(),from.at(2).toDouble());
  		GsTLPoint end_loc(to.at(0).toDouble(),to.at(1).toDouble(),to.at(2).toDouble());
  		it->second.push_back(std::make_pair( nodeid, std::make_pair(start_loc,end_loc)));
  	}

  }
  grid->set_log_geometry(dh_log_segments);


  return grid;
}


bool Sgems_folder_input_filter::read_properties(QDir dir,const QDomElement& root, Geostat_grid* grid, std::string* errors){

  long int filesize = grid->size()*sizeof( float );

	QDomElement elem = root.firstChildElement("GsTLGridProperty");
	for(; !elem.isNull(); elem = elem.nextSiblingElement("GsTLGridProperty") ) {
		std::string prop_name = elem.attribute("name").toStdString();
	//	bool isCategorical = elem.attribute("type") == "Categorical";
    GsTLGridProperty* prop;
    QString filepath =  dir.absoluteFilePath(elem.attribute("filepath"));
    if(elem.attribute("type") == "Categorical") {
    	std::string  cdef_name = elem.attribute("categoryDefinition").toStdString();
    	GsTLGridCategoricalProperty* cprop = grid->add_categorical_property_from_disk(prop_name, filepath.toStdString().c_str(), cdef_name );
    	prop = dynamic_cast<GsTLGridProperty*>(cprop);
    }
    else if(elem.attribute("type") == "Continuous") {
    	 prop = grid->add_property_from_disk( prop_name, filepath.toStdString().c_str() );
    }
    else if(elem.attribute("type") == "Weights") {
      prop = grid->add_weight_property_from_disk( prop_name, filepath.toStdString().c_str() );
    }

    if(elem.hasAttribute("parameters")) {
      std::string  params = elem.attribute("parameters").toStdString();
      prop->set_parameters(params);
    }

  //  std::fstream stream(elem.attribute("filepath").toStdString().c_str());
 //   stream.read( (char*) prop->data(), filesize );

	}
	return true;

}

bool Sgems_folder_input_filter::read_regions(QDir dir,const QDomElement& root, Geostat_grid* grid, std::string* errors){
	QDomElement elem = root.firstChildElement("GsTLGridRegion");

	for(; !elem.isNull(); elem = elem.nextSiblingElement("GsTLGridRegion") ) {
		std::string region_name = elem.attribute("name").toStdString();
		GsTLGridRegion*	region = grid->add_region( region_name );
//		std::fstream stream;
		QString filepath =  dir.absoluteFilePath(elem.attribute("filepath"));

	  QFile file(filepath );
	  if( !file.open( QIODevice::ReadOnly ) ) {
			errors->append("Could not open file for region "+region_name);
			return false;
	  }
		QDataStream stream( &file );
		#if QT_VERSION >= 0x040600
			stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
		#endif
    for( GsTLInt k = 0; k < grid->size() ; k++ ) {
       bool val;
       stream >> val;
       region->set_region_value( val, k );
     }
    file.close();
	}
  return true;

}


bool Sgems_folder_input_filter::read_group(const QDomElement& root, Geostat_grid* grid, std::string* errors){

	QDomElement elemGroup = root.firstChildElement("GsTLGridPropertyGroup");

	for(; !elemGroup.isNull(); elemGroup = elemGroup.nextSiblingElement("GsTLGridPropertyGroup") ) {
		std::string name = elemGroup.attribute("name").toStdString();
		std::string type = elemGroup.attribute("type").toStdString();
		std::string info = elemGroup.attribute("info").toStdString();
		QString members = elemGroup.attribute("memberProperties");
		QStringList pname_in_group =  members.split(";");
		GsTLGridPropertyGroup *group = grid->add_group(name,type);
		group->set_group_info(info);
		for(int i=0; i< pname_in_group.size(); ++i) {
			group->add_property(grid->property(pname_in_group[i].toStdString()));
		}

	}
	return true;

}

bool Sgems_folder_input_filter::read_category_definition(const QDomElement& root, Geostat_grid* grid, std::string* errors){
	QDomElement elemDef = root.firstChildElement("CategoricalDefinition");

	for(; !elemDef.isNull(); elemDef = elemDef.nextSiblingElement("CategoricalDefinition")) {
		QString name = elemDef.attribute("name");
		QStringList cat_names = elemDef.attribute("categoryNames").split(";");

	  SmartPtr<Named_interface> ni =
	    Root::instance()->interface( categoricalDefinition_manager+"/"+name.toStdString()  );
	  CategoricalPropertyDefinitionName* cat_definition =
	    dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

	  if(cat_definition == 0) {
	  	bool ok = create_categorial_definition( name,  cat_names );
    }
    else {
	    //If the definition already exist check if it the same
	    bool is_conflict = check_for_conflict(cat_definition, cat_names);
	    // Nothing to be done, already loaded

	    while( is_conflict ) {
	  	  name.append("_0");
		    ni = Root::instance()->interface( categoricalDefinition_manager+"/"+name.toStdString()  );
		    cat_definition = dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
		    if(cat_definition == 0)
		  	  return create_categorial_definition( name,  cat_names );
		    is_conflict = check_for_conflict(cat_definition, cat_names );
	    }
    }

	}
	return true;

}

bool Sgems_folder_input_filter::check_for_conflict(CategoricalPropertyDefinitionName* def,
																										const QStringList& cat_names) {
	if( def->number_of_category() != cat_names.size() ) return true;
	for( int i =0; i<cat_names.size(); i++  ) {
		if( cat_names[i].toStdString() != def->get_category_name(i) ) return true;
	}
	return false;
}

bool Sgems_folder_input_filter::create_categorial_definition(
	 QString& name,
	 QStringList& cat_names) {

  cat_names.prepend(name);
  std::string parameters = cat_names.join( Actions::separator.c_str() ).toStdString();

  // call the CopyProperty action
  Error_messages_handler error_messages;
  std::string command( "NewCategoricalDefinition" );

  SmartPtr<Named_interface> ni =
    Root::instance()->interface( projects_manager + "/" + "project" );
  GsTL_project* project = dynamic_cast<GsTL_project*>( ni.raw_ptr() );
  return project->execute( command, parameters, &error_messages );
}


/* -------------------------------------------------
 * Output folder
 *  -------------------------------------------------
 */

 Named_interface* Sgems_folder_output_filter::create_new_interface( std::string& ) {
   return new Sgems_folder_output_filter();
 }

Sgems_folder_output_filter::Sgems_folder_output_filter() {
	// TODO Auto-generated constructor stub

}

Sgems_folder_output_filter::~Sgems_folder_output_filter() {
	// TODO Auto-generated destructor stub
}



bool Sgems_folder_output_filter::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);
 
    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }
 
            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
 
    return result;
}


bool Sgems_folder_output_filter::write( std::string outfile,
		const Named_interface* ni,
        std::string* errors ) {

  const Geostat_grid* grid = dynamic_cast<const Geostat_grid*>(ni);
  if(grid == 0) return false;

	QDomDocument doc("gridGeometry");
	QDomElement root = doc.createElement("Grid");
	root.setAttribute("type",grid->classname().c_str());
	root.setAttribute("name",get_grid_name(grid).c_str());
	doc.appendChild(root);

  QString dirname = outfile.c_str();
  if( !dirname.endsWith( ".grid" ) && !dirname.endsWith( ".grid/" ) )
	dirname.append( ".grid" );

  QDir dir(dirname);
  if (dir.exists()) {
//	  bool ok = dir.rmdir(dirname);
    bool ok = this->removeDir(dirname);
		if( !ok ) {
		  errors->append( "can't overwrite directory: " + dirname.toStdString() );
		  return false;
		}

  }

  dir.mkdir(dirname);

  bool ok_geometry;
  QDomElement elemGeom;

  if ( grid->classname() == "Masked_grid")
  	elemGeom =  write_masked_grid_geometry(dir, doc,grid);
  else if( grid->classname() == "Point_set" )
  	elemGeom =  write_pointset_geometry( dir, doc, grid );
  else if( grid->classname() == "Cgrid" )
  	elemGeom =  write_cartesian_grid_geometry( dir, doc,  grid );
  else if( grid->classname() == "Log_data_grid" )
  	elemGeom =  write_log_data_grid_geometry( dir, doc,  grid );
  else {
	  errors->append( "The grid type "+grid->classname()+" cannot be saved with this filter" );
	  return false;
  }


  if(elemGeom.isNull()){
	  errors->append( "can't create the geometry header: " + dirname.toStdString() );
	  return false;
	}

  root.appendChild(elemGeom);

	 QDomElement elemProps = write_properties(dirname,doc,grid);
	 root.appendChild(elemProps);

	 QDomElement elemRegions = write_regions(dirname,doc,grid);
	 root.appendChild(elemRegions);

	 QDomElement elemGroup = write_group(doc,grid);
	 root.appendChild(elemGroup);

	 QDomElement elemCatDef = write_category_definition(doc,grid);
	 root.appendChild(elemCatDef);

  //QFile file( dir.absolutePath()+"/grid_geometry.xml" );
  QFile file( dir.absoluteFilePath("grid_geometry.xml" ) );
  if( !file.open( QIODevice::WriteOnly ) ) {
	if( errors )
	  errors->append( "can't write to file: " + outfile );
	return false;
  }

  QTextStream stream( &file );
  stream << doc.toString();

  file.close();

  return true;

}

QDomElement Sgems_folder_output_filter::write_masked_grid_geometry(QDir dir, QDomDocument& dom, const Geostat_grid* grid){
	const Reduced_grid *mgrid =  dynamic_cast<const Reduced_grid*>( grid );

	QDomElement elem = write_cartesian_grid_geometry(dir, dom, grid);
	elem.setAttribute("nActiveCells",mgrid->size());

  QFile file( dir.absoluteFilePath("gridmask.sgems" ) );
  if( !file.open( QIODevice::WriteOnly ) ) {
  	elem.clear();
  	return elem;
  }
	QDataStream stream( &file );
	#if QT_VERSION >= 0x040600
		stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	#endif

	const std::vector<bool> mask = mgrid->mask();
	std::vector<bool>::const_iterator it_mask = mask.begin();
	for(; it_mask != mask.end(); ++it_mask) stream<< *it_mask;
	file.close();
	return elem;

}
QDomElement Sgems_folder_output_filter::write_pointset_geometry( QDir dir, QDomDocument& doc,const Geostat_grid* grid ){
	const Point_set* pset = dynamic_cast<const Point_set*>(grid);
	QDomElement elemGeom = doc.createElement("Geometry");

  QFile file( dir.absoluteFilePath("coordinates.sgems" ) );
  if( !file.open( QIODevice::WriteOnly ) ) {
  	elemGeom.clear();
  	return elemGeom;
  }

	QDataStream stream( &file );
	#if QT_VERSION >= 0x040600
		stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	#endif


	elemGeom.setAttribute("size",pset->size());
//	elemGeom.setAttribute("coordinates",pset->size());

	// write the x,y,z coordinates of each point
	const std::vector<Point_set::location_type>& locs = pset->point_locations();
	std::vector<Point_set::location_type>::const_iterator vec_it = locs.begin();
	for( ; vec_it != locs.end(); ++vec_it ) {
		stream << vec_it->x() << vec_it->y() << vec_it->z();
	}
	file.close();

	return elemGeom;
}


QDomElement Sgems_folder_output_filter::write_cartesian_grid_geometry( QDir dir, QDomDocument& doc, const Geostat_grid* grid ){

	const Cartesian_grid* cgrid = dynamic_cast<const Cartesian_grid*>(grid);

	 QDomElement elemGeom = doc.createElement("Geometry");
	 elemGeom.setAttribute("ox",cgrid->origin().x());
	 elemGeom.setAttribute("oy",cgrid->origin().y());
	 elemGeom.setAttribute("oz",cgrid->origin().z());
	 elemGeom.setAttribute("nx",cgrid->nx());
	 elemGeom.setAttribute("ny",cgrid->ny());
	 elemGeom.setAttribute("nz",cgrid->nz());
	 elemGeom.setAttribute("dx",cgrid->cell_dimensions().x());
	 elemGeom.setAttribute("dy",cgrid->cell_dimensions().y());
	 elemGeom.setAttribute("dz",cgrid->cell_dimensions().z());
   elemGeom.setAttribute("rotation_z_angle",cgrid->rotation_z());

	 return elemGeom;

}

QDomElement Sgems_folder_output_filter::write_log_data_geometry( QDomDocument& doc, const Log_data* ldata ){
	QDomElement elemGeom = doc.createElement("Log_Geometry");
	elemGeom.setAttribute("name",ldata->name().c_str());
	elemGeom.setAttribute("size",ldata->number_of_segments());

	Log_data::nodeid_to_log_coords::const_iterator it = ldata->position_begin();


	for( ; it != ldata->position_end(); ++it ) {
		QDomElement elemSegment = doc.createElement("Segment");
		elemSegment.setAttribute("nodeid", QString::number(it->first));
		QString from = QString("%1,%2,%3").arg(it->second.first.x()).arg(it->second.first.y()).arg(it->second.first.z());
		QString to = QString("%1,%2,%3").arg(it->second.second.x()).arg(it->second.second.y()).arg(it->second.second.z());
		elemSegment.setAttribute("from", from);
		elemSegment.setAttribute("to", to);
		elemGeom.appendChild(elemSegment);
	}

	return elemGeom;

}


QDomElement Sgems_folder_output_filter::write_log_data_grid_geometry( QDir dir, QDomDocument& doc, const Geostat_grid* grid ){
	const Log_data_grid* lgrid = dynamic_cast<const Log_data_grid*>(grid);

	QDomElement elemGeom = doc.createElement("Geometry");
	  QFile file( dir.absoluteFilePath("coordinates.sgems" ) );
	  if( !file.open( QIODevice::WriteOnly ) ) {
	  	elemGeom.clear();
	  	return elemGeom;
	  }

	QDataStream stream( &file );
	#if QT_VERSION >= 0x040600
		stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	#endif

	// write the x,y,z coordinates of each point
	const std::vector<Point_set::location_type>& locs = lgrid->point_locations();
	std::vector<Point_set::location_type>::const_iterator vec_it = locs.begin();
	for( ; vec_it != locs.end(); ++vec_it ) {
		stream << vec_it->x() << vec_it->y() << vec_it->z();
	}
	file.close();



	 elemGeom.setAttribute("size",lgrid->size());
	 elemGeom.setAttribute("number_of_logs",lgrid->number_of_logs());

	 for(int i=0; i< lgrid->number_of_logs(); ++i) {
		 Log_data ldata = lgrid->get_log_data(i);
		 QDomElement elem_log = this->write_log_data_geometry(doc,&ldata);
		 elemGeom.appendChild(elem_log);

	 }

	 return elemGeom;

}

QDomElement
Sgems_folder_output_filter::write_properties(QDir dir, QDomDocument& doc, const Geostat_grid* grid)
{
	QDomElement elemProps = doc.createElement("Properties");
	std::list<std::string> plist = grid->property_list();
	std::list<std::string>::iterator it = plist.begin();
	QString prop_path = dir.absoluteFilePath("properties");
	if( !dir.exists(prop_path)) {
		bool ok = dir.mkdir(prop_path );
		if(ok == false) {
			GsTLcerr << "Can't create sub-directory. Check that the directory is writable\n"
							 << "and that there is enough disk space left" << gstlIO::end;
			return elemProps;
		}
	}


	//Write each property
	for(; it!=plist.end(); ++it) {
		QDomElement elemProp = doc.createElement("GsTLGridProperty");
		const GsTLGridProperty* prop = grid->property(*it);
		elemProp.setAttribute("name",prop->name().c_str());
		const GsTLGridCategoricalProperty* cprop = dynamic_cast<const GsTLGridCategoricalProperty*>(prop);
		bool isCategorical = cprop != 0;
    elemProp.setAttribute("type",prop->item_type());

    if(prop->item_type() == "CategoricalProperty") {
      elemProp.setAttribute("type","Categorical");
    }
    else if(prop->item_type() == "ContinuousProperty") {
    	 elemProp.setAttribute("type","Continuous");
    }
    else if(prop->item_type() == "WeightsProperty") {
      elemProp.setAttribute("type","Weights");
    }
		//elemProp.setAttribute("type",isCategorical?"Categorical":"Continuous");
		if(isCategorical) {
			elemProp.setAttribute("categoryDefinition",cprop->get_category_definition()->name().c_str());
		}

    std::string params = prop->parameters();
    if(!params.empty()) {
      elemProp.setAttribute("parameters",params.c_str());
    }

		std::string prop_filename = "properties/property__"+prop->name()+".sgems";
	//	QString qfilepath = dir.relativeFilePath(""+filename.c_str());
//		std::string prop_filename = qfilepath.toStdString();
		elemProp.setAttribute("filepath",prop_filename.c_str());
		elemProps.appendChild(elemProp);
		prop_filename = dir.absoluteFilePath(prop_filename.c_str()).toStdString();

		// We use the same format than the DiskAccessor, so that we may be able to
		// construct the properties without having to load them in memory
		// Careful potential incompatibility between 32 and 64 bits machine

		// if the file already exists, erase its content by opening it in write mode
		// (I don't know any other easy way to do that...)
		std::ofstream eraser( prop_filename.c_str() );
		eraser.close();

		std::fstream prop_stream;
		// Open a stream to the cache file in read/write mode
		prop_stream.open( prop_filename.c_str(),
						std::ios::in | std::ios::out | std::ios::binary );
		if( !prop_stream ) {
			GsTLcerr << "Can't write file. Check that the directory is writable\n"
							 << "and that there is enough disk space left" << gstlIO::end;
		}
		if(prop->is_in_memory()) {
			long int remaining = static_cast<long int>( prop->size() ) *
													 static_cast<long int>( sizeof(float) );
			prop_stream.write( (char*) prop->data(), remaining );
		}
		else { // The file is already on disk.  Should find a way to simply copy the stream from the os

			for( int i=0; i < prop->size() ; i++  ) {
				float z;
				if(prop->is_informed(i))  z = prop->get_value(i);
				else z = GsTLGridProperty::no_data_value;
				prop_stream.write( (char*) (&z),  sizeof(float) );
				//prop_stream<<(char*)(&z);
			}

//			dstStream << srcStream.rdbuf();
		}
		prop_stream.close();
	}
	return elemProps;
}

QDomElement
Sgems_folder_output_filter::write_regions(QDir dir, QDomDocument& doc,const Geostat_grid* grid)
{
	QDomElement elemRegions = doc.createElement("Regions");
	std::list<std::string> rlist = grid->region_list();
	std::list<std::string>::iterator it = rlist.begin();
	QString region_path =  dir.absoluteFilePath("regions");
	if( !dir.exists( region_path )) {
		bool ok = dir.mkdir( region_path );
		if(ok == false) {
			GsTLcerr << "Can't create sub-directory. Check that the directory is writable\n"
							 << "and that there is enough disk space left" << gstlIO::end;
			return elemRegions;
		}
	}

	//Write each region
	for(; it!=rlist.end(); ++it) {
		QDomElement elemRegion = doc.createElement("GsTLGridRegion");
		const GsTLGridRegion* region = grid->region(*it);
		elemRegion.setAttribute("name",region->name().c_str());


		QString region_filename = "regions/region__";
		region_filename.append(region->name().c_str());
		region_filename.append(".sgems");
		elemRegion.setAttribute("filepath",region_filename);
		elemRegions.appendChild(elemRegion);

		region_filename = dir.absoluteFilePath(region_filename);
	  QFile file(region_filename );
	  if( !file.open( QIODevice::WriteOnly ) ) {
	  	elemRegion.clear();
	  	continue;
	  }
		QDataStream stream( &file );
		#if QT_VERSION >= 0x040600
			stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
		#endif

    for(GsTLGridRegion::const_iterator it=region->begin(); it!=region->end(); ++it)  {
    	bool flag = *it;
    	stream << *it;
		}
    file.close();
	}

	return elemRegions;
}

QDomElement
Sgems_folder_output_filter::write_group(QDomDocument& doc, const Geostat_grid* grid){
	QDomElement elemGroups = doc.createElement("PropertyGroups");
	std::list<std::string> glist = grid->get_group_names();
	std::list<std::string>::iterator it = glist.begin();

	//Write each region
	for(; it!=glist.end(); ++it) {
		QDomElement elemGroup = doc.createElement("GsTLGridPropertyGroup");
		const GsTLGridPropertyGroup* group = grid->get_group(*it);
		elemGroup.setAttribute("name",group->name().c_str());
		elemGroup.setAttribute("type",group->type().c_str());

		std::string info = group->get_group_info();
		if(!info.empty()) {
			elemGroup.setAttribute("groupInfo",info.c_str());
		}

		std::vector<std::string> pnames = group->property_names();
		if(pnames.empty() )continue;
		std::string pname_in_group = pnames[0];
		for(unsigned int i=1; i<pnames.size(); ++i ) {
			pname_in_group.append(";"+pnames[i]);
		}
		elemGroup.setAttribute("memberProperties",pname_in_group.c_str());
		elemGroups.appendChild(elemGroup);

	}
	return elemGroups;
}

QDomElement Sgems_folder_output_filter::write_category_definition(QDomDocument& dom, const Geostat_grid* grid){

	QDomElement elemCats = dom.createElement("CategoricalDefinitions");

	std::list<std::string> cat_props = grid->categorical_property_list();
	if(cat_props.empty()) {
		elemCats.clear();
		return elemCats;
	}
	// Get the categorical definition used by the grid
	std::list<std::string> cat_defs;
	std::list<std::string>::const_iterator it = cat_props.begin();
	for( ; it != cat_props.end(); ++it) {
		const GsTLGridCategoricalProperty* cprop = grid->categorical_property(*it);
		cat_defs.push_back( cprop->get_category_definition()->name() );
	}
	cat_defs.sort();
	std::list<std::string>::iterator unique_end = std::unique(cat_defs.begin(), cat_defs.end());

	for(std::list<std::string>::iterator it = cat_defs.begin(); it !=unique_end; ++it ) {
	  SmartPtr<Named_interface> ni =
	    Root::instance()->interface( categoricalDefinition_manager+"/"+*it  );
	  appli_assert( ni );

	  CategoricalPropertyDefinition* cat_definition =
	    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
	  if(cat_definition == 0) continue;

	  QDomElement elemCat = dom.createElement("CategoricalDefinition");
	  QString cat_def_name = cat_definition->name().c_str();
	  elemCat.setAttribute("name",cat_def_name );


	  if(cat_def_name == "Default") continue;

	  CategoricalPropertyDefinitionName* cat_definition_name =
	    dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
	  if(cat_definition == 0) continue;

	  QStringList cat_names;
    std::vector<std::string> names = cat_definition_name->category_names();
	  std::vector<std::string>::const_iterator  it_name = names .begin();
	  for( ; it_name != names.end(); it_name++ ) {
	  	cat_names.append(it_name->c_str());
	  }

	  elemCat.setAttribute("categoryNames",cat_names.join(";"));
	  elemCats.appendChild(elemCat);

	}
	return elemCats;

}


std::string Sgems_folder_output_filter::get_grid_name(const Geostat_grid* grid){
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager );
  Manager* grid_manager = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( grid_manager );
  return grid_manager->name( (Named_interface*) grid ).c_str();
}
