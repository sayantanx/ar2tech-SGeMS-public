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
 * gstl_item_selection_model.cpp
 *
 *  Created on: May 31, 2011
 *      Author: aboucher
 */


#include <GsTLAppli/gui/models/gstl_item_selection_model.h>



GsTL_ItemSelectionModel::GsTL_ItemSelectionModel(Project_ItemModel * model)
 : QItemSelectionModel(model){

}

GsTL_ItemSelectionModel::GsTL_ItemSelectionModel(Project_ItemModel * model, QObject* parent)
: QItemSelectionModel(model, parent) {

}


GsTL_ItemSelectionModel::~GsTL_ItemSelectionModel() {
	// TODO Auto-generated destructor stub
}


void GsTL_ItemSelectionModel::select ( const QModelIndex & index, QItemSelectionModel::SelectionFlags command ){
	if(index.isValid()) {
		this->select(QItemSelection(index,index),command);
	}




}

void GsTL_ItemSelectionModel::select ( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command ){
//	/We only care for the first selected item
	QModelIndexList index_list = selection.indexes();
	for( int i=0; i<index_list.size() ; ++i) {
		QModelIndex index = index_list.at(i);
		if(!index.isValid()) continue;
	//	if (command != QItemSelectionModel::Toggle) return;

		bool is_selected = this->isSelected(index);

		QItemSelection item_selected;
		QItemSelection item_deselected;

		GsTL_item* item = static_cast<GsTL_item*>(index.internalPointer());
		//Check if a grid was selected
		GsTL_grid_item* grid_item = dynamic_cast<GsTL_grid_item*>(item);
		if(grid_item) {
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

		GsTL_property_item* prop_item = dynamic_cast<GsTL_property_item*>(item);
		if(prop_item) {
			if(is_selected) {

				item_deselected.append(	QItemSelectionRange(index) );
				item_deselected.append(	QItemSelectionRange(index.parent()) ); // remove the group id

			}
			else {  // Need to check if another property was selected
				QModelIndexList selected_index_list = 	this->selectedIndexes();
				for(int i=0; i<selected_index_list.size(); ++i) {

					GsTL_item* item_in_selection = static_cast<GsTL_item*>(selected_index_list.at(i).internalPointer());
					GsTL_property_item* prop_item_in_selection = dynamic_cast<GsTL_property_item*>(item_in_selection);
					if(prop_item_in_selection == 0) continue;
					// Does it has the same grid parent
					if(prop_item->gridName() != prop_item_in_selection->gridName()) continue;

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

		GsTL_region_item* region_item = dynamic_cast<GsTL_region_item*>(item);
		if(region_item) {
			if(is_selected) {

				item_deselected.append(	QItemSelectionRange(index) );
				item_deselected.append(	QItemSelectionRange(index.parent()) ); // remove the group

			}
			else {  // Need to check if another region was selected
				QModelIndexList selected_index_list = 	this->selectedIndexes();
				for(int i=0; i<selected_index_list.size(); ++i) {

					GsTL_item* item_in_selection = static_cast<GsTL_item*>(selected_index_list.at(i).internalPointer());
					GsTL_region_item* region_item_in_selection = dynamic_cast<GsTL_region_item*>(item_in_selection);
					if(region_item_in_selection == 0) continue;
					// Does it has the same grid parent
					if(region_item->gridName() != region_item_in_selection->gridName()) continue;

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

void GsTL_ItemSelectionModel::reset(){
	QItemSelectionModel::reset();
	QItemSelectionModel::select(selections_,QItemSelectionModel::Select);
	selections_ = this->selection();
}
