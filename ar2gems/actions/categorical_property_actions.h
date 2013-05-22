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


#ifndef __CATEGORICAL_PROPERTY_ACTIONS_H__ 
#define __CATEGORICAL_PROPERTY_ACTIONS_H__ 

#include <GsTLAppli/actions/common.h>
#include <GsTLAppli/actions/action.h> 


#include <qstring.h>
 
#include <string> 
#include <set>

class GsTL_project; 
class Error_messages_handler;



class ACTIONS_DECL Convert_continuous_to_categorical_property: public Action {
 public:
  static Named_interface* create_new_interface( std::string& );

 public:
  Convert_continuous_to_categorical_property() {}
  virtual ~Convert_continuous_to_categorical_property() {}

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors );
  virtual bool exec();
};


class ACTIONS_DECL Convert_categorical_property_to_sequential_coding : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Convert_categorical_property_to_sequential_coding() {} 
  virtual ~Convert_categorical_property_to_sequential_coding() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec(); 

protected : 

}; 


class ACTIONS_DECL Convert_definition_to_sequential_coding : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Convert_definition_to_sequential_coding() {} 
  virtual ~Convert_definition_to_sequential_coding() {}
 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();

protected : 

}; 

#endif
