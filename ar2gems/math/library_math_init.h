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





#ifndef __GSTLAPPLI_LIBRARY_MATH_INIT_H__ 
#define __GSTLAPPLI_LIBRARY_MATH_INIT_H__ 

#include <math/common.h>
#include <utils/manager.h>
 
class MATH_DECL library_math_init { 
 public: 
  static int init_lib() ; 
   
  static int release_lib(); 
 
 private: 
  static int references_; 
   
  static bool bind_continuous_distribution_factories(Manager* dir);
  static bool bind_categorical_distribution_factories(Manager* dir);
//  static bool bind_model_factories(Manager* dir);
}; 
 
 
 
extern "C" MATH_DECL int libGsTLAppli_math_init(); 
extern "C" MATH_DECL int libGsTLAppli_math_release(); 
 
 
 
 
#endif 
