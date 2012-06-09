/* -----------------------------------------------------------------------------
** Copyright© 2012 Advanced Resources and Risk Technology, LLC
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



#ifndef COPY_PROPERTY_DIALOG_H
#define COPY_PROPERTY_DIALOG_H

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/appli/project.h>

#include <GsTLAppli/extra/qtplugins/selectors.h>

#include <QDialog>
#include <QCheckBox>


class GUI_DECL Copy_property_dialog : public QDialog {
  Q_OBJECT

public:
  Copy_property_dialog( GsTL_project* proj,
                        QWidget* parent = 0, const char* name = 0 );
  QString source() const;
  QString source_property() const ;
  QString destination() const;
  QString destination_property() const;
  bool overwrite() const;
  bool mark_as_hard() const;

  std::string parameters() const;

public slots:
  void copy_property();

protected:
  GridSelectorBasic* source_;
  SinglePropertySelector* source_property_;
  GridSelectorBasic* destination_;
  SinglePropertySelector* destination_property_;
//  QComboBox* destination_property_;
  QCheckBox* overwrite_;
  QCheckBox* mark_as_hard_;

  GsTL_project* project_;

};

#endif // COPY_PROPERTY_DIALOG_H
