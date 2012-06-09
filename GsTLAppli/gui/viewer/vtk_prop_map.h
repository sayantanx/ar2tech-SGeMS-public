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
 * vtk_prop_map.h
 *
 *  Created on: May 13, 2011
 *      Author: aboucher
 */

#ifndef VTK_PROP_MAP_H_
#define VTK_PROP_MAP_H_

#include <GsTLAppli/math/gstlvector.h>
#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/appli/project.h>

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>


#include <vtkRenderer.h>

class GUI_DECL Vtk_prop_map
{
private:
	typedef std::map<std::string, SmartPtr<GsTL_vtkProp> > vtkProp_Map;

public:
	typedef vtkProp_Map::iterator iterator;
	typedef vtkProp_Map::const_iterator const_iterator;
	typedef std::pair<bool, GsTL_vtkProp*> pair;

public:

	Vtk_prop_map(vtkRenderer* vtk_renderer);
	Vtk_prop_map():vtk_renderer_(0){}
	~Vtk_prop_map()
	{
	}

	void set_renderer(vtkRenderer* renderer){vtk_renderer_ = renderer;}
	vtkRenderer* get_renderer(){return vtk_renderer_;}
	//void scene_graph(SoGroup* scene_root);

	/** Gets the description corresponding to object "obj_name". If the description
	 * is not already in the database of descriptions, a new description is created
	 * and added to the database and to the Open Inventor scene-graph.
	 * It returns a pair:
	 *   - a bool which is true if the description was in the map before
	 *   - a pointer to the requested description
	 */
	std::pair<bool, GsTL_vtkProp*> vtk_prop(const std::string& obj_name);

	/** Removes the description of \c obj_name from the database and from the
	 * Open Inventor scene graph. It returns true if the deletion was successful.
	 */
	bool delete_vtk_prop(const std::string& obj_name);

	iterator find(const std::string& obj_name);
	iterator begin()
	{
		return available_props_.begin();
	}
	const_iterator begin() const
	{
		return available_props_.begin();
	}
	iterator end()
	{
		return available_props_.end();
	}
	const_iterator end() const
	{
		return available_props_.end();
	}

private:
	vtkProp_Map available_props_;
	vtkRenderer* vtk_renderer_;
};


#endif /* VTK_PROP_MAP_H_ */
