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
 * GsTLGridCategoricalProperty.cpp
 *
 *  Created on: Mar 29, 2010
 *      Author: aboucher
 */

#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/appli/manager_repository.h> 
#include <sstream>



/*
 *   GsTLGridCategoricalProperty
 */

GsTLGridWeightProperty::GsTLGridWeightProperty(
		GsTLInt size, const std::string& name,
		property_type default_value)
		: GsTLGridProperty(size,name,no_data_value)
{
}


GsTLGridWeightProperty::GsTLGridWeightProperty(
		GsTLInt size, const std::string& name, const std::string& in_filename,
		property_type default_value)
		: GsTLGridProperty(size,name,in_filename, no_data_value)
{
}



QString GsTLGridWeightProperty::item_type() const{
	return "WeightsProperty";
}
