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
 * GsTLGridCategoricalProperty.h
 *
 *  Created on: Mar 29, 2010
 *      Author: aboucher
 */

#ifndef GSTLGRIDCATEGORICALPROPERTY_H_
#define GSTLGRIDCATEGORICALPROPERTY_H_

#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/utils/named_interface.h>
#include <set>

#include <QColor>

Named_interface* create_new_categorical_definition( std::string& );

class GRID_DECL CategoricalPropertyDefinition :public Named_interface {
public:
  typedef std::set<const GsTLGridProperty*, compareGsTLGridProperty > property_set;
	CategoricalPropertyDefinition(){}
	virtual ~CategoricalPropertyDefinition(){}

	virtual std::string get_category_name(unsigned int id) const = 0;
  virtual std::string get_category_name_from_index(unsigned int index) const;

	virtual bool is_category_exist(std::string name) const = 0;

	virtual int category_id(std::string name) const = 0;

  virtual bool is_sequential_coding() const = 0;

  virtual int category_id_from_index(int index ) const = 0;
  virtual int index_from_category_id(int cat_id ) const = 0;


//  virtual int numerical_code(int id) const {return id;}
//  virtual int numerical_code(std::string name) const { this->numerical_code( this->category_id(name) );}
  
  virtual std::string name() const = 0;

  void register_property(const GsTLGridProperty* prop);

  void unregister_property(const GsTLGridProperty* prop);

  property_set::const_iterator begin_property() const;
  property_set::const_iterator end_property() const;

  virtual QColor color(unsigned int cat_id) const =0;
  virtual QColor color(std::string name) const;
  virtual float red(unsigned int cat_id) const =0;
  virtual float green(unsigned int cat_id) const =0;
  virtual float blue(unsigned int cat_id) const =0;
  virtual float alpha(unsigned int cat_id) const =0;
  virtual void color(unsigned int cat_id, QColor color)=0;
  virtual void color(unsigned int cat_id, float r, float g, float b, float alpha=1.0)=0;

  virtual QColor color_from_index(unsigned int index) const {return color(this->category_id_from_index(index));}
  virtual float red_from_index(unsigned int index) const {return red(this->category_id_from_index(index));}
  virtual float green_from_index(unsigned int index) const {return green(this->category_id_from_index(index));}
  virtual float blue_from_index(unsigned int index) const {return blue(this->category_id_from_index(index));}
  virtual float alpha_from_index(unsigned int index) const {return alpha(this->category_id_from_index(index));}
  virtual void color_from_index(unsigned int index, QColor color) { this->color(this->category_id_from_index(index), color); }
  virtual void color_from_index(unsigned int index, float r, float g, float b, float alpha=1.0) {
    color(this->category_id_from_index(index), r,g,b,alpha);
  }


protected :
  property_set props_;

};



class Categorical_color_coding : public GsTL_object_item {
  public :

  Categorical_color_coding(std::string name, int code, GsTL_object_item* parent);
  ~Categorical_color_coding();

  QColor color() const {return color_;}
  QString name() const {return item_name_;}
  int code() const {return code_;}
  void color( QColor color ){color_ = color;}
  void name(QString name){item_name_ = name;}

  virtual Qt::ItemFlags flags(int column=0) const;
  virtual GsTL_object_item *child(int row){return 0;}
  virtual const GsTL_object_item *child(int row) const {return 0;}
  //virtual int childCount() const{return 0;}
  virtual bool set_item_data(QVariant value, int column= 0 );


private :
 // const CategoricalPropertyDefinition* parent_;
  QColor color_;
  int code_;
};





class GRID_DECL CategoricalPropertyDefinitionName :public CategoricalPropertyDefinition {

public:
	CategoricalPropertyDefinitionName();
	CategoricalPropertyDefinitionName( const std::string& filename );
//  CategoricalPropertyDefinitionName(std::vector<std::string> names);

	virtual ~CategoricalPropertyDefinitionName();

	virtual std::string get_category_name(unsigned int id) const;

	virtual bool is_category_exist(std::string name) const;
  virtual bool is_category_exist(int id) const;

	virtual int category_id(std::string name) const;

  virtual bool is_sequential_coding() const;

  virtual std::string name() const;

	virtual int number_of_category() const;

	bool rename(int id, std::string name);
  bool rename_from_index(int id, std::string name);

  void load_from_file(const std::string& filename);

	void set_category_names(std::vector<std::string> names);

  int add_category(int code, std::string name);
	//int add_category(std::string name);

  void set_definiton_name(std::string name);

//  std::vector<std::string>::const_iterator begin_category_name() const;
//  std::vector<std::string>::const_iterator end_category_name() const;

