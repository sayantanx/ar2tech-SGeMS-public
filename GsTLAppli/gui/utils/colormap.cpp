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
 * colormap.cpp
 *
 *  Created on: May 16, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/manager.h>


Colormap::Colormap() {
//	color_table_ = vtkLookupTable::New();
//	color_transfer_function_ = vtkColorTransferFunction::New();
//	opacity_transfer_function_ = vtkPiecewiseFunction::New();
//	opacity_transfer_function_->AllowDuplicateScalarsOn();
//	this->set_default_nan_color();
}


Colormap::~Colormap(){
//  color_table_->Delete();
//  color_transfer_function_->Delete();
//  opacity_transfer_function_->Delete();


}




Colormap::Colormap(const Colormap& rhs )
{
	//color_table_ = vtkLookupTableWithEnabling::New();
//	color_table_ = vtkLookupTable::New();
//	color_transfer_function_ = vtkColorTransferFunction::New();
//	opacity_transfer_function_ = vtkPiecewiseFunction::New();

//	color_table_->DeepCopy(rhs.color_table_);
//	color_transfer_function_->DeepCopy(rhs.color_transfer_function_);
//	opacity_transfer_function_->DeepCopy(rhs.opacity_transfer_function_);
//	opacity_transfer_function_->AllowDuplicateScalarsOn();
}
/*
Colormap& Colormap::operator = ( const Colormap& rhs ) {
  if( &rhs != this )   {
	color_table_ = rhs.color_table_;
	color_transfer_function_ = rhs.color_transfer_function_;
	opacity_transfer_function_ = rhs.opacity_transfer_function_;
  }

  return *this;
}

bool Colormap::operator == ( const Colormap& rhs ) {
  if(  color_table_->GetRange()[0] != rhs.color_table_->GetRange()[0] ) return false;
  if(  color_table_->GetRange()[1] != rhs.color_table_->GetRange()[1] ) return false;
}
*/



/** Compute the color corresponding to "value"
 *  Parameters:
 *    value - value whose color is to be looked up
 *    r, g, b - component colors for the value in question
 */
/*
void Colormap::color( float value,
		      float& r, float& g, float& b ) const{

	double rgb[3];
	color_table_->GetColor((double)value, rgb);
	r = static_cast<float>(rgb[0]);
	g = static_cast<float>(rgb[1]);
	b = static_cast<float>(rgb[2]);

}
*/
/** Compute the color corresponding to "value"
 *  Parameters:
 *    value - value whose color is to be looked up
 *    r, g, b - component colors for the value in question
 */
/*
void Colormap::color( float value,
		      double& r, double& g, double& b ) const{

	double rgb[3];
	color_table_->GetColor((double)value, rgb);
	r = rgb[0];
	g = rgb[1];
	b = rgb[2];

}

RGB_color Colormap::color( float value ) const {
  RGB_color c;
  color( value, c.red(), c.green(), c.blue() );
  return c;
}

void Colormap::set_nan_color(float r, float g, float b, float a) {
	color_table_->SetNanColor(r,g,b,a);
}

void Colormap::set_default_nan_color() {
	color_table_->SetNanColor(0.803,0.788,0.788,0.8);
  color_transfer_function_->SetNanColor(0.803,0.788,0.788);
}


void Colormap::set_bounds(float min, float max){
	color_table_->SetRange(min,max);
}

void Colormap::upper_bound( float max ) {
	double* range;
	range = color_table_->GetRange();
  appli_assert( range[0] <= max );

  color_table_->SetRange(range[0],max);

}



void Colormap::lower_bound( float min ) {
	double* range;
	range = color_table_->GetRange();
  appli_assert( min <= range[1] );

  color_table_->SetRange(min,range[1]);


}


void Colormap::get_bounds(float &min, float &max){
	double* range;
	range = color_table_->GetRange();
	min  = range[0];
	max  = range[1];

}
float Colormap::lower_bound() const{
	return 	color_table_->GetRange()[0];
}

float Colormap::upper_bound() const{
	return 	color_table_->GetRange()[1];
}


void Colormap::refresh(){
	this->init_color_table();
//	this->init_transfer_function();
}


/*
void Color_table::refresh(){
	this->init_color_table();
	this->init_transfer_function();
}


void Color_table::init_color_table() {

  color_table_->SetNumberOfTableValues(colors_->colors_count());
  color_table_->Build();
  int nColors = color_table_->GetNumberOfTableValues();

  for( int i=0; i < nColors ; i++ ) {
  	float r,g,b;
  	colors_->color(i, r,g,b);
  	color_table_->SetTableValue(i,r,g,b);
  }

  color_table_->SetRampToLinear();
  this->set_default_nan_color();

}
*/
/*
 *  -----------
 */

