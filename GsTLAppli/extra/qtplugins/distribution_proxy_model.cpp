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



#include "distribution_proxy_model.h"

#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/manager.h>

Distribution_proxy_model::Distribution_proxy_model(QObject *parent)
  : QSortFilterProxyModel(parent)
{
  Root_model* model = dynamic_cast<Root_model*>(Root::instance()->model());
  if(model)
	  this->setSourceModel(model);
  this->setDynamicSortFilter(true);
}

Distribution_proxy_model::~Distribution_proxy_model()
{

}

QModelIndex Distribution_proxy_model::distribution_root_index(){

  if(this->sourceModel() == 0) return QModelIndex();

  Root_model* source_model = dynamic_cast<Root_model*>(this->sourceModel());
  QModelIndex src_index = source_model->top_level_manager_index(distributions_manager.c_str());
  return this->mapFromSource(src_index); 
}

QModelIndex Distribution_proxy_model::continuous_distribution_root_index(){
  if(this->sourceModel() == 0) return QModelIndex();

  Root_model* source_model = dynamic_cast<Root_model*>(this->sourceModel());
  QModelIndex src_index = source_model->top_level_manager_index(distributions_manager.c_str());

  int n = source_model->rowCount(src_index);
  for(int i=0; i<n; ++i) {
    QModelIndex index = src_index.child(i,0);
    if(index.data() == "Continuous") return this->mapFromSource(index); 
  }
  return QModelIndex();

/*
  QModelIndex parent = this->distribution_root_index();
  return parent.child(0,0);
  */
}
QModelIndex Distribution_proxy_model::categorical_distribution_root_index(){
  QModelIndex parent = this->distribution_root_index();
  return parent.child(1,0);
}

bool Distribution_proxy_model::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
  QModelIndex index1 = this->sourceModel()->index(sourceRow, 1, sourceParent);

  return this->sourceModel()->data(index1).toString() != "Factory";

}

/*
  ------------------------------------------------------
*/

Distribution_type_proxy_model::Distribution_type_proxy_model(QObject *parent)
  : QSortFilterProxyModel(parent)
{
  Root_model* model = dynamic_cast<Root_model*>(Root::instance()->model());
  if(model)
	  this->setSourceModel(model);
  this->setDynamicSortFilter(true);
}

Distribution_type_proxy_model::~Distribution_type_proxy_model()
{

}

QModelIndex Distribution_type_proxy_model::distribution_root_index(){

  if(this->sourceModel() == 0) return QModelIndex();

  Root_model* source_model = dynamic_cast<Root_model*>(this->sourceModel());
  QModelIndex src_index = source_model->top_level_manager_index(distributions_manager.c_str());

  return this->mapFromSource(src_index); 

}

QModelIndex Distribution_type_proxy_model::continuous_distribution_root_index(){
  if(this->sourceModel() == 0) return QModelIndex();

  Root_model* source_model = dynamic_cast<Root_model*>(this->sourceModel());
//  QModelIndex src_index = source_model->top_level_manager_index(distributions_manager.c_str());
  QModelIndex src_index = source_model->top_level_manager_index(continuous_distributions_manager.c_str());
 // QModelIndex src_index = source_model->manager_index(continuous_distributions_manager.c_str());

  return this->mapFromSource(src_index);

/*
  int n = source_model->rowCount(src_index);
  for(int i=0; i<n; ++i) {
    QModelIndex index = src_index.child(i,0);
    if(index.data() == "Continuous") return this->mapFromSource(index); 
  }
  return QModelIndex();
  */

//  QModelIndex parent = this->distribution_root_index();
//  return parent.child(0,0);
}
QModelIndex Distribution_type_proxy_model::categorical_distribution_root_index(){
  QModelIndex parent = this->distribution_root_index();
  return parent.child(1,0);
}

bool Distribution_type_proxy_model::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
  QModelIndex index1 = this->sourceModel()->index(sourceRow, 1, sourceParent);

  return this->sourceModel()->data(index1).toString() == "Factory" || this->sourceModel()->data(index1).toString() == "Manager";

}