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



#ifndef CHART_DIALOG_H
#define CHART_DIALOG_H

#include <charts/chart_mdi_area.h>
#include <charts/common.h>

#include <QDialog>
#include <QMainWindow>
#include <QItemSelectionModel>

class CHARTS_DECL Chart_dialog : public QMainWindow 
{
  Q_OBJECT

public:
    Chart_dialog(QWidget *parent);
    ~Chart_dialog();

private:

  Chart_mdi_area* mdi_area_;
  //QMdiArea* mdi_area_;
//  QItemSelectionModel* cont_histo_selection_;
//  QItemSelectionModel* cat_histo_selection_;

  /*
private slots:
  void show_continuous_histogram();
  void show_categorical_histogram();
  

private :
  QWidget* build_continuous_histogram_selector(QWidget* parent);
  QWidget* build_categorical_histogram_selector(QWidget* parent);

  Continuous_histo_chart_model* cont_histo_chart_model_;
  */  
};

#endif // EDA_DIALOG_H