Named_interface* Colormap_continuous::create_new_interface(std::string&){
	return new Colormap_continuous;
}

Colormap_continuous::Colormap_continuous()
	: Colormap()
{
  color_table_ = vtkLookupTable::New();
}
Colormap_continuous::Colormap_continuous( Color_scale* colors, float min, float max )
 : Colormap(), colors_( colors ), alpha_(1.0){
  //color_table_ = vtkLookupTableWithEnabling::New();
  color_table_ = vtkLookupTable::New();
//  color_transfer_function_ = vtkColorTransferFunction::New();
//  opacity_transfer_function_ = vtkPiecewiseFunction::New();
//  opacity_transfer_function_->AllowDuplicateScalarsOn();
  this->set_default_nan_color();
	this->lower_bound(min);
	this->upper_bound(max);
}

Colormap_continuous::Colormap_continuous( Color_scale* colors )
  : Colormap(), colors_( colors ), alpha_(1.0){
  //color_table_ = vtkLookupTableWithEnabling::New();
  color_table_ = vtkLookupTable::New();
//  color_transfer_function_ = vtkColorTransferFunction::New();
//  opacity_transfer_function_ = vtkPiecewiseFunction::New();
//  opacity_transfer_function_->AllowDuplicateScalarsOn();
  this->set_default_nan_color();
}

Colormap_continuous::~Colormap_continuous() {
  color_table_->Delete(); 
}

void Colormap_continuous::setLogScale(bool on){
	if(on) {
		double* range = color_table_->GetTableRange();
		if(range[0]<=0) return;
		color_table_->SetScaleToLog10();
		//color_transfer_function_->SetScaleToLog10();
    this->set_default_nan_color();

	}
	else {
		color_table_->SetScaleToLinear();
		//color_transfer_function_->SetScaleToLinear();
    this->set_default_nan_color();
	}

}

void Colormap_continuous::color_scale( Color_scale* colors ) {
  colors_ = SmartPtr<Color_scale>( colors );
//  init_color_table();
}

bool Colormap_continuous::is_log_scale(){
	return color_table_->GetScale() == 1;
}


void Colormap_continuous::set_alpha(float a){
	std::cout<<a;
	if(a != alpha_) {
		alpha_ = a;
		this->init_color_table();
	}
}

float Colormap_continuous::get_alpha(void) const{
	return alpha_;
}

void Colormap_continuous::init_color_table() {

  color_table_->SetNumberOfTableValues(colors_->colors_count());
  color_table_->Build();
  int nColors = color_table_->GetNumberOfTableValues();

  for( int i=0; i < nColors ; i++ ) {
  	float r,g,b;
  	colors_->color(i, r,g,b);
  	color_table_->SetTableValue(i,r,g,b, alpha_);
  }

  color_table_->SetRampToLinear();
  this->set_default_nan_color();

}


Colormap_continuous& Colormap_continuous::operator = ( const Colormap_continuous& rhs ) {
  if( &rhs != this )   {
	color_table_ = rhs.color_table_;
//	color_transfer_function_ = rhs.color_transfer_function_;
//	opacity_transfer_function_ = rhs.opacity_transfer_function_;
  }

  return *this;
}

bool Colormap_continuous::operator == ( const Colormap_continuous& rhs ) {
  if(  color_table_->GetRange()[0] != rhs.color_table_->GetRange()[0] ) return false;
  if(  color_table_->GetRange()[1] != rhs.color_table_->GetRange()[1] ) return false;
}


vtkScalarsToColors* Colormap_continuous::color_table() const {
  return vtkScalarsToColors::SafeDownCast(color_table_ );
}

/** Compute the color corresponding to "value"
 *  Parameters:
 *    value - value whose color is to be looked up
 *    r, g, b - component colors for the value in question
 */
void Colormap_continuous::color( float value,
		      float& r, float& g, float& b ) const{

	double rgb[3];
	color_table_->GetColor((double)value, rgb);
	r = static_cast<float>(rgb[0]);
	g = static_cast<float>(rgb[1]);
	b = static_cast<float>(rgb[2]);

}

/** Compute the color corresponding to "value"
 *  Parameters:
 *    value - value whose color is to be looked up
 *    r, g, b - component colors for the value in question
 */
void Colormap_continuous::color( float value,
		      double& r, double& g, double& b ) const{

	double rgb[3];
	color_table_->GetColor((double)value, rgb);
	r = rgb[0];
	g = rgb[1];
	b = rgb[2];

}

RGB_color Colormap_continuous::color( float value ) const {
  RGB_color c;
  color( value, c.red(), c.green(), c.blue() );
  return c;
}

