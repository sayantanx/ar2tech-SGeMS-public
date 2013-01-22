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
 * colormap.h
 *
 *  Created on: May 16, 2011
 *      Author: aboucher
 */

#ifndef COLORMAP_H_
#define COLORMAP_H_


#include <GsTL/utils/smartptr.h>

#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>



#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/colorscale.h>

#include <vtkLookupTableWithEnabling.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>


#include <string>
#include <vector>



class GUI_DECL Colormap: public Named_interface {
public:
	Colormap();
	Colormap(const Colormap& rhs );

	virtual ~Colormap();


	virtual Colormap& operator = ( const Colormap& rhs );

	bool operator == ( const Colormap& rhs );

	vtkLookupTable* color_table() const { return color_table_; }
	vtkColorTransferFunction* color_transfer_function() const { return color_transfer_function_; }
	vtkPiecewiseFunction* opacity_transfer_function() const { return opacity_transfer_function_; }

	void set_default_nan_color();
	void set_nan_color(float r, float g, float b, float alpha=0.9);

  virtual int color_count() const = 0;

	RGB_color color( float value ) const ;
	void color( float value, float& r, float& g, float& b) const;
	void color( float value, double& r, double& g, double& b) const;

	void set_bounds( float min, float max );
	void get_bounds( float &min, float &max );
	void upper_bound( float max );
	void lower_bound( float min );
	float upper_bound() const;
	float lower_bound() const;

	void refresh();

protected :
	virtual void init_color_table()=0;
//	void init_transfer_function();

protected :

	float nan_r, nan_g, nan_b, nan_a;
	
	//It would make sense to use vtkLookupTableWithEnabling but
	// for some reason it does not work.  Both the enabled array and
	// the Nan value are not working
	//vtkLookupTableWithEnabling* color_table_;
	vtkLookupTable* color_table_;

	//For volume rendering
	vtkColorTransferFunction* color_transfer_function_;
	vtkPiecewiseFunction* opacity_transfer_function_;

};




class Colormap_continuous: public Colormap {

public:
	static Named_interface* create_new_interface(std::string&);

public:
	Colormap_continuous();
	virtual ~Colormap_continuous(){}
	Colormap_continuous( Color_scale* colors, float min, float max );
	Colormap_continuous( Color_scale* colors );

  virtual int color_count()const {return colors_->colors_count();};

	void color_scale( Color_scale* colors );
	Color_scale* color_scale() const { return colors_.raw_ptr(); }
	
  void set_alpha(float a);
	float get_alpha(void) const;

	void setLogScale(bool on);
	bool is_log_scale();

protected :
	virtual void init_color_table();

protected :
	SmartPtr<Color_scale> colors_;
  float alpha_;
};


class Colormap_categorical: public Colormap {

public:
	static Named_interface* create_new_interface(std::string&);

public:
	Colormap_categorical();
	Colormap_categorical(const CategoricalPropertyDefinition* cat_def, int n_cat);
	virtual ~Colormap_categorical(){}

	void set_categorical_definition(const CategoricalPropertyDefinition* cat_def, int n_cat);
	const CategoricalPropertyDefinition* get_categorical_definition() const;
	void set_number_of_categories(int n_cat);
	int get_number_of_categories() const;

  virtual int color_count() const {return n_cat_;};

//	bool initialize(Color_scale* colors, float min, float max);

protected :
	virtual void init_color_table();

protected :
	const CategoricalPropertyDefinition* cat_def_;
	unsigned int n_cat_;

};


#endif /* COLORMAP_H_ */
