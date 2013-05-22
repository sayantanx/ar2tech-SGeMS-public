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
 * project_proxy_model.h
 *
 *  Created on: Jun 8, 2011
 *      Author: aboucher
 */

#ifndef PROJECT_PROXY_MODEL_H_
#define PROJECT_PROXY_MODEL_H_

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/qtplugins/common.h>

#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QAbstractProxyModel>
#include <QSortFilterProxyModel>

#include <set>

class QTPLUGINS_DECL Filter_root_proxy_model : public QSortFilterProxyModel{
  Q_OBJECT
public :
  Filter_root_proxy_model(QObject *parent = 0);
  virtual ~Filter_root_proxy_model();

  QModelIndex project_root_index();
  QModelIndex grid_root_index(QString grid_name);
  QModelIndex property_root_index(QString grid_name);
  QModelIndex region_root_index(QString grid_name);
  QModelIndex manager_index(QString manager_path);
  /*
  QModelIndex mds_root_index();
  QModelIndex mdsSpace_root_index(QString space_name);
  QModelIndex mds_metric_property_root_index(QString space_name);
  QModelIndex mds_metric_response_root_index(QString space_name);
  */
  QModelIndex response_root_index();
  QModelIndex response_property_root_index(QString response_name);

  int columnCount(const QModelIndex &/*parent*/) const;

protected :
  bool filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const;

};

class QTPLUGINS_DECL Filter_root_proxy_checkable_model : public Filter_root_proxy_model{
  Q_OBJECT
public :
  Filter_root_proxy_checkable_model(QObject *parent = 0);
  virtual ~Filter_root_proxy_checkable_model(){}

  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
  
  virtual bool	removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );


  Qt::ItemFlags flags(const QModelIndex &index) const;

signals:
  void item_checked(const QModelIndex& index);

protected slots:
  void update_checked_items(const QModelIndex& index);
  void beginRemoveRows ( const QModelIndex & parent, int first, int last );

private :
  void update_indexes(const QList<QModelIndex>& src_indexes);

private :
  std::map<GsTL_object_item*, bool> checked_states_;
  std::set<GsTL_object_item*> currently_selected_;
  //std::map<void*, bool> checked_states_;
};



class QTPLUGINS_DECL Visualization_proxy_model : public QAbstractProxyModel{

	Q_OBJECT
public:
	Visualization_proxy_model(QObject *parent = 0);
	virtual ~Visualization_proxy_model();

  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;
  virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
  virtual QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex &child) const;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex	mapFromSource ( const QModelIndex & sourceIndex ) const;
  virtual QModelIndex	mapToSource ( const QModelIndex & proxyIndex ) const;

signals:
  void item_checked(const QModelIndex& index);

protected slots:
  void update_checked_items(const QModelIndex& index);

private :
  void update_indexes(const QList<QModelIndex>& src_indexes);
  void remove_selections(const QModelIndex& index);
  void remove_selections(GsTL_object_item* item);

private slots :
	void begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
private :
//	GsTL_object_item* root_;
	Manager* root_;
  std::map<GsTL_object_item*, bool> checked_states_;
  std::set<GsTL_object_item*> currently_selected_;

  bool removal_flag_;

};



#endif /* PROJECT_PROXY_MODEL_H_ */
