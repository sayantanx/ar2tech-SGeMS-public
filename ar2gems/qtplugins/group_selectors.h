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



/**********************************************************************
** Author: Alexandre Boucher
** Copyright (C) 2002-2004 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of the "extra" module of the Stanford Geostatistical Earth
** Modeling Software (SGEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact Advanced Resources and Risk Technology, LLC
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#ifndef __GSTLAPPLI_QTPLUGINS_GROUPSELECTORS_H__
#define __GSTLAPPLI_QTPLUGINS_GROUPSELECTORS_H__


#include <GsTLAppli/qtplugins/common.h>
#include <GsTLAppli/qtplugins/selectors.h>
#include <GsTLAppli/appli/project.h>

#include <qcombobox.h>
#include <QListWidget>
#include <QGroupBox>
#include <qdialog.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>

#include <vector>
#include <set>

class QString;
class QLabel;
class QKeyEvent;




/** Enables to choose a single property from a grid
 */ 
class QTPLUGINS_DECL  SinglePropertyGroupSelector : public QComboBox {

  Q_OBJECT

 public:
  SinglePropertyGroupSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~SinglePropertyGroupSelector() {}
  
  public slots:
    virtual void show_property_groups( const QString& grid_name );

  protected:
    QString current_grid_;
};



/** A widget to choose multiple properties from a grid
 */ 
class QTPLUGINS_DECL  MultiPropertyGroupSelector : public QListWidget {

  Q_OBJECT

 public:
  MultiPropertyGroupSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultiPropertyGroupSelector() {}
  
  public slots:
    void show_property_groups( const QString& grid_name );

  protected slots:
    void selection_size();

 signals:
  void selected_count( int );
};

#endif