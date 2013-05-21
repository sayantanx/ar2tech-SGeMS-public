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
 * property_proxy_model.cpp
 *
 *  Created on: Sep 2, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/qtplugins/property_proxy_model.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/qtplugins/grid_proxy_model.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>


/*
 *   Property_proxy_model
 */

Property_proxy_model::Property_proxy_model(Geostat_grid* grid, QObject *parent)
    : QAbstractProxyModel(parent), grid_(grid) {
    root_ = dynamic_cast<Grid_property_manager*>( grid_->properties_item() );

    Root_model* model = dynamic_cast<Root_model*>(Root::instance()->model());
    this->setSourceModel(model);

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
    /*
 ok = QObject::connect(this->sourceModel(),SIGNAL(rowsRemoved(const QModelIndex& , int , int ) ),
      this, SLOT(end_remove_proxy_rows(const QModelIndex& , int , int)));
 Q_ASSERT(ok);
  */
}

Property_proxy_model::~Property_proxy_model() {
}

const Geostat_grid* Property_proxy_model::current_grid() const{
    return grid_;
}


int Property_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Qt::ItemFlags Property_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}



QModelIndex Property_proxy_model::index(int row, int column, const QModelIndex &parent)
const
{

    if( row < root_->childCount() ) {
        return createIndex(row, column, root_->child(row) );
    }
    else return QModelIndex();

}


int Property_proxy_model::rowCount(const QModelIndex &parent) const
{
    return root_->childCount();
}


QModelIndex Property_proxy_model::parent(const QModelIndex &child) const
{
    return QModelIndex();

}




QVariant
Property_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant Property_proxy_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
    	QIcon icon;
    	GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    	QString type = item->item_data(1).toString();
    	if(type == "CategoricalProperty") {
            icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cat_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "ContinuousProperty") {
            icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cont_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	return icon;
    }

    if( role == Qt::DisplayRole ) {
    	GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    	return item->item_data(index.column());
    }

    return QVariant();
}

QModelIndex	Property_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{

    GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
    if( dynamic_cast<GsTLGridProperty*>(item) == 0) return QModelIndex();
    return createIndex(sourceIndex.row(),sourceIndex.column(), sourceIndex.internalPointer());

}
QModelIndex	Property_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{

    QModelIndex parent_index = this->createIndex( root_->row(),0, static_cast<void*>(root_) );

    return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(), parent_index);

}


void Property_proxy_model::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;

    //	emit this->layoutAboutToBeChanged();
    this->beginInsertRows(QModelIndex(),start,end);
    //	this->beginInsertRows(parent,start,end);

}

void Property_proxy_model::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;
    //	emit this->layoutAboutToBeChanged();
    this->beginRemoveRows(QModelIndex(),start,end);
    this->endRemoveRows();
    /*
 QModelIndex parent = mapFromSource(source_parent);
 if(!parent.isValid()) return;
 emit this->layoutAboutToBeChanged();
 this->beginRemoveRows(parent,start,end);
*/
}


void Property_proxy_model::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;
    this->endInsertRows();
    //	emit this->layoutChanged();
    /*
 QModelIndex parent = mapFromSource(source_parent);
 if(!parent.isValid()) return;
 //this->beginInsertRows(parent, start,end);
 this->endInsertRows();
 emit this->layoutChanged();
 */

}

void Property_proxy_model::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;

    this->endRemoveRows();
    //	emit this->layoutChanged();
}


/*
 *   Property_proxy_model_no_selection
 */

Property_proxy_model_no_selection::Property_proxy_model_no_selection(Geostat_grid* grid, QObject *parent)
    : Property_proxy_model(grid,parent) {

    no_item_ = new No_selection_item();


}


Property_proxy_model_no_selection::~Property_proxy_model_no_selection()
{
    delete no_item_;

}


QModelIndex Property_proxy_model_no_selection::index(int row, int column, const QModelIndex &parent)
const
{
    if(row == 0) {
        return createIndex(row, column, no_item_ );
    }

    else if( row < root_->childCount()+1 ) {
        return createIndex(row, column, root_->child(row-1) );
    }
    else return QModelIndex();

}


int Property_proxy_model_no_selection::rowCount(const QModelIndex &parent) const
{
    return root_->childCount()+1;
}


QModelIndex	Property_proxy_model_no_selection::mapFromSource ( const QModelIndex & sourceIndex ) const{

    GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
    if( dynamic_cast<GsTLGridProperty*>(item) == 0) return QModelIndex();
    return createIndex(sourceIndex.row()+1,sourceIndex.column(), sourceIndex.internalPointer());

}
QModelIndex	Property_proxy_model_no_selection::mapToSource ( const QModelIndex & proxyIndex ) const{

    if(proxyIndex.row() == 0 ) return QModelIndex();

    QModelIndex parent_index = this->createIndex( root_->row(),0, static_cast<void*>(root_) );

    return this->sourceModel()->index(proxyIndex.row()-1, proxyIndex.column(), parent_index);

}


void Property_proxy_model_no_selection::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;

    //	emit this->layoutAboutToBeChanged();
    this->beginInsertRows(QModelIndex(),start+1,end+1);


}

void Property_proxy_model_no_selection::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;
    //	emit this->layoutAboutToBeChanged();
    this->beginRemoveRows(QModelIndex(),start+1,end+1);
    this->endRemoveRows();

}


void Property_proxy_model_no_selection::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;
    this->endInsertRows();
    //	emit this->layoutChanged();

}

void Property_proxy_model_no_selection::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
    GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
    if(item != root_) return;

    this->endRemoveRows();
    //	emit this->layoutChanged();
}

