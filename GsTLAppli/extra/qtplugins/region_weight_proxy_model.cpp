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

#include <GsTLAppli/extra/qtplugins/region_weight_proxy_model.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>
#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/extra/qtplugins/grid_proxy_model.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>


/*
 *   Region_weight_proxy_model
 */

Region_weight_proxy_model::Region_weight_proxy_model(QString grid_name,QObject *parent)
 : QAbstractProxyModel(parent) {
   grid_ = dynamic_cast<Geostat_grid*>(Root::instance()->interface(gridModels_manager+"/"+grid_name.toStdString()).raw_ptr());

   if(grid_) {
     std::list<std::string> regions = grid_->region_list();
     for(std::list<std::string>::const_iterator it = regions.begin(); it != regions.end();  ++it) 
       region_names_.append(it->c_str());
     
     std::list<std::string> weights = grid_->weight_property_list();
     for(std::list<std::string>::const_iterator it = weights.begin(); it != weights.end();  ++it) 
       weight_names_.append(it->c_str());
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

Region_weight_proxy_model::~Region_weight_proxy_model() {
}


int Region_weight_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Qt::ItemFlags Region_weight_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}



QModelIndex Region_weight_proxy_model::index(int row, int column, const QModelIndex &parent)
            const
{
	/*
    if (!hasIndex(row, column, parent) )
        return QModelIndex();
*/
  if( row < region_names_.size() + weight_names_.size() ) {
		return createIndex(row, column, 0 );
	}
	else return QModelIndex();

}


int Region_weight_proxy_model::rowCount(const QModelIndex &parent) const
{
	return region_names_.size() + weight_names_.size();
}


QModelIndex Region_weight_proxy_model::parent(const QModelIndex &child) const
{
	return QModelIndex();

}




QVariant
Region_weight_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant Region_weight_proxy_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
    	QIcon icon;
      if( index.row() < region_names_.size()  ) { // this is a region
        icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/region.svg"), QSize(), QIcon::Normal, QIcon::Off);
      }
      else {  // This is a weight property
        icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/weight.svg"), QSize(), QIcon::Normal, QIcon::Off);
      }
    	return icon;
    }

    if( role == Qt::DisplayRole ) {
      int row  = index.row();
      if(index.column() == 0) return  row < region_names_.size() ? region_names_.at(row) : weight_names_.at(row -  region_names_.size());
    }

    return QVariant();
}

QModelIndex	Region_weight_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{

	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<GsTLGridRegion*>(item) ) {
    return createIndex(sourceIndex.row(),sourceIndex.column(), 0);
  }
  else if( dynamic_cast<GsTLGridWeightProperty*>(item) ) {
    int i = weight_names_.indexOf(item->item_name());
    return createIndex(i + region_names_.size(),sourceIndex.column(), 0);

  }
  return QModelIndex();
}

QModelIndex	Region_weight_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{
  /*
	GsTL_object_item *item = static_cast<GsTL_object_item*>(proxyIndex.internalPointer());
	if( dynamic_cast<GsTLGridRegion*>(item) ) {
    return createIndex(proxyIndex.row(),sourceIndex.column(), 0);
  }
  else if( dynamic_cast<GsTLGridWeightProperty*>(item) ) {
    int i = weight_names_.indexOf(item->item_name());
    return createIndex(i + region_names_.size(),proxyIndex.column(), 0);
  }
  */
  return QModelIndex();

}


void Region_weight_proxy_model::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());

  // Check to see if the source parent is the property manager or the region manager
  if(item->parent() != grid_) return;
  
  if(item->item_name() == "Properties" ) { //The start end may not be continuous, so far we only delete them one at a time, potentially dangerous
    QModelIndex child_index = source_parent.child(start,0);
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child_index.internalPointer());
    //int i =  weight_names_.indexOf(child_item->item_name());
    int p_start = region_names_.size() + weight_names_.size();
    this->beginInsertRows(QModelIndex(),p_start,p_start + start - end);
    for(int i = start ; i <= end ; ++i  ) {
      weight_names_.append(child_item->item_name());
    }
  }
  else if(item->item_name() == "Regions" ) {
    QModelIndex child_index = source_parent.child(start,0);
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child_index.internalPointer());
    this->beginInsertRows(QModelIndex(),start,end);
    for(int i = start ; i <= end ; ++i  ) {
      region_names_.insert(i, child_item->item_name());
    }
  }

}

void Region_weight_proxy_model::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());

  // Check to see if the source parent is the property manager or the region manager
  if(item->parent() != grid_) return;
  
  if(item->item_name() == "Properties" ) { //The start end may not be continuous, so far we only delete them one at a time, potentially dangerous
    QModelIndex child_index = source_parent.child(start,0);
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child_index.internalPointer());
    int i =  weight_names_.indexOf(child_item->item_name());
    this->beginRemoveRows(QModelIndex(),region_names_.size()+i,region_names_.size() + i + start - end);
    for(int j=i ; j<= i + start - end ; ++j  ) {
      weight_names_.removeAt(j);
    }
  }
  else if(item->item_name() == "Regions" ) {
    this->beginRemoveRows(QModelIndex(),start,end);
    for(int j=start ; j<= end ; ++j  ) {
      region_names_.removeAt(j);
    }
  }

}