  std::vector<std::string> category_names() const;
  std::vector<int> category_codes() const;

  virtual int category_id_from_index(int index ) const;
  virtual int index_from_category_id(int cat_id ) const;
/*
  virtual QColor color(unsigned int cat_id) const { return cat_coding_[cat_id]->color();}
  virtual float red(unsigned int cat_id) const {return cat_coding_[cat_id]->color().redF();}
  virtual float green(unsigned int cat_id) const {return cat_coding_[cat_id]->color().greenF();}
  virtual float blue(unsigned int cat_id) const {return cat_coding_[cat_id]->color().blueF();}
  virtual float alpha(unsigned int cat_id) const {return cat_coding_[cat_id]->color().alphaF();}
  virtual void color(unsigned int cat_id, QColor color){cat_coding_[cat_id]->color(color);}
  virtual void color(unsigned int cat_id, float r, float g, float b, float alpha=1.0){
    QColor color;
    color.setRgbF(r,g,b,alpha);
    cat_coding_[cat_id]->color(color);
  }
*/
  virtual QColor color(unsigned int cat_id) const;
  virtual float red(unsigned int cat_id) const;
  virtual float green(unsigned int cat_id) const;
  virtual float blue(unsigned int cat_id) const;
  virtual float alpha(unsigned int cat_id) const;
  virtual void color(unsigned int cat_id, QColor color){cat_coding_[cat_id]->color(color);}
  virtual void color(unsigned int cat_id, float r, float g, float b, float alpha=1.0){
    QColor color;
    color.setRgbF(r,g,b,alpha);
    cat_coding_[cat_id]->color(color);
  }





  // GsTL-Object functions
  virtual GsTL_object_item *child(int row){return cat_coding_[row];}
  virtual const GsTL_object_item *child(int row) const;
  virtual int childCount() const{return cat_coding_.size();}
  //virtual Qt::ItemFlags flags(int column=0){return  Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsDragEnabled | Qt::ItemIsEditable;}
  //virtual bool set_item_data(QVariant value, int column= 0 );


protected :
  //std::vector<Categorical_color_coding*> cat_coding_;
  std::map<int,Categorical_color_coding*> cat_coding_;
  
  //std::vector<std::string> cat_names_;
  std::string name_;
  //std::vector<QColor> colors_;

};

class GRID_DECL CategoricalPropertyDefinitionDefault :public CategoricalPropertyDefinition {

public:

  static QColor default_color(int cat_id);

  CategoricalPropertyDefinitionDefault();
	virtual ~CategoricalPropertyDefinitionDefault();

	virtual std::string get_category_name(unsigned int id) const;

	virtual bool is_category_exist(std::string name) const;

	virtual int category_id(std::string name) const;

  virtual bool is_sequential_coding() const {return true;}

  virtual int category_id_from_index(int index ) const {return index;}
  virtual int index_from_category_id(int cat_id ) const {return cat_id;}

  virtual std::string name() const;
/*
  virtual QColor color(unsigned int cat_id)const { return colors_[cat_id%12];}
  virtual float red(unsigned int cat_id) const {return colors_[cat_id%12].redF();}
  virtual float green(unsigned int cat_id) const {return colors_[cat_id%12].greenF();}
  virtual float blue(unsigned int cat_id)const {return colors_[cat_id%12].blueF();}
  virtual float alpha(unsigned int cat_id) const {return colors_[cat_id%12].alphaF();}
  virtual void color(unsigned int cat_id, QColor color){colors_[cat_id%12] = color;}
  virtual void color(unsigned int cat_id, float r, float g, float b, float alpha=1.0){colors_[cat_id%12].setRgbF(r,g,b,alpha);}
*/

  virtual QColor color(unsigned int cat_id) const { return cat_coding_[cat_id]->color();}
  virtual float red(unsigned int cat_id) const {return cat_coding_[cat_id]->color().redF();}
  virtual float green(unsigned int cat_id) const {return cat_coding_[cat_id]->color().greenF();}
  virtual float blue(unsigned int cat_id) const {return cat_coding_[cat_id]->color().blueF();}
  virtual float alpha(unsigned int cat_id) const {return cat_coding_[cat_id]->color().alphaF();}
  virtual void color(unsigned int cat_id, QColor color){cat_coding_[cat_id]->color(color);}
  virtual void color(unsigned int cat_id, float r, float g, float b, float alpha=1.0){
    QColor color;
    color.setRgbF(r,g,b,alpha);
    cat_coding_[cat_id]->color(color);
  }

