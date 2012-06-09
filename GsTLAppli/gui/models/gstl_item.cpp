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




#include <GsTLAppli/gui/models/gstl_item.h>

#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>

#include <QStringList>

/*
GsTL_item::GsTL_item()
{
}

*/


/*
   GsTL_root_item
*/


GsTL_root_item::GsTL_root_item(GsTL_project* proj )
    :GsTL_item(0),proj_(proj)
{

}

GsTL_root_item::~GsTL_root_item(){}


GsTL_item *GsTL_root_item::child(int row){

    std::list<std::string>::const_iterator it = proj_->objects_list().begin();
    for(int i=0; i< row; ++i, ++it){}

    Geostat_grid* grid = dynamic_cast<Geostat_grid*>(
                  Root::instance()->interface(
                                              gridModels_manager + "/" + *it
                                              ).raw_ptr()
                  );

 //return 0;
    std::string n = *it;
    return new GsTL_grid_item(proj_, grid, QString::fromStdString(*it),this);
}

int GsTL_root_item::childCount() const{
	if(proj_==0) return 0;
    return proj_->objects_list().size();
}

// There are two columns
// 1 - name of the grid
// 2 - type of grid
int GsTL_root_item::columnCount() const{
    return 2;
}


QVariant GsTL_root_item::data(int column) const{
    return QVariant();
}

int GsTL_root_item::row() const{
    return 0;
}


//  --------------------------------

//   GsTL_grid_item

//  --------------------------------


GsTL_grid_item::GsTL_grid_item():GsTL_item(0){}

GsTL_grid_item::GsTL_grid_item(GsTL_project* proj, Geostat_grid* grid, QString name, GsTL_item *parent)
    :GsTL_item(parent),proj_(proj), grid_(grid),gridName_(name)
{
}

GsTL_grid_item::~GsTL_grid_item(){}


// The first child is all the property
// the last child if the region
GsTL_item *GsTL_grid_item::child(int row){
	if(row == 0)
		return new GsTL_group_all_property_item(grid_,this);


    std::list<std::string> groups = grid_->get_group_names();

    if(row < groups.size()+1) {
		std::list<std::string>::const_iterator it = groups.begin();
		for(int i=0; i<(row-1); ++i, ++it){}
		GsTLGridPropertyGroup* group = grid_->get_group( *it );
		return new GsTL_group_property_item(group, this);
    }

    return new GsTL_group_region_item(grid_, this);

}

//Return the number of groups +2 (all props and regions)
int GsTL_grid_item::childCount() const{
    std::list<std::string> groups = grid_->get_group_names();
    return groups.size()+2;

}

// There are two columns
// 1 - name of the grid
// 2 - type of grid
int GsTL_grid_item::columnCount() const{
    return 2;
}


QVariant GsTL_grid_item::data(int column) const{
    if( column == 0 ) return gridName_;
    if( column == 1) return QString::fromStdString(grid_->classname());
    else return QVariant();
}

int GsTL_grid_item::row() const{
	std::list<std::string>::const_iterator it = proj_->objects_list().begin();
	for(int i=0; it != proj_->objects_list().end(); ++i, ++it){
		if(*it == gridName_.toStdString()) return i;
	}
	return 0;
}

void GsTL_grid_item::delete_grid(){
	std::string grid_name = gridName_.toStdString();
	bool ok = Root::instance()->delete_interface(gridModels_manager + "/" + grid_name);
	if (ok)
	{
		proj_->deleted_object(grid_name);
	}
}


//
//   GsTL_group_property_item
//

GsTL_group_property_item::GsTL_group_property_item():GsTL_item(0){}

GsTL_group_property_item::GsTL_group_property_item(GsTLGridPropertyGroup* group, GsTL_item *parent)
    :GsTL_item(parent),group_(group), grid_(0)
{
    if(parent) {
        GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(parentItem_);
        assert(gridItem);
        if(gridItem) {
            gridName_ = gridItem->gridName();
            grid_ = gridItem->grid();
        }
    }

}

GsTL_group_property_item::~GsTL_group_property_item(){}


GsTL_item *GsTL_group_property_item::child(int row){
	GsTLGridPropertyGroup::property_map::const_iterator it = group_->begin_property();
	for(int i=0; i< row; ++i,++it) {}
	return new GsTL_property_item(it->second, this);
}

int GsTL_group_property_item::childCount() const{
	return group_->size();
}

// There are two columns
// 1 - name of the property
// 2 - type of property
int GsTL_group_property_item::columnCount() const{
    return 2;
}


QVariant GsTL_group_property_item::data(int column) const{
    if( column == 0 ) return QString::fromStdString(group_->name());
    if( column == 1) {
    	return QString::fromStdString(group_->type());
    }

    else return QVariant();
}

