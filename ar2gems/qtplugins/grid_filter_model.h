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



#ifndef GRID_FILTER_MODEL_H
#define GRID_FILTER_MODEL_H

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>

#include <GsTLAppli/qtplugins/common.h>
#include <GsTLAppli/qtplugins/grid_filter_model_item.h>

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QAbstractProxyModel>
#include <QAbstractTableModel>
#include <QColor>

/*
class QTPLUGINS_DECL Grid_filter_proxy_model : public QSortFilterProxyModel{

	Q_OBJECT
public:
	Grid_filter_proxy_model(QObject *parent = 0);
	virtual ~Grid_filter_proxy_model();



};

*/



class Grid_filter_model : public QAbstractItemModel
{
  Q_OBJECT

public:
    Grid_filter_model(QObject *parent);
    Grid_filter_model(const Geostat_grid* grid, int ncat,  QObject *parent);
    ~Grid_filter_model();

    void initialize();

    Grid_filter* create_filter() const;
    QStringList get_grid_filter_name() const;

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;


    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    const Grid_filter_item* get_grid_filter_item(int row) const {return grid_filters_[row];}
    


public slots :
  void set_grid(const Geostat_grid* grid);
  void set_grid(const QString& grid_name);

private slots :

private:

  const Geostat_grid* grid_;
  Manager* grid_filter_manager_;

  QString available_filters_;

  std::vector<Grid_filter_item*> grid_filters_;

   
};




#endif // CATEGORICAL_DEFINITION_PROXY_MODEL_H
