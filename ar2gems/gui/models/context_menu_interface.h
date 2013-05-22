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
 * context_menu_interface.h
 *
 *  Created on: May 27, 2011
 *      Author: aboucher
 */

#ifndef CONTEXT_MENU_INTERFACE_H_
#define CONTEXT_MENU_INTERFACE_H_

#include <utils/gstl_object_item.h>
#include <appli/project.h>
#include <gui/common.h>
//#include <gui/models/gstl_item.h>

#include <QMenu>
#include <QIcon>

class GUI_DECL Context_menu_interface : public Named_interface {
public:
	Context_menu_interface();
	virtual ~Context_menu_interface();

	virtual QMenu* context_menu(GsTL_object_item* item=0);
	virtual void add_action(QAction* action, QString sub_menu_name = "");
//	virtual void add_submenu(QMenu* submenu);
	virtual QMenu* submenu(QString title);
  virtual void initialize(GsTL_object_item* item){}


protected :
	QList<QAction*> actions_;
	QMenu* menu_;
	QList<QMenu*> submenus_;
};

class GUI_DECL Context_menu_grid : public Context_menu_interface {
public:

	static Named_interface* create_new_interface(std::string&);

	Context_menu_grid();
	virtual ~Context_menu_grid();
};

class Context_menu_property : public Context_menu_interface {
public:

	static Named_interface* create_new_interface(std::string&);

	Context_menu_property();
	virtual ~Context_menu_property();

  virtual void initialize(GsTL_object_item* item);

protected :
	void load_unary_actions();

};


class GUI_DECL Context_menu_region : public Context_menu_interface {
public:

	static Named_interface* create_new_interface(std::string&);

	Context_menu_region();
	virtual ~Context_menu_region();

//  virtual void initialize(GsTL_object_item* item);

};

#endif /* CONTEXT_MENU_INTERFACE_H_ */