int GsTL_group_property_item::row() const{
    if(!parentItem_) return 0;

    GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(parentItem_);
    assert(gridItem);
    if(gridItem == 0) return 0;

    Geostat_grid* grid = gridItem->grid();
    std::list<std::string> groups = grid->get_group_names();
    std::list<std::string>::const_iterator it = groups.begin();
    for(int i=0; it != groups.end(); ++i, ++it) {
        if( *it == group_->name() ) {
            return i+1;
        }
    }
    return 0;
}

GsTLGridPropertyGroup* GsTL_group_property_item::group() {
	return group_;
}


//
//   GsTL_all_property_item
// Contains all the property of a grid.  This is the first row and is not repeated
//

GsTL_group_all_property_item::GsTL_group_all_property_item():GsTL_item(0){}

/*
GsTL_group_all_property_item::GsTL_group_all_property_item(GsTL_item *parent)
    :GsTL_item(parent), grid_(0)
{
    if(parent) {
        GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(parentItem_);
        assert(gridItem);
        if(gridItem) {
            gridName_ = gridItem->gridName();
            grid_ = gridItem->grid();
        }
    }

}
*/

GsTL_group_all_property_item::GsTL_group_all_property_item(Geostat_grid* grid,GsTL_item *parent)
    :GsTL_item(parent), grid_(grid)
{
    if(parent) {
        GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(parentItem_);
        if(gridItem) {
            gridName_ = gridItem->gridName();
        }
    }
    else {
    	// Find the name of the grid

    }

}

GsTL_group_all_property_item::~GsTL_group_all_property_item(){}


GsTL_item *GsTL_group_all_property_item::child(int row){
    std::list<std::string> props = grid_->property_list();

    std::list<std::string>::const_iterator it = props.begin();
    for( int i=0; i<row; i++, ++it){}
    return new GsTL_property_item( grid_->property(*it),this);
}

int GsTL_group_all_property_item::childCount() const{
    std::list<std::string> props = grid_->property_list();
    return props.size();
}

// There are two columns
// 1 - name of the property
// 2 - type of property
int GsTL_group_all_property_item::columnCount() const{
    return 2;
}


QVariant GsTL_group_all_property_item::data(int column) const{
    if( column == 0 ) return QString("Properties");

    else return QVariant();
}

int GsTL_group_all_property_item::row() const{

    return 0;
}

QString GsTL_group_all_property_item::gridName() const{
	return gridName_;
}
Geostat_grid* GsTL_group_all_property_item::grid(){
	return grid_;
}

//
//   GsTL_property_item
//

GsTL_property_item::GsTL_property_item():GsTL_item(0){}

GsTL_property_item::GsTL_property_item(GsTLGridProperty* prop, GsTL_item *parent)
    :GsTL_item(parent),prop_(prop), grid_(0)
{
    if(parentItem_) {
    	GsTL_group_all_property_item* group_all_item = dynamic_cast<GsTL_group_all_property_item*>(parentItem_);
        if(group_all_item) {
            gridName_ = group_all_item->gridName();
            grid_ = group_all_item->grid();

        }
        else {
        	GsTL_group_property_item* group_item = dynamic_cast<GsTL_group_property_item*>(parentItem_);
            if(group_item) {
                gridName_ = group_item->gridName();
                grid_ = group_item->grid();
            }
        }
    }

}

GsTL_property_item::~GsTL_property_item(){}

QString GsTL_property_item::type()const{
    GsTLGridCategoricalProperty* cprop = dynamic_cast<GsTLGridCategoricalProperty*>(prop_);
    if(cprop) return QString("Categorical_property");
    else return QString("Continuous_property");

}

GsTL_item *GsTL_property_item::child(int row){
	return 0;
}

int GsTL_property_item::childCount() const{
	return 0;
}

// There are two columns
// 1 - name of the property
// 2 - type of property
int GsTL_property_item::columnCount() const{
    return 2;
}


QVariant GsTL_property_item::data(int column) const{
    if( column == 0 ) return QString::fromStdString(prop_->name());
    if( column == 1) {
        GsTLGridCategoricalProperty* cprop = dynamic_cast<GsTLGridCategoricalProperty*>(prop_);
        if(cprop) return QString("Categorical");
        else return QString("Continuous");
    }

    else return QVariant();
}

