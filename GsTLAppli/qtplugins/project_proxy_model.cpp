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


#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/qtplugins/project_proxy_model.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>

#include <iostream>
#include <QStringList>
#include <QMimeData>
#include <QIcon>



Filter_root_proxy_model::Filter_root_proxy_model(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    /*
  this->setFilterRegExp(QRegExp("^((?!Factory).)*$", Qt::CaseInsensitive,
                                             QRegExp::FixedString));

  this->setFilterKeyColumn(1);
  */
    Root_model* model = dynamic_cast<Root_model*>(Root::instance()->model());
    this->setSourceModel(model);
    this->setDynamicSortFilter(true);

}

int Filter_root_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Filter_root_proxy_model::~Filter_root_proxy_model(){}

bool Filter_root_proxy_model::filterAcceptsRow(int sourceRow,
                                               const QModelIndex &sourceParent) const
{
    QModelIndex index1 = this->sourceModel()->index(sourceRow, 1, sourceParent);

    return this->sourceModel()->data(index1).toString() != "Factory";

}


QModelIndex Filter_root_proxy_model::project_root_index()
{
    Manager* root = dynamic_cast<Manager*>(Root::instance()->interface(gridModels_manager).raw_ptr());

    QModelIndex root_index = this->index(0,0,QModelIndex());  //root
    QModelIndex object_index = this->index(root->parent()->row(),0,root_index);  //Object

    return this->index(root->row(),0,object_index);

}
/*
QModelIndex Filter_root_proxy_model::mds_root_index()
{
    Manager* root =
            dynamic_cast<Manager*>(Root::instance()->
                                   interface(mdsSpace_manager).raw_ptr());

    QModelIndex root_index = this->index(0,0,QModelIndex());  //root
    QModelIndex object_index = this->index(root->row(),0,root_index);  //Object

    return this->index(object_index.row(),0,root_index);
}

QModelIndex Filter_root_proxy_model::mdsSpace_root_index(QString space_name)
{
    QModelIndex mds_root = this->mds_root_index();
    int nrow = this->rowCount(mds_root);
    for (unsigned int i = 0; i < nrow; ++i)
    {
        QModelIndex index = this->index(i,0,this->mds_root_index());
        if (index.data() == space_name)
            return index;

    }

    return QModelIndex();
}

QModelIndex Filter_root_proxy_model::mds_metric_property_root_index(QString space_name)
{
    QModelIndex mdsSpaceIndex = this->mdsSpace_root_index(space_name);

    int nrow = this->rowCount(mdsSpaceIndex);
    for (unsigned int i = 0; i < nrow; ++i)
    {
        QModelIndex index = this->index(i,0,mdsSpaceIndex);
        if (index.data() == "MetricProperties")
            return index;

    }

    return QModelIndex();

}

QModelIndex Filter_root_proxy_model::mds_metric_response_root_index(QString space_name)
{
    QModelIndex mdsSpaceIndex = this->mdsSpace_root_index(space_name);

    int nrow = this->rowCount(mdsSpaceIndex);
    for (unsigned int i = 0; i < nrow; ++i)
    {
        QModelIndex index = this->index(i,0,mdsSpaceIndex);
        if (index.data() == "MetricResponses")
            return index;

    }

    return QModelIndex();

}
*/
QModelIndex Filter_root_proxy_model::grid_root_index(QString grid_name){
    QModelIndex project_root = this->project_root_index();
    int nrow = this->rowCount(project_root);
    for(int i=0; i<nrow; ++i) {
        QModelIndex index = this->index(i,0,project_root);
        if(index.data() == grid_name) return index;
    }
    return QModelIndex();
}

QModelIndex Filter_root_proxy_model::property_root_index(QString grid_name){
    QModelIndex grid_index = this->grid_root_index(grid_name);
    if( !grid_index.isValid() ) return QModelIndex();

    int nrow = this->rowCount(grid_index);
    for(int i=0; i<nrow; ++i) {
        QModelIndex index = this->index(i,0,grid_index);
        if(index.data() == "Properties") return index;
    }
    return QModelIndex();

}

QModelIndex Filter_root_proxy_model::region_root_index(QString grid_name){
    QModelIndex grid_index = this->grid_root_index(grid_name);
    if( !grid_index.isValid() ) return QModelIndex();

    int nrow = this->rowCount(grid_index);
    for(int i=0; i<nrow; ++i) {
        QModelIndex index = this->index(i,0,grid_index);
        if(index.data() == "Regions") return index;
    }
    return QModelIndex();
}

