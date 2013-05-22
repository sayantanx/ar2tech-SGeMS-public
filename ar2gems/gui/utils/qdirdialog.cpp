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
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
** Modeling Software (GEMS)
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
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#include <GsTLAppli/gui/utils/qdirdialog.h>


QDirDialog::QDirDialog( const QString& extension, 
                        const QString& dirName, QWidget* parent,
                        const char* name, 
                        QFileDialog::FileMode mode,
                        bool modal )
  : QFileDialog( parent, QString::null, dirName),
    extension_( extension ) {

  if(name)
    setObjectName(name);

  setModal(modal);
  init_dialog( mode );
}

QDirDialog::QDirDialog( const QString& extension, 
                        QWidget* parent,
                        const char* name, 
                        QFileDialog::FileMode mode,
                        bool modal )
  : QFileDialog( parent, Qt::Dialog),
    extension_( extension ) {

  setModal(modal);
  if(name)
    setObjectName(name);
  
  init_dialog( mode );
}


void QDirDialog::check_selected_directory( const QString& dirName ) {
  if( !dirName.endsWith( extension_, Qt::CaseInsensitive) ) return;

  selected_directory_ = dirName;
  this->accept();
}


void QDirDialog::init_dialog( QFileDialog::FileMode mode ) {
  setFileMode( mode );

  QObject::connect( this, SIGNAL( directoryEntered( const QString& ) ),
                    this, SLOT( check_selected_directory( const QString& ) ) );

}


void QDirDialog::accept() {
  QDialog::accept();
  QStringList sl = selectedFiles();
  if( selected_directory_.isEmpty() ) {
    selected_directory_ = sl[0];
  }
}
