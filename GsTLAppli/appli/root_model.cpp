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
 * root_model.cpp
 *
 *  Created on: Jun 6, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>

Named_interface* Root_model::create_new_interface(std::string&){
	return new Root_model;
}

Root_model::Root_model(QObject *parent)
	: GsTL_ItemModel(parent) //:QAbstractItemModel(parent)
{
	root_ = dynamic_cast<GsTL_object_item*>(Root::instance());
}


Root_model::~Root_model()
{

}




int Root_model::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

Qt::ItemFlags Root_model::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    return item->flags(index.column());
    //return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}

void Root_model::begin_insert_item(GsTL_object_item* parent, int position){
	QModelIndex index = this->createIndex(parent->row(),0,parent);
//  emit this->layoutAboutToBeChanged();
	this->beginInsertRows ( index, position, position );
}

void Root_model::end_insert_item(){
	this->endInsertRows();
//  emit this->layoutChanged();
}



void Root_model::begin_remove_item(GsTL_object_item* parent, int position){
	QModelIndex index = this->createIndex(parent->row(),0,parent);
//  emit this->layoutAboutToBeChanged();
	this->beginRemoveRows ( index, position, position );
}
void Root_model::end_remove_item(){
	this->endRemoveRows();
//  emit this->layoutChanged();
}

QModelIndex Root_model::index(int row, int column, const QModelIndex &parent)
            const
{
  /*
    if (!hasIndex(row, column, parent))
        return QModelIndex();

// If the parent is invalid, then it refers to the root
     if (!parent.isValid()) {  // The parent is the root element
       return createIndex(row, column, root_->child(row));
     }
     GsTL_object_item *parent_item = static_cast<GsTL_object_item*>(parent.internalPointer());

     if(parent_item == 0) return QModelIndex();
    
     GsTL_object_item *child_item = parent_item->child(row);
     if (child_item)
        return createIndex(row, column, child_item);
     else
        return QModelIndex();
  */

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    // Only the first column has children
    if(parent.column() > 0)
      return QModelIndex();

    if (!parent.isValid())
      return createIndex(0, column,root_);

    if(parent.column() > 0)
      return QModelIndex();

    GsTL_object_item *parent_item = static_cast<GsTL_object_item*>(parent.internalPointer());

    if(parent_item == 0) return QModelIndex();

    GsTL_object_item *child_item = parent_item->child(row);
    if (child_item)
        return createIndex(row, column, child_item);
    else
        return QModelIndex();

}


int Root_model::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid()) {
   //   return root_->childCount();
      //parent_item =  root_;
      return 1;
    }
    else {
      GsTL_object_item * parent_item = static_cast<GsTL_object_item*>(parent.internalPointer());
      return parent_item->childCount();
    }


/*
    if (!parent.isValid())
    	parent_item = root_;
    else
    	parent_item = static_cast<GsTL_object_item*>(parent.internalPointer());

    return parent_item->childCount();
    */
}


QModelIndex Root_model::parent(const QModelIndex &child) const
{

    if (!child.isValid())
        return QModelIndex();

    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child.internalPointer());
    if(child_item == 0 || child_item == root_) return QModelIndex();
    GsTL_object_item *parent_item = child_item->parent();
    if(parent_item == 0 ) return QModelIndex();

    return createIndex(parent_item->row(), 0, parent_item);
    /*
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child.internalPointer());
    if(child_item == 0 || child_item == root_) return QModelIndex();
    GsTL_object_item *parent_item = child_item->parent();
    if(parent_item == 0 || parent_item == root_) return QModelIndex();

    return createIndex(parent_item->row(), 0, parent_item);
*/


/*
    GsTL_object_item *child_item = static_cast<GsTL_object_item*>(child.internalPointer());
    if(child_item == root_)
    	return QModelIndex();

    GsTL_object_item *parent_item = child_item->parent();

    if ( !parent_item )// || parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parent_item->row(), 0, parent_item);
    */
}




QVariant
Root_model::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant Root_model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // The icon should be store in the GsTL_object_item or in the repository
    if (role == Qt::DecorationRole && index.column() == 0) {
    	QIcon icon;
    	GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    	QString type = item->item_data(1).toString();
    	if(type == "Cgrid") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cgrid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Point_set") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/pset.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Masked_grid") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/mgrid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Masked_grid") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/mgrid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Log_data_grid") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/log_grid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Log data") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/log_data.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "CategoricalProperty") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cat_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "ContinuousProperty") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cont_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "WeightsProperty") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/weight_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
      }
    	else if(type.contains("ContinuousDistribution") ) {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/distributions.svg"), QSize(), QIcon::Normal, QIcon::Off);
      }
    	else if(type.contains("CategoricalDistribution") ) {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cat_histo.svg"), QSize(), QIcon::Normal, QIcon::Off);
      } 
    	else if(type == "Region") {
        	icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/region.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Manager") {
        icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/folder.png"), QSize(), QIcon::Normal, QIcon::Off);
    		//icon = QIcon::fromTheme("folder");
    	}
    	else if(type == "Factory") {
        icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/application-x-executable.png"), QSize(), QIcon::Normal, QIcon::Off);
    		//icon = QIcon::fromTheme("exec");
    	}
    	return icon;
    }

    if( role == Qt::DisplayRole ) {
    	GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    	//Manager* mng = dynamic_cast<Manager*>(item);
    	return item->item_data(index.column());
    }

    return QVariant();
}

