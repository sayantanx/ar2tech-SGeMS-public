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




#include <charts/categorical_histogram_proxy_model.h>
#include <appli/manager_repository.h>
#include <grid/grid_model/geostat_grid.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>
#include <QtAlgorithms>


Categorical_histogram_proxy_model::Categorical_histogram_proxy_model(QObject *parent)
 : QAbstractItemModel(parent), current_id_(0) {
	grid_manager_ = dynamic_cast<Manager*>(Root::instance()->interface(gridModels_manager).raw_ptr());
  
	model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//	this->setSourceModel(model_);
  this->set_connections();

}


Categorical_histogram_proxy_model::Categorical_histogram_proxy_model(QList< GsTL_object_item*> items, QObject *parent)
 : QAbstractItemModel(parent),current_id_(0) {
	grid_manager_ = dynamic_cast<Manager*>(Root::instance()->interface(gridModels_manager).raw_ptr());


  for(int i=0; i< items.size(); ++i) {

    Grid_categorical_property* prop = dynamic_cast< Grid_categorical_property*>(items.at(i));
    if(prop) {
      Categorical_histogram_property_item* prop_item = new Categorical_histogram_property_item(prop, current_id_);
      current_id_++;
      items_.insert(items_.begin(), prop_item);
   //   lookup_items_.insert( std::make_pair(prop, prop_item) );
      continue;
    }

    GsTLGridPropertyGroup* group = dynamic_cast< GsTLGridPropertyGroup*>(items.at(i));
    if(group) {
      Categorical_histogram_group_item* group_item  = new Categorical_histogram_group_item(group, current_id_);
      current_id_++;
      items_.insert(items_.begin(), group_item);
  //    lookup_items_.insert( std::make_pair( group, group_item ) );
      continue;
    }

  }
 
	model_ = dynamic_cast<Root_model*>(Root::instance()->model());
//	this->setSourceModel(model_);
  this->set_connections();

}

Categorical_histogram_proxy_model::~Categorical_histogram_proxy_model() {

  std::set< Categorical_histogram_item*>::iterator it = items_.begin();
  for( ; it != items_.end(); ++it ) {
    delete *it;
  }


}


void Categorical_histogram_proxy_model::set_connections(){
  	bool ok;


	ok = QObject::connect(model_,SIGNAL(rowsAboutToBeRemoved(const QModelIndex& , int , int ) ),
					 this, SLOT(begin_remove_proxy_rows(const QModelIndex& , int , int)));
	Q_ASSERT(ok);


	ok = QObject::connect(model_,SIGNAL(rowsRemoved(const QModelIndex& , int , int ) ),
					 this, SLOT(end_remove_proxy_rows(const QModelIndex& , int , int)));
	Q_ASSERT(ok);
}


int Categorical_histogram_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 3;
}

Qt::ItemFlags Categorical_histogram_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    //Cannot edit the name (column 0) but is checkable
    if( index.column() == 0 ) {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable;
    }
    else {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled  | Qt::ItemIsEditable;
    }

}
/*
bool Categorical_histogram_proxy_model::insert_row(Grid_categorical_property* prop, Grid_region* region, Grid_categorical_property* weights, QColor color){
  std::map<GsTL_object_item*,Categorical_histogram_item*>::iterator it =  lookup_items_.find(prop);
  if( it != lookup_items_.end() ) return false;

  Categorical_histogram_item* item = new Categorical_histogram_property_item(prop);
  item->weights(weights);
  item->region(region);
  item->color(color);

  std::set<Categorical_histogram_item*>::iterator it_insert = std::lower_bound(items_.begin(), items_.end(),item);
  unsigned int row = std::distance(items_.begin(), it_insert);

  this->beginInsertRows(QModelIndex() ,row,row );  //All the insertion are at the top
  items_.insert(it_insert,item);
  lookup_items_[prop] = item;
  this->endInsertRows();

  emit this->data_added(item);

  return true;

}

bool Categorical_histogram_proxy_model::insert_row(GsTLGridPropertyGroup* group, Grid_region* region, Grid_categorical_property* weights, QColor color){
  std::map<GsTL_object_item*,Categorical_histogram_item*>::iterator it =  lookup_items_.find(group);
  if( it != lookup_items_.end() ) return false;

  Categorical_histogram_item* item = new Categorical_histogram_group_item(group);
  item->weights(weights);
  item->region(region);
  item->color(color);

  std::set<Categorical_histogram_item*>::iterator it_insert = std::lower_bound(items_.begin(), items_.end(),item);
  unsigned int row = std::distance(items_.begin(), it_insert);

  this->beginInsertRows(QModelIndex() ,row,row );  //All the insertion are at the top
  items_.insert(it_insert,item);
  lookup_items_[group] = item;
  this->endInsertRows();

  emit this->data_added(item);

  return true;

}
*/

