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
 * project_proxy_model.cpp
 *
 *  Created on: Jun 8, 2011
 *      Author: aboucher
 */


#include <appli/manager_repository.h>
#include <grid/grid_model/geostat_grid.h>
#include <qtplugins/grid_proxy_model.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>

No_selection_item::No_selection_item()
	: GsTL_object_item() {
	item_name_ = "<- None ->";
    item_type_ = "";
}
No_selection_item::~No_selection_item(){}


/*
 *   Grid_proxy_model
 */



/*
 *   Grid_proxy_model
 */

Grid_proxy_model::Grid_proxy_model(QObject *parent)
 : QAbstractProxyModel(parent) {
	grid_manager_ = dynamic_cast<Manager*>(Root::instance()->interface(gridModels_manager).raw_ptr());
  if(grid_manager_ == 0) return;
  
//  QModelIndex root_index = model->project_root_index();
//  int ngrid = 

  // Get all the existing grid from the manager
  Manager::interface_iterator it = grid_manager_->begin_interfaces();
  for( ; it != grid_manager_->end_interfaces(); ++it ) {
    grid_names_.append((*it)->item_name() );
    grid_types_.append((*it)->item_type() );
  }
 
	model_ = dynamic_cast<Root_model*>(Root::instance()->model());
	this->setSourceModel(model_);

	bool ok;

	ok = QObject::connect(this->sourceModel(),SIGNAL(rowsAboutToBeInserted(const QModelIndex& , int , int ) ),
					 this, SLOT(begin_insert_proxy_rows(const QModelIndex& , int , int)));
	Q_ASSERT(ok);

	ok = QObject::connect(this->sourceModel(),SIGNAL(rowsAboutToBeRemoved(const QModelIndex& , int , int ) ),
					 this, SLOT(begin_remove_proxy_rows(const QModelIndex& , int , int)));
	Q_ASSERT(ok);

	ok = QObject::connect(this->sourceModel(),SIGNAL(rowsInserted(const QModelIndex& , int , int ) ),
					 this, SLOT(end_insert_proxy_rows(const QModelIndex& , int , int)));
	Q_ASSERT(ok);

	ok = QObject::connect(this->sourceModel(),SIGNAL(rowsRemoved(const QModelIndex& , int , int ) ),
					 this, SLOT(end_remove_proxy_rows(const QModelIndex& , int , int)));
	Q_ASSERT(ok);

}

Grid_proxy_model::~Grid_proxy_model() {
}


int Grid_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Qt::ItemFlags Grid_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}



QModelIndex Grid_proxy_model::index(int row, int column, const QModelIndex &parent)
            const
{
	/*
    if (!hasIndex(row, column, parent) )
        return QModelIndex();
*/
  if( row < grid_names_.size() ) {
		return createIndex(row, column, 0 );
	}
	else return QModelIndex();

}


int Grid_proxy_model::rowCount(const QModelIndex &parent) const
{
	return grid_names_.size();
}


QModelIndex Grid_proxy_model::parent(const QModelIndex &child) const
{
	return QModelIndex();

}




QVariant
Grid_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Type");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QVariant Grid_proxy_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
    	QIcon icon;
      QString type = grid_types_.at(index.row());
    	if(type == "Cgrid") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cgrid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Point_set") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/pset.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Masked_grid") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/mgrid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Log_data_grid") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/log_grid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	return icon;
    }

    if( role == Qt::DisplayRole ) {
      if(index.column() == 0) return  grid_names_.at(index.row());
      else if(index.column() == 1) return  grid_types_.at(index.row());
    }

    return QVariant();
}

QModelIndex	Grid_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{

	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<Geostat_grid*>(item) == 0) return QModelIndex();
  if(sourceIndex.row() > grid_names_.size()) return QModelIndex();
	return createIndex(sourceIndex.row(),sourceIndex.column(), 0);

}
QModelIndex	Grid_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{

  return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(), model_->project_root_index() );

}


void Grid_proxy_model::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

//	emit this->layoutAboutToBeChanged();
	this->beginInsertRows(QModelIndex(),start,end);
