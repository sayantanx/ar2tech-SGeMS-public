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



#ifndef DISTRIBUTION_PROXY_MODEL_H
#define DISTRIBUTION_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class Distribution_proxy_model : public QSortFilterProxyModel
{
  Q_OBJECT

public:
    Distribution_proxy_model(QObject *parent);
    ~Distribution_proxy_model();

  int columnCount(const QModelIndex &/*parent*/) const {return 1;}

  QModelIndex distribution_root_index();
  QModelIndex continuous_distribution_root_index();
  QModelIndex categorical_distribution_root_index();

protected :
  bool filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const;

    
};

class Distribution_type_proxy_model : public QSortFilterProxyModel
{
  Q_OBJECT

public:
    Distribution_type_proxy_model(QObject *parent);
    ~Distribution_type_proxy_model();

  int columnCount(const QModelIndex &/*parent*/) const{return 1;}

  QModelIndex distribution_root_index();
  QModelIndex continuous_distribution_root_index();
  QModelIndex categorical_distribution_root_index();

protected :
  bool filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const;

    
};


#endif // DISTRIBUTION_PROXY_MODEL_H
