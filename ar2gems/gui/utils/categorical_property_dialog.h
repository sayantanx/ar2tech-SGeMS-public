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


#ifndef _CATEGORICAL_PROPERTY_DIALOG_H__
#define _CATEGORICAL_PROPERTY_DIALOG_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/qtplugins/categorical_selectors.h>

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>

#include <QLineEdit>

#include <qdialog.h>


class GUI_DECL Convert_to_categorical_dialog : public QDialog {

  Q_OBJECT

public:

  Convert_to_categorical_dialog( GsTL_project* proj,QWidget* parent = 0, const char* name = 0 );
  virtual ~Convert_to_categorical_dialog() {}
  
protected slots:
  void execute();
  void execute_close();

private:
  GridSelectorBasic* grid_selector_;
  OrderedPropertySelector* props_selector_;;
  GsTL_project* project_;
};


class GUI_DECL Sequentialize_categorical_property_dialog : public QDialog {

  Q_OBJECT

public:

  Sequentialize_categorical_property_dialog( GsTL_project* proj,QWidget* parent = 0, const char* name = 0 );
  virtual ~Sequentialize_categorical_property_dialog() {}
  
protected slots:
  void execute();
  void execute_close();

private:
  GridSelectorBasic* grid_selector_;
  SingleCategoricalPropertySelector* cprop_selector_;
  GsTL_project* project_;

};

class GUI_DECL Sequentialize_categorical_definition_dialog : public QDialog {

  Q_OBJECT

public:

  Sequentialize_categorical_definition_dialog( GsTL_project* proj,QWidget* parent = 0, const char* name = 0 );
  virtual ~Sequentialize_categorical_definition_dialog() {}

protected slots:
  void execute();
  void execute_close();

private:
  CategoricalDefinitionSelector* cdef_selector_;
  GsTL_project* project_;

};

#endif