bool Categorical_histogram_proxy_model::insert_row(Categorical_histogram_item* item){
  std::set<Categorical_histogram_item*>::iterator it_insert = std::lower_bound(items_.begin(), items_.end(),item);
  unsigned int row = std::distance(items_.begin(), it_insert);

  this->beginInsertRows(QModelIndex() ,row,row );  //All the insertion are at the top
  items_.insert(it_insert,item);
  this->endInsertRows();

  emit this->data_added(item);

  return true;
}

bool Categorical_histogram_proxy_model::insert_row(Categorical_distribution* dist,QColor color){
  Categorical_histogram_item* item = new Categorical_histogram_distribution_item(dist, current_id_);
  current_id_++;
  item->color(color);
  return this->insert_row(item);
}

bool Categorical_histogram_proxy_model::insert_row(Grid_categorical_property* prop, QColor color){
//  if (this->is_item_exist(prop) ) return false;

  Categorical_histogram_item* item = new Categorical_histogram_property_item(prop, current_id_);
  current_id_++;
  item->color(color);

  return this->insert_row(item);
}

bool Categorical_histogram_proxy_model::insert_row(Grid_categorical_property* prop, Grid_region* region, QColor color){
  //if (this->is_item_exist(prop, region) ) return false;

  Categorical_histogram_item* item = new Categorical_histogram_property_item(prop,current_id_);
  current_id_++;
  item->region(region);
  item->color(color);

  return this->insert_row(item);
}

bool Categorical_histogram_proxy_model::insert_row(Grid_categorical_property* prop, GsTLGridWeightProperty* weights, QColor color){

  //if (this->is_item_exist(prop, weights) ) return false;
  Categorical_histogram_item* item = new Categorical_histogram_property_item(prop, current_id_);
  current_id_++;
  item->weights(weights);
  item->color(color);
  return this->insert_row(item);
}

bool Categorical_histogram_proxy_model::insert_row(GsTLGridPropertyGroup* group, QColor color){

  //if (this->is_item_exist(group) ) return false;;

  Categorical_histogram_item* item = new Categorical_histogram_group_item(group, current_id_);
  current_id_++;
  item->color(color);

  return this->insert_row(item);

}


bool Categorical_histogram_proxy_model::insert_row(GsTLGridPropertyGroup* group, Grid_region* region, QColor color){

  //if (this->is_item_exist(group, region) ) return false;;

  Categorical_histogram_item* item = new Categorical_histogram_group_item(group, current_id_);
  current_id_++;
  item->region(region);
  item->color(color);

  return this->insert_row(item);

}

bool Categorical_histogram_proxy_model::insert_row(GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weights, QColor color){
  //if (this->is_item_exist(group, weights) ) return false;

  Categorical_histogram_item* item = new Categorical_histogram_group_item(group, current_id_);
  current_id_++;
  item->weights(weights);
  item->color(color);

  return this->insert_row(item);

}

bool Categorical_histogram_proxy_model::insert_rows(std::vector<GsTL_object_item*> new_object_items){

  bool ok = false;
  for(int i=0; i<new_object_items.size(); ++i) {
//    std::map<GsTL_object_item*,Categorical_histogram_item*>::iterator it =  lookup_items_.find(new_object_items[i]);
//    if( it != lookup_items_.end() ) continue;

    //Categorical_histogram_item* new_item=0;
    Grid_categorical_property* prop = dynamic_cast< Grid_categorical_property*>(new_object_items[i]);
    if(prop) {
      ok = ok | this->insert_row(prop);
    }

    GsTLGridPropertyGroup* group = dynamic_cast< GsTLGridPropertyGroup*>(new_object_items[i]);
    if(group) {
      ok = ok | this->insert_row(group);
    }
  }

  return ok;
/*
    if(new_item == 0) continue;

    //Add the data to the model

    std::set<Categorical_histogram_item*>::iterator it_insert = std::lower_bound(items_.begin(), items_.end(),new_item);
    unsigned int row = std::distance(items_.begin(), it_insert);

    this->beginInsertRows(QModelIndex() ,row,row );  //All the insertion are at the top
    items_.insert(it_insert,new_item);
    lookup_items_[new_object_items[i]] = new_item;
    this->endInsertRows();

    //Need to emit some sort of signal to indicate a new data has been added such that is can be displayed
    emit this->data_added(new_item);

  }
  return true;
  */
}