//	this->beginInsertRows(parent,start,end);

}

void Grid_proxy_model::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;
//	emit this->layoutAboutToBeChanged();
	this->beginRemoveRows(QModelIndex(),start,end);
//  this->endRemoveRows();

}


void Grid_proxy_model::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

  //Add the grid into the list
  for(int i = start; i<= end; ++i) {
    item = static_cast<GsTL_object_item*>(source_parent.child(i,0).internalPointer());
    grid_names_.insert(i,item->item_name());
    grid_types_.insert(i,item->item_type());
  }

	this->endInsertRows();
//	emit this->layoutChanged();


}

void Grid_proxy_model::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

  //remove the grid from the list
  for(int i = end; i>= start; --i) {
//    item = static_cast<GsTL_object_item*>(source_parent.child(i,0).internalPointer());
    grid_names_.removeAt(i);
    grid_types_.removeAt(i);
  }

	this->endRemoveRows();
//	emit this->layoutChanged();
}


/*
 *   Grid_proxy_model_no_selection
 */

Grid_proxy_model_no_selection::Grid_proxy_model_no_selection(QObject *parent)
 : Grid_proxy_model(parent) {

    grid_names_.prepend("<- None ->" );
    grid_types_.prepend("");


}


Grid_proxy_model_no_selection::~Grid_proxy_model_no_selection()
{

}

/*
QModelIndex Grid_proxy_model_no_selection::index(int row, int column, const QModelIndex &parent)
            const
{
	if(row == 0) {
		return createIndex(row, column, 0 );
	}

	else if( row < root_->childCount()+1 ) {
		return createIndex(row, column, root_->child(row-1) );
	}
	else return QModelIndex();

}
*/
/*
int Grid_proxy_model_no_selection::rowCount(const QModelIndex &parent) const
{
	return root_->interface_count()+1;
}
*/

QModelIndex	Grid_proxy_model_no_selection::mapFromSource ( const QModelIndex & sourceIndex ) const{
/*
	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<Geostat_grid*>(item) == 0) return QModelIndex();
	return createIndex(sourceIndex.row()+1,sourceIndex.column(), sourceIndex.internalPointer());
*/
	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<Geostat_grid*>(item) == 0) return QModelIndex();
  if(sourceIndex.row() > grid_names_.size()-1) return QModelIndex();
	return createIndex(sourceIndex.row()+1,sourceIndex.column(), 0);

}


QModelIndex	Grid_proxy_model_no_selection::mapToSource ( const QModelIndex & proxyIndex ) const{

  if(proxyIndex.row() == 0 ) return QModelIndex();
  return this->sourceModel()->index(proxyIndex.row()-1, proxyIndex.column(), model_->project_root_index() );


}


void Grid_proxy_model_no_selection::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

//	QModelIndex parent = mapFromSource(source_parent);
//	if(!parent.isValid()) return;
//	emit this->layoutAboutToBeChanged();
	this->beginInsertRows(QModelIndex(),start+1,end+1);
//	this->beginInsertRows(parent,start,end);

}

void Grid_proxy_model_no_selection::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;
//	emit this->layoutAboutToBeChanged();
	this->beginRemoveRows(QModelIndex(),start+1,end+1);

}


void Grid_proxy_model_no_selection::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

  //Add the grid into the list
  // Check if an item that is currently selected is removed
  // set the selection to None in that case

  for(int i = start; i<= end; ++i) {
    item = static_cast<GsTL_object_item*>(source_parent.child(i,0).internalPointer());
    grid_names_.insert(i+1,item->item_name());
    grid_types_.insert(i+1,item->item_type());
  }

	this->endInsertRows();


}

void Grid_proxy_model_no_selection::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

  //remove the grid from the list
  for(int i = end; i>= start; --i) {
//    item = static_cast<GsTL_object_item*>(source_parent.child(i,0).internalPointer());
    grid_names_.removeAt(i+1);
    grid_types_.removeAt(i+1);
  }

	this->endRemoveRows();
//	emit this->layoutChanged();
}





