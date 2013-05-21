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
 * manager_dialog.cpp
 *
 *  Created on: Jun 6, 2011
 *      Author: aboucher
 */


#include <GsTLAppli/gui/utils/manager_dialog.h>

#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <GsTLAppli/qtplugins/project_proxy_model.h>

#include <QTreeView>
#include <QVBoxLayout>
#include <QAbstractItemModel>


Manager_dialog::Manager_dialog(QWidget* parent) : QDialog(parent){

    QTreeView* tree = new QTreeView(this);
    this->setLayout(new QVBoxLayout(this));
    this->layout()->addWidget(tree);

    Root_model* model = dynamic_cast<Root_model*>(Root::instance()->model());
    tree->setModel(model);
    tree->setRootIndex(model->index(0,0,QModelIndex()));
}

Manager_dialog::~Manager_dialog() {
	// TODO Auto-generated destructor stub
}


Project_dialog::Project_dialog(QWidget* parent) : QDialog(parent){

    QTreeView* tree = new QTreeView(this);
    this->setLayout(new QVBoxLayout(this));
    this->layout()->addWidget(tree);

    Filter_root_proxy_model* proxy_model = new Filter_root_proxy_model(this);
    tree->setModel(proxy_model);
    tree->setRootIndex(proxy_model->project_root_index());
}

Project_dialog::~Project_dialog() {
	// TODO Auto-generated destructor stub
}
