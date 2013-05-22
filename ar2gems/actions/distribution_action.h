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



#ifndef __GSTLAPPLI_DISTRIBUTION_ACTION_H__ 
#define __GSTLAPPLI_DISTRIBUTION_ACTION_H__

#include <GsTLAppli/actions/action.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/project.h>
#include<vector>

class New_distribution_action :  public Action
{
public:

  static Named_interface* create_new_interface( std::string& );

  New_distribution_action(void);
  virtual ~New_distribution_action(void);

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();

private:

  std::string dist_type_;
  std::string dist_name_;
  std::vector<float> params_;
  bool is_continuous_;
};


class Load_distribution_action :  public Action
{
public:

  static Named_interface* create_new_interface( std::string& );

  Load_distribution_action(void);
  virtual ~Load_distribution_action(void);

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();

private:

};

class Save_distribution_action :  public Action
{
public:

  static Named_interface* create_new_interface( std::string& );

  Save_distribution_action(void);
  virtual ~Save_distribution_action(void);

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();

private:

};


class Delete_distribution_action :  public Action
{
public:

  static Named_interface* create_new_interface( std::string& );

  Delete_distribution_action(void);
  virtual ~Delete_distribution_action(void);

  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ); 
  virtual bool exec();

private:

};

#endif