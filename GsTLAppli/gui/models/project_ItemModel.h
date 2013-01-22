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
 * GsTL_ItemModel.h
 *
 *  Created on: Apr 28, 2011
 *      Author: aboucher
 */

#ifndef GSTL_ITEMMODEL_H_
#define GSTL_ITEMMODEL_H_

#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/models/gstl_item.h>


#include <QModelIndex>
#include <QVariant>
#include <QAbstractItemModel>


class GUI_DECL Project_ItemModel : public GsTL_ItemModel
 {
     Q_OBJECT

 public:

     static Named_interface* create_new_interface(std::string&);

     Project_ItemModel(QObject *parent = 0);
     Project_ItemModel(GsTL_item* rootItem, QObject *parent = 0);

     void setRootItem(GsTL_item* rootItem);

     virtual ~Project_ItemModel();

     virtual QVariant data(const QModelIndex &index, int role) const;
     virtual Qt::ItemFlags flags(const QModelIndex &index) const;
     virtual QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const;
     virtual QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     virtual QModelIndex parent(const QModelIndex &child) const;
     virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
     virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

     virtual bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex());
     virtual bool removeRows(int position, int rows, const QModelIndex &parent);

 	virtual QStringList mimeTypes() const;
 	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;


 	// Needed for GsTL_ItemModel
    virtual void begin_add_item(std::string type_hierarchy, std::string type_name);
    virtual void begin_remove_item(std::string type_hierarchy, std::string type_name);
    virtual void begin_update_item(std::string type_hierarchy, std::string type_name);

    virtual void end_add_item(std::string type_hierarchy, std::string type_name);
    virtual void end_remove_item(std::string type_hierarchy, std::string type_name);
    virtual void end_update_item(std::string type_hierarchy, std::string type_name);

    QModelIndex grid_index(QString gridName);
    QModelIndex root_index();



public slots :
	void delete_object(const QModelIndex &index);
	virtual void interface_changed();

protected :

	void add_property_to_grid( Geostat_grid* grid, GsTLGridProperty* prop );

 protected:
     GsTL_item *rootItem_;
//     QList<Gstl_item_action*> grid_actions_;
//     QList<Gstl_item_action*> property_actions_;
//     QList<Gstl_item_action*> group_actions_;
//     QList<Gstl_item_action*> region_actions_;
 };

#endif /* GSTL_ITEMMODEL_H_ */
