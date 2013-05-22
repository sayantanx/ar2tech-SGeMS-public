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

#include <filters/sgems_folder_grid_geometry.h>
#include <grid/grid_model/geostat_grid.h>
#include <grid/grid_model/cartesian_grid.h>
#include <grid/grid_model/point_set.h>
#include <grid/grid_model/log_data_grid.h>
#include <grid/grid_model/reduced_grid.h>
#include <grid/grid_model/structured_grid.h>
#include <appli/manager_repository.h>
#include <utils/manager.h>


Named_interface* Cartesian_grid_geometry_xml_io::create_new_interface(std::string& str){
  return new Cartesian_grid_geometry_xml_io;
}

QDomElement Cartesian_grid_geometry_xml_io::
  write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const{

	const Cartesian_grid* cgrid = dynamic_cast<const Cartesian_grid*>(grid);

	 QDomElement elemGeom = dom.createElement("Geometry");
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


Geostat_grid* Cartesian_grid_geometry_xml_io::
  read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const {

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

// --------------------------------------------------------

Named_interface* Pointset_geometry_xml_io::create_new_interface(std::string& str){
  return new Pointset_geometry_xml_io;
}

QDomElement Pointset_geometry_xml_io::
  write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const{


	const Point_set* pset = dynamic_cast<const Point_set*>(grid);
	QDomElement elemGeom = dom.createElement("Geometry");

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


Geostat_grid* Pointset_geometry_xml_io::
  read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const {

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

// --------------------------------------------------------

Named_interface* Masked_grid_geometry_xml_io::create_new_interface(std::string& str){
  return new Masked_grid_geometry_xml_io;
}

QDomElement Masked_grid_geometry_xml_io::
  write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const{

	const Reduced_grid *mgrid =  dynamic_cast<const Reduced_grid*>( grid );

  Cartesian_grid_geometry_xml_io cgrid_io;

  QDomElement elem = cgrid_io.write_grid_geometry(dir, dom, grid);
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


Geostat_grid* Masked_grid_geometry_xml_io::
  read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const {


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

// --------------------------------------------------------

Named_interface* Log_data_grid_geometry_xml_io::create_new_interface(std::string& str){
  return new Log_data_grid_geometry_xml_io;
}

QDomElement Log_data_grid_geometry_xml_io::
  write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const{

	const Log_data_grid* lgrid = dynamic_cast<const Log_data_grid*>(grid);

	QDomElement elemGeom = dom.createElement("Geometry");
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
		 QDomElement elem_log = this->write_log_data_geometry(dom,&ldata);
		 elemGeom.appendChild(elem_log);

	 }

	 return elemGeom;

}


QDomElement Log_data_grid_geometry_xml_io::write_log_data_geometry( QDomDocument& doc, const Log_data* ldata ) const{
	QDomElement elemGeom = doc.createElement("Log_Geometry");
	elemGeom.setAttribute("name",ldata->name().c_str());
	elemGeom.setAttribute("size",ldata->number_of_segments());

  std::map<int, Log_data::Segment_geometry>::const_iterator it = ldata->segment_begin();


	for( ; it != ldata->segment_end(); ++it ) {
		QDomElement elemSegment = doc.createElement("Segment");
		elemSegment.setAttribute("nodeid", QString::number(it->first));
    elemSegment.setAttribute("from", QString::number(it->second.from));
    elemSegment.setAttribute("to", QString::number(it->second.to));
		QString from = QString("%1,%2,%3").arg(it->second.start.x()).arg(it->second.start.y()).arg(it->second.start.z());
		QString to = QString("%1,%2,%3").arg(it->second.end.x()).arg(it->second.end.y()).arg(it->second.end.z());
		elemSegment.setAttribute("start", from);
		elemSegment.setAttribute("end", to);
		elemGeom.appendChild(elemSegment);
	}

	return elemGeom;

}


Geostat_grid* Log_data_grid_geometry_xml_io::
  read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const {

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
//  typedef std::vector< std::pair< int, std::pair<GsTLPoint ,GsTLPoint > > > segment_geometry_vectorT;
//  segment_geometry_vectorT  log_segments;
//  std::map<std::string, segment_geometry_vectorT>  dh_log_segments;
  std::vector<Log_data::Segment_geometry> segment_geometries;
  std::map<std::string,std::vector<Log_data::Segment_geometry> > grid_geometries;

  QDomElement elemLog = elemGeom.firstChildElement("Log_Geometry");
  for( ; !elemLog.isNull(); elemLog = elemLog.nextSiblingElement("Log_Geometry") ){
  	std::vector<Log_data::Segment_geometry> segment_vector;
  	std::string log_name = elemLog.attribute("name").toStdString();
  	grid_geometries[log_name] = segment_vector;
    std::map<std::string, std::vector<Log_data::Segment_geometry> >::iterator it = grid_geometries.find(log_name);
  	QDomElement elemSegment = elemLog.firstChildElement("Segment");
  	for( ; !elemSegment.isNull(); elemSegment = elemSegment.nextSiblingElement("Segment") ){
  		int nodeid = elemSegment.attribute("nodeid").toInt();
  		QStringList start_str = elemSegment.attribute("start").split(",");
  		QStringList end_str = elemSegment.attribute("end").split(",");
  		GsTLPoint start_loc(start_str.at(0).toDouble(),start_str.at(1).toDouble(),start_str.at(2).toDouble());
  		GsTLPoint end_loc(end_str.at(0).toDouble(),end_str.at(1).toDouble(),end_str.at(2).toDouble());
      double from = elemSegment.attribute("from").toDouble();
      double to = elemSegment.attribute("to").toDouble();
      Log_data::Segment_geometry test(nodeid, start_loc, end_loc, from,to);
      it->second.push_back( Log_data::Segment_geometry(nodeid, start_loc, end_loc, from,to) );
  	}

  }
  grid->set_log_geometry(grid_geometries);


  return grid;

}

// --------------------------------------------------------

Named_interface* Structured_grid_geometry_xml_io::create_new_interface(std::string& str){
  return new Structured_grid_geometry_xml_io;
}

QDomElement Structured_grid_geometry_xml_io::
  write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const{

	const Reduced_grid *mgrid =  dynamic_cast<const Reduced_grid*>( grid );

	const Structured_grid* struct_grid = dynamic_cast<const Structured_grid*>(grid);

	 QDomElement elemGeom = dom.createElement("Geometry");
	 elemGeom.setAttribute("nx",struct_grid->nx());
	 elemGeom.setAttribute("ny",struct_grid->ny());
	 elemGeom.setAttribute("nz",struct_grid->nz());
   elemGeom.setAttribute("rotation_z_angle",struct_grid->rotation_z());

  QFile file( dir.absoluteFilePath("corner_coordinates.sgems" ) );
  if( !file.open( QIODevice::WriteOnly ) ) {
  	elemGeom.clear();
  	return elemGeom;
  }
	QDataStream stream( &file );
  /*
	#if QT_VERSION >= 0x040600
		stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	#endif
  */
  stream.setFloatingPointPrecision(QDataStream::DoublePrecision);

  int n_points = (struct_grid->nx()+1)*(struct_grid->ny()+1)*(struct_grid->nz()+1);

	for(int i=0; i<n_points; ++i) {
    GsTLPoint pt = struct_grid->get_corner_point_locations(i);
    stream<< pt.x();
    stream<< pt.y();
    stream<< pt.z();
  }
	file.close();
	return elemGeom;

}

Geostat_grid* Structured_grid_geometry_xml_io::
  read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const {


	QString grid_name = elem.attribute("name");

	QDomElement elemGeom = elem.firstChildElement("Geometry");

  int nx = elemGeom.attribute("nx").toInt();
  int ny = elemGeom.attribute("ny").toInt();
  int nz = elemGeom.attribute("nz").toInt();

  float rotation_z_angle = elemGeom.attribute("rotation_z_angle").toFloat();


// Read the mask
 // std::string maskfile = dir.absolutePath().toStdString()+"/gridmask.sgems";
 // std::fstream stream(maskfile.c_str());

  QFile file( dir.absolutePath()+"/corner_coordinates.sgems" );
  if( !file.open( QIODevice::ReadOnly ) ) {
  	errors->append("Could not open the coordinates for the grid geometry");
  	return 0;
  }
	QDataStream stream( &file );
  stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
/*
#if QT_VERSION >= 0x040600
  stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
#endif
*/
  int n_points = (nx+1)*(ny+1)*(nz+1); 
  std::vector<GsTLPoint> coords;
	coords.reserve(n_points);
	for( GsTLInt k = 0; k < n_points ; k++ ) {
    GsTLPoint corner_coords;
		stream >> corner_coords.x();
    stream >> corner_coords.y();
    stream >> corner_coords.z();
		coords.push_back( corner_coords );
	}

  std::string final_grid_name;
	SmartPtr<Named_interface> ni =
		Root::instance()->new_interface( "Structured_grid://" + grid_name.toStdString(),
		gridModels_manager + "/" + grid_name.toStdString());
	Structured_grid* grid = dynamic_cast<Structured_grid*>( ni.raw_ptr() );
	if(grid==0) return 0;

	if (!grid) {
		errors->append("could not create the grid");
		return 0;
	}

  grid->set_dimensions( nx, ny, nz);
  grid->set_structured_points(coords);

	return grid;


}
