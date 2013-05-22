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



#ifndef __GSTLAPPLI_NSET_NOT_INFORMED_DIALOG_H__ 
#define __GSTLAPPLI_NSET_NOT_INFORMED_DIALOG_H__ 

#include <GsTLAppli/gui/common.h>

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/qlineedit.h>

class GridSelectorBasic;
class SinglePropertySelector;
class MultipleCategorySelector;
class QGroupBox;

class GsTL_project;



class Set_not_informed_dialog : public QDialog {
  Q_OBJECT

public:
  Set_not_informed_dialog( GsTL_project* proj,
                        QWidget* parent = 0, const char* name = 0 );
  QString selected_grid() const;
  QString selected_property() const ;
  QString new_region_name() const;
  QString get_min_filter_value() const;
  QString get_max_filter_value() const;

protected slots:

  bool execute();
  void execute_and_close();

protected:
  GsTL_project* project_;
  GridSelectorBasic* gridSelector_;
  SinglePropertySelector* propSelector_;


  QGroupBox* min_max_box_;
  QLineEdit *minFilter_, *maxFilter_;


};

 
#endif 
