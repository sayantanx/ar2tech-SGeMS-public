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
 Author: Aboucher: ar2tech
 Copyright: ar2tech
*/
//-----------------------------------------------
/** Dialog box to display about sgems 
 */

#ifndef ABOUT_SGEMS_H_
#define ABOUT_SGEMS_H_

#include <GsTLAppli/gui/common.h>
#include <QDialog>
#include <QTabWidget>

class GUI_DECL About_sgems : public QDialog
{
public:
  About_sgems(QWidget * parent = 0);


private:
  QTabWidget* tab_;

  void insert_about_page();
  void insert_what_is_new_page();
  void insert_python_page();
  void insert_ar2tech_page();
  void insert_SCRF_page();
  void insert_help_page();
  

  
  
};

#endif