int GsTL_property_item::row() const{
    if(!parentItem_) return 0;


	GsTL_group_all_property_item* group_all_item = dynamic_cast<GsTL_group_all_property_item*>(parentItem_);
    if(group_all_item) {
		Geostat_grid* grid = group_all_item->grid();
		std::list<std::string> props = grid->property_list();
		std::list<std::string>::const_iterator it = props.begin();
		for(int i=0; it != props.end(); ++i, ++it) {
			if( *it == prop_->name() ) {
				return i;
			}
		}
    }

	GsTL_group_property_item* group_item = dynamic_cast<GsTL_group_property_item*>(parentItem_);
	if(group_item) {
		GsTLGridPropertyGroup* group = group_item->group();
		std::vector<std::string> props = group->property_names();
		std::vector<std::string>::const_iterator it = props.begin();
		for(int i=0; it != props.end(); ++i, ++it) {
			if( *it == prop_->name() ) {
				return i;
			}
		}
	}


    return 0;
}

QString GsTL_property_item::gridName() const{
	return gridName_;
}
GsTLGridProperty* GsTL_property_item::property(){
	return prop_;
}

Geostat_grid* GsTL_property_item::grid(){
  return grid_;
}

//
//   GsTL_group_region_item
// Contains all the regions of a grid.  This is the last row and is not repeated
//

GsTL_group_region_item::GsTL_group_region_item():GsTL_item(0){}
/*
GsTL_group_region_item::GsTL_group_region_item(GsTL_item *parent)
    :GsTL_item(parent), grid_(0)
{
    if(parent) {
        GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(parentItem_);
        if(gridItem) {
            gridName_ = gridItem->gridName();
            grid_ = gridItem->grid();
        }
    }

}
*/

GsTL_group_region_item::GsTL_group_region_item(Geostat_grid* grid,GsTL_item *parent)
    :GsTL_item(parent), grid_(grid)
{
    if(parent) {
        GsTL_grid_item* gridItem = dynamic_cast<GsTL_grid_item*>(parentItem_);
        if(gridItem) {
            gridName_ = gridItem->gridName();
        }
    }
    else {
    	// Find the name of the grid

    }

}

GsTL_group_region_item::~GsTL_group_region_item(){}


GsTL_item *GsTL_group_region_item::child(int row){
    std::list<std::string> regions = grid_->region_list();

    std::list<std::string>::const_iterator it = regions.begin();
    for( int i=0; i<row; i++, ++it){}
    return new GsTL_region_item( grid_->region(*it),this);
}

int GsTL_group_region_item::childCount() const{
    std::list<std::string> regions = grid_->region_list();
    return regions.size();
}

// There are two columns
// 1 - name of the property
// 2 - type of property
int GsTL_group_region_item::columnCount() const{
    return 2;
}


QVariant GsTL_group_region_item::data(int column) const{
    if( column == 0 ) return QString("Regions");

    else return QVariant();
}

int GsTL_group_region_item::row() const{
    std::list<std::string> groups = grid_->get_group_names();
    return groups.size()+1;
}

QString GsTL_group_region_item::gridName() const{
	return gridName_;
}
Geostat_grid* GsTL_group_region_item::grid(){
	return grid_;
}

//
//   GsTL_region_item
//

GsTL_region_item::GsTL_region_item():GsTL_item(0){}

GsTL_region_item::GsTL_region_item(GsTLGridRegion* region, GsTL_item *parent)
    :GsTL_item(parent),region_(region), grid_(0)
{
    if(parent) {
    	GsTL_group_region_item* parentItem = dynamic_cast<GsTL_group_region_item*>(parentItem_);
        if(parentItem) {
            gridName_ = parentItem->gridName();
            grid_ = parentItem->grid();
        }
    }

}

GsTL_region_item::~GsTL_region_item(){}


GsTL_item *GsTL_region_item::child(int row){
	return 0;

}

int GsTL_region_item::childCount() const{
	return 0;
}

// There are two columns
// 1 - name of the region
// 2 - type of property
int GsTL_region_item::columnCount() const{
    return 2;
}


QVariant GsTL_region_item::data(int column) const{
    if( column == 0 ) return QString::fromStdString(region_->name());
    return QVariant();
}

int GsTL_region_item::row() const{
    if(!parentItem_) return 0;

    GsTL_group_region_item* group_region_Item = dynamic_cast<GsTL_group_region_item*>(parentItem_);
    if(group_region_Item == 0) return 0;

    Geostat_grid* grid = group_region_Item->grid();
    std::list<std::string> regions = grid->region_list();
    std::list<std::string>::const_iterator it = regions.begin();
    for(int i=0; it != regions.end(); ++i, ++it) {
        if( *it == region_->name() ) {
            return i;
        }
    }
    return 0;
}

QString GsTL_region_item::gridName() const{
	return gridName_;
}
GsTLGridRegion* GsTL_region_item::region(){
	return region_;
}

