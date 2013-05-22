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



#ifndef __GSTLAPPLI_PROPERTY_TRANSFORMER_ACTIONS_H__ 
#define __GSTLAPPLI_PROPERTY_TRANSFORMER_ACTIONS_H__ 

#include <GsTLAppli/actions/common.h>
#include <GsTLAppli/actions/action.h> 


#include <qstring.h>
 
#include <string> 
#include <set>


class Error_messages_handler;

class ACTIONS_DECL New_pca_transformer : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  New_pca_transformer() {} 
  virtual ~New_pca_transformer() {} 
   
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();
 
}; 


class ACTIONS_DECL Multivariate_forward_transform : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Multivariate_forward_transform() {} 
  virtual ~Multivariate_forward_transform() {} 
   
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();
 
}; 


class ACTIONS_DECL Multivariate_back_transform : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Multivariate_back_transform() {} 
  virtual ~Multivariate_back_transform() {} 
   
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();
 
};


class ACTIONS_DECL Delete_multivariate_transforms : public Action { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  Delete_multivariate_transforms() {} 
  virtual ~Delete_multivariate_transforms() {} 
   
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();
 
};

#endif
