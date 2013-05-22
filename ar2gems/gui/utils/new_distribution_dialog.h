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



#ifndef CREATE_DISTRIBUTION_DIALOG_H
#define CREATE_DISTRIBUTION_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <gui/common.h>
#include "ui_create_distribution_dialog.h"
#include <qtplugins/distribution_selectors.h>
#include <qtplugins/distribution_parameter_widget.h>
#include <charts/chart_distribution.h>

class GUI_DECL  New_distribution_dialog : public QDialog
{
    Q_OBJECT

public:
    New_distribution_dialog(QWidget *parent = 0);
    ~New_distribution_dialog();

private slots:
 // void modify_params();
  void show_params_new(const QModelIndex& index);
  void show_params_existing(const QModelIndex& index);

  void create_edit_distribution();
  void create_edit_distribution_close();
  void close_dialog();

private:
    Ui::Create_distribution_dialog ui;

    Continuous_distribution_type_table* dist_type_tree_;
    Continuous_distribution_table* dist_tree_;
    Chart_distribution* chart_;

    Continuous_distribution_parameter_widget* params_;

    Continuous_distribution* dist_;
};

#endif // CREATE_DISTRIBUTION_DIALOG_H