void Colormap_continuous::set_nan_color(float r, float g, float b, float a) {
	color_table_->SetNanColor(r,g,b,a);
}

void Colormap_continuous::set_default_nan_color() {
	color_table_->SetNanColor(0.803,0.788,0.788,0.8);
//  color_transfer_function_->SetNanColor(0.803,0.788,0.788);
}


void Colormap_continuous::set_bounds(float min, float max){
	color_table_->SetRange(min,max);
}

void Colormap_continuous::upper_bound( float max ) {
	double* range;
	range = color_table_->GetRange();
  appli_assert( range[0] <= max );

  color_table_->SetRange(range[0],max);

}



void Colormap_continuous::lower_bound( float min ) {
	double* range;
	range = color_table_->GetRange();
  appli_assert( min <= range[1] );

  color_table_->SetRange(min,range[1]);


}


void Colormap_continuous::get_bounds(float &min, float &max){
	double* range;
	range = color_table_->GetRange();
	min  = range[0];
	max  = range[1];

}
float Colormap_continuous::lower_bound() const{
	return 	color_table_->GetRange()[0];
}

float Colormap_continuous::upper_bound() const{
	return 	color_table_->GetRange()[1];
}


void Colormap_continuous::refresh(){
	this->init_color_table();
//	this->init_transfer_function();
}





/*
 *  -----------
 */

Colormap_categorical::Colormap_categorical()
: Colormap(), cat_def_(0), n_cat_(0)
{
  discrete_color_table_ = vtkDiscretizableColorTransferFunction::New();
  opacity_piecewise_fnc_ = vtkPiecewiseFunction::New();
}


Colormap_categorical::Colormap_categorical( const CategoricalPropertyDefinition* cat_def, int ncat)
: Colormap(), n_cat_(ncat), cat_def_(cat_def){

  discrete_color_table_ = vtkDiscretizableColorTransferFunction::New();
  discrete_color_table_->DiscretizeOff();
  //discrete_color_table_->DiscretizeOn();
  discrete_color_table_->IndexedLookupOn();
  discrete_color_table_->EnableOpacityMappingOn();
  opacity_piecewise_fnc_ = vtkPiecewiseFunction::New();
  discrete_color_table_->SetScalarOpacityFunction(opacity_piecewise_fnc_);


  const CategoricalPropertyDefinitionName* cdef = dynamic_cast<const CategoricalPropertyDefinitionName*>(cat_def);
  if(cdef) {
    n_cat_ = cdef->number_of_category();
  }
  else {
    n_cat_ = ncat;
  }


//  this->set_bounds(cdef->category_id_from_index(0), cdef->category_id_from_index(n_cat_-1));
//	this->lower_bound(cdef->category_id_from_index(0));
//  this->upper_bound( cdef->category_id_from_index(n_cat_-1) );

  this->init_color_table();
}


Colormap_categorical::~Colormap_categorical() {
  discrete_color_table_->Delete(); 
  opacity_piecewise_fnc_->Delete();
}

Named_interface* Colormap_categorical::create_new_interface(std::string&){
	return new Colormap_categorical;
}

vtkScalarsToColors* Colormap_categorical::color_table() const {
  return vtkScalarsToColors::SafeDownCast(discrete_color_table_ );
}

void Colormap_categorical::set_categorical_definition(const CategoricalPropertyDefinition* cat_def, int ncat){
 
	cat_def_ = cat_def;
  const CategoricalPropertyDefinitionName* cdef = dynamic_cast<const CategoricalPropertyDefinitionName*>(cat_def);
  if(cdef) {
    n_cat_ = cdef->number_of_category();
  }
  else {
    n_cat_ = ncat;
  }

  this->init_color_table();
}

const CategoricalPropertyDefinition*
Colormap_categorical::get_categorical_definition() const{
	return cat_def_;
}

/* If the number of category does not match the
 * associated catDef, remove the cat_def and replace it with the default one.
 */
void Colormap_categorical::set_number_of_categories(int n_cat){

  if(n_cat_ == n_cat ) return;

	const CategoricalPropertyDefinitionName* cdef_name =
			dynamic_cast<const CategoricalPropertyDefinitionName*>(cat_def_);

	if(cdef_name  && cdef_name->number_of_category() != n_cat ) {
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( categoricalDefinition_manager+"/Default" );
	  cat_def_ = dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());

	}
	n_cat_= n_cat;

  this->init_color_table();

}

int Colormap_categorical::get_number_of_categories() const{
	return n_cat_;
}


