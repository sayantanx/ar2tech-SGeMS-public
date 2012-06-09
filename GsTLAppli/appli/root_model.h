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
 * root_model.h
 *
 *  Created on: Jun 6, 2011
 *      Author: aboucher
 */

#ifndef ROOT_MODEL_H_
#define ROOT_MODEL_H_

#include <QAbstractItemModel>
#include <GsTLAppli/utils/gstl_object_item.h>
#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/utils/gstl_item_model.h>
//#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/common.h>



#include <QModelIndex>
#include <QVariant>
#include <QAbstractItemModel>

class APPLI_DECL Root_model : public GsTL_ItemModel { //QAbstractItemModel, Named_interface {

	Q_OBJECT
public:

    static Named_interface* create_new_interface(std::string&);

    Root_model(QObject *parent = 0);

    virtual ~Root_model();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual void begin_insert_item(GsTL_object_item* parent, int position);
    virtual void end_insert_item();

    virtual void begin_remove_item(GsTL_object_item* parent, int position);
    virtual void end_remove_item();
//    virtual bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex());
//   virtual bool removeRows(int position, int rows, const QModelIndex &parent);

//	virtual QStringList mimeTypes() const;
//	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

    QModelIndex project_root_index() const;
    QModelIndex grid_index(QString grid_name) const; 
    QModelIndex property_root_index(QString grid_name) const;
    QModelIndex region_root_index(QString grid_name) const;
    QModelIndex top_level_manager_index(QString manager_name) const;
    QModelIndex manager_index(QString manager_name) const;


private :
	GsTL_object_item* root_;

};

#endif /* ROOT_MODEL_H_ */