bool Categorical_histogram_proxy_model::remove_row(Categorical_histogram_item* item){
    std::set<Categorical_histogram_item*>::iterator it = items_.find(item);

    if(it == items_.end()) return false;

    unsigned int row = std::distance(items_.begin(), it);
    this->beginRemoveRows(QModelIndex() ,row,row );  //All the removal are at the top 
    items_.erase(it);
   
    if(item->type() == "Property") {
      emit this->data_removed(item->id());
    }
    else if(item->type() == "Group") {
      Categorical_histogram_group_item* group_item = dynamic_cast<Categorical_histogram_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        emit this->data_removed(group_item->children(j)->id());
      }
    }
    delete item;

    this->endRemoveRows();
}

/*
bool Categorical_histogram_proxy_model::remove_rows(std::vector<GsTL_object_item*> new_object_items){

  for(int i=0; i<new_object_items.size(); ++i) {
    std::map<GsTL_object_item*,Categorical_histogram_item*>::iterator it =  lookup_items_.find(new_object_items[i]);
    if( it != lookup_items_.end() ) continue; // not present

    std::set<Categorical_histogram_item*>::iterator it_set = items_.find(it->second);

    unsigned int row = std::distance(items_.begin(), it_set);
    this->beginRemoveRows(QModelIndex() ,row,row );  //All the removal are at the top 
    Categorical_histogram_item* item_to_be_removed = it->second;
    lookup_items_.erase(it);
    items_.erase(it_set);
    this->endRemoveRows();
    
    //Need to emit some sort of signal to indicate a new data has been removed in order to update the display
    if(item_to_be_removed->type() == "Property") {
      Grid_categorical_property* prop = dynamic_cast<Grid_categorical_property*>(item_to_be_removed->object_item());
      emit this->data_removed(prop);
    }
    else if(item_to_be_removed->type() == "Group") {
      Categorical_histogram_group_item* group_item = dynamic_cast<Categorical_histogram_group_item*>(item_to_be_removed);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Grid_categorical_property* prop = dynamic_cast<Grid_categorical_property*>(item_to_be_removed->children(j)->object_item());
        emit this->data_removed(prop);
      }
    }
    delete item_to_be_removed;
  }

  return true;
  

}
*/

QModelIndex Categorical_histogram_proxy_model::index(int row, int column, const QModelIndex &parent)
            const
{

  if(!parent.isValid()) {
    if(row < items_.size())  {
      std::set< Categorical_histogram_item*>::iterator it = items_.begin();
      for(int i=0; i<row; ++i,++it){}
      return createIndex(row, column, static_cast<void*>(*it) );
    }
    else return QModelIndex();
  }
  else {  // This is a property within a group
    Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(parent.internalPointer());
    if(row < item->children_count()) return createIndex(row, column, static_cast<void*>(item->children(row)) );
    else return QModelIndex();
  }

  return QModelIndex();

}


int Categorical_histogram_proxy_model::rowCount(const QModelIndex &parent) const
{
  if(!parent.isValid()) return items_.size();
  Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(parent.internalPointer());
  return item->children_count();

}


QModelIndex Categorical_histogram_proxy_model::parent(const QModelIndex &child) const
{
  Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(child.internalPointer());
  Categorical_histogram_item* parent_item = item->parent();
  if( parent_item == 0 ) return QModelIndex();
  
  std::set<Categorical_histogram_item*>::const_iterator it = items_.find(parent_item);
  if(it == items_.end()) return QModelIndex();

  int row = std::distance(items_.begin(), it);
  return createIndex(row,0,static_cast<void*>(parent_item));
  //Find the row number 
  //QList<Categorical_histogram_item*>::const_iterator it = qBinaryFind(items_.begin(),items_.end(), parent_item, histogram_item_less_than);
}




