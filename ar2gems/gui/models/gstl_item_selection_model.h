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
 * gstl_item_selection_model.h
 *
 *  Created on: May 31, 2011
 *      Author: aboucher
 */

#ifndef GSTL_ITEM_SELECTION_MODEL_H_
#define GSTL_ITEM_SELECTION_MODEL_H_

#include <gui/models/gstl_item.h>
#include <gui/models/project_ItemModel.h>

#include <QItemSelectionModel>


/*
 * Selection constraint
 * 1 - Multiple grids
 * For a given grid:
 * 	- only one property
 *  - only one region
 *
 */

class GsTL_ItemSelectionModel : public QItemSelectionModel {
public:
	GsTL_ItemSelectionModel(Project_ItemModel * model);
	GsTL_ItemSelectionModel(Project_ItemModel * model, QObject* parent);
	virtual ~GsTL_ItemSelectionModel();

public slots :
	virtual void select ( const QModelIndex & index, QItemSelectionModel::SelectionFlags command );
	virtual void select ( const QItemSelection & selection, QItemSelectionModel::SelectionFlags command );
	virtual void reset();

private :

	//QList<QStringList> selections_;
	//QModelIndexList selected_indexes_;
	QItemSelection selections_;



};

#endif /* GSTL_ITEM_SELECTION_MODEL_H_ */
