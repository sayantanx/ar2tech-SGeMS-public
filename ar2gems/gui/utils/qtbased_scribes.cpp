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

#include <gui/utils/qtbased_scribes.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qtextedit.h>
#include <qregexp.h>


QTextedit_scribe::QTextedit_scribe( QTextEdit* message_box, 
                                    const QString& color ) {
  message_box_ = message_box;
  opening_tag_ = "";
  closing_tag_ = "";

  if( !color.isEmpty() ) {
    opening_tag_ = QString( "<font color=" ) + color + QString( ">" );
    closing_tag_ = "</font>";
  }
}


void QTextedit_scribe::write( const std::string& str, const Channel* ch ) {
  if( !message_box_ || str.empty() ) return ;

  QString qstr( str.c_str() );

  if( dynamic_cast<const Error_channel*>( ch ) )
    qstr.prepend( "<font color=red>error</font>: " );

  if( !opening_tag_.isEmpty() ) {
    QStringList list = qstr.split("\n", QString::SkipEmptyParts);
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
       (*it).prepend( opening_tag_ );
       (*it).append( closing_tag_ );
       message_box_->append( *it );
    }
  }
  else {
    message_box_->append( qstr  );
  }
}
