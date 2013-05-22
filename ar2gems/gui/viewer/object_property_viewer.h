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
 * object_property_viewer.h
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#ifndef OBJECT_PROPERTY_VIEWER_H_
#define OBJECT_PROPERTY_VIEWER_H_

#include <Qt>
#include <QWidget>
#include <appli/project.h>

class Object_property_viewer : public QWidget, public Project_view
{

	Q_OBJECT
public:
	Object_property_viewer();
	virtual ~Object_property_viewer();

	//Project View interface
	virtual void update(std::string obj) =0 ;
	virtual void new_object(std::string obj) =0;
	virtual void deleted_object(std::string obj) =0;
};

#endif /* OBJECT_PROPERTY_VIEWER_H_ */