  // GsTL-Object functions
  virtual GsTL_object_item *child(int row){return cat_coding_[row];}
  virtual const GsTL_object_item *child(int row) const {return cat_coding_[row];}
  virtual int childCount() const{return cat_coding_.size();}

protected :
  std::vector<Categorical_color_coding*> cat_coding_;
  int current_n_categ_in_use_;
  std::vector<QColor> colors_; //Create a dictionary of colors  Use the default categroical (then repeat them)
  


};


class GRID_DECL GsTLGridCategoricalProperty: public GsTLGridProperty {
public:
	GsTLGridCategoricalProperty( GsTLInt size, const std::string& name,
				const std::string cat_definition_name = "Default",
				property_type default_value = no_data_value );
	GsTLGridCategoricalProperty(const std::string& in_filename, GsTLInt size, const std::string& name,
				const std::string cat_definition_name = "Default",
				property_type default_value = no_data_value );
/*
	GsTLGridCategoricalProperty( GsTLGridProperty* cont_prop,
				const std::string cat_definition_name = "Default",
				property_type default_value = no_data_value );
*/
	virtual ~GsTLGridCategoricalProperty();

	/** Return the name of the class
	*/
	virtual std::string classname() const {return "GsTLGridCategoricalProperty";}

	  /** Returns the category name value of the ith element.
	  */
	inline std::string get_category_name( GsTLInt id ) const;

	/** Returns the an indicator (0-1) for a given category. 
  * Return -1, if the category does not exist
	*/
	inline int get_indicator_value( GsTLInt id, int category ) const;

	/** Returns the an indicator (0-1) for a given category.
  * Return -1, if the category does not exist
	*/
  inline int get_indicator_value( GsTLInt id, std::string category_name ) const;

	/** Changes the value of the ith element to \a val.
	*/
	inline void set_value( property_type val, GsTLInt id );

	/** Changes the value of the ith element to \a val.
  */
  inline void set_value( std::string val, GsTLInt id );

  /** Get the coding definition for the categories
  */
  inline const CategoricalPropertyDefinition* get_category_definition() const;
  inline CategoricalPropertyDefinition* get_category_definition();


  /** Get the number of category
  */
  int get_number_of_category();
  int get_number_of_category() const;

  /** Set the coding definition for the categories
  */
  bool set_category_definition( std::string cat_definition_name);
  bool set_category_definition( CategoricalPropertyDefinition* cat_definition);

  virtual QString item_type() const;

protected :
  int compute_number_of_category() const;

protected :
	  CategoricalPropertyDefinition* cat_definitions_;
    unsigned int number_of_categories_;

    std::set<int> category_codes_;
};

inline std::string
GsTLGridCategoricalProperty::get_category_name( GsTLInt id ) const {
  appli_assert( accessor_->get_property_value( id ) != no_data_value );
  property_type val = accessor_->get_property_value( id );
  return cat_definitions_->get_category_name(val);
}

inline int 
GsTLGridCategoricalProperty::get_indicator_value( GsTLInt id, int category ) const{
  return this->get_value(id)==category?1:0;
}

	/** Returns the an indicator (0-1) for a given category.
	*/
inline int 
GsTLGridCategoricalProperty::get_indicator_value( GsTLInt id, std::string category_name ) const{
  int code = cat_definitions_->category_id(category_name);
  if( code <0 ) return -1;
  return this->get_indicator_value(id,code);
}


inline
void GsTLGridCategoricalProperty::set_value( property_type val, GsTLInt id ) {
  unsigned int cat = static_cast<unsigned int>(val);
  if( cat >= number_of_categories_) number_of_categories_ = cat+1;
  accessor_->set_property_value( cat, id );
}


inline
void GsTLGridCategoricalProperty::set_value( std::string val, GsTLInt id ) {
  int code = cat_definitions_->category_id(val);
  if( code >= number_of_categories_) number_of_categories_ = code +1;
  if( code >= 0 )
	  accessor_->set_property_value( code, id );
}


inline
const CategoricalPropertyDefinition*
	GsTLGridCategoricalProperty::get_category_definition( ) const {
	return cat_definitions_;
}

inline
CategoricalPropertyDefinition*
	GsTLGridCategoricalProperty::get_category_definition( ) {
	return cat_definitions_;
}

inline 
int GsTLGridCategoricalProperty::get_number_of_category() {
	if(number_of_categories_==0)
		number_of_categories_ = this->compute_number_of_category();
  return number_of_categories_;
}

inline
int GsTLGridCategoricalProperty::get_number_of_category() const {
	if(number_of_categories_==0)
		return this->compute_number_of_category();
	else
		return number_of_categories_;
}



#endif /* GSTLGRIDCATEGORICALPROPERTY_H_ */
