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
 * project_selection_model.cpp
 *
 *  Created on: Jun 9, 2011
 *      Author: aboucher
 */


#include <GsTLAppli/gui/models/project_selection_model.h>
#include <GsTLAppli/utils/gstl_object_item.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>
/*
 * gstl_item_selection_model.cpp
 *
 *  Created on: May 31, 2011
 *      Author: aboucher
 */


Project_selection_model::Project_selection_model(QAbstractItemModel * model)
 : QItemSelectionModel(model){

   proxy_model_ = dynamic_cast<QSortFilterProxyModel*>(model);

}

Project_selection_model::Project_selection_model(QAbstractItemModel * model, QObject* parent)
: QItemSelectionModel(model, parent) {
  proxy_model_ = dynamic_cast<QSortFilterProxyModel*>(model);

}


Project_selection_model::~Project_selection_model() {
	// TODO Auto-generated destructor stub
}


void Project_selection_model::select ( const QModelIndex & index, QItemSelectionModel::SelectionFlags command ){
	if(index.isValid()) {
		this->select(QItemSelection(index,index),command);
	}




}

void Project_selection_model::select ( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command ){
//	/We only care for the first selected item
	QModelIndexList index_list = selection.indexes();
	for( int i=0; i<index_list.size() ; ++i) {
		QModelIndex index = index_list.at(i);
		if(!index.isValid()) continue;

		bool is_selected = this->isSelected(index);

		QItemSelection item_selected;
		QItemSelection item_deselected;

    //GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
		GsTL_object_item* item = static_cast<GsTL_object_item*>(proxy_model_->mapToSource(index).internalPointer());
		//Check if a grid was selected
		Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item);
		if(grid) {
			if(is_selected) {
				item_deselected.append(	QItemSelectionRange(index) );
			}
			else {
				item_selected.append(QItemSelectionRange(index) );
			}
			QItemSelectionModel::select(item_deselected,QItemSelectionModel::Deselect);
			QItemSelectionModel::select(item_selected,QItemSelectionModel::Select);

			selections_ = this->selection();
			return;
		}

		Grid_continuous_property* prop = dynamic_cast<Grid_continuous_property*>(item);
		if(prop == 0) {
			GsTL_object_property_item* object_prop = dynamic_cast<GsTL_object_property_item*>(item);
			if(object_prop) prop = object_prop->property();
		}
		if(prop) {
			if(is_selected) {

				item_deselected.append(	QItemSelectionRange(index) );
				item_deselected.append(	QItemSelectionRange(index.parent()) ); // remove the group id

			}
			else {  // Need to check if another property was selected
				QModelIndexList selected_index_list = 	this->selectedIndexes();
				for(int i=0; i<selected_index_list.size(); ++i) {

					GsTL_object_item* item_in_selection = static_cast<GsTL_object_item*>(proxy_model_->mapToSource(selected_index_list.at(i)).internalPointer());
          //GsTL_object_item* item_in_selection = static_cast<GsTL_object_item*>(selected_index_list.at(i).internalPointer());
					Grid_continuous_property* prop_in_selection = dynamic_cast<Grid_continuous_property*>(item_in_selection);
					if(prop_in_selection == 0) {
						GsTL_object_property_item* object_prop_in_selection = dynamic_cast<GsTL_object_property_item*>(item_in_selection);
						if(object_prop_in_selection) prop_in_selection = object_prop_in_selection->property();
					}
					if(prop_in_selection == 0) continue;
					// Does it has the same grid parent
					if(item->parent()->parent()->item_data(0) != item_in_selection->parent()->parent()->item_data(0)) continue;

					item_deselected.append(	QItemSelectionRange(selected_index_list.at(i))  );
					item_deselected.append(	QItemSelectionRange(selected_index_list.at(i).parent())  );

				}
				item_selected.append(QItemSelectionRange(index) );
				item_selected.append(QItemSelectionRange(index.parent()) );  // group
				item_selected.append(QItemSelectionRange(index.parent().parent()) );  // grid
			}
			QItemSelectionModel::select(item_deselected,QItemSelectionModel::Deselect);
			QItemSelectionModel::select(item_selected,QItemSelectionModel::Select);

			selections_ = this->selection();
			return;
		}

		Grid_region* region = dynamic_cast<Grid_region*>(item);
		if(region) {
			if(is_selected) {

				item_deselected.append(	QItemSelectionRange(index) );
				item_deselected.append(	QItemSelectionRange(index.parent()) ); // remove the group

			}
			else {  // Need to check if another region was selected
				QModelIndexList selected_index_list = 	this->selectedIndexes();
				for(int i=0; i<selected_index_list.size(); ++i) {

					GsTL_object_item* item_in_selection = static_cast<GsTL_object_item*>(proxy_model_->mapToSource(selected_index_list.at(i)).internalPointer());
          //GsTL_object_item* item_in_selection = static_cast<GsTL_object_item*>(selected_index_list.at(i).internalPointer());
					Grid_region* region_in_selection = dynamic_cast<Grid_region*>(item_in_selection);
					if(region_in_selection == 0) continue;
					// Does it has the same grid parent
					if(item->parent()->parent()->item_data(0) != item_in_selection->parent()->parent()->item_data(0)) continue;

					item_deselected.append(	QItemSelectionRange(selected_index_list.at(i))  );
				}
				item_selected.append(QItemSelectionRange(index) );
				item_selected.append(QItemSelectionRange(index.parent()) );
			}
			QItemSelectionModel::select(item_deselected,QItemSelectionModel::Deselect);
			QItemSelectionModel::select(item_selected,QItemSelectionModel::Select);

			selections_ = this->selection();
			return;
		}


	}
}

void Project_selection_model::reset(){
	QItemSelectionModel::reset();
	QItemSelectionModel::select(selections_,QItemSelectionModel::Select);
	selections_ = this->selection();
}

