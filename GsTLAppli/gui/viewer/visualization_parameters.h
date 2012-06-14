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
 * param_viewer.h
 *
 *  Created on: Aug 18, 2011
 *      Author: aboucher
 */

#ifndef PARAM_VIEWER_H_
#define PARAM_VIEWER_H_

#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>

#include <Qt>
#include <QWidget>


//class GUI_DECL Param_viewer : public QWidget, Named_interface{
class GUI_DECL Visualization_parameters : public Named_interface{
//Q_OBJECT
public:
	Visualization_parameters();
	virtual ~Visualization_parameters();

	GsTL_vtkProp* vtk_prop();
  Geostat_grid* grid();

	virtual void display()=0;
	virtual void undisplay()=0;
	virtual QWidget* interface()=0;

signals :
  void rendering_modified();

protected :
	GsTL_vtkProp* vtk_prop_;

};

/*
//class GUI_DECL Param_viewer : public QWidget, Named_interface{
class GUI_DECL Visualization_param_viewer : public QWidget{
 Q_OBJECT

public:
  Visualization_param_viewer(QWidget* parent=0):QWidget(parent){}
  virtual ~Visualization_param_viewer(){}

signals :
  void rendering_modified();

};
*/
#endif /* PARAM_VIEWER_H_ */