//QModelIndex Filter_root_proxy_model::response_root_index()
//{
//    Manager* root =
//            dynamic_cast<Manager*>(Root::instance()->
//                                   interface(responseData_manager).raw_ptr());

//    QModelIndex root_index = this->index(0,0,QModelIndex());  //root
//    QModelIndex object_index = this->index(root->row(),0,root_index);  //Object
//    QModelIndex metric_index = this->index(0,0,object_index);

//    return this->index(metric_index.row(),0,object_index);
//}

//QModelIndex Filter_root_proxy_model::response_property_root_index(
//    QString response_name)
//{
//    QModelIndex response_root = this->response_root_index();

//    int nrow = this->rowCount(response_root);

//    for (unsigned int i = 0; i < nrow; ++i)
//    {
//        QModelIndex index = this->index(i,0,this->response_root_index());
//        if (index.data() == response_name)
//            return index;
//    }

//    return QModelIndex();
//}

 QModelIndex Filter_root_proxy_model::manager_index(QString manager_path){
   QModelIndex src_index = dynamic_cast<Root_model*>(this->sourceModel())->manager_index(manager_path);
   return this->mapFromSource(src_index);
 }

Filter_root_proxy_checkable_model::Filter_root_proxy_checkable_model(QObject *parent)
    : Filter_root_proxy_model(parent)
{
}


QVariant Filter_root_proxy_checkable_model::data(const QModelIndex &index, int role) const
{

    if(role == Qt::DecorationRole ||  role == Qt::DisplayRole) {
        return Filter_root_proxy_model::data(index, role);
    }

    if(  role == Qt::CheckStateRole && index.column() == 0) {
        //    Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
        GsTL_object_item* item  = static_cast<GsTL_object_item*>(mapToSource(index).internalPointer());

        std::map<GsTL_object_item*, bool>::const_iterator it = checked_states_.find(item);
        if( it == checked_states_.end() ) return Qt::Unchecked;
        else return it->second ? Qt::Checked : Qt::Unchecked;

    }
    return QVariant();
}

bool Filter_root_proxy_checkable_model::setData ( const QModelIndex & index, const QVariant & value, int role ){

    if(!index.isValid()) return false;

    else if( role == Qt::CheckStateRole && index.column() == 0) {
        GsTL_object_item* item  = static_cast<GsTL_object_item*>(mapToSource(index).internalPointer());
        update_checked_items(index);
        emit item_checked(index);
        //checked_states_[item] = value.toBool();
        return true;
    }


    GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    return item->set_item_data(value, index.column());

}

Qt::ItemFlags Filter_root_proxy_checkable_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    //Cannot edit the name (column 0) but is checkable
    if( index.column() == 0 ) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable;
    }


}



