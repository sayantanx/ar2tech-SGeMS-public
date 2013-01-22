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


#ifndef SGEMS_GRID_GEOMETRY_FILTER_H_
#define SGEMS_GRID_GEOMETRY_FILTER_H_

#include <GsTLAppli/filters/common.h>
#include <GsTLAppli/utils/named_interface.h>
#include <QDir>
#include <QDomElement>

class Geostat_grid;
class Log_data;

class FILTERS_DECL Grid_geometry_xml_io : public Named_interface {
public :

  virtual QDomElement write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const = 0;
  virtual Geostat_grid* read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const =0;

};


class FILTERS_DECL Cartesian_grid_geometry_xml_io : public Grid_geometry_xml_io {
public :

  static Named_interface* create_new_interface(std::string& str);

  virtual QDomElement write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const;
  virtual Geostat_grid* read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const;

};

class FILTERS_DECL Pointset_geometry_xml_io : public Grid_geometry_xml_io {
public :

  static Named_interface* create_new_interface(std::string& str);

  virtual QDomElement write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const;
  virtual Geostat_grid* read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const;

};


class FILTERS_DECL Masked_grid_geometry_xml_io : public Grid_geometry_xml_io {
public :

  static Named_interface* create_new_interface(std::string& str);

  virtual QDomElement write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const;
  virtual Geostat_grid* read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const;

};

class FILTERS_DECL Log_data_grid_geometry_xml_io : public Grid_geometry_xml_io {
public :

  static Named_interface* create_new_interface(std::string& str);

  virtual QDomElement write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const;
  virtual Geostat_grid* read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const;

private :
  QDomElement write_log_data_geometry( QDomDocument& doc, const Log_data* ldata ) const;

};

class FILTERS_DECL Structured_grid_geometry_xml_io : public Grid_geometry_xml_io {
public :

  static Named_interface* create_new_interface(std::string& str);

  virtual QDomElement write_grid_geometry( QDir dir, QDomDocument& dom, const  Geostat_grid* grid) const;
  virtual Geostat_grid* read_grid_geometry(QDir dir,const QDomElement& elem, std::string* errors) const;

};


#endif