void Colormap_categorical::init_color_table() {

//  discrete_color_table_->SetNumberOfValues( n_cat_ );
  discrete_color_table_->GetScalarOpacityFunction()->RemoveAllPoints();
  discrete_color_table_->RemoveAllPoints();

  discrete_color_table_->SetNumberOfValues( n_cat_ );

  for( int i=0; i < n_cat_ ; i++ ) {
  	float r,g,b;
    int cat_id = cat_def_->category_id_from_index( i );

    discrete_color_table_->AddRGBPoint(cat_id,cat_def_->red(cat_id),cat_def_->green(cat_id),cat_def_->blue(cat_id));
    discrete_color_table_->GetScalarOpacityFunction()->AddPoint(cat_id,cat_def_->alpha(cat_id));


    //color_table_->SetTableValue(cat_id,cat_def_->red(i),cat_def_->green(cat_id),cat_def_->blue(cat_id), cat_def_->alpha(cat_id));
    //color_table_->SetTableValue(i,cat_def_->red(i),cat_def_->green(i),cat_def_->blue(i), cat_def_->alpha(i));
  }

  discrete_color_table_->Build();
  for( int i=0; i < n_cat_ ; i++ ) {
    int cat_id = cat_def_->category_id_from_index( i );
    discrete_color_table_->SetAnnotation(cat_id, cat_def_->get_category_name( cat_id)  );
  }
  discrete_color_table_->Build();

  this->set_default_nan_color();

}



Colormap_categorical& Colormap_categorical::operator = ( const Colormap_categorical& rhs ) {
  if( &rhs != this )   {
	discrete_color_table_ = rhs.discrete_color_table_;
//	color_transfer_function_ = rhs.color_transfer_function_;
//	opacity_transfer_function_ = rhs.opacity_transfer_function_;
  }

  return *this;
}

bool Colormap_categorical::operator == ( const Colormap_categorical& rhs ) {
  if(  discrete_color_table_->GetRange()[0] != rhs.discrete_color_table_->GetRange()[0] ) return false;
  if(  discrete_color_table_->GetRange()[1] != rhs.discrete_color_table_->GetRange()[1] ) return false;
}




/** Compute the color corresponding to "value"
 *  Parameters:
 *    value - value whose color is to be looked up
 *    r, g, b - component colors for the value in question
 */
void Colormap_categorical::color( float value,
		      float& r, float& g, float& b ) const{

	double rgb[3];
	discrete_color_table_->GetColor((double)value, rgb);
	r = static_cast<float>(rgb[0]);
	g = static_cast<float>(rgb[1]);
	b = static_cast<float>(rgb[2]);

}

/** Compute the color corresponding to "value"
 *  Parameters:
 *    value - value whose color is to be looked up
 *    r, g, b - component colors for the value in question
 */
void Colormap_categorical::color( float value,
		      double& r, double& g, double& b ) const{

	double rgb[3];
	discrete_color_table_->GetColor((double)value, rgb);
	r = rgb[0];
	g = rgb[1];
	b = rgb[2];

}

RGB_color Colormap_categorical::color( float value ) const {
  RGB_color c;
  color( value, c.red(), c.green(), c.blue() );
  return c;
}

void Colormap_categorical::set_nan_color(float r, float g, float b, float a) {
	//color_table_->SetNanColor(r,g,b,a);
  discrete_color_table_->SetNanColor(r,g,b);
}

void Colormap_categorical::set_default_nan_color() {
	//color_table_->SetNanColor(0.803,0.788,0.788,0.8);
  discrete_color_table_->SetNanColor(0.803,0.788,0.788);
//  color_transfer_function_->SetNanColor(0.803,0.788,0.788);
}


void Colormap_categorical::set_bounds(float min, float max){
//	discrete_color_table_->SetRange(min,max);
}

void Colormap_categorical::upper_bound( float max ) {
	double* range;
	range = discrete_color_table_->GetRange();
  appli_assert( range[0] <= max );

//  discrete_color_table_->SetRange(range[0],max);

}



void Colormap_categorical::lower_bound( float min ) {
	double* range;
	range = discrete_color_table_->GetRange();
  appli_assert( min <= range[1] );

//  discrete_color_table_->SetRange(min,range[1]);


}


void Colormap_categorical::get_bounds(float &min, float &max){
	double* range;
	range = discrete_color_table_->GetRange();
	min  = range[0];
	max  = range[1];

}
float Colormap_categorical::lower_bound() const{
	return 	discrete_color_table_->GetRange()[0];
}

float Colormap_categorical::upper_bound() const{
	return 	discrete_color_table_->GetRange()[1];
}


void Colormap_categorical::refresh(){
	this->init_color_table();
//	this->init_transfer_function();
}

