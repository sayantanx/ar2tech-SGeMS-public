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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>


Geostat_grid::Geostat_grid(std::string name) :
	 name_(name),coord_mapper_(0){}

Neighborhood* Geostat_grid::colocated_neighborhood( const std::string& prop ) {
  return new Colocated_neighborhood( this, prop );
}

Neighborhood* Geostat_grid::colocated_neighborhood( ) {
  return new Colocated_neighborhood( this );
}

int Geostat_grid::row() const{
	if(parent_interface_ == 0) return 0;
	Manager* parent  = dynamic_cast<Manager*>(parent_interface_);
	if(parent==0) return 0;
	Manager::interface_iterator begin = parent->begin_interfaces();
	Manager::interface_iterator end = parent->end_interfaces();

	for(int i=0 ; begin != end; ++begin, ++i) {
		if( begin->raw_ptr() == this) {
			return i;
		}
	}
	return 0;
}

/*
 *
 */

/*
int Geostat_grid::columnCount(const QModelIndex &) const
{
    return 1;
}


int Geostat_grid::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    GsTL_object_item *parentItem;

    if (!parent.isValid()) {
        return this->childCount();
    }
    else {
    	GsTL_object_item* parentItem = static_cast<GsTL_object_item*>(parent.internalPointer());
        return parentItem  ? parentItem->childCount() :0;
    }

    return parentItem->childCount();
}

Qt::ItemFlags Geostat_grid::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;


}



QModelIndex Geostat_grid::index(int row, int column, const QModelIndex &parent)
            const
{
	if(!parent.isValid()) {
		createIndex(0,0,(void*)this);
	}

    if (!hasIndex(row, column, parent))
        return QModelIndex();

    GsTL_object_item *parentItem;

    if (!parent.isValid())
        parentItem = parent_;
    else
        parentItem = static_cast<GsTL_object_item*>(parent.internalPointer());

    GsTL_object_item *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

// Need to be transfered into the implementation


QModelIndex Geostat_grid::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    GsTL_object_item *childItem = static_cast<GsTL_object_item*>(child.internalPointer());
    GsTL_object_item *parentItem = childItem->parent();

    if (!parentItem )// || parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}




QVariant
Geostat_grid::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant Geostat_grid::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole) {
    	QIcon icon;
    	GsTL_object_item *item = static_cast<GsTL_object_item*>(index.internalPointer());
    	QString type = item->item_data(1).toString();
    	if(type == "Categorical") {
    		icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/cat_property.svg"), QSize(), QIcon::Normal, QIcon::Off);
    	}
    	else if(type == "Continuous") {
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





QStringList Geostat_grid::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}
*/
/*
QMimeData *Geostat_grid::mimeData(const QModelIndexList &indexes) const
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
*/