void Filter_root_proxy_checkable_model::update_checked_items(const QModelIndex& index){

    if(!index.isValid()) return;

    QModelIndex src_index = this->mapToSource(index);

    //The selected index will already be updated by the setData function
    QList<QModelIndex> index_to_update;

    GsTL_object_item* item = static_cast<GsTL_object_item*>(this->mapToSource(index).internalPointer());

    std::map<GsTL_object_item*, bool>::iterator it = checked_states_.find(item);
    bool is_selected = it != checked_states_.end() && it->second;

    //Check if a grid was selected
    Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item);
    if(grid) {
        if(is_selected) {
            checked_states_[item] = false;
            currently_selected_.erase(item);
        }
        else {
            checked_states_[item] = true;
            currently_selected_.insert(currently_selected_.begin(), item);
        }
        return;
    }

    Grid_continuous_property* prop = dynamic_cast<Grid_continuous_property*>(item);
    if(prop == 0) {
        GsTL_object_property_item* object_prop = dynamic_cast<GsTL_object_property_item*>(item);
        if(object_prop) prop = object_prop->property();
    }
    if(prop) {
        QModelIndex grid_index = src_index.parent().parent();
        if(is_selected) {
            checked_states_[item] = false;
            currently_selected_.erase(item);
            checked_states_[item->parent()] = false;  //group
            currently_selected_.erase(item->parent());
        }
        else {  // Need to check if another property was selected
            std::set<GsTL_object_item*>::iterator it_selection = currently_selected_.begin();
            std::vector<std::set<GsTL_object_item*>::iterator> it_to_be_deleted;
            for( ; it_selection != currently_selected_.end(); ++it_selection) {

                Grid_continuous_property* prop_in_selection = dynamic_cast<Grid_continuous_property*>(*it_selection);
                if(prop_in_selection == 0) {
                    GsTL_object_property_item* object_prop_in_selection = dynamic_cast<GsTL_object_property_item*>(*it_selection);
                    if(object_prop_in_selection) prop_in_selection = object_prop_in_selection->property();
                }
                if(prop_in_selection == 0) continue;
                // Does it has the same grid parent
                if(item->parent()->parent()->item_data(0) != (*it_selection)->parent()->parent()->item_data(0)) continue;

                //Must be removed from the selection
                it_to_be_deleted.push_back(it_selection);
                //Find the correspondinf index to it_selection
                QModelIndex index_group = src_index.child((*it_selection)->parent()->row(), 0);
                QModelIndex index_prop = index_group.child((*it_selection)->row(), 0);
                index_to_update.append(index_group);
                index_to_update.append(index_prop);

            }
            for(int i=0; i<it_to_be_deleted.size(); ++i) {
                checked_states_[*it_to_be_deleted[i]] = false;
                currently_selected_.erase(*it_to_be_deleted[i]);
            }

            checked_states_[item] = true;
            currently_selected_.insert(currently_selected_.begin(), item);
            checked_states_[item->parent()] = true; // group
            currently_selected_.insert(currently_selected_.begin(), item->parent());
            checked_states_[item->parent()->parent()] = true; // grid
            currently_selected_.insert(currently_selected_.begin(), item->parent()->parent());

        }
        this->update_indexes(index_to_update);
        return;
    }

    Grid_region* region = dynamic_cast<Grid_region*>(item);
    if(region) {
        if(is_selected) {
            checked_states_[item] = false;
            currently_selected_.erase(item);
            checked_states_[item->parent()] = false;  //group
            currently_selected_.erase(item->parent());

        }
        else {  // Need to check if another region was selected
            std::set<GsTL_object_item*>::iterator it_selection = currently_selected_.begin();
            std::vector<std::set<GsTL_object_item*>::iterator> it_to_be_deleted;
            for( ; it_selection != currently_selected_.end(); ++it_selection) {

                Grid_region* region_in_selection = dynamic_cast<Grid_region*>(*it_selection);
                if(region_in_selection == 0) continue;
                // Does it has the same grid parent
                if(item->parent()->parent()->item_data(0) != (*it_selection)->parent()->parent()->item_data(0)) continue;

                it_to_be_deleted.push_back(it_selection);
                QModelIndex index_group = src_index.child((*it_selection)->parent()->row(), 0);
                QModelIndex index_prop = index_group.child((*it_selection)->row(), 0);
                index_to_update.append(index_group);
                index_to_update.append(index_prop);
            }
            for(int i=0; i<it_to_be_deleted.size(); ++i) {
                checked_states_[*it_to_be_deleted[i]] = false;
                currently_selected_.erase(*it_to_be_deleted[i]);
            }
            checked_states_[item] = true;
            currently_selected_.insert(currently_selected_.begin(), item);
            checked_states_[item->parent()] = true; // group
            currently_selected_.insert(currently_selected_.begin(), item->parent());
            checked_states_[item->parent()->parent()] = true; // grid
            currently_selected_.insert(currently_selected_.begin(), item->parent()->parent());
        }
        this->update_indexes(index_to_update);
        return;
    }

}


void Filter_root_proxy_checkable_model::update_indexes(const QList<QModelIndex>& src_indexes){

    for(int i=0; i<src_indexes.size(); ++i) {
        QModelIndex index =  this->mapFromSource( src_indexes.at(i)  );
        this->dataChanged(index, index );
    }

}

bool	Filter_root_proxy_checkable_model::removeRows ( int row, int count, const QModelIndex & parent ){

  for(int i=0; i<count; ++i, ++row) {
    QModelIndex index =  this->mapFromSource( parent.child(row,0) );
    GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    
    if( currently_selected_.find( item) == currently_selected_.end() ) continue;
    
    update_checked_items(index);
  }

  

  return QSortFilterProxyModel::removeRows ( row, count, parent );

}