QVariant
Categorical_histogram_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Filter");
            case 2:
                return tr("Color");
            case 3:
                return tr("Style");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QVariant Categorical_histogram_proxy_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
    	return QVariant();
    }

    if( role == Qt::DisplayRole ) {
      Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(index.internalPointer());
      if(index.column() == 0) return  item->item_name();
      if(index.column() == 1) {
        if( item->region() ) return QString(item->region()->name().c_str());
        else if( item->weights() ) return QString(item->weights()->name().c_str());
        else return QString("<-None->");
      }
      else if(index.column() == 2) return  item->color(); 
    }

    if( role == Qt::CheckStateRole && index.column() == 0) {
      Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(index.internalPointer());
      return item->is_visible() ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

bool Categorical_histogram_proxy_model::setData ( const QModelIndex & index, const QVariant & value, int role ){
  bool ok = false;
  if( role == Qt::EditRole ) {
      Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(index.internalPointer());
      if(index.column() == 1) {

        QString option = value.toString();
        if(option.endsWith("_R_")) {
          option.chop(3);
          item->region(option);
          item->weights(0);
        }
        else if(option.endsWith("_W_")) {
          option.chop(3);
          item->weights(option);
          item->region(0);
        }
        else {
          item->region(0);
          item->weights(0);
        }
        emit this->data_filter_changed(item);
      }

      if(index.column() == 2) {
        QColor color = value.value<QColor>();
        if(color.isValid()) {
          item->color(color);
          emit this->data_color_changed(item);
          ok = true;
        }
      
      }
  }
  else if( role == Qt::CheckStateRole && index.column() == 0) {
      Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(index.internalPointer());
      item->set_visibility( value.toBool() );
      emit this->data_visibility_changed(item);
      ok = true;
   }

  return ok;
}

/*
QModelIndex	Categorical_histogram_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{

  
	GsTL_object_item *object_item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());

  std::map<GsTL_object_item*,Categorical_histogram_item*>::const_iterator it = lookup_items_.find(object_item);

  if(it == lookup_items_.end()) return QModelIndex();

  Categorical_histogram_item* item = it->second;

  // check if the item is a top item or a child of a group
  if(item->parent() == 0) {
    std::set<Categorical_histogram_item*>::const_iterator it = items_.find(item);
    if(it == items_.end()) return QModelIndex();

    int row = std::distance(items_.begin(), it);
    return createIndex(row,sourceIndex.column(),static_cast<void*>(item));
  }
  else {  //child of a group
    Categorical_histogram_group_item* parent = dynamic_cast<Categorical_histogram_group_item*>(item->parent());
    int row = parent->row_for_children(item);
    if(row<0) return QModelIndex();
    return createIndex(row,sourceIndex.column(),static_cast<void*>(item));

  }
  
  return QModelIndex();
}


QModelIndex	Categorical_histogram_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{

  if(!proxyIndex.isValid() ) return QModelIndex();
  if(proxyIndex.column() > 0 ) return QModelIndex(); // Only the first index is mappable to the source

  Categorical_histogram_item* item = static_cast<Categorical_histogram_item*>(proxyIndex.internalPointer());

  if(item->type() == "CategoricalProperty") {
    GsTL_object_item* object_item = dynamic_cast<Grid_categorical_property*>(item->object_item() );
    QModelIndex prop_parent = model_->property_root_index(item->grid_name());
    return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(), prop_parent );
  }
  else if(item->type() == "Group") {
    QModelIndex grid_index = model_->grid_index(item->grid_name());
    int nrow = model_->rowCount(grid_index);
    for(int i=0; i<nrow;++i) {
      QModelIndex index = grid_index.child(i,0);
      if(index.data() == item->item_name() ) 
        return this->sourceModel()->index(i, proxyIndex.column(), grid_index );
    }
  }

  return QModelIndex();

}

*/

/*
void Categorical_histogram_proxy_model::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

  QModelIndex index = this->mapFromSource(source_parent);
  if(!index.isValid()) return;

	this->beginInsertRows(index,start,end);
}
*/

void Categorical_histogram_proxy_model::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){

  //Check if the data is currently loaded
  // A property, a region or a set of weights can be removed;  Need to check for each
  
  //std::vector<Categorical_histogram_item*> items_to_remove;
  std::set<Categorical_histogram_item*> items_to_remove;
  for(int i=start ; i <= end; ++i) {
    QModelIndex source_index = source_parent.child(i,0);
    //QModelIndex proxy_index = this->mapFromSource(source_index);
    //if(!proxy_index.isValid()) continue;
    GsTL_object_item* item = static_cast<GsTL_object_item*>(source_index.internalPointer());
    find_items_to_be_removed(item, items_to_remove);

 //   items_to_remove.push_back( lookup_items_[item] );

    // this->beginRemoveRows(QModelIndex(),start,end); The problem is that not all the range start-end may be removed
    // Will instead try to do a layout changed or reset

  }


  this->beginResetModel();
  std::set<Categorical_histogram_item*>::iterator it = items_to_remove.begin();
  for( ; it != items_to_remove.end(); ++it) {
    items_.erase(*it);
  }
  //if(!items_to_remove.empty() ) this->reset();

}

