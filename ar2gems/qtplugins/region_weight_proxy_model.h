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



#ifndef REGION_WEIGHT_PROXY_MODEL_H
#define REGION_WEIGHT_PROXY_MODEL_H


#include <utils/manager.h>
#include <appli/root_model.h>
#include <grid/grid_model/geostat_grid.h>
#include <qtplugins/common.h>


#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QAbstractProxyModel>



class QTPLUGINS_DECL Region_weight_proxy_model : public QAbstractProxyModel{

	Q_OBJECT
public:
	Region_weight_proxy_model(QString grid_name, QObject *parent = 0);
	virtual ~Region_weight_proxy_model();


    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex	mapFromSource ( const QModelIndex & sourceIndex ) const;
    virtual QModelIndex	mapToSource ( const QModelIndex & proxyIndex ) const;

protected slots :
	virtual void begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);

protected :

	Manager* grid_manager_;
  Root_model* model_;
  QString grid_name_;
  Geostat_grid* grid_;

protected :

  QStringList region_names_;
  QStringList weight_names_;

};


class QTPLUGINS_DECL Region_weight_proxy_model_no_selection : public Region_weight_proxy_model{

	Q_OBJECT
public:
	Region_weight_proxy_model_no_selection(QString grid_name,QObject *parent = 0);
	virtual ~Region_weight_proxy_model_no_selection();


//    virtual QVariant data(const QModelIndex &index, int role) const;
//    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
//    virtual QVariant headerData(int section, Qt::Orientation orientation,
//                        int role = Qt::DisplayRole) const;
//    virtual QModelIndex index(int row, int column,
//                      const QModelIndex &parent = QModelIndex()) const;
//    virtual QModelIndex parent(const QModelIndex &child) const;
//    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
//    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex	mapFromSource ( const QModelIndex & sourceIndex ) const;
    virtual QModelIndex	mapToSource ( const QModelIndex & proxyIndex ) const;

//	virtual QStringList mimeTypes() const;
//	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

protected slots :
	virtual void begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
private :


};


#endif // REGION_WEIGHT_PROXY_MODEL_H