bool Root_model::setData ( const QModelIndex & index, const QVariant & value, int role ){

  if(!index.isValid()) return false;
  GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
  return item->set_item_data(value, index.column());

}


 QModelIndex Root_model::project_root_index() const
{
  const Manager *root = dynamic_cast<const Manager*>(Root::instance()->interface(gridModels_manager).raw_ptr());

  if(root == 0) return QModelIndex();

  QModelIndex root_index = this->index(0,0,QModelIndex());  //root
  QModelIndex object_index = this->index(root->parent()->row(),0,root_index);  //Object

  return this->index(root->row(),0,object_index);

}

 QModelIndex Root_model::top_level_manager_index(QString manager_name) const
{
  const Manager *manager = dynamic_cast<const Manager*>(Root::instance()->interface(manager_name.toStdString()).raw_ptr());

  const GsTL_object_item* item_search = manager;

  // Need to find the parent starting from the root
  
  // Go up to the root with the row numbers
  std::list<int> rows;
  while( item_search != 0 ) {
    rows.push_front(item_search->row());
    item_search = item_search->parent();
  }
  rows.pop_front();

  // Go down from the root with the path obtained from the row numbers;
  QModelIndex index = this->index(0,0,QModelIndex());  //root
  while( !rows.empty() ) {
    QModelIndex index_temp = this->index( rows.front(), 0, index );
    rows.pop_front();
    index  = index_temp;
  }

  return index;

}

QModelIndex Root_model::manager_index(QString manager_name) const
{
  const Manager *manager = dynamic_cast<const Manager*>(Root::instance()->interface(manager_name.toStdString()).raw_ptr());

  QModelIndex root_index = this->index(0,0,QModelIndex());  //root
  return this->index(manager->row(),0,root_index);  

//  return this->index(manager->row(),0,object_index);

}

QModelIndex Root_model::grid_index(QString grid_name) const{
   QModelIndex project_root = this->project_root_index();
   int nrow = this->rowCount(project_root);
   for(int i=0; i<nrow; ++i) {
     QModelIndex index = this->index(i,0,project_root);
     if(index.data() == grid_name) return index;
   }
   return QModelIndex();
}

 QModelIndex Root_model::property_root_index(QString grid_name) const{
   QModelIndex grid_index = this->grid_index(grid_name);
   if( !grid_index.isValid() ) return QModelIndex();

   int nrow = this->rowCount(grid_index);
   for(int i=0; i<nrow; ++i) {
     QModelIndex index = this->index(i,0,grid_index);
     if(index.data() == "Properties") return index;
   }
   return QModelIndex();

 }

 QModelIndex Root_model::region_root_index(QString grid_name) const{
   QModelIndex grid_index = this->grid_index(grid_name);
   if( !grid_index.isValid() ) return QModelIndex();

   int nrow = this->rowCount(grid_index);
   for(int i=0; i<nrow; ++i) {
     QModelIndex index = this->index(i,0,grid_index);
     if(index.data() == "Regions") return index;
   }
   return QModelIndex();
 }




/*
QModelIndex Root_model::grid_index(QString grid_name){

	int childCount = root_->childCount();
	for(int row=0; row<childCount; ++row) {
		GsTL_object_item* grid_item = root_->child(row);
		if(grid_item->data(0).toString() != grid_name) continue;

		return createIndex(row, 0, grid_item);
	}

	return QModelIndex();
}

QModelIndex Root_model::root_index(){
	return createIndex(0, 0, root_);
}

QStringList Root_model::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}

QMimeData *Root_model::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
	QString mimeCompleteString;

	for (int i = 0; i < indexes.size(); i+=3) {
		QModelIndex index = indexes.at(i);
		if(!index.isValid()) {
    		continue;
		}
		if (i != 0)
			mimeCompleteString.append("\n");

		GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());


		GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(item);
		if( gridItem ) {
    		mimeCompleteString.append("PropPath/"+gridItem->data(0).toString());
    		continue;
		}
		GsTL_property_item* propItem = dynamic_cast<GsTL_property_item*>(item);
		if( propItem ) {
    		GsTL_object_item* parentGridItem = propItem->parent();
    		QString mimeString = QString("PropPath/%1/%2").arg(parentGridItem->data(0).toString(),
															   propItem->data(0).toString());

    		mimeCompleteString.append(mimeString);
    		continue;
		}
	}

	mimeData->setText(mimeCompleteString);
    return mimeData;
}
*/