void Categorical_histogram_proxy_model::find_items_to_be_removed(GsTL_object_item* item, std::set< Categorical_histogram_item*>& items_to_be_removed){

  //Is it a grid?
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item);
  if(grid != 0 ) {
    std::set< Categorical_histogram_item*>::iterator it = items_.begin();
    for( ; it!= items_.end(); ++it) {
      if( (*it)->grid() == grid ) 
        items_to_be_removed.insert(items_to_be_removed.begin(), *it);
    }
    return;
  }

  Grid_categorical_property* prop = dynamic_cast<Grid_categorical_property*>(item);

  if(prop) {  
    std::set< Categorical_histogram_item*>::iterator it = items_.begin();
    for( ; it!= items_.end(); ++it) {
      Categorical_histogram_property_item* p_item = dynamic_cast<Categorical_histogram_property_item*>(*it);
      if(p_item && p_item->prop() == prop  ) 
        items_to_be_removed.insert(items_to_be_removed.begin(), *it);
    }
    return;
  }

  Grid_region* region = dynamic_cast<Grid_region*>(item);

  if(region) {  //could either be a prop or a weight
    std::set< Categorical_histogram_item*>::iterator it = items_.begin();
    for( ; it!= items_.end(); ++it) {
      Categorical_histogram_group_item* g_item = dynamic_cast<Categorical_histogram_group_item*>(*it);
      if(g_item && g_item->region() == region ) 
        items_to_be_removed.insert(items_to_be_removed.begin(), *it);
    }
    return;
  }

}


bool Categorical_histogram_proxy_model::is_item_exist(Grid_categorical_property* prop){
  std::set< Categorical_histogram_item*>::iterator it = items_.begin();
  for( ; it!= items_.end(); ++it) {
    Categorical_histogram_property_item* p_item = dynamic_cast<Categorical_histogram_property_item*>(*it);
    if(p_item && p_item->prop() == prop ) return true;
  }
  return false;
}

bool Categorical_histogram_proxy_model::is_item_exist(Grid_categorical_property* prop, Grid_region* region){
  std::set< Categorical_histogram_item*>::iterator it = items_.begin();
  for( ; it!= items_.end(); ++it) {
    Categorical_histogram_property_item* p_item = dynamic_cast<Categorical_histogram_property_item*>(*it);
    if(p_item && p_item->prop() == prop && p_item->region() == region ) return true;
  }
  return false;
}

bool Categorical_histogram_proxy_model::is_item_exist(Grid_categorical_property* prop, GsTLGridWeightProperty* weights){
  std::set< Categorical_histogram_item*>::iterator it = items_.begin();
  for( ; it!= items_.end(); ++it) {
    Categorical_histogram_property_item* p_item = dynamic_cast<Categorical_histogram_property_item*>(*it);
    if(p_item && p_item->prop() == prop && p_item->weights() == weights ) return true;
  }
  return false;
}

bool Categorical_histogram_proxy_model::is_item_exist(GsTLGridPropertyGroup* group){
  std::set< Categorical_histogram_item*>::iterator it = items_.begin();
  for( ; it!= items_.end(); ++it) {
    Categorical_histogram_group_item* g_item = dynamic_cast<Categorical_histogram_group_item*>(*it);
    if(g_item && g_item->group() == group  ) return true;
  }
  return false;
}

bool Categorical_histogram_proxy_model::is_item_exist(GsTLGridPropertyGroup* group, Grid_region* region){
  std::set< Categorical_histogram_item*>::iterator it = items_.begin();
  for( ; it!= items_.end(); ++it) {
    Categorical_histogram_group_item* g_item = dynamic_cast<Categorical_histogram_group_item*>(*it);
    if(g_item && g_item->group() == group && g_item->region() == region ) return true;
  }
  return false;
}

bool Categorical_histogram_proxy_model::is_item_exist(GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weights){
  std::set< Categorical_histogram_item*>::iterator it = items_.begin();
  for( ; it!= items_.end(); ++it) {
    Categorical_histogram_group_item* g_item = dynamic_cast<Categorical_histogram_group_item*>(*it);
    if(g_item && g_item->group() == group && g_item->weights() == weights ) return true;
  }
  return false;
}


/*
void Categorical_histogram_proxy_model::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
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
*/
void Categorical_histogram_proxy_model::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
  this->endResetModel();
}

