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



#ifndef __GSTLAPPLI_GRID_REGION_TEMP_SELECTOR_H__ 
#define __GSTLAPPLI_GRID_REGION_TEMP_SELECTOR_H__ 

#include <grid/grid_model/geostat_grid.h>
#include <grid/grid_model/grid_region.h>
#include <grid/grid_model/grid_property.h>

/* This class temporary set a new region as 
* active and region and revert back
* to the original region when the destructor is
* called
*/
class Temporary_gridRegion_Selector {
public :
  Temporary_gridRegion_Selector():region_name_(""), grid_(NULL){}

  Temporary_gridRegion_Selector(std::string new_region_name,
    Geostat_grid* grid) {
      set_temporary_region(new_region_name, grid);
  }

  bool set_temporary_region(std::string new_region_name,
    Geostat_grid* grid)  {
      grid_ = grid;
      Grid_region* reg = grid_->selected_region();
      if(!reg)
        region_name_ = "";
      else 
        region_name_ = reg->name();
      return grid_->select_region( new_region_name );
  }

  // If region_name_ is empty, the region will be unselected,
  // see region_manager
  ~Temporary_gridRegion_Selector() {
    if(grid_) grid_->select_region( region_name_ );
  }

protected:
  std::string region_name_;
  Geostat_grid* grid_;

};
 

class Temporary_propRegion_Selector {
public :
  Temporary_propRegion_Selector():region_(NULL), prop_(NULL){}

  Temporary_propRegion_Selector(const Grid_region* new_region,
    Grid_continuous_property* prop) {
      set_temporary_region(new_region, prop);
  }


  void set_temporary_region(const Grid_region* new_region,
    Grid_continuous_property* prop)  {
      prop_ = prop;
      region_ = prop_->get_region();
      prop_->set_region( new_region );
  }


  // If region_name_ is empty, the region will be unselected,
  // see region_manager
  ~Temporary_propRegion_Selector() {
    if(prop_) prop_->set_region( region_ );
  }

protected:
  const Grid_region* region_;
  Grid_continuous_property* prop_;

};

#endif