void Filter_root_proxy_checkable_model::beginRemoveRows ( const QModelIndex & parent, int first, int last ){

  for(; first<last; ++first) {
    QModelIndex index =  this->mapFromSource( parent.child(first,0) );
    GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    
    if( currently_selected_.find( item) == currently_selected_.end() ) continue;
    
    update_checked_items(index);
  }

  

  QAbstractItemModel::beginRemoveRows ( parent, first, last );
}


/*
*      Visualization_proxy_model
*/

Visualization_proxy_model::Visualization_proxy_model(QObject *parent)
 : QAbstractProxyModel(parent),root_(0){
	root_ = dynamic_cast<Manager*>(Root::instance()->interface(gridModels_manager).raw_ptr());
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

	ok = QObject::connect(this->sourceModel(),SIGNAL(rowsRemoved(const QModelIndex& , int , int ) ),
					 this, SLOT(end_remove_proxy_rows(const QModelIndex& , int , int)));
	Q_ASSERT(ok);
  
}

Visualization_proxy_model::~Visualization_proxy_model() {
}


int Visualization_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Qt::ItemFlags Visualization_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable;
    


}



QModelIndex Visualization_proxy_model::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid()) {
      //if(root_->interface_count()==0) return QModelIndex();
      return createIndex(row, column,root_->child(row));
    }

    GsTL_object_item *parent_item = static_cast<GsTL_object_item*>(parent.internalPointer());

    if(parent_item == 0  ) return QModelIndex();

    GsTL_object_item *child_item = parent_item->child(row);
    if (child_item)
        return createIndex(row, column, child_item);
    else
        return QModelIndex();

}


int Visualization_proxy_model::rowCount(const QModelIndex &parent) const
{

    if (!parent.isValid()) {
   //   return root_->childCount();
   //   parent_item =  root_;
      return root_->interface_count();
    }

    if (parent.column() > 0)
        return 0;


    
    GsTL_object_item * parent_item = static_cast<GsTL_object_item*>(parent.internalPointer());
    if(parent_item == root_) {  //Need to remove the factory
    	return root_->interface_count();
    }
    return parent_item->childCount();
    
}


QModelIndex Visualization_proxy_model::parent(const QModelIndex &child) const
{

    if (!child.isValid())
        return QModelIndex();

    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child.internalPointer());

    if( dynamic_cast<Geostat_grid*>(child_item)) return QModelIndex();
    //if(child_item == 0 || child_item == root_) return QModelIndex();
    GsTL_object_item *parent_item = child_item->parent();
    if(parent_item == 0 ) return QModelIndex();

    return createIndex(parent_item->row(), 0, parent_item);

}


QVariant
Visualization_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant Visualization_proxy_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

   

  if(role == Qt::DecorationRole ||  role == Qt::DisplayRole) {
    return  this->sourceModel()->data(index,role);
  }

  if(  role == Qt::CheckStateRole && index.column() == 0) {
  //    Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
    GsTL_object_item* item  = static_cast<GsTL_object_item*>(mapToSource(index).internalPointer());
    
    std::map<GsTL_object_item*, bool>::const_iterator it = checked_states_.find(item);
    if( it == checked_states_.end() ) return Qt::Unchecked;
    else return it->second ? Qt::Checked : Qt::Unchecked;

  }
  return QVariant();

}


bool Visualization_proxy_model::setData ( const QModelIndex & index, const QVariant & value, int role ){

  if(!index.isValid()) return false;

  else if( role == Qt::CheckStateRole && index.column() == 0) {
    GsTL_object_item* item  = static_cast<GsTL_object_item*>(mapToSource(index).internalPointer());
    update_checked_items(index);
    emit item_checked(index);
    //checked_states_[item] = value.toBool();
    return true;
    emit dataChanged(index, index);
  }

  
  GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
  return item->set_item_data(value, index.column());

}

QModelIndex	Visualization_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{
	
	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());

  // Check if it is above the grid_manager level
	GsTL_object_item *item_search  =item;
	while( item_search != 0 ) {
		if(item_search->parent() == root_) {
	//		if(item == root_) return createIndex(sourceIndex.row(),sourceIndex.column(), root_);
			if(item->parent() == root_) {  // check if it is a factory (i.e. not a grid)
				if( dynamic_cast<Geostat_grid*>(item) == 0) return QModelIndex();
			}
			return createIndex(sourceIndex.row(),sourceIndex.column(), item);
		}
		item_search = item_search->parent();
	}
	return QModelIndex();
}