void Region_weight_proxy_model::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

/*


	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

  //Add the grid into the list
  for(int i = start; i<= end; ++i) {
    item = static_cast<GsTL_object_item*>(source_parent.child(i,0).internalPointer());
    grid_names_.insert(i,item->item_name());
    grid_types_.insert(i,item->item_type());
  }
  */
	this->endInsertRows();
//	emit this->layoutChanged();


}

void Region_weight_proxy_model::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
  /*
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != grid_manager_) return;

  //remove the grid from the list
  for(int i = end; i>= start; --i) {
//    item = static_cast<GsTL_object_item*>(source_parent.child(i,0).internalPointer());
    grid_names_.removeAt(i);
    grid_types_.removeAt(i);
  }
  */
	this->endRemoveRows();
//	emit this->layoutChanged();
}


/*
 *   Grid_proxy_model_no_selection
 */

Region_weight_proxy_model_no_selection::Region_weight_proxy_model_no_selection(QString grid_name, QObject *parent)
 : Region_weight_proxy_model(grid_name,parent) {

    region_names_.prepend("<- None ->" );

}


Region_weight_proxy_model_no_selection::~Region_weight_proxy_model_no_selection()
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

QModelIndex	Region_weight_proxy_model_no_selection::mapFromSource ( const QModelIndex & sourceIndex ) const{

	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<GsTLGridRegion*>(item) ) {
    return createIndex(sourceIndex.row()+1,sourceIndex.column(), 0);
  }
  else if( dynamic_cast<GsTLGridWeightProperty*>(item) ) {
    int i = weight_names_.indexOf(item->item_name());
    return createIndex(i + region_names_.size(),sourceIndex.column(), 0);  //same here since region_names_ contains the no selection

  }
  return QModelIndex();



/*
	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<Geostat_grid*>(item) == 0) return QModelIndex();
	return createIndex(sourceIndex.row()+1,sourceIndex.column(), sourceIndex.internalPointer());
*/
  /*
	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
	if( dynamic_cast<Geostat_grid*>(item) == 0) return QModelIndex();
  if(sourceIndex.row() > grid_names_.size()-1) return QModelIndex();
	return createIndex(sourceIndex.row()+1,sourceIndex.column(), 0);
  */
}


QModelIndex	Region_weight_proxy_model_no_selection::mapToSource ( const QModelIndex & proxyIndex ) const{
  return QModelIndex();
  /*
  if(proxyIndex.row() == 0 ) return QModelIndex();
  return this->sourceModel()->index(proxyIndex.row()-1, proxyIndex.column(), model_->project_root_index() );
  */

}


void Region_weight_proxy_model_no_selection::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());

  // Check to see if the source parent is the property manager or the region manager
  if(item->parent() != grid_) return;
  
  if(item->item_name() == "Properties" ) { //The start end may not be continuous, so far we only delete them one at a time, potentially dangerous
    QModelIndex child_index = source_parent.child(start,0);
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child_index.internalPointer());
    //int i =  weight_names_.indexOf(child_item->item_name());
    int p_start = region_names_.size() + weight_names_.size();
    this->beginInsertRows(QModelIndex(),p_start,p_start + start - end);
    for(int i = start ; i <= end ; ++i  ) {
      weight_names_.append(child_item->item_name());
    }
  }
  else if(item->item_name() == "Regions" ) {
    QModelIndex child_index = source_parent.child(start,0);
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child_index.internalPointer());
    this->beginInsertRows(QModelIndex(),start,end);
    for(int i = start ; i <= end ; ++i  ) {
      region_names_.insert(i+1, child_item->item_name());
    }
  }

}

void Region_weight_proxy_model_no_selection::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());

  // Check to see if the source parent is the property manager or the region manager
  if(item->parent() != grid_) return;
  
  if(item->item_name() == "Properties" ) { //The start end may not be continuous, so far we only delete them one at a time, potentially dangerous
    QModelIndex child_index = source_parent.child(start,0);
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child_index.internalPointer());
    int i =  weight_names_.indexOf(child_item->item_name());
    this->beginRemoveRows(QModelIndex(),region_names_.size()+i,region_names_.size() + i + start - end);
    for(int j=i ; j<= i + start - end ; ++j  ) {
      weight_names_.removeAt(j);
    }
  }
  else if(item->item_name() == "Regions" ) {
    this->beginRemoveRows(QModelIndex(),start,end);
    for(int j=start ; j<= end ; ++j  ) {
      region_names_.removeAt(j+1);
    }
  }

}


void Region_weight_proxy_model_no_selection::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){


	this->endInsertRows();


}

void Region_weight_proxy_model_no_selection::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){

	this->endRemoveRows();
//	emit this->layoutChanged();
}






