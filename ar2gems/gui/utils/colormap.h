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

#include <utils/named_interface.h>
#include <utils/gstl_messages.h>
#include <grid/grid_model/grid_categorical_property.h>



#include <gui/common.h>
#include <gui/utils/colorscale.h>

#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkScalarsToColors.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
/*
#include <vtkLookupTableWithEnabling.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
*/

#include <string>
#include <vector>



class GUI_DECL Colormap: public Named_interface {
public:
	Colormap();
	Colormap(const Colormap& rhs );

	virtual ~Colormap();

  /*
	virtual Colormap& operator = ( const Colormap& rhs );

	bool operator == ( const Colormap& rhs );
  */
	virtual vtkScalarsToColors* color_table() const =0; //{ return scalars_to_colors_table_; }
//	vtkColorTransferFunction* color_transfer_function() const { return color_transfer_function_; }
//	vtkPiecewiseFunction* opacity_transfer_function() const { return opacity_transfer_function_; }

	virtual void set_default_nan_color() =0;
	virtual void set_nan_color(float r, float g, float b, float alpha=0.9) =0;

  virtual int color_count() const = 0;

	virtual RGB_color color( float value ) const =0 ;
	virtual void color( float value, float& r, float& g, float& b) const =0 ;
	virtual void color( float value, double& r, double& g, double& b) const =0;

	virtual void set_bounds( float min, float max ) =0;
	virtual void get_bounds( float &min, float &max ) =0;
	virtual void upper_bound( float max ) =0;
	virtual void lower_bound( float min ) =0;
	virtual float upper_bound() const =0;
	virtual float lower_bound() const =0;

	virtual void refresh() =0;

protected :
//	virtual void init_color_table()=0;
//	void init_transfer_function();

protected :

	float nan_r, nan_g, nan_b, nan_a;
	
	//It would make sense to use vtkLookupTableWithEnabling but
	// for some reason it does not work.  Both the enabled array and
	// the Nan value are not working
	//vtkLookupTableWithEnabling* color_table_;
//  vtkScalarsToColors* scalars_to_colors_table_;

	//vtkLookupTable* color_table_;
  
	//For volume rendering
//	vtkColorTransferFunction* color_transfer_function_;
//	vtkPiecewiseFunction* opacity_transfer_function_;

};




class Colormap_continuous: public Colormap {

public:
	static Named_interface* create_new_interface(std::string&);

public:
	Colormap_continuous();
	virtual ~Colormap_continuous();
	Colormap_continuous( Color_scale* colors, float min, float max );
	Colormap_continuous( Color_scale* colors );

	Colormap_continuous& operator = ( const Colormap_continuous& rhs );

	bool operator == ( const Colormap_continuous& rhs );

  virtual int color_count()const {return colors_->colors_count();};

	virtual vtkScalarsToColors* color_table() const; 

	virtual void set_default_nan_color();
	virtual void set_nan_color(float r, float g, float b, float alpha=0.9);

	virtual RGB_color color( float value ) const;
	virtual void color( float value, float& r, float& g, float& b) const;
	virtual void color( float value, double& r, double& g, double& b) const;

	virtual void set_bounds( float min, float max );
	virtual void get_bounds( float &min, float &max );
	virtual void upper_bound( float max );
	virtual void lower_bound( float min );
	virtual float upper_bound() const;
	virtual float lower_bound() const;

	virtual void refresh();

	void color_scale( Color_scale* colors );
	Color_scale* color_scale() const { return colors_.raw_ptr(); }
	
  void set_alpha(float a);
	float get_alpha(void) const;

	void setLogScale(bool on);
	bool is_log_scale();

protected :
	virtual void init_color_table();

protected :
  vtkLookupTable* color_table_;
	SmartPtr<Color_scale> colors_;
  float alpha_;
};


class Colormap_categorical: public Colormap {

public:
	static Named_interface* create_new_interface(std::string&);

public:
	Colormap_categorical();
	Colormap_categorical(const CategoricalPropertyDefinition* cat_def, int n_cat);
	virtual ~Colormap_categorical();

	Colormap_categorical& operator = ( const Colormap_categorical& rhs );
  bool operator == ( const Colormap_categorical& rhs );

	void set_categorical_definition(const CategoricalPropertyDefinition* cat_def, int n_cat);
	const CategoricalPropertyDefinition* get_categorical_definition() const;
	void set_number_of_categories(int n_cat);
	int get_number_of_categories() const;

  virtual int color_count() const {return n_cat_;};

	virtual vtkScalarsToColors* color_table() const; 

	virtual void set_default_nan_color();
	virtual void set_nan_color(float r, float g, float b, float alpha=0.9);

	virtual RGB_color color( float value ) const;
	virtual void color( float value, float& r, float& g, float& b) const;
	virtual void color( float value, double& r, double& g, double& b) const;

	virtual void set_bounds( float min, float max );
	virtual void get_bounds( float &min, float &max );
	virtual void upper_bound( float max );
	virtual void lower_bound( float min );
	virtual float upper_bound() const;
	virtual float lower_bound() const;

	virtual void refresh();

//	bool initialize(Color_scale* colors, float min, float max);

protected :
	virtual void init_color_table();

protected :
  vtkDiscretizableColorTransferFunction* discrete_color_table_;
  vtkPiecewiseFunction* opacity_piecewise_fnc_;
	const CategoricalPropertyDefinition* cat_def_;
	unsigned int n_cat_;

};


#endif /* COLORMAP_H_ */
