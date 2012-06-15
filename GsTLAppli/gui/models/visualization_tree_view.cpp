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
 * visualization_tree_view.cpp
 *
 *  Created on: May 25, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/gui/models/visualization_tree_view.h>

#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/actions/obj_manag_actions.h>
//#include <GsTLAppli/gui/models/project_ItemModel.h>
#include <GsTLAppli/gui/models/gstl_item.h>
#include <GsTLAppli/gui/models/context_menu_interface.h>
#include <GsTLAppli/gui/models/gstl_item_selection_model.h>
#include <GsTLAppli/gui/models/gstl_item_action.h>
#include <GsTLAppli/extra/qtplugins/project_proxy_model.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/project.h>


#include <QMenu>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStyleOption>
#include <QEvent>

Visualization_tree_view::Visualization_tree_view(QWidget* parent)
 : QTreeView(parent){

	this->setContextMenuPolicy(Qt::CustomContextMenu);
//  this->setSelectionMode(QAbstractItemView::NoSelection);
	this->setSelectionBehavior(QAbstractItemView::SelectItems);
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
  this->setExpandsOnDoubleClick(true);

	//this->setExpandsOnDoubleClick(false);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
	    this, SLOT(contextMenu(const QPoint&)));
}

Visualization_tree_view::~Visualization_tree_view() {
//  this->selectionModel()->clear();
}

/*
void
Visualization_tree_view::contextMenuEvent(QContextMenuEvent *event){

	QAction *openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("Open an existing file"));

  QMenu menu(this);
  menu.addAction(openAct);
  menu.exec(event->globalPos());
}
*/




void Visualization_tree_view::contextMenu(const QPoint& point){
  //QSortFilterProxyModel* proxy_model = dynamic_cast<QSortFilterProxyModel*>(this->model());
  Visualization_proxy_model* proxy_model = dynamic_cast<Visualization_proxy_model*>(this->model());
  QModelIndex index = proxy_model->mapToSource(this->indexAt(point));
 //   QModelIndex index = this->indexAt(point);

  

	if( !index.isValid() ) return;


	GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());

	SmartPtr<Named_interface> ni = Root::instance()->interface(
								contextMenus_manager + "/" + item->item_type().toStdString()
								);

	Context_menu_interface* menu_ni = dynamic_cast<Context_menu_interface*>(ni.raw_ptr());

	if(menu_ni == 0) return;

  menu_ni->initialize(item);

	QPoint globalPoint = this->mapToGlobal(point);
	QAction* action = menu_ni->context_menu()->exec(globalPoint);
	Gstl_item_action* selected_action = dynamic_cast<Gstl_item_action*>(action);

	if (selected_action == 0) return;  //Assume that a slot was associated with that QAction

	if(selected_action->text() == "Delete") {
    if( item->item_type().contains("Property") ) {
      Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item->parent()->parent());
      if(grid) {
        grid->remove_property(item->item_name().toStdString());
      }
    }
    else if( item->item_type().contains("Region") ) {
      Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item->parent()->parent());
      if(grid) {
        grid->remove_region(item->item_name().toStdString());
      }
    }
    else if( dynamic_cast<Geostat_grid*>(item) ) {
      GsTL_project* project = dynamic_cast<GsTL_project*>(Root::instance()->interface(projects_manager+"/project").raw_ptr());
      Error_messages_handler errors;
      Delete_objects remove_grid_action;
      std::string params = item->item_name().toStdString();
      remove_grid_action.init(params,project,&errors);
      remove_grid_action.exec();

    }
	 // We must delete from the parent
	}
	else {
		selected_action->execute(item);
	}

}


