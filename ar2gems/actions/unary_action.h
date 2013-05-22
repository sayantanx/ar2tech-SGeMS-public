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



#ifndef UNARY_ACTION_H_
#define UNARY_ACTION_H_

#include <actions/common.h>
#include <actions/action.h>
#include <grid/grid_model/geostat_grid.h>
#include <grid/grid_model/geovalue.h>

class ACTIONS_DECL Unary_action: public PropertyNoParameterAction {
protected:
	Unary_action(const std::string& _name) :
		PropertyNoParameterAction(_name) {
	}

public:
	virtual ~Unary_action() {
	}


	virtual bool init(std::string& parameters, GsTL_project* proj, Error_messages_handler* errors);
	virtual bool exec();
  virtual bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){return true;}

protected:
//	std::string name_;
	std::string grid_name_;
	std::string property_name_;
	std::string new_property_name_;
	Geostat_grid* grid_;

	GsTL_project* proj_;
	Error_messages_handler* errors_;
};


class Exponential_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Exponential_transform_action();
	}

public:
	Exponential_transform_action() : Unary_action("exp"){}
	virtual ~Exponential_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		_new_val = std::exp(_val);
		return true;
	}
};


class Log10_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Log10_transform_action();
	}

public:
	Log10_transform_action(): Unary_action("log10"){}
	virtual ~Log10_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val <= 0.0) return false;
		_new_val = std::log10(_val);
		return true;
	}
};


class Log_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Log_transform_action();
	}

public:
	Log_transform_action(): Unary_action("log"){}
	virtual ~Log_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val <= 0.0) return false;
		_new_val = std::log(_val);
		return true;
	}
};

class Sqrt_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Sqrt_transform_action();
	}

public:
	Sqrt_transform_action(): Unary_action("sqrt"){}
	virtual ~Sqrt_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val < 0.0) return false;
		_new_val = std::sqrt(_val);
		return true;
	}
};


class Absolute_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Absolute_transform_action();
	}

public:
	Absolute_transform_action(): Unary_action("absolute"){}
	virtual ~Absolute_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		_new_val = std::abs(_val);

		return true;
	}
};

class Inverse_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Inverse_transform_action();
	}

public:
	Inverse_transform_action(): Unary_action("inverse"){}
	virtual ~Inverse_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val != 0.0) return false;
		_new_val = 1./_val;
		return true;
	}
};

class Sine_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Sine_transform_action();
	}

public:
	Sine_transform_action(): Unary_action("sin"){}
	virtual ~Sine_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val == Grid_continuous_property::no_data_value) return false;
		_new_val = std::sin(_val);
		return true;
	}
};

class Cosine_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Cosine_transform_action();
	}

public:
	Cosine_transform_action(): Unary_action("cos"){}
	virtual ~Cosine_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val == Grid_continuous_property::no_data_value) return false;
		_new_val = std::cos(_val);
		return true;
	}
};


class Complement_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Complement_transform_action();
	}

public:
	Complement_transform_action(): Unary_action("complement"){}
	virtual ~Complement_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val < 0 || _val > 1) return false;
    _new_val = 1.-_val;
		return true;
	}
};

class Logit_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Logit_transform_action();
	}

public:
	Logit_transform_action(): Unary_action("logit"){}
	virtual ~Logit_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val < 0 || _val >= 1) return false;
    _new_val = std::log(_val/(1-_val));
		return true;
	}
};


class Square_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Square_transform_action();
	}

public:
	Square_transform_action(): Unary_action("square"){}
	virtual ~Square_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		_new_val = _val*_val;
		return true;
	}
};

class Logistic_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Logistic_transform_action();
	}

public:
	Logistic_transform_action(): Unary_action("logistic"){}
	virtual ~Logistic_transform_action(){}
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val){
		if (_val == Grid_continuous_property::no_data_value) return false;
    _new_val = 1/(1 + exp(-_val));
		return true;
	}
};


class Standardize_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Standardize_transform_action();
	}

public:
	Standardize_transform_action():Unary_action("standardize"){}
	virtual ~Standardize_transform_action(){}

	virtual bool init(std::string& parameters, GsTL_project* proj, Error_messages_handler* errors);
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val);

protected :
	Geovalue::property_type mean_;
	Geovalue::property_type stdev_;
};


class Unit_scaling_transform_action: public Unary_action {
public:
	static Named_interface* create_new_interface(std::string&) {
		return new Unit_scaling_transform_action();
	}

public:
	Unit_scaling_transform_action():Unary_action("unitScaling"){}
	virtual ~Unit_scaling_transform_action(){}

	virtual bool init(std::string& parameters, GsTL_project* proj, Error_messages_handler* errors);
	bool transform(const Geovalue::property_type& _val, Geovalue::property_type& _new_val);

protected :
	Geovalue::property_type min_;
	Geovalue::property_type max_;
};

#endif /* UNARY_ACTION_H_ */