QModelIndex	Visualization_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{

  if(!proxyIndex.isValid()) return QModelIndex();

  QModelIndex parent_index = proxyIndex.parent();
  if(!parent_index.isValid()) {  // This is a grid (top level for that proxy model)
    QModelIndex src_parent = dynamic_cast<Root_model*>(this->sourceModel())->project_root_index();
    return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(),src_parent);
  }
  else  {
  	return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(),proxyIndex.parent());
  }

}


void Visualization_proxy_model::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

  GsTL_object_item* parent_item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
 
  if(parent_item == root_) {
    this->beginInsertRows(QModelIndex(),start,end);
  }
  else {
  	QModelIndex parent = mapFromSource(source_parent);
	  if(!parent.isValid()) return;
	  this->beginInsertRows(parent,start,end);
  }

}


void Visualization_proxy_model::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
  GsTL_object_item* parent_item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
  if(parent_item == root_)  {
    this->endInsertRows();
  }
  else {
	  QModelIndex parent = mapFromSource(source_parent);
	  if(!parent.isValid()) return;

	  this->endInsertRows();
  }


}


void Visualization_proxy_model::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){

  GsTL_object_item* parent_item = static_cast<GsTL_object_item*>(source_parent.internalPointer());



  bool item_removed = false;
  if(parent_item == root_) {
    this->beginRemoveRows(QModelIndex(),start,end);
    item_removed = true;
  }
  else {
	QModelIndex parent = mapFromSource(source_parent);
	  if(!parent.isValid()) {
      return;
    }
    this->beginRemoveRows(parent, start, end);
    item_removed = true;
  }

  if(item_removed) {
    for(; start<=end; ++start) {
      QModelIndex index =  this->mapFromSource( source_parent.child(start,0) );
      GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());

      this->remove_selections(item);
    
//      if( currently_selected_.find( item) == currently_selected_.end() ) continue;
    
//      update_checked_items(index);

      
    }
  }


}




void Visualization_proxy_model::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){

  GsTL_object_item* parent_item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
  if(parent_item == root_)  {
    this->endRemoveRows();
  }
  else {
	  QModelIndex parent = mapFromSource(source_parent);
	  if(!parent.isValid()) return;

	  this->endRemoveRows();
  }

//	QModelIndex parent = mapFromSource(source_parent);
//	if(!parent.isValid()) return;
//  if(removal_flag_)
//	  this->endRemoveRows();

}

void Visualization_proxy_model::remove_selections(const QModelIndex& index){
  GsTL_object_item* item = static_cast<GsTL_object_item*>(this->mapToSource(index).internalPointer());

  std::map<GsTL_object_item*, bool>::iterator it = checked_states_.find(item);
  bool is_selected = it != checked_states_.end() && it->second;

}

void Visualization_proxy_model::remove_selections(GsTL_object_item* item){

  currently_selected_.erase(item);
  checked_states_.erase(item);

  //Check for the children
  int nchild = item->childCount();
  for(int i=0; i<nchild; ++i) {
    GsTL_object_item* child_item = item->child(i);
    this->remove_selections(child_item);
  }

}


