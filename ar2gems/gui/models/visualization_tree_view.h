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
 * visualization_tree_view.h
 *
 *  Created on: May 25, 2011
 *      Author: aboucher
 */

#ifndef VISUALIZATION_TREE_VIEW_H_
#define VISUALIZATION_TREE_VIEW_H_

//#include <GsTLAppli/gui/models/gstl_item.h>
#include <GsTLAppli/utils/gstl_object_item.h>

#include <QTreeView>
#include <QContextMenuEvent>
#include <QPaintEvent>


class Visualization_tree_view : public QTreeView{

	Q_OBJECT

public:
	Visualization_tree_view(QWidget* parent=0);
	virtual ~Visualization_tree_view();


protected :

protected slots:
	void contextMenu(const QPoint& positionXY);

//	QItemSelectionModel::SelectionFlags
//	selectionCommand ( const QModelIndex & index, const QEvent * event = 0 ) const;


//	bool delete_grid();
/*
	void delete_object_from_tree();
	void delete_property_from_tree();
	void delete_property_group_from_tree();
	void delete_region_from_tree();
*/
//	void contextMenuEvent(QContextMenuEvent *event);
};

#endif /* VISUALIZATION_TREE_VIEW_H_ */
