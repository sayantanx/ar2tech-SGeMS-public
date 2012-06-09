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



/*
 * project_selection_model.h
 *
 *  Created on: Jun 9, 2011
 *      Author: aboucher
 */

#ifndef PROJECT_SELECTION_MODEL_H_
#define PROJECT_SELECTION_MODEL_H_


//#include <GsTLAppli/gui/models/project_proxy_model.h>

#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>


class Project_selection_model : public QItemSelectionModel{

public:
	Project_selection_model(QAbstractItemModel * model);
	Project_selection_model(QAbstractItemModel * model, QObject* parent);
	virtual ~Project_selection_model();

public slots :
	virtual void select ( const QModelIndex & index, QItemSelectionModel::SelectionFlags command );
	virtual void select ( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command );
	virtual void reset();

private :

	//QList<QStringList> selections_;
	//QModelIndexList selected_indexes_;
	QItemSelection selections_;
  QSortFilterProxyModel* proxy_model_;
};






#endif /* PROJECT_SELECTION_MODEL_H_ */
