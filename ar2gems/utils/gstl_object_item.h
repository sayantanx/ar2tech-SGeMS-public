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
 * gstl_object_item.h
 *
 *  Created on: Jun 3, 2011
 *      Author: aboucher
 */

#ifndef GSTL_OBJECT_ITEM_H_
#define GSTL_OBJECT_ITEM_H_

#include <QVariant>
#include <QString>

class GsTL_object_item {

public:


	GsTL_object_item(): parent_(0){}
	GsTL_object_item(GsTL_object_item* parent):parent_(parent){}

	//Access to the data, usefull if the GsTL_object_item is a wrapper over the actual data
	virtual void* data_pointer(){return static_cast<void*>(this);}

	virtual void set_parent_item(GsTL_object_item* parent) {parent_ = parent;}

    virtual GsTL_object_item *child(int row){return 0;}
    virtual const GsTL_object_item *child(int row) const {return 0;}
    virtual int childCount() const{return 0;}
    virtual int columnCount() const{return 2;}
    virtual QVariant item_data(int column) const{
    	if( column == 0 ) return item_name_;
    	else if ( column == 1 ) return item_type_;
    	else return QVariant();
    }
    virtual int row() const{
    	if(this->parent() == 0) return 0;
    	for(int i=0;i<this->parent()->childCount(); ++i) {
    		if(this->parent()->child(i) == this) return i;
    	}
    	return 0;
    }
    virtual GsTL_object_item *parent() {return parent_;}
    virtual const GsTL_object_item *parent()const{return parent_;}
    void item_name(QString name) {item_name_=name;}
    void item_type(QString type) {item_type_=type;}
    virtual QString item_type() const {return item_type_;}
    virtual QString item_name() const {return item_name_;}

    //The base class is not editable
    virtual Qt::ItemFlags flags(int column=0) const {return  Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled;}
    virtual bool set_item_data(QVariant value, int column= 0 ){return false;}

protected :
    GsTL_object_item* parent_;
    QString item_name_;
    QString item_type_;

};

#endif /* GSTL_OBJECT_ITEM_H_ */
