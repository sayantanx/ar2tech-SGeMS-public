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
 * context_menu_interface.cpp
 *
 *  Created on: May 27, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>

#include <GsTLAppli/actions/action.h>
#include <GsTLAppli/actions/unary_action.h>

#include <GsTLAppli/gui/models/context_menu_interface.h>
#include <GsTLAppli/gui/models/gstl_item_action.h>


Context_menu_interface::Context_menu_interface()
 : Named_interface()  {
	menu_ = new QMenu();
}

Context_menu_interface::~Context_menu_interface() {
	delete menu_;
}


QMenu* Context_menu_interface::context_menu(GsTL_object_item* item){

	if(item != 0) {
		for(int i=0; i < actions_.size(); ++i) {
			Gstl_item_action* action = dynamic_cast<Gstl_item_action*>(actions_.at(i));
			if(action) {
				action->initialize(item);
			}
		}
	}

	return menu_;
}

void Context_menu_interface::add_action(QAction* action, QString sub_menu_title){
	if(sub_menu_title.isEmpty()) {
		menu_->addAction(action);
	}
	else {
		QMenu* submenu=0;
		// Find if the submenu aklready exist
		for(int i = 0; i<submenus_.size(); ++i) {
			if( submenus_.at(i)->title() == sub_menu_title ) {
				submenu = submenus_.at(i);
				break;
			}
		}

		if(submenu==0) {
			submenu = menu_->addMenu(sub_menu_title);
			submenus_.append(submenu);
		}

		submenu->addAction(action);
	}
	actions_.append(action);
}
/*
void Context_menu_interface::add_submenu(QMenu* submenu){
	menu_->addMenu(submenu);
	submenus_.append(submenu);
	submenu->setParent(menu_);
}
*/
QMenu* Context_menu_interface::submenu(QString title){
	for(int i = 0; i<submenus_.size(); ++i) {
		if( submenus_.at(i)->title() == title ) {
			return submenus_.at(i);
		}
	}
	return 0;

}


/*
 *
 */

Named_interface* Context_menu_grid::create_new_interface(std::string&) {
	return new Context_menu_grid;
}

Context_menu_grid::Context_menu_grid()
 : Context_menu_interface(){

	QAction* action;
/*
	QStringList plist = QIcon::themeSearchPaths();
	for(int i=0; i<plist.size(); ++i) {
		std::string path = plist.value(i).toStdString();
		int i=5;
	}
	*/
	action = new Gstl_item_action(QIcon::fromTheme("edit-delete"),"Delete",menu_);
	menu_->addAction(action);
	actions_.append(action);

	QMenu* submenu = menu_->addMenu("Trend");
	submenus_.append(submenu);
	action = new Grid_trend_item_action("X",menu_);
	actions_.append(action);
	submenu->addAction(action);

	action = new Grid_trend_item_action("-X",menu_);
	actions_.append(action);
	submenu->addAction(action);

	action = new Grid_trend_item_action("Y",menu_);
	actions_.append(action);
	submenu->addAction(action);

	action = new Grid_trend_item_action("-Y",menu_);
	actions_.append(action);
	submenu->addAction(action);

	action = new Grid_trend_item_action("Z",menu_);
	actions_.append(action);
	submenu->addAction(action);

	action = new Grid_trend_item_action("-Z",menu_);
	actions_.append(action);
	submenu->addAction(action);
}
Context_menu_grid::~Context_menu_grid(){}

/*
 *
 */

Named_interface* Context_menu_property::create_new_interface(std::string&) {
	return new Context_menu_property;
}


Context_menu_property::Context_menu_property()
 : Context_menu_interface(){


	QAction* action_delete = new Gstl_item_action("Delete",menu_);
	menu_->addAction(action_delete);
	actions_.append(action_delete);

  /* Need to update the model, the problem is that
   * the row number will change, hence need a smart updating of the model

	QAction* action_rename = new Rename_property_item_action(menu_);
	menu_->addAction(action_rename);
	actions_.append(action_rename);
  */
  QAction* action_swap_to_ram = new Swap_to_RAM_property_item_action(menu_);
	menu_->addAction(action_swap_to_ram);
	actions_.append(action_swap_to_ram);

  QAction* action_swap_to_disk = new Swap_to_disk_property_item_action(menu_);
	menu_->addAction(action_swap_to_disk);
	actions_.append(action_swap_to_disk);

	QAction* action_histo = new Unary_property_item_action("ShowHistogram", menu_);
	action_histo->setText("Histogram");
  menu_->addAction(action_histo);
  actions_.append(action_histo);

	menu_->addSeparator();

	this->load_unary_actions();


}

Context_menu_property::~Context_menu_property(){
  
}


void Context_menu_property::initialize(GsTL_object_item* item){
  for(int i=0; i<actions_.size(); ++i) {
    Gstl_item_action* action = dynamic_cast<Gstl_item_action*>(actions_.at(i));
    if(action == 0) continue;
    action->initialize(item);

  }


}

void Context_menu_property::load_unary_actions(){
	SmartPtr<Named_interface> ni = Root::instance()->interface(actions_manager);
	Manager* manager = dynamic_cast<Manager*> (ni.raw_ptr());
	if (manager)
	{
		QMenu* unary_action_menu = menu_->addMenu("Data transform");
		Manager::type_iterator begin = manager->begin();
		Manager::type_iterator end = manager->end();
		for (; begin != end; ++begin)
		{
			std::string name = *begin;
			SmartPtr<Named_interface> instance = manager->new_interface(name);
			PropertyNoParameterAction* uaction = dynamic_cast<PropertyNoParameterAction*> (instance.raw_ptr());
			if (uaction)
			{
				QAction* action = new Unary_property_item_action( name.c_str(), unary_action_menu );
				action->setText(name.c_str());
				actions_.push_back(action);
				unary_action_menu->addAction(action);
			}
		}
	}
}



/*
 *
 */

Named_interface* Context_menu_region::create_new_interface(std::string&) {
	return new Context_menu_region;
}


Context_menu_region::Context_menu_region()
 : Context_menu_interface(){


	QAction* action_delete = new Gstl_item_action("Delete",menu_);
	menu_->addAction(action_delete);
	actions_.append(action_delete);
/*
	QAction* action_rename = new Rename_property_item_action(menu_);
	menu_->addAction(action_rename);
	actions_.append(action_rename);
*/
}

Context_menu_region::~Context_menu_region(){
  
}


