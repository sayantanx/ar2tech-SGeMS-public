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



#ifndef HISTOGRAM_CHART_MODEL_H
#define HISTOGRAM_CHART_MODEL_H

#include <GsTLAppli/charts/common.h>
#include <GsTLAppli/appli/root_model.h>
#include <QSortFilterProxyModel>
#include <QMimeData>

class CHARTS_DECL Continuous_histo_chart_model : public QSortFilterProxyModel
{
  Q_OBJECT

public:
    Continuous_histo_chart_model(QObject *parent);
    ~Continuous_histo_chart_model();

  QModelIndex root_index();
  int columnCount(const QModelIndex &/*parent*/) const {return 1;}


  virtual QMimeData* mimeData(const QModelIndexList &indexes) const;

protected :
  bool filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const;

private :
  Root_model* model_;
    
};


class CHARTS_DECL Categorical_histo_chart_model : public QSortFilterProxyModel
{
  Q_OBJECT

public:
    Categorical_histo_chart_model(QObject *parent);
    ~Categorical_histo_chart_model();

  QModelIndex root_index();
  int columnCount(const QModelIndex &/*parent*/) const {return 1;}

protected :
  bool filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const;
 
private :
  Root_model* model_;
};

#endif // HISTOGRAM_CHART_MODEL_H
