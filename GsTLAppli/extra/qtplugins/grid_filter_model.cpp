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




#include <GsTLAppli/extra/qtplugins/grid_filter_model.h>
#include <GsTLAppli/extra/qtplugins/grid_filter_named_interface.h>
#include <GsTLAppli/appli/manager_repository.h>

Grid_filter_model::Grid_filter_model(QObject *parent)
 : QAbstractItemModel(parent) {


}

Grid_filter_model::~Grid_filter_model() {

  for(int i=0; i< grid_filters_.size(); ++i) {
    delete grid_filters_[i];
  }

}


void Grid_filter_model::initialize(){
  available_filters_.clear();

  Manager* grid_filter_item_manager = dynamic_cast<Manager*>(Root::instance()->interface(qitem_grid_filter_model_manager).raw_ptr());

  //grid_filter_manager_ = dynamic_cast<Manager*>(Root::instance()->interface(gridFilter_manager).raw_ptr());
  if( !grid_filter_item_manager  ) return;
  
  // Get all the existing cdef from the manager
   Manager::type_iterator it = grid_filter_item_manager->begin();
  for( ; it != grid_filter_item_manager->end(); ++it ) {
    SmartPtr<Named_interface> ni = Root::instance()->new_interface(*it+"://", qitem_grid_filter_model_manager+"/");
    Grid_filter_named_interface* grid_filter_ni = dynamic_cast<Grid_filter_named_interface*>(ni.raw_ptr());
    if( grid_filter_ni == 0 ) continue;
    grid_filters_.push_back(grid_filter_ni->filter_item());
    grid_filters_.back()->set_row(grid_filters_.size() -1);

    available_filters_.append( QString::fromStdString(*it) );
  }
}

int Grid_filter_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

Qt::ItemFlags Grid_filter_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    if( index.column() == 0 ) {
      return Qt::ItemIsEnabled  | Qt::ItemIsUserCheckable ;
    }
    else {
      return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }


}



QModelIndex Grid_filter_model::index(int row, int column, const QModelIndex &parent)
            const
{
	/*
    if (!hasIndex(row, column, parent) )
        return QModelIndex();
*/
  if( grid_filters_.size() == 0) return QModelIndex();

  if(!parent.isValid()) {
    return createIndex(row, column, grid_filters_[row] );
  }

  Grid_filter_item* item = static_cast<Grid_filter_item*>(parent.internalPointer());

  if( row < item->child_count()) {
    return createIndex(row, column, item->child( row ) );
	}
	else return QModelIndex();

}


QModelIndex Grid_filter_model::parent(const QModelIndex &child) const{

  Grid_filter_item* child_item = static_cast<Grid_filter_item*>(child.internalPointer());
  Grid_filter_item* parent_item = child_item->parent();

  if( parent_item == 0) return QModelIndex();

  return this->createIndex( parent_item->row(), child.column(),  static_cast<void*>(parent_item) );


// find the row index for the parent  



}

int Grid_filter_model::rowCount(const QModelIndex &parent) const
{

  if(!parent.isValid()) {
    return grid_filters_.size();
  }

  Grid_filter_item* item = static_cast<Grid_filter_item*>(parent.internalPointer());
  return item->child_count();

}



QVariant
Grid_filter_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Type");
            case 1:
              return tr("parameters");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QVariant Grid_filter_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Grid_filter_item* item = static_cast<Grid_filter_item*>(index.internalPointer());

    if( role == Qt::DisplayRole ) {
      if(index.column() == 0) return  item->filter_name();
      if(index.column() == 1) return  item->parameter();
//      else if(index.column() == 1) return  grid_types_.at(index.row());
    }
    else if( role == Qt::CheckStateRole && index.column() == 0) {
  //    Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
      return item->is_selected() ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

bool Grid_filter_model::setData ( const QModelIndex & index, const QVariant & value, int role  )
{

  if(role == Qt::EditRole && index.column() == 1) {
    Grid_filter_item* item = static_cast<Grid_filter_item*>(index.internalPointer());
    item->set_parameter( value );
  }
  else if( role == Qt::CheckStateRole && index.column() == 0) {
      Grid_filter_item* item = static_cast<Grid_filter_item*>(index.internalPointer());
      item->set_selected(value.toBool());
   }
  return true;
}


void Grid_filter_model::set_grid(const Geostat_grid* grid){
  grid_ = grid;
  for(int i=0; i< grid_filters_.size(); ++i) {
    grid_filters_[i]->set_grid( grid_);
  }
}

void Grid_filter_model::set_grid(const QString& grid_name){

  SmartPtr<Named_interface> ni = Root::instance()->interface(gridModels_manager+"/"+grid_name.toStdString() );

  this->set_grid( dynamic_cast<Geostat_grid*>(ni.raw_ptr()) );


}


Grid_filter* Grid_filter_model::create_filter() const{

  int n_filters_active = 0;
  std::vector<Grid_filter_item*> active_filters_;

  for(int i=0; i< grid_filters_.size(); ++i) {
    if( grid_filters_[i]->is_selected()  ) {
      active_filters_.push_back(grid_filters_[i]);
    }
  }


  if(active_filters_.size() == 0) return 0;
  else if(active_filters_.size() == 1) {
    return active_filters_[0]->create_filter();
  }
  else { // We assume an intersection
    Grid_filter_intersection* filter = new Grid_filter_intersection();
    for(int i=0; i< active_filters_.size(); ++i) {
      Grid_filter* sub_filter = active_filters_[i]->create_filter();
      if(sub_filter==0)  {
        delete filter;
        return 0;
      }
      filter->add_filter(sub_filter);
    }
    return filter;
  }

  return 0;

}


QStringList Grid_filter_model::get_grid_filter_name() const{

  int n_filters_active = 0;
  QStringList filter_params;

  for(int i=0; i< grid_filters_.size(); ++i) {
    if( grid_filters_[i]->is_selected()  ) {
      filter_params.append( grid_filters_[i]->parameter().toString()  );
    }
  }
  return filter_params;

}