/*
QItemSelectionModel::SelectionFlags
Visualization_tree_view::selectionCommand ( const QModelIndex & index, const QEvent * event) const{
	return QItemSelectionModel::NoUpdate;
	QEvent::Type etype = event->type();

	switch (event->type()) {
	case QEvent::MouseButtonDblClick:
		return QTreeView::selectionCommand(index, event);
	default:
		return QItemSelectionModel::NoUpdate;
	}

}
*/
/*
void Visualization_tree_view::show_grid_context_menu(GsTL_grid_item* item, const QPoint& point){


	SmartPtr<Named_interface> ni = Root::instance()->interface(
								contextMenus_manager + "/" + item->grid()->classname()
								);

	Context_menu_interface* menu_ni = dynamic_cast<Context_menu_interface*>(ni.raw_ptr());

	appli_assert(menu_ni);

	QPoint globalPoint = this->mapToGlobal(point);
	QAction* action = menu_ni->context_menu()->exec(globalPoint);
	Gstl_item_action* selected_action = dynamic_cast<Gstl_item_action*>(action);

	if (selected_action == 0) return;  //Assume that a slot was associated with that QAction

	if(selected_action->text() == "Delete") {
		this->delete_grid();
	}
	else {
		selected_action->execute(item);
	}
}

void Visualization_tree_view::show_group_context_menu(GsTL_group_property_item* item, const QPoint& point){
	SmartPtr<Named_interface> ni = Root::instance()->interface(
								contextMenus_manager + "/" + item->group()->type()
								);

	Context_menu_interface* menu_ni = dynamic_cast<Context_menu_interface*>(ni.raw_ptr());

	appli_assert(menu_ni);

	QPoint globalPoint = this->mapToGlobal(point);
	QAction* action = menu_ni->context_menu()->exec(globalPoint);
	Gstl_item_action* selected_action = dynamic_cast<Gstl_item_action*>(action);

	if (selected_action == 0) return;  //Assume that a slot was associated with that QAction

	if(selected_action->text() == "Delete") {
		this->delete_grid();
	}
	else {
		selected_action->execute(item);
	}
}
void Visualization_tree_view::show_property_context_menu(GsTL_property_item* item, const QPoint& point){
	SmartPtr<Named_interface> ni = Root::instance()->interface(
								contextMenus_manager + "/" + item->property()->classname()
								);

	Context_menu_interface* menu_ni = dynamic_cast<Context_menu_interface*>(ni.raw_ptr());

	appli_assert(menu_ni);

	QPoint globalPoint = this->mapToGlobal(point);
	QAction* action = menu_ni->context_menu(item)->exec(globalPoint);
	Gstl_item_action* selected_action = dynamic_cast<Gstl_item_action*>(action);

	if (selected_action == 0) return;  //Assume that a slot was associated with that QAction

	if(selected_action->text() == "Delete") {
		//this->delete_property();
	}
	else {
		selected_action->execute(item);
	}
//	emit this->model()->dataChanged();
}
void Visualization_tree_view::show_region_context_menu(GsTL_region_item* item, const QPoint& point){

}


bool Visualization_tree_view::delete_grid()
{
	QModelIndex index = this->currentIndex();



	Project_ItemModel *model = dynamic_cast<Project_ItemModel*>(this->model());

	model->delete_object(index);

	return true;
}
*/
/*
void Visualization_tree_view::paintEvent(QPaintEvent *pe) {
  QStyleOption o;
  o.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
};
*/
/*

	emit model->rowsAboutToBeRemoved( index.parent(), index.row(),index.row() );
	model->removeRow( index.row(), index.parent() );



	GsTL_grid_item* grid_item = static_cast<GsTL_grid_item*>(index.internalPointer());

	std::string obj_name = grid_item->gridName().toStdString();
	bool ok = Root::instance()->delete_interface(gridModels_manager + "/" + obj_name);
	if (ok)
	{
		SmartPtr<Named_interface> ni = Root::instance()->interface(projects_manager + "/" + "project");
		GsTL_project* proj = dynamic_cast<GsTL_project*> (ni.raw_ptr());
		proj->deleted_object(obj_name);
	}
	emit model->rowsRemoved( index.parent(), index.row(),index.row() );
	return ok;
	*/

