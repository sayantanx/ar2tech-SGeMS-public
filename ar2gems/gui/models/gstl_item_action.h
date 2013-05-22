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
 * gstl_item_action.h
 *
 *  Created on: May 26, 2011
 *      Author: aboucher
 */

#ifndef GSTL_ITEM_ACTION_H_
#define GSTL_ITEM_ACTION_H_

//#include <gui/models/gstl_item.h>
#include <utils/gstl_object_item.h>
#include <appli/project.h>

#include <QAction>
#include <QStringList>


class Gstl_item_action : public QAction {

	Q_OBJECT
public:
	Gstl_item_action(QObject * parent=0);
	Gstl_item_action(const QString& text, QObject * parent=0);
	Gstl_item_action(const QIcon & icon, const QString& text, QObject * parent=0);
	virtual ~Gstl_item_action();

	virtual void initialize(GsTL_object_item* item);
	virtual bool execute(GsTL_object_item* item){ return true;}

protected :
	GsTL_project* project_;

};

class Grid_trend_item_action : public Gstl_item_action {

	Q_OBJECT
public:
	Grid_trend_item_action(QString direction_, QObject * parent=0);
	virtual ~Grid_trend_item_action();

	virtual bool execute(GsTL_object_item* item);

protected :
	QString direction_;

};


class Rename_property_item_action : public Gstl_item_action {

	Q_OBJECT
public:
	Rename_property_item_action(QObject * parent=0);
	virtual ~Rename_property_item_action(){}

	virtual bool execute(GsTL_object_item* item);
};


class Swap_to_RAM_property_item_action : public Gstl_item_action {

	Q_OBJECT
public:
	Swap_to_RAM_property_item_action(QObject * parent=0);
	virtual ~Swap_to_RAM_property_item_action();

	virtual void initialize(GsTL_object_item* item);
	virtual bool execute(GsTL_object_item* item);
};

class Swap_to_disk_property_item_action : public Gstl_item_action {

	Q_OBJECT
public:
	Swap_to_disk_property_item_action(QObject * parent=0);
	virtual ~Swap_to_disk_property_item_action();

	virtual void initialize(GsTL_object_item* item);
	virtual bool execute(GsTL_object_item* item);
};

class Unary_property_item_action : public Gstl_item_action {

	Q_OBJECT
public:
	Unary_property_item_action(QString unary_action_name, QObject * parent=0);
	virtual ~Unary_property_item_action();

	virtual bool execute(GsTL_object_item* item);

protected :
	bool execute_action(QString params);

protected :
	QString unary_action_name_;

};



//Python script

/*
class Delete_grid_item_action : public Gstl_item_action {

	Q_OBJECT
public:
	Delete_grid_item_action(QObject * parent=0);
	virtual ~Delete_grid_item_action();

	virtual bool execute(GsTL_item* item);

protected :
	GsTL_project* project_;

};
*/

#endif /* GSTL_ITEM_ACTION_H_ */
