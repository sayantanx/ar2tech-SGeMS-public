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



#ifndef __LIBRARY_QTPLUGINS_INIT_H__ 
#define __LIBRARY_QTPLUGINS_INIT_H__ 
 
#include <GsTLAppli/extra/qtplugins//common.h>
#include <GsTLAppli/appli/manager_repository.h> 
 
#include <iostream> 
 
/** 
 * As any library class, it has a init_lib() and release_lib() function, 
 * plus a reference count that keeps track of how many times the library 
 * has been loaded. 
 */ 
 
 
class QTPLUGINS_DECL library_qtplugins_init { 
  public: 
  static int init_lib() ; 
     
  static int release_lib(); 
 
  private: 
  static int references_; 
   
  static bool bind_grid_filter_items(Manager* dir); 

}; 
 
 
extern "C" QTPLUGINS_DECL int lib_qtplugins_init(); 
extern "C" QTPLUGINS_DECL int lib_qtplugins_release(); 

 
#endif 
