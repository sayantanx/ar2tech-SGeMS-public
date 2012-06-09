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
 * weight_property_proxy_model.cpp
 *
 *  Created on: nov 19, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/extra/qtplugins/weight_property_proxy_model.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/extra/qtplugins/grid_proxy_model.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>


/*
 *   Property_proxy_model
 */

Weight_property_proxy_model::Weight_property_proxy_model( QObject *parent)
 : QAbstractProxyModel(parent), grid_(0), root_(0) {


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

void Weight_property_proxy_model::set_grid(const QString grid_name){
  Named_interface* ni = Root::instance()->interface(gridObject_manager+"/"+grid_name.toStdString()).raw_ptr();
  if(!ni) return;

  grid_ = dynamic_cast<Geostat_grid*>( ni );
  root_ = dynamic_cast<Grid_property_manager*>( grid_->properties_item() );

  weight_names_.clear();
  std::list<std::string> wprop_names =  grid_->weight_property_list();
  std::list<std::string>::const_iterator it =  wprop_names.begin();
  for( ;it != wprop_names.end(); ++it ) {
    weight_names_.append( grid_->property(*it)->name().c_str()  );
  }
  this->reset();
}

Weight_property_proxy_model::~Weight_property_proxy_model() {
}

const Geostat_grid* Weight_property_proxy_model::current_grid() const{
	return grid_;
}


int Weight_property_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Qt::ItemFlags Weight_property_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}



QModelIndex Weight_property_proxy_model::index(int row, int column, const QModelIndex &parent)
            const
{

	if( row < root_->childCount() ) {
		return createIndex(row, column, root_->child(row) );
	}
	else return QModelIndex();

}


int Weight_property_proxy_model::rowCount(const QModelIndex &parent) const
{
	return root_->childCount();
}


QModelIndex Weight_property_proxy_model::parent(const QModelIndex &child) const
{
	return QModelIndex();

}




QVariant
Weight_property_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant Weight_property_proxy_model::data(const QModelIndex &index, int role) const
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

QModelIndex	Weight_property_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{

	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<GsTLGridProperty*>(item) == 0) return QModelIndex();
	return createIndex(sourceIndex.row(),sourceIndex.column(), sourceIndex.internalPointer());

}
QModelIndex	Weight_property_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{

	QModelIndex parent_index = this->createIndex( root_->row(),0, static_cast<void*>(root_) );

	return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(), parent_index);

}


void Weight_property_proxy_model::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != root_) return;

//	emit this->layoutAboutToBeChanged();
	this->beginInsertRows(QModelIndex(),start,end);
//	this->beginInsertRows(parent,start,end);

}

void Weight_property_proxy_model::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
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


void Weight_property_proxy_model::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
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

void Weight_property_proxy_model::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != root_) return;

	this->endRemoveRows();
//	emit this->layoutChanged();
}


/*
 *   Property_proxy_model_no_selection
 */

Weight_property_proxy_model_no_selection::Weight_property_proxy_model_no_selection( QObject *parent)
 : Weight_property_proxy_model(parent) {

   weight_names_.append("<-None->");

	//no_item_ = new No_selection_item();


}

void Weight_property_proxy_model_no_selection::set_grid(const QString grid_name){
  Named_interface* ni = Root::instance()->interface(gridModels_manager+"/"+grid_name.toStdString()).raw_ptr();
  if(!ni) return;

  grid_ = dynamic_cast<Geostat_grid*>( ni );
  root_ = dynamic_cast<Grid_property_manager*>( grid_->properties_item() );

  weight_names_.clear();
  weight_names_.append("<-None->");
  std::list<std::string> wprop_names =  grid_->weight_property_list();
  std::list<std::string>::const_iterator it =  wprop_names.begin();
  for( ;it != wprop_names.end(); ++it ) {
    weight_names_.append( grid_->property(*it)->name().c_str()  );
  }
  this->reset();
}


Weight_property_proxy_model_no_selection::~Weight_property_proxy_model_no_selection()
{
	//delete no_item_;

}


QModelIndex Weight_property_proxy_model_no_selection::index(int row, int column, const QModelIndex &parent)
            const
{

  if(row < weight_names_.size())
    return createIndex(row, column, (void*)0 );
  else
    return QModelIndex();

}

QVariant Weight_property_proxy_model_no_selection::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
      if(index.row() > 0)
        return QIcon(":/icons/appli/Pixmaps/weight_property.svg");
      else
        return QVariant();
    }

    if( role == Qt::DisplayRole ) {
      return weight_names_[index.row()];
    }

    return QVariant();
}


int Weight_property_proxy_model_no_selection::rowCount(const QModelIndex &parent) const
{
	return weight_names_.size();
}



QModelIndex	Weight_property_proxy_model_no_selection::mapFromSource ( const QModelIndex & sourceIndex ) const{

  if(sourceIndex.column() != 0 ) return QModelIndex();

	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
  if(item->item_type() != "WeightProperty") return QModelIndex();
  GsTLGridWeightProperty* wprop =   dynamic_cast<GsTLGridWeightProperty*>(item);

  if(wprop->parent() != root_) return QModelIndex(); 

  int row = weight_names_.indexOf(sourceIndex.data().toString());

	return createIndex(row,0, (void*)0);

}
QModelIndex	Weight_property_proxy_model_no_selection::mapToSource ( const QModelIndex & proxyIndex ) const{

	if(proxyIndex.row() == 0 ) return QModelIndex();

  //Search for the 
  GsTLGridWeightProperty* wprop = grid_->weight_property(proxyIndex.data().toString().toStdString());

  Root_model* rmodel = dynamic_cast<Root_model*>(this->sourceModel());
  QModelIndex src_parent_index = rmodel->property_root_index(grid_->item_name());

	return this->sourceModel()->index(wprop->row(), proxyIndex.column(), src_parent_index);

}


void Weight_property_proxy_model_no_selection::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

  // Always add new term at the end
  // At this point there is no way to know if the insertion is a weight property or not.
  // the insertion must be done at the end_insert_proxy_rows function;

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != root_) return;

  //Find the row number in the proxy model
  QModelIndex src_index = this->sourceModel()->index(start,0,source_parent);
  QModelIndex proxy_index = this->mapFromSource(src_index);
  

	this->beginInsertRows(QModelIndex(),start+1,end+1);


}

void Weight_property_proxy_model_no_selection::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item_parent = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item_parent != root_) return;

  //Find the row number in the proxy model
  QModelIndex src_index = this->sourceModel()->index(start,0,source_parent);
  //Check if it is a weight property
  GsTL_object_item *item = static_cast<GsTL_object_item*>(src_index.internalPointer());
  if(item->item_type() != "WeightProperty") return;

  int row = weight_names_.indexOf(item->item_name());

// Add it at the end of the list
	this->beginInsertRows(QModelIndex(),row,row);
  weight_names_.removeAt(row);
	this->endInsertRows();

}


void Weight_property_proxy_model_no_selection::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item_parent = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item_parent != root_) return;

  //Find the row number in the proxy model
  QModelIndex src_index = this->sourceModel()->index(start,0,source_parent);
  //Check if it is a weight property
  GsTL_object_item *item = static_cast<GsTL_object_item*>(src_index.internalPointer());
  if(item->item_type() != "WeightProperty") return;

// Add it at the end of the list
	this->beginInsertRows(QModelIndex(),weight_names_.size(),weight_names_.size());
  weight_names_.append(item->item_name());
	this->endInsertRows();


}

void Weight_property_proxy_model_no_selection::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){


}

