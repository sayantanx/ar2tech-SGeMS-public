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
 * grid_param_viewer.h
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#ifndef GRID_PARAM_VIEWER_H_
#define GRID_PARAM_VIEWER_H_

#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/log_data_grid.h>
#include <GsTLAppli/grid/grid_model/structured_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_strati_grid.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_cgrid.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_mgrid.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_pointset.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_log.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_structured_grid.h>
#include <GsTLAppli/gui/common.h>

#include <GsTLAppli/gui/viewer/visualization_parameters.h>

#include <Qt>
#include <QWidget>
#include <QToolBox>
#include <QTabWidget>
#include <QRadioButton>



//This a more complex one since it also include some control over the display, surface vs mesh, contour, etc...

class GUI_DECL Grid_param_viewer : public QTabWidget  {

	Q_OBJECT
public:
	Grid_param_viewer();
	virtual ~Grid_param_viewer();

	virtual void initialize(GsTL_vtkProp* vtk_prop)=0;

	const Geostat_grid* grid();
	QString grid_name() const;

// Project View interface
	virtual void update(std::string obj);
	virtual void new_object(std::string obj);
	virtual void deleted_object(std::string obj);

signals :
  void rendering_modified();

protected slots:
	void colorbar_display_option(const QString& location);


protected :
	virtual void init_visualization_page();
	virtual void init_stats_page()=0;

protected :
	const Geostat_grid* grid_;
	GsTL_vtkProp* vtk_prop_;

protected :
	QString current_prop_name_;


};

class GUI_DECL SGrid_param_viewer: public Grid_param_viewer
{
	Q_OBJECT
public :
	SGrid_param_viewer();
	virtual ~SGrid_param_viewer(){}

protected :
	virtual void init_visualization_page();
  virtual void init_section_page();

public slots:
	void cell_representation( const QString & text);
	void show_edges( bool on );
	virtual void show_grid_outline( bool on );

protected slots:
	void show_edge_button(const QString& type);

protected :
	QRadioButton* radio_edge_button_;
	vtkProp_strati_grid *vtk_prop_strati_;
};


class GUI_DECL CGrid_param_viewer: public SGrid_param_viewer
{
	Q_OBJECT
public :

	static Named_interface* create_new_interface(std::string&);

	CGrid_param_viewer();
	CGrid_param_viewer(GsTL_vtkProp* vtk_prop);
	virtual ~CGrid_param_viewer(){}

	virtual void initialize(GsTL_vtkProp* vtk_prop);

protected :
	virtual void init_stats_page();

protected :
	const Cartesian_grid* cgrid_;
	vtkProp_cgrid* vtk_prop_cgrid_;
};



class GUI_DECL MGrid_param_viewer: public SGrid_param_viewer
{
	Q_OBJECT
public :

//	static Named_interface* create_new_interface(std::string&);

	MGrid_param_viewer();
	MGrid_param_viewer(GsTL_vtkProp* vtk_prop);
	virtual ~MGrid_param_viewer(){}
	virtual void initialize(GsTL_vtkProp* vtk_prop);

protected :
	virtual void init_stats_page();
	const Reduced_grid* mgrid_;
	vtkProp_mgrid* vtk_prop_mgrid_;
};




class GUI_DECL PSet_param_viewer: public Grid_param_viewer
{
	Q_OBJECT
public :

//	static Named_interface* create_new_interface(std::string&);

	PSet_param_viewer();
	PSet_param_viewer(GsTL_vtkProp* vtk_prop);
	virtual ~PSet_param_viewer(){}
	virtual void initialize(GsTL_vtkProp* vtk_prop);

protected :
	virtual void init_stats_page();
	virtual void init_visualization_page();
	void bounding_box( GsTLPoint& min, GsTLPoint& max );

protected slots:
	void point_size( int size );

protected :
	const Point_set* pset_;
	vtkProp_pointset* vtk_prop_pset_;

};




class GUI_DECL Log_grid_param_viewer: public Grid_param_viewer
{
	Q_OBJECT
public :

//	static Named_interface* create_new_interface(std::string&);

	Log_grid_param_viewer();
	Log_grid_param_viewer(GsTL_vtkProp* vtk_prop);
	virtual ~Log_grid_param_viewer(){}
	virtual void initialize(GsTL_vtkProp* vtk_prop);

protected :
	virtual void init_stats_page();
	virtual void init_visualization_page();
	void bounding_box( GsTLPoint& min, GsTLPoint& max );

protected slots:
	void line_width( int size );

protected :
	const Log_data_grid* log_grid_;
	vtkProp_log* vtk_prop_log_;

};

class GUI_DECL Structured_grid_param_viewer: public SGrid_param_viewer
{
	Q_OBJECT
public :

	static Named_interface* create_new_interface(std::string&);

	Structured_grid_param_viewer();
	Structured_grid_param_viewer(GsTL_vtkProp* vtk_prop);
	virtual ~Structured_grid_param_viewer(){}

	virtual void initialize(GsTL_vtkProp* vtk_prop);

protected :
	virtual void init_stats_page();

protected :
	Structured_grid* struct_grid_;
	vtkProp_structured_grid* vtk_prop_struct_grid_;
};

class Grid_viz_parameters : public Visualization_parameters {
public :


	Grid_viz_parameters(): Visualization_parameters(){}
	virtual ~Grid_viz_parameters(){}

	virtual void display();
	virtual void undisplay();

	virtual void initialize(Geostat_grid* grid, vtkRenderer* renderer)=0;
	virtual QWidget* interface()=0;

protected :
	Geostat_grid* grid_;
};




class CGrid_viz_parameters : public Grid_viz_parameters {
public :

	static Named_interface* create_new_interface(std::string&);

	CGrid_viz_parameters();
	virtual ~CGrid_viz_parameters();


	virtual void initialize(Geostat_grid* grid, vtkRenderer* renderer);


	virtual QWidget* interface();

protected :
	CGrid_param_viewer* viewer_;
};


class MGrid_viz_parameters : public Grid_viz_parameters {
public :

	static Named_interface* create_new_interface(std::string&);

	MGrid_viz_parameters();
	virtual ~MGrid_viz_parameters();


	virtual void initialize(Geostat_grid* grid, vtkRenderer* renderer);
	virtual QWidget* interface();

protected :
	MGrid_param_viewer* viewer_;
};

class PSet_viz_parameters : public Grid_viz_parameters {
public :

	static Named_interface* create_new_interface(std::string&);

	PSet_viz_parameters();
	virtual ~PSet_viz_parameters();


	virtual void initialize(Geostat_grid* grid, vtkRenderer* renderer);
	virtual QWidget* interface();

protected :
	PSet_param_viewer* viewer_;
};

class Log_grid_viz_parameters : public Grid_viz_parameters {
public :

	static Named_interface* create_new_interface(std::string&);

	Log_grid_viz_parameters();
	virtual ~Log_grid_viz_parameters();


	virtual void initialize(Geostat_grid* grid, vtkRenderer* renderer);
	virtual QWidget* interface();

protected :
	Log_grid_param_viewer* viewer_;
};

class Structured_grid_viz_parameters : public Grid_viz_parameters {
public :

	static Named_interface* create_new_interface(std::string&);

	Structured_grid_viz_parameters();
	virtual ~Structured_grid_viz_parameters();


	virtual void initialize(Geostat_grid* grid, vtkRenderer* renderer);
	virtual QWidget* interface();

protected :
  Structured_grid_param_viewer* viewer_;
};

#endif /* GRID_PARAM_VIEWER_H_ */
