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
 * Weight_property.h
 *
 *  Created on: Nov 10, 2011
 *      Author: aboucher
 */

#ifndef GSTLGRIDWEIGHTPROPERTY_H_
#define GSTLGRIDWEIGHTPROPERTY_H_

#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/utils/named_interface.h>
#include <set>

#include <QColor>



class GRID_DECL GsTLGridWeightProperty: public GsTLGridProperty {
public:
	GsTLGridWeightProperty( GsTLInt size, const std::string& name,
								property_type default_value = no_data_value );
	GsTLGridWeightProperty( GsTLInt size, const std::string& name, const std::string& in_filename,
				property_type default_value = no_data_value );
  virtual ~GsTLGridWeightProperty(){}

	/** Return the name of the class
	*/
	virtual std::string classname() const {return "GsTLGridWeightProperty";}

  virtual QString item_type() const;

};

#endif /* GSTLGRIDCATEGORICALPROPERTY_H_ */
