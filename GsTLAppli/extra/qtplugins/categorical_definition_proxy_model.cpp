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




#include <GsTLAppli/extra/qtplugins/categorical_definition_proxy_model.h>
#include <GsTLAppli/appli/manager_repository.h>

Categorical_definition_proxy_model::Categorical_definition_proxy_model(QObject *parent)
 : QAbstractProxyModel(parent) {
   cat_def_manager_ = dynamic_cast<Manager*>(Root::instance()->interface(categoricalDefinition_manager).raw_ptr());
  
//  QModelIndex root_index = model->project_root_index();
//  int ngrid = 

  // Get all the existing cdef from the manager
  Manager::interface_iterator it = cat_def_manager_->begin_interfaces();
  for( ; it != cat_def_manager_->end_interfaces(); ++it ) {
    cdef_names_.append((*it)->item_name() );
//    cdefs_.append( dynamic_cast  (*it)->item_type() );
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

Categorical_definition_proxy_model::~Categorical_definition_proxy_model() {
}


int Categorical_definition_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Qt::ItemFlags Categorical_definition_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}



QModelIndex Categorical_definition_proxy_model::index(int row, int column, const QModelIndex &parent)
            const
{
	/*
    if (!hasIndex(row, column, parent) )
        return QModelIndex();
*/
  if( row < cdef_names_.size() ) {
		return createIndex(row, column, 0 );
	}
	else return QModelIndex();

}


int Categorical_definition_proxy_model::rowCount(const QModelIndex &parent) const
{
	return cdef_names_.size();
}


QModelIndex Categorical_definition_proxy_model::parent(const QModelIndex &child) const
{
	return QModelIndex();

}


QVariant
Categorical_definition_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
              return tr("# of categories");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QVariant Categorical_definition_proxy_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
/*
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
*/
    if( role == Qt::DisplayRole ) {
      if(index.column() == 0) return  cdef_names_.at(index.row());
//      else if(index.column() == 1) return  grid_types_.at(index.row());
    }

    return QVariant();
}

QModelIndex	Categorical_definition_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{

  if(sourceIndex.column() != 0) return QModelIndex();
	GsTL_object_item *item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());
  if( dynamic_cast<CategoricalPropertyDefinition*>(item) == 0) return QModelIndex();
  if(sourceIndex.row() > cdef_names_.size()) return QModelIndex();
	return createIndex(sourceIndex.row(),sourceIndex.column(), 0);

}
QModelIndex	Categorical_definition_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{

  return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(), model_->project_root_index() );

}


void Categorical_definition_proxy_model::begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){

	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != cat_def_manager_) return;
	this->beginInsertRows(QModelIndex(),start,end);

}

void Categorical_definition_proxy_model::begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
  if(item != cat_def_manager_) return;
	this->beginRemoveRows(QModelIndex(),start,end);
}


void Categorical_definition_proxy_model::end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != cat_def_manager_) return;

  //Add the grid into the list
  for(int i = start; i<= end; ++i) {
    item = static_cast<GsTL_object_item*>(source_parent.child(i,0).internalPointer());
    cdef_names_.insert(i,item->item_name());
    //grid_types_.insert(i,item->item_type());
  }

	this->endInsertRows();
//	emit this->layoutChanged();


}

void Categorical_definition_proxy_model::end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end){
	GsTL_object_item *item = static_cast<GsTL_object_item*>(source_parent.internalPointer());
	if(item != cat_def_manager_) return;

  //remove the grid from the list
  for(int i = end; i>= start; --i) {
    cdef_names_.removeAt(i);
 //   grid_types_.removeAt(i);
  }

	this->endRemoveRows();
}




/*
 ------------------------------------------------
 */

Category_proxy_model::Category_proxy_model(QObject *parent)
 : cdef_(0),QAbstractProxyModel(parent) {
	root_ = dynamic_cast<Manager*>(Root::instance()->interface(categoricalDefinition_manager).raw_ptr());
  
	model_ = dynamic_cast<Root_model*>(Root::instance()->model());
	this->setSourceModel(model_);
  flags_name_ = Qt::ItemIsEnabled |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
  flags_color_ = Qt::ItemIsEnabled |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
  this->set_connections();

}



Category_proxy_model::Category_proxy_model(CategoricalPropertyDefinitionName* cdef, QObject *parent)
 : cdef_(cdef),QAbstractProxyModel(parent) {
	root_ = dynamic_cast<Manager*>(Root::instance()->interface(categoricalDefinition_manager).raw_ptr());
  
	model_ = dynamic_cast<Root_model*>(Root::instance()->model());
	this->setSourceModel(model_);
  this->set_connections();

  flags_name_ = Qt::ItemIsEnabled |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
  flags_color_ = Qt::ItemIsEnabled |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;

  this->initialize(cdef);

}

Category_proxy_model::Category_proxy_model(CategoricalPropertyDefinition* cdef, int ncat, QObject *parent)
 : cdef_(cdef),QAbstractProxyModel(parent) {
	root_ = dynamic_cast<Manager*>(Root::instance()->interface(categoricalDefinition_manager).raw_ptr());
  
	model_ = dynamic_cast<Root_model*>(Root::instance()->model());
	this->setSourceModel(model_);
  this->set_connections();

  flags_name_ = Qt::ItemIsEnabled |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
  flags_color_ = Qt::ItemIsEnabled |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;

  this->initialize(cdef, ncat);

}


void Category_proxy_model::initialize(CategoricalPropertyDefinitionName* cdef){
  if(cdef == 0) return;
  cdef_ = cdef;
  nrow_ = cdef->number_of_category();
  visibility_.clear();
  visibility_.insert(visibility_.begin(),nrow_,true);

}

