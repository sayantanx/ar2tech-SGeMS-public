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



#ifndef GSTL_ROOT_ITEM_H
#define GSTL_ROOT_ITEM_H

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>
#include <QList>
#include <QVariant>


class GUI_DECL GsTL_item
{
public:
    GsTL_item();
    GsTL_item(GsTL_item* parentItem):parentItem_(parentItem){}

    virtual QString type()const=0;

    virtual ~GsTL_item(){}

    virtual GsTL_item *child(int row) = 0;
    virtual int childCount() const=0;
    virtual int columnCount() const=0;
    virtual QVariant data(int column) const=0;
    virtual int row() const=0;
    GsTL_item *parent(){return parentItem_;}

protected:
    GsTL_item *parentItem_;

};


class GUI_DECL GsTL_root_item : public GsTL_item
{
public:
    GsTL_root_item():GsTL_item(0){}

    GsTL_root_item(GsTL_project* proj);
    virtual ~GsTL_root_item();

    virtual QString type()const {return "Root";}

    virtual GsTL_item *child(int row);
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual int row() const;

    virtual GsTL_project* project(){return proj_;}
 //   virtual GsTL_item *parent(){return 0;}

private:
    GsTL_project* proj_;

};


class GUI_DECL GsTL_grid_item : public GsTL_item
{
public:
    GsTL_grid_item();

    GsTL_grid_item(GsTL_project* proj, Geostat_grid* grid, QString name, GsTL_item *parent);
    virtual ~GsTL_grid_item();

    virtual QString type()const {return "Grid";}

    virtual GsTL_item *child(int row);
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual int row() const;
//    virtual GsTL_item *parent();

    Geostat_grid* grid(){return grid_;}
    QString gridName(){return gridName_;}

public slots :
	void delete_grid();

private:
    Geostat_grid* grid_;
    QString gridName_;
    GsTL_project* proj_;

};

class GUI_DECL GsTL_group_property_item : public GsTL_item
{
public:
	GsTL_group_property_item();

	GsTL_group_property_item(GsTLGridPropertyGroup* group, GsTL_item *parent = 0);
    virtual ~GsTL_group_property_item();

    virtual QString type()const {return "GroupProperty";}

    virtual GsTL_item *child(int row);
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual int row() const;

    QString gridName() const {return gridName_;}
    GsTLGridPropertyGroup* group();
    Geostat_grid* grid(){return grid_;}

private:
    Geostat_grid* grid_;
    GsTLGridPropertyGroup* group_;
    QString gridName_;
};

class GUI_DECL GsTL_group_all_property_item : public GsTL_item
{
public:
	GsTL_group_all_property_item();

	GsTL_group_all_property_item(Geostat_grid* grid, GsTL_item *parent = 0);
//	GsTL_group_all_property_item(GsTL_item *parent = 0);
    virtual ~GsTL_group_all_property_item();

    virtual QString type()const {return "AllProperties";}


    virtual GsTL_item *child(int row);
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual int row() const;

    QString gridName() const;
    Geostat_grid* grid();

private:
    Geostat_grid* grid_;
    QString gridName_;
};

class GUI_DECL GsTL_property_item : public GsTL_item
{
public:
    GsTL_property_item();

    GsTL_property_item(GsTLGridProperty* prop, GsTL_item *parent = 0);
    virtual ~GsTL_property_item();

    virtual QString type() const;


    virtual GsTL_item *child(int row);
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual int row() const;

    QString gridName() const;
    GsTLGridProperty* property();
    Geostat_grid* grid();

private:
    Geostat_grid* grid_;
    GsTLGridProperty* prop_;
    QString gridName_;
};



class GUI_DECL GsTL_group_region_item : public GsTL_item
{
public:
	GsTL_group_region_item();

	GsTL_group_region_item(Geostat_grid* grid, GsTL_item *parent = 0);
//	GsTL_group_region_item(GsTL_item *parent = 0);
    virtual ~GsTL_group_region_item();

    virtual QString type()const {return "GroupRegion";}

    virtual GsTL_item *child(int row);
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual int row() const;

    QString gridName() const;
    Geostat_grid* grid();


private:
    Geostat_grid* grid_;
    QString gridName_;
};

class GUI_DECL GsTL_region_item : public GsTL_item
{
public:
	GsTL_region_item();

	GsTL_region_item(GsTLGridRegion* prop, GsTL_item *parent = 0);
    virtual ~GsTL_region_item();

    virtual QString type()const {return "Region";}


    virtual GsTL_item *child(int row);
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant data(int column) const;
    virtual int row() const;

    QString gridName() const;
    GsTLGridRegion* region();


private:
    GsTLGridRegion* region_;
    QString gridName_;
    Geostat_grid* grid_;
};

#endif // GSTL_ITEM_H
