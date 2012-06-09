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
 * colorbar_controller.h
 *
 *  Created on: Aug 9, 2011
 *      Author: aboucher
 */

#ifndef COLORBAR_CONTROLLER_H_
#define COLORBAR_CONTROLLER_H_

#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>

#include <QWidget>

#include <vtkRenderer.h>
#include <vtkScalarBarActor.h>


class Geostat_grid;


class Colorbar_controller : public QWidget{

	Q_OBJECT
public:
	Colorbar_controller(vtkRenderer* renderer, QWidget* parent =0);
	virtual ~Colorbar_controller();

	vtkScalarBarActor* color_bar(GsTL_vtkProp* prop);
	bool is_colorbar_visible(GsTL_vtkProp* prop);


public slots:
	void update_colorbars();
	void update_colorbars(GsTL_vtkProp* prop);
	void remove_color_bar(GsTL_vtkProp* prop);
	void add_color_bar(GsTL_vtkProp* prop, bool is_visible);
	void set_colorbar_visible(GsTL_vtkProp* prop, bool visibility);
	void set_colorbar_orientation(GsTL_vtkProp* prop, bool is_vertical);
	void set_colorbar_width(GsTL_vtkProp* prop, float width);
	void set_colorbar_height(GsTL_vtkProp* prop, float height);
	void set_colorbars_text_color(bool is_background_white);
	void set_number_of_ticks(GsTL_vtkProp* prop, int n_ticks);

protected :

	bool colorbar_to_latest_display;

	typedef std::map<GsTL_vtkProp*,std::pair<bool,vtkScalarBarActor*> > prop_to_colorbarsT;
	prop_to_colorbarsT colorbars_;


protected :
	vtkRenderer* renderer_;

};

#endif /* COLORBAR_CONTROLLER_H_ */