void Category_proxy_model::initialize(CategoricalPropertyDefinition* cdef, int ncat){
  cdef_ = cdef;
  nrow_ = ncat;
  visibility_.clear();
  visibility_.insert(visibility_.begin(),nrow_,true);
}


Category_proxy_model::~Category_proxy_model() {
  /*
  std::set< Categorical_item*>::iterator it = items_.begin();
  for( ; it != items_.end(); ++it ) {
    delete *it;
  }
  */

}


void Category_proxy_model::set_connections(){
  	bool ok;

/*
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
  */
}


int Category_proxy_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;  //Name and color
}

Qt::ItemFlags Category_proxy_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    //Cannot edit the name (column 0) but is checkable
    if( index.column() == 0 ) {
      return flags_name_;
      //return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
    }
    else {
      return flags_color_;
      //return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled  | Qt::ItemIsEditable;
    }

}


QModelIndex Category_proxy_model::index(int row, int column, const QModelIndex &parent)
            const
{
  if(cdef_ == 0) return QModelIndex();
  //  if(row < items_.size())  {
  if(row < nrow_)  {
 //     std::set< Categorical_item*>::iterator it = items_.begin();
 //     for(int i=0; i<row; ++i,++it){}
      return createIndex(row, column, static_cast<void*>(0) );
  }
  else return QModelIndex();

}


int Category_proxy_model::rowCount(const QModelIndex &parent) const
{

  return cdef_==0 ? 0 : nrow_;

  //return items_.size();

}


QModelIndex Category_proxy_model::parent(const QModelIndex &child) const
{
  return QModelIndex();

}




QVariant
Category_proxy_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Color");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QVariant Category_proxy_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
    	return QVariant();
    }

    Qt::ItemFlags flags = this->flags(index);

    if( role == Qt::DisplayRole ) {
  //    Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
      if(index.column() == 0) return  QString::fromStdString( cdef_->get_category_name_from_index(index.row()) );    //item->category_name();
      else if(index.column() == 1) return  cdef_->color_from_index(index.row()); 
    }

    if( (flags & Qt::ItemIsUserCheckable) !=0 & role == Qt::CheckStateRole && index.column() == 0) {
  //    Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
      return visibility_[index.row()] ? Qt::Checked : Qt::Unchecked;
    }

    return QVariant();
}

bool Category_proxy_model::setData ( const QModelIndex & index, const QVariant & value, int role ){

  if( role == Qt::EditRole ) {
    if(index.column() == 0) {
      if(value.toString().isEmpty()) return false;
      CategoricalPropertyDefinitionName* cdef_named = dynamic_cast<CategoricalPropertyDefinitionName*>(cdef_);
      if(cdef_named == 0) return false;  // cannot change name for the default cat definition
      cdef_named->rename_from_index(index.row(), value.toString().toStdString());
      return true;
    }
    if (index.column() == 1) {
      cdef_->color_from_index(index.row(), value.value<QColor>() );
      return true;
    }
  }
  else if( role == Qt::CheckStateRole && index.column() == 0) {
      Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
      int row = index.row();
      visibility_[row] = value.toBool();
 //     emit this->checked_category_changed(row , visibility_[row] );
      return true;
   }
  return false;
 // bool ok = this->sourceModel()->setData(index, value, role );
  /*
  bool ok = false;
  if( role == Qt::EditRole ) {
      Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
      if(index.column() == 1) {
        QColor color = value.value<QColor>();
        if(color.isValid()) {
          item->color(color);
          // emit this->data_color_changed(item);  Need to notify the source model, but need to be sure that it is not a self-feeding loop
          ok = true;
        }
      }
  }
  else if( role == Qt::CheckStateRole && index.column() == 0) {
      Categorical_item* item = static_cast<Categorical_item*>(index.internalPointer());
      item->set_visibility( value.toBool() );
//      emit this->category_selection_changed(item);
      ok = true;
   }
   */
//  return ok;
}

void Category_proxy_model::use_check_state(bool on){

}


QModelIndex	Category_proxy_model::mapFromSource ( const QModelIndex & sourceIndex ) const{

  if(sourceIndex.column() !=0 ) return QModelIndex(); //Only map the first column, the second one is different (color versus type)
  GsTL_object_item *object_item = static_cast<GsTL_object_item*>(sourceIndex.internalPointer());


  Categorical_color_coding* cat_coding = dynamic_cast<Categorical_color_coding*>(object_item);
  if(cat_coding == 0) return QModelIndex();
  return this->createIndex(sourceIndex.row(),sourceIndex.column(),static_cast<void*>(0));
  /*
  std::set<Categorical_item*>::const_iterator it = items_.begin();
  int row = 0;
  for(; it != items_.end(); ++it, ++row ) {
    if((*it)->category_name() == cat_coding->name()) break;
  }
  if(it == items_.end()) return  QModelIndex();
  return this->createIndex(row,sourceIndex.column(),static_cast<void*>(*it));
  */

}


QModelIndex	Category_proxy_model::mapToSource ( const QModelIndex & proxyIndex ) const{

  if(!proxyIndex.isValid()) return QModelIndex();
  if(proxyIndex.column() > 0 ) return QModelIndex(); // Only the first index is mappable to the source

  //Index of cdef manager
  QModelIndex cdef_manager_index = model_->top_level_manager_index(categoricalDefinition_manager.c_str());
  QModelIndex cdef_index = this->sourceModel()->index(cdef_->row(),proxyIndex.column(),cdef_manager_index);
  
  return this->sourceModel()->index(proxyIndex.row(), proxyIndex.column(), cdef_index );

}

