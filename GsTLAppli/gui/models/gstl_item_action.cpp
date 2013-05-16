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
 * gstl_item_action.cpp
 *
 *  Created on: May 26, 2011
 *      Author: aboucher
 */


#include <GsTLAppli/gui/models/gstl_item_action.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>

#include <GsTLAppli/extra/qtplugins/rename_dialog.h>

Gstl_item_action::Gstl_item_action(QObject * parent)
: QAction(parent)
{
	SmartPtr<Named_interface> ni = Root::instance()->interface(projects_manager + "/" + "project");
	project_ = dynamic_cast<GsTL_project*>(ni.raw_ptr());
}

Gstl_item_action::Gstl_item_action(const QString& text, QObject * parent)
:  QAction(text,parent)
{
	SmartPtr<Named_interface> ni = Root::instance()->interface(projects_manager + "/" + "project");
	project_ = dynamic_cast<GsTL_project*>(ni.raw_ptr());
}

Gstl_item_action::Gstl_item_action(const QIcon & icon, const QString& text, QObject * parent)
:  QAction(icon, text,parent)
{
	SmartPtr<Named_interface> ni = Root::instance()->interface(projects_manager + "/" + "project");
	project_ = dynamic_cast<GsTL_project*>(ni.raw_ptr());
}


Gstl_item_action::~Gstl_item_action() {
	// TODO Auto-generated destructor stub
}

void Gstl_item_action::initialize(GsTL_object_item* item){
	this->setEnabled(true);
}





Grid_trend_item_action::Grid_trend_item_action(QString direction, QObject * parent)
 : Gstl_item_action(parent), direction_(direction){
	this->setText(direction_);
	this->setToolTip(QString("Create a %1 trend between 0 and 1 ").arg(direction));

}

Grid_trend_item_action::~Grid_trend_item_action(){

}

bool Grid_trend_item_action::execute(GsTL_object_item* item)
{
	Error_messages_handler error_messages;

	Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item);

	if(grid == 0) return false;

	std::string grid_name = item->item_data(0).toString().toStdString();
	bool ok = project_->execute("CreateTrend",grid_name+"::"+direction_.toStdString(), &error_messages);

	if (!ok)
	{
		GsTLcerr << "Command CreateTrend in the "<<direction_.toStdString()<<" direction could not be performed: \n";
		if (!error_messages.empty())
			GsTLcerr << error_messages.errors() << "\n";
		GsTLcerr << gstlIO::end;
		return false;
	}
	return true;

}


/*
 *
 */

Rename_property_item_action::Rename_property_item_action(QObject * parent)
 : Gstl_item_action(parent)
{
	this->setText("Rename");
	this->setToolTip("Rename a property");
}

bool Rename_property_item_action::execute(GsTL_object_item* item){

	GsTLGridProperty* prop = dynamic_cast<GsTLGridProperty*>(item);
	if(prop == 0) return false;

	std::string old_name = prop->name();
	Rename_dialog rename_diag("Property",old_name.c_str(), dynamic_cast<QWidget*>(this->parent()));
	int r = rename_diag.exec();
	if (r==1) {
		Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item->parent()->parent());
		if(grid == 0) return false;
		return grid->reNameProperty(old_name, rename_diag.new_name().toStdString() );
	}
	else
		return false;

}

/*
 *
 */

Swap_to_RAM_property_item_action::
Swap_to_RAM_property_item_action(QObject * parent)
 : Gstl_item_action(parent) {
	this->setText("Swap to memory");
	this->setToolTip("Swap the data from the disk to the memory");
}
Swap_to_RAM_property_item_action::
~Swap_to_RAM_property_item_action(){}

void Swap_to_RAM_property_item_action::initialize(GsTL_object_item* item){
	GsTLGridProperty* prop = dynamic_cast<GsTLGridProperty*>(item);

	if(prop == 0) {
		this->setVisible(false);
	}
	else if(prop->is_in_memory()) {
		this->setVisible(false);
	}
	else {
		this->setVisible(true);
	}
}

bool Swap_to_RAM_property_item_action::execute(GsTL_object_item* item){
	GsTLGridProperty* prop = dynamic_cast<GsTLGridProperty*>(item);
	if(prop) {
		prop->swap_to_memory();
	}
  return true;
}


/*
 *
 */


Swap_to_disk_property_item_action::
Swap_to_disk_property_item_action(QObject * parent)
 : Gstl_item_action(parent) {
	this->setText("Swap to Disk");
	this->setToolTip("Swap the data from memory to the disk");
}
Swap_to_disk_property_item_action::
~Swap_to_disk_property_item_action(){}

void Swap_to_disk_property_item_action::initialize(GsTL_object_item* item){
	GsTLGridProperty* prop = dynamic_cast<GsTLGridProperty*>(item);
	if(prop == 0) {
		this->setVisible(false);
	}
	else if(prop->is_in_memory()) {
		this->setVisible(true);
	}
	else {
		this->setVisible(false);
	}
}

bool Swap_to_disk_property_item_action::execute(GsTL_object_item* item){
	GsTLGridProperty* prop = dynamic_cast<GsTLGridProperty*>(item);
	if(prop) {
		prop->swap_to_disk();
		return true;
	}
	return false;
}

Unary_property_item_action::
Unary_property_item_action(QString unary_action_name, QObject * parent)
 : Gstl_item_action(parent), unary_action_name_(unary_action_name){
	this->setText("unary_action_name");
	this->setToolTip(QString("Perform the command %1").arg(unary_action_name));
}
Unary_property_item_action::~Unary_property_item_action(){}

bool Unary_property_item_action::execute(GsTL_object_item* item){

	Error_messages_handler error_messages;
	GsTLGridProperty* prop = dynamic_cast<GsTLGridProperty*>(item);
  if(prop == 0) { //May be inside a region
    GsTL_object_property_item* prop_in_group = dynamic_cast<GsTL_object_property_item*>(item);
    if(prop_in_group)
      prop = prop_in_group->property();
  }

	if(prop==0) {
		GsTLcerr << "Command " << unary_action_name_.toStdString() << " could not be performed; a property was not correctly selected";
		GsTLcerr << gstlIO::end;
		return false;
	}

	std::string grid_name = item->parent()->parent()->item_data(0).toString().toStdString();  //prop->group->grid
	std::string params = grid_name+"::"+prop->name();
	bool ok = project_->execute(unary_action_name_.toStdString(), params , &error_messages);

	if (!ok)
	{
		GsTLcerr << "Command " << unary_action_name_.toStdString() << " could not be performed: \n";
		if (!error_messages.empty())
			GsTLcerr << error_messages.errors() << "\n";
		GsTLcerr << gstlIO::end;
	}
	return ok;
}


