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



#include <GsTLAppli/qtplugins/distribution_selectors.h>

#include <GsTLAppli/qtplugins/distribution_proxy_model.h>

#include <QHeaderView>

Distribution_tree::Distribution_tree(QWidget *parent)
  : QTreeView(parent)
{
  Distribution_proxy_model* model = new  Distribution_proxy_model(this);
  this->setModel(model);
  this->setRootIndex(model->distribution_root_index());
}

Distribution_tree::~Distribution_tree()
{

}


Continuous_distribution_table::Continuous_distribution_table(QWidget *parent)
  : QTableView(parent)
{
  Distribution_proxy_model* model = new  Distribution_proxy_model(this);
  this->setModel(model);
  this->setRootIndex(model->continuous_distribution_root_index());
  this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

Continuous_distribution_table::~Continuous_distribution_table()
{

}

Continuous_distribution_selector::Continuous_distribution_selector(QWidget *parent)
  : QComboBox(parent)
{
  Distribution_proxy_model* model = new  Distribution_proxy_model(this);
  this->setModel(model);
  this->setRootModelIndex(model->continuous_distribution_root_index());

}

Continuous_distribution_selector::~Continuous_distribution_selector()
{
  
}


Categorical_distribution_table::Categorical_distribution_table(QWidget *parent)
  : QTableView(parent)
{
  Distribution_proxy_model* model = new  Distribution_proxy_model(this);
  this->setModel(model);
  this->setRootIndex(model->categorical_distribution_root_index());
  this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  
}

Categorical_distribution_table::~Categorical_distribution_table()
{

}



Distribution_type_tree::Distribution_type_tree(QWidget *parent)
  : QTreeView(parent)
{
  Distribution_type_proxy_model* model = new  Distribution_type_proxy_model(this);
  this->setModel(model);
  this->setRootIndex(model->distribution_root_index());
}

Distribution_type_tree::~Distribution_type_tree()
{

}


Continuous_distribution_type_table::Continuous_distribution_type_table(QWidget *parent)
  : QTableView(parent)
{
  Distribution_type_proxy_model* model = new  Distribution_type_proxy_model(this);
  this->setModel(model);
  this->setRootIndex(model->continuous_distribution_root_index());
  this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

Continuous_distribution_type_table::~Continuous_distribution_type_table()
{

}


Categorical_distribution_type_table::Categorical_distribution_type_table(QWidget *parent)
  : QTableView(parent)
{
  Distribution_type_proxy_model* model = new  Distribution_type_proxy_model(this);
  this->setModel(model);
  this->setRootIndex(model->categorical_distribution_root_index());
  this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
}

Categorical_distribution_type_table::~Categorical_distribution_type_table()
{

}