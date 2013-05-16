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
 * rename_dialog.h
 *
 *  Created on: May 27, 2011
 *      Author: aboucher
 */

#ifndef RENAME_DIALOG_H_
#define RENAME_DIALOG_H_

#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/extra/qtplugins/common.h>

#include <QDialog>
#include <QLineEdit>

class QTPLUGINS_DECL Rename_dialog : public QDialog
{

	Q_OBJECT
public:
	Rename_dialog( QString object_type, QString old_name,  QWidget* parent = 0);
	virtual ~Rename_dialog();

	QString new_name();

protected :
	QLineEdit* line_new_name_;
};

#endif /* RENAME_DIALOG_H_ */
