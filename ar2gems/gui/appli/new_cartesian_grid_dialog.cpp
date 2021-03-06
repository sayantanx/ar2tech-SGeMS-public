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

#include <gui/appli/new_cartesian_grid_dialog.h>
#include <actions/defines.h>
#include <utils/string_manipulation.h>

#include <qobject.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qmessagebox.h>

#include <sstream>

using namespace String_Op;

NewCartesianGridDialog::NewCartesianGridDialog( QWidget* parent,
						const char* name )
  : QDialog( parent, Qt::Window ) {

  setupUi(this);
  QObject::connect( (QObject*) ok_button_, SIGNAL(clicked()),
		    this, SLOT(accept()) );
  QObject::connect( (QObject*) cancel_button_, SIGNAL(clicked()),
		    this, SLOT(reject()) );
  
}


std::string NewCartesianGridDialog::grid_parameters() const {
  std::ostringstream out;
  QByteArray tmp = name_->text().toLatin1();
  out << tmp.trimmed().constData() << Actions::separator
      << qstring2string(nx_->text()) << Actions::separator
      << qstring2string(ny_->text()) << Actions::separator
      << qstring2string(nz_->text()) << Actions::separator
      << qstring2string(x_size_->text()) << Actions::separator
      << qstring2string(y_size_->text()) << Actions::separator
      << qstring2string(z_size_->text()) << Actions::separator
      << qstring2string(xmn_->text()) << Actions::separator
      << qstring2string(ymn_->text()) << Actions::separator
      << qstring2string(zmn_->text()) << Actions::separator
      << qstring2string(rotation_z_angle_->cleanText());
  return out.str();
}


void NewCartesianGridDialog::accept() {
  if( name_->text().isEmpty() ) {
    QMessageBox::warning( this, "Warning", "A name for the new  grid must be provided",
                           QMessageBox::Ok,  Qt::NoButton );
    return;
  }

  QDialog::accept();
}

