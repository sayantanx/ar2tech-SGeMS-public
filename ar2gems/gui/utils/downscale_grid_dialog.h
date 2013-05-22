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


#ifndef DOWNSCALE_GRID_DIALOG_H
#define DOWNSCALE_GRID_DIALOG_H

#include <QDialog>

class QSpinBox;
class GridSelector;
class QLineEdit;

class Downscale_grid_dialog : public QDialog
{
  Q_OBJECT

public:
    Downscale_grid_dialog(QWidget *parent);
    ~Downscale_grid_dialog();

protected slots:
  void create_and_close();
  void create();

private:

  QSpinBox* nx_;
  QSpinBox* ny_;
  QSpinBox* nz_;
  GridSelector* grid_selector_;
  QLineEdit* grid_name_;


    
};

#endif // DOWNSCALE_GRID_DIALOG_H