/*
 void Project_ItemModel::update( std::string obj ){
	 this->layoutAboutToBeChanged();
	 this->layoutChanged();
 }

 void Project_ItemModel::new_object( std::string obj ){
	 this->layoutAboutToBeChanged();
	 this->layoutChanged();
 }

 void Project_ItemModel::deleted_object( std::string obj ){
	 this->layoutAboutToBeChanged();
	 this->layoutChanged();
 }
*/
/*
 bool Project_ItemModel::removeRows(int position, int rows, const QModelIndex &parent){
	 if(!parent.isValid()) return false;
	 beginRemoveRows(parent, position, position+rows-1);
	 GsTL_item* item = static_cast<GsTL_item*>(parent.internalPointer());
	 GsTL_root_item* root_item = dynamic_cast<GsTL_root_item*>(item);
	 if(root_item) {
		 GsTL_grid_item* grid_item =  dynamic_cast<GsTL_grid_item*>(root_item->child(position));
		 grid_item->delete_grid();

	 }
	 endRemoveRows();
	 return true;

 }


void Project_ItemModel::begin_add_item(std::string type_hierarchy,std::string type_name){
	QStringList types = QString(type_hierarchy.c_str()).split("/");
	QStringList names = QString(type_name.c_str()).split("/");
	if(types.isEmpty() || names.isEmpty() || types.size() != names.size()) return;

	QModelIndex insert_parent_index;
	int position;

	// can either add a grid, a group, a property (in the all_group and/or in a specific group) or a region
	QModelIndex root_index = this->root_index();
	if(types.size() == 1) { // Adding a grid, always at the end
		insert_parent_index = root_index;
		position = root_->childCount();
	}
	else {
		QString grid_name = names.at(0);
	    Geostat_grid* grid = dynamic_cast<Geostat_grid*>(
	                  Root::instance()->interface(
	                                              gridModels_manager + "/" + grid_name.toStdString()
	                                              ).raw_ptr()
	                  );
	    if(grid == 0) return;

	    GsTL_root_item *root_item = dynamic_cast<GsTL_root_item*>(root_);
	    GsTL_grid_item * grid_item = new GsTL_grid_item(root_item->project(),  grid, grid_name, root_ );

	    QModelIndex grid_index = root_index.child(grid_item->row(),0);

		if(types.size() == 2) {  // adding a group or a property

			if(types.at(1) == "group") {
				insert_parent_index = grid_index;
				std::string new_group_name = names.at(1).toStdString();
				std::list<std::string>  group_names = grid->get_group_names();
				std::list<std::string>::iterator it = group_names.begin();
				for(int i=0 ; it != group_names.end() ; ++it) {
					if( *it > new_group_name ) {
						position = i;
						break;
					}
				}
			}
			else if(types.at(1) == "property") {
				insert_parent_index = grid_index.child(0,0);
				//Need to add it to the all property group and the specific group
				std::string new_prop_name = names.at(1).toStdString();
				std::list<std::string> props = grid->property_list();
				std::list<std::string>::const_iterator it_prop = props.begin();
				for(int i=0;it_prop != props.end(); ++it_prop, ++i) {
					if(*it_prop > new_prop_name) {
						position = i;
						break;
					}
				}
			}

		}
	}

	this->beginInsertRows(insert_parent_index,position, position);
}

void Project_ItemModel::begin_remove_item(std::string type_hierarchy,std::string type_name){

}

void Project_ItemModel::begin_update_item(std::string type_hierarchy,std::string type_name){

}


void Project_ItemModel::end_add_item(std::string type_hierarchy,std::string type_name){
	QStringList types = QString(type_hierarchy.c_str()).split("/");
	QStringList names = QString(type_name.c_str()).split("/");
	if(types.isEmpty() || names.isEmpty() || types.size() != names.size()) return;
	this->reset();


}

void Project_ItemModel::end_remove_item(std::string type_hierarchy,std::string type_name){

}

void Project_ItemModel::end_update_item(std::string type_hierarchy,std::string type_name){

}


 void Project_ItemModel::delete_object(const QModelIndex &index)
 {
	if(!index.isValid()) return;

	int position = index.row();
	removeRows(position, position, index.parent());

 }

 void Project_ItemModel::add_property_to_grid( Geostat_grid* grid, GsTLGridProperty* prop ){

 }


 void Project_ItemModel::interface_changed(){
	 QModelIndex root_index = this->root_index();
	 emit dataChanged(root_index, root_index);
	 emit layoutChanged();
 }
*/
