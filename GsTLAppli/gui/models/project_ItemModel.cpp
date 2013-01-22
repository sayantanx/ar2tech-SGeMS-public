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
 * GsTL_ItemModel.cpp
 *
 *  Created on: Apr 28, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/models/project_ItemModel.h>

#include <QStringList>
#include <QMimeData>
#include <QIcon>

Named_interface* Project_ItemModel::create_new_interface(std::string&){
	return new Project_ItemModel;
}

Project_ItemModel::Project_ItemModel(QObject *parent)
	:GsTL_ItemModel(parent), rootItem_(0)
{
}


Project_ItemModel::Project_ItemModel(GsTL_item* rootItem, QObject *parent)
	:GsTL_ItemModel(parent), rootItem_(rootItem)
{
}

Project_ItemModel::~Project_ItemModel()
{
	delete rootItem_;
}


void Project_ItemModel::setRootItem(GsTL_item* rootItem){
	rootItem_ = rootItem;

}


int Project_ItemModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

Qt::ItemFlags Project_ItemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}



QModelIndex Project_ItemModel::index(int row, int column, const QModelIndex &parent)
            const
{
	/*
	if(!parent.isValid()) {
		return createIndex(0,0,rootItem_);
	}
*/
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    GsTL_item *parentItem;

    if (!parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<GsTL_item*>(parent.internalPointer());

    GsTL_item *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}


int Project_ItemModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    GsTL_item *parentItem;

    if (!parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<GsTL_item*>(parent.internalPointer());

    return parentItem->childCount();
}


QModelIndex Project_ItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    GsTL_item *childItem = static_cast<GsTL_item*>(child.internalPointer());
    GsTL_item *parentItem = childItem->parent();

    if (!parentItem )// || parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}




QVariant
Project_ItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant Project_ItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
    	QIcon icon;
    	GsTL_item *item = static_cast<GsTL_item*>(index.internalPointer());
    	QString type = item->data(1).toString();
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
    	else if(type == "Categorical") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cat_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Continuous") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cont_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	return icon;
    }

    if( role == Qt::DisplayRole ) {
    	GsTL_item *item = static_cast<GsTL_item*>(index.internalPointer());
    	return item->data(index.column());
    }

    return QVariant();
}


QModelIndex Project_ItemModel::grid_index(QString grid_name){

	int childCount = rootItem_->childCount();
	for(int row=0; row<childCount; ++row) {
		GsTL_item* grid_item = rootItem_->child(row);
		if(grid_item->data(0).toString() != grid_name) continue;

		return createIndex(row, 0, grid_item);
	}

	return QModelIndex();
}

QModelIndex Project_ItemModel::root_index(){
	return createIndex(0, 0, rootItem_);
}

QStringList Project_ItemModel::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}

QMimeData *Project_ItemModel::mimeData(const QModelIndexList &indexes) const
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

		GsTL_item* item = static_cast<GsTL_item*>(index.internalPointer());


		GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(item);
		if( gridItem ) {
    		mimeCompleteString.append("PropPath/"+gridItem->data(0).toString());
    		continue;
		}
		GsTL_property_item* propItem = dynamic_cast<GsTL_property_item*>(item);
		if( propItem ) {
    		GsTL_item* parentGridItem = propItem->parent();
    		QString mimeString = QString("PropPath/%1/%2").arg(parentGridItem->data(0).toString(),
															   propItem->data(0).toString());

    		mimeCompleteString.append(mimeString);
    		continue;
		}
	}

	mimeData->setText(mimeCompleteString);
    return mimeData;
}
bool Project_ItemModel::insertRows( int row, int count, const QModelIndex & parent){
	return true;
}

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

 bool Project_ItemModel::removeRows(int position, int rows, const QModelIndex &parent){
	 if(!parent.isValid()) return false;
	 beginRemoveRows(parent, position, position+rows-1);
	 GsTL_item* item = static_cast<GsTL_item*>(parent.internalPointer());
	 GsTL_root_item* root_item = dynamic_cast<GsTL_root_item*>(item);
	 if(root_item) {
		 GsTL_grid_item* grid_item =  dynamic_cast<GsTL_grid_item*>(root_item->child(position));
		 grid_item->delete_grid();
/*
		std::string obj_name = grid_item->gridName().toStdString();
		bool ok = Root::instance()->delete_interface(gridModels_manager + "/" + obj_name);
		if (ok)
		{
			SmartPtr<Named_interface> ni = Root::instance()->interface(projects_manager + "/" + "project");
			GsTL_project* proj = dynamic_cast<GsTL_project*> (ni.raw_ptr());
			proj->deleted_object(obj_name);
		}
		*/
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
		position = rootItem_->childCount();
	}
	else {
		QString grid_name = names.at(0);
	    Geostat_grid* grid = dynamic_cast<Geostat_grid*>(
	                  Root::instance()->interface(
	                                              gridModels_manager + "/" + grid_name.toStdString()
	                                              ).raw_ptr()
	                  );
	    if(grid == 0) return;

	    GsTL_root_item *root_item = dynamic_cast<GsTL_root_item*>(rootItem_);
	    GsTL_grid_item * grid_item = new GsTL_grid_item(root_item->project(),  grid, grid_name, rootItem_ );

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
