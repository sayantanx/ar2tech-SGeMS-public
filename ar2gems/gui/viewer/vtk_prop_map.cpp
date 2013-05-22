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
 * vtk_prop_map.cpp
 *
 *  Created on: May 13, 2011
 *      Author: aboucher
 */

#include <gui/viewer/vtk_prop_map.h>

#include <utils/gstl_messages.h>
#include <appli/manager_repository.h>
#include <grid/grid_model/geostat_grid.h>


Vtk_prop_map::Vtk_prop_map(vtkRenderer* vtk_renderer):
    vtk_renderer_(vtk_renderer){
}

std::pair<bool, GsTL_vtkProp*> Vtk_prop_map::vtk_prop(const std::string& obj_name)
{
	bool already_there = true;
	vtkProp_Map::iterator it = available_props_.find(obj_name);

	if (it == available_props_.end())
	{
		already_there = false;

		SmartPtr<Named_interface> model = Root::instance()->interface(gridModels_manager + "/" + obj_name);

		Geostat_grid* grid_obj = dynamic_cast<Geostat_grid*> (model.raw_ptr());
		if (!grid_obj)
			return std::make_pair(false, (GsTL_vtkProp*) 0);

		SmartPtr<Named_interface> vtk_prop_ni = Root::instance()->new_interface(grid_obj->classname(), vtkProps_manager + "/");

		GsTL_vtkProp* vtk_prop = dynamic_cast<GsTL_vtkProp*> (vtk_prop_ni.raw_ptr());
		appli_assert(vtk_prop != 0);

		vtk_prop->init(grid_obj, vtk_renderer_);

		/*
	// TODO AB: Should I pass a pointer to the vtkRenderer?
		if( vtk_prop->prop()->IsA("vtkActor") )
		  vtk_renderer_->AddActor(vtkActor::SafeDownCast(vtk_prop->prop()));
		else if( vtk_prop->prop()->IsA("vtkVolume") )
		  vtk_renderer_->AddVolume(vtkVolume::SafeDownCast(vtk_prop->prop()));
*/
			std::pair<vtkProp_Map::iterator, bool> ins = available_props_.insert(std::make_pair(obj_name, SmartPtr<GsTL_vtkProp> (vtk_prop)));

			appli_assert(ins.second == true);
			it = ins.first;
	}

	return std::make_pair(already_there, it->second.raw_ptr());
}

bool Vtk_prop_map::delete_vtk_prop(const std::string& obj_name)
{
  vtkProp_Map::iterator it = available_props_.find(obj_name);

	if (it == available_props_.end())
		return false;

	available_props_.erase(it);

	return true;
}

Vtk_prop_map::iterator Vtk_prop_map::find(const std::string& obj_name)
{
	return available_props_.find(obj_name);
}
