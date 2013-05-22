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
 * visualization_profile.cpp
 *
 *  Created on: May 11, 2011
 *      Author: aboucher
 */

#include "visualization_profile.h"

Named_interface* create_new_interface(std::string&){
	return new VisualizationProfile;
}

VisualizationProfile::VisualizationProfile() {
	// TODO Auto-generated constructor stub

}

VisualizationProfile::~VisualizationProfile() {
	// TODO Auto-generated destructor stub
}

bool VisualizationProfile::
register_colormap( std::string grid_name, std::string prop_name,
					  Colormap* table ){
	std::string key = grid_name+"/"+prop_name;
	cmaps_[key] = table;
	return true;
}

bool VisualizationProfile::
unregister_colormap( std::string grid_name, std::string prop_name){
	std::string key = grid_name+"/"+prop_name;
	cmaps_.erase(key);
	return true;
}

Colormap* VisualizationProfile::
get_colormap(std::string grid_name, std::string prop_name){
	std::string key = grid_name+"/"+prop_name;
	std::map<std::string, Colormap*>::iterator it = cmaps_.find(key);
	if(it == cmaps_.end())
		return 0;
	return it->second;
}

bool VisualizationProfile::initialize_from_dom(QDomDocument){
  return true;

}
QDomDocument VisualizationProfile::get_dom_properties(){
	return QDomDocument();
}
