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
 * gstl_item_model.h
 *
 *  Created on: Jun 7, 2011
 *      Author: aboucher
 */

#ifndef GSTL_ITEM_MODEL_H_
#define GSTL_ITEM_MODEL_H_

#include <GsTLAppli/utils/named_interface.h>

#include <QModelIndex>
#include <QVariant>
#include <QAbstractItemModel>


class UTILS_DECL GsTL_ItemModel : public QAbstractItemModel, public Named_interface
 {
     Q_OBJECT

 public:

     static Named_interface* create_new_interface(std::string&);

     GsTL_ItemModel(QObject *parent = 0) :QAbstractItemModel(parent), Named_interface(){}

     virtual ~GsTL_ItemModel(){}

     virtual QVariant data(const QModelIndex &index, int role) const {return QVariant();}
     virtual Qt::ItemFlags flags(const QModelIndex &index) const {return 0;}
     virtual QVariant headerData(int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole) const {return QVariant();}
     virtual QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const {return QModelIndex();}
     virtual QModelIndex parent(const QModelIndex &child) const {return QModelIndex();}
     virtual int rowCount(const QModelIndex &parent = QModelIndex()) const {return 0;}
     virtual int columnCount(const QModelIndex &parent = QModelIndex()) const {return 0;}

     virtual void begin_insert_item(GsTL_object_item* parent, int position){}
     virtual void end_insert_item(){}

     virtual void begin_remove_item(GsTL_object_item* parent, int position){}
     virtual void end_remove_item(){}

     virtual void begin_add_item(std::string type_hierarchy, std::string type_name){}
     virtual void begin_remove_item(std::string type_hierarchy, std::string type_name){}
     virtual void begin_update_item(std::string type_hierarchy, std::string type_name){}

     virtual void end_add_item(std::string type_hierarchy, std::string type_name){}
     virtual void end_remove_item(std::string type_hierarchy, std::string type_name){}
     virtual void end_update_item(std::string type_hierarchy, std::string type_name){}

public slots:
	virtual void interface_changed(){}


 };


#endif /* GSTL_ITEM_MODEL_H_ */
