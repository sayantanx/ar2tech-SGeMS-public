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



#ifndef __NEW_REGION_FROM_GRID_FILTER_H__ 
#define __NEW_REGION_FROM_GRID_FILTER_H__ 

#include <gui/common.h>
#include <grid/grid_filter.h>

#include <QApplication>
#include <QDialog>
#include <QLineEdit>

class GridSelectorBasic;
class Grid_filter_selector;

class GsTL_project;



class New_region_from_grid_filter_dialog : public QDialog {
  Q_OBJECT

public:
  New_region_from_grid_filter_dialog( QWidget* parent = 0 );
  QString selected_grid() const;

  QString new_region_name() const;

private slots:

  bool create_region();
  void create_region_and_close();

private:
  GridSelectorBasic* gridSelector_;
  Grid_filter_selector* grid_filter_view_;

  bool isCategorical_;
  QLineEdit *new_region_;


};

 
#endif 
