/* -----------------------------------------------------------------------------
** Copyright� 2012 Advanced Resources and Risk Technology, LLC
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



#ifndef SAVE_GRID_DIALOG_H
#define SAVE_GRID_DIALOG_H

#include <gui/common.h>
#include <appli/project.h>

#include <QFileDialog>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>
#include <QString>
#include <QFrame>
//-----------------------------------------------
/** Dialog box to save a grid. 
 */ 
class GUI_DECL Save_grid_dialog : public QFileDialog { 
 
  Q_OBJECT 
 
 public: 
  Save_grid_dialog( const GsTL_project* project,  
		    QWidget * parent = 0, const char * name = 0 ); 
  QString selected_grid() const; 
  const std::vector<QString> & selected_p() const { return selected_; }
  bool maskToRegular() {
	  if (!saveRegular_->isEnabled()) return false;
	  return saveRegular_->isChecked();
  }
 
  protected slots: 
    virtual void gridChanged(const QString &);
    virtual void accept();
 
 private: 
  QComboBox* grid_selector_; 
  QListWidget * propList_;
  std::vector<QString> selected_;

  QCheckBox * saveRegular_;
  QFrame * masked_as_regular_frame_;

}; 
 

#endif // SAVE_GRID_DIALOG_H