void Visualization_proxy_model::update_checked_items(const QModelIndex& index){

	if(!index.isValid()) return;

  QModelIndex src_index = this->mapToSource(index);

  //The selected index will already be updated by the setData function
  QList<QModelIndex> index_to_update;
  index_to_update.append(src_index);
  
  GsTL_object_item* item = static_cast<GsTL_object_item*>(this->mapToSource(index).internalPointer());

  std::map<GsTL_object_item*, bool>::iterator it = checked_states_.find(item);
  bool is_selected = it != checked_states_.end() && it->second;

	//Check if a grid was selected
	Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item);
	if(grid) {
    if(is_selected) {
      checked_states_[item] = false;
      currently_selected_.erase(item);
    }
    else {
      checked_states_[item] = true;
      currently_selected_.insert(currently_selected_.begin(), item);
    }
		return;
	}

	Grid_continuous_property* prop = dynamic_cast<Grid_continuous_property*>(item);
	if(prop == 0) {
		GsTL_object_property_item* object_prop = dynamic_cast<GsTL_object_property_item*>(item);
		if(object_prop) prop = object_prop->property();
	}
	if(prop) {
    QModelIndex grid_index = src_index.parent().parent();
		if(is_selected) {
      checked_states_[item] = false;
      currently_selected_.erase(item);
      checked_states_[item->parent()] = false;  //group
      currently_selected_.erase(item->parent());
		}
		else {  // Need to check if another property was selected
      std::set<GsTL_object_item*>::iterator it_selection = currently_selected_.begin();
      std::vector<std::set<GsTL_object_item*>::iterator> it_to_be_deleted;
			for( ; it_selection != currently_selected_.end(); ++it_selection) {

				Grid_continuous_property* prop_in_selection = dynamic_cast<Grid_continuous_property*>(*it_selection);
				if(prop_in_selection == 0) {
					GsTL_object_property_item* object_prop_in_selection = dynamic_cast<GsTL_object_property_item*>(*it_selection);
					if(object_prop_in_selection) prop_in_selection = object_prop_in_selection->property();
				}
				if(prop_in_selection == 0) continue;
				// Does it has the same grid parent
				if(item->parent()->parent()->item_data(0) != (*it_selection)->parent()->parent()->item_data(0)) continue;

        //Must be removed from the selection
        it_to_be_deleted.push_back(it_selection);
        //Find the correspondinf index to it_selection
        QModelIndex index_group = src_index.child((*it_selection)->parent()->row(), 0);
        QModelIndex index_prop = index_group.child((*it_selection)->row(), 0);
        index_to_update.append(index_group);
        index_to_update.append(index_prop);




			}
      for(int i=0; i<it_to_be_deleted.size(); ++i) {
        checked_states_[*it_to_be_deleted[i]] = false;
        currently_selected_.erase(*it_to_be_deleted[i]);
      }

      checked_states_[item] = true; 
      currently_selected_.insert(currently_selected_.begin(), item);
      checked_states_[item->parent()] = true; // group
      currently_selected_.insert(currently_selected_.begin(), item->parent());
      checked_states_[item->parent()->parent()] = true; // grid
      currently_selected_.insert(currently_selected_.begin(), item->parent()->parent());

		}
    this->update_indexes(index_to_update);
		return;
	}

	Grid_region* region = dynamic_cast<Grid_region*>(item);
	if(region) {
		if(is_selected) {
      checked_states_[item] = false;
      currently_selected_.erase(item);
      checked_states_[item->parent()] = false;  //group
      currently_selected_.erase(item->parent());

		}
		else {  // Need to check if another region was selected
      std::set<GsTL_object_item*>::iterator it_selection = currently_selected_.begin();
      std::vector<std::set<GsTL_object_item*>::iterator> it_to_be_deleted;
			for( ; it_selection != currently_selected_.end(); ++it_selection) {

				Grid_region* region_in_selection = dynamic_cast<Grid_region*>(*it_selection);
				if(region_in_selection == 0) continue;
				// Does it has the same grid parent
				if(item->parent()->parent()->item_data(0) != (*it_selection)->parent()->parent()->item_data(0)) continue;

        it_to_be_deleted.push_back(it_selection);
        QModelIndex index_group = src_index.child((*it_selection)->parent()->row(), 0);
        QModelIndex index_prop = index_group.child((*it_selection)->row(), 0);
        index_to_update.append(index_group);
        index_to_update.append(index_prop);
			}
      for(int i=0; i<it_to_be_deleted.size(); ++i) {
        checked_states_[*it_to_be_deleted[i]] = false;
        currently_selected_.erase(*it_to_be_deleted[i]);
      }
      checked_states_[item] = true; 
      currently_selected_.insert(currently_selected_.begin(), item);
      checked_states_[item->parent()] = true; // group
      currently_selected_.insert(currently_selected_.begin(), item->parent());
      checked_states_[item->parent()->parent()] = true; // grid
      currently_selected_.insert(currently_selected_.begin(), item->parent()->parent());
		}
    this->update_indexes(index_to_update);
		return;
	}

}


void Visualization_proxy_model::update_indexes(const QList<QModelIndex>& src_indexes){

  for(int i=0; i<src_indexes.size(); ++i) {
    QModelIndex index =  this->mapFromSource( src_indexes.at(i)  );
    this->dataChanged(index, index );
  }

}
