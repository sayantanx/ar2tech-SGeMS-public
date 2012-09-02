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

#include <GsTLAppli/gui/QWidget_value_accessors/qspin_box_accessor.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <qstring.h>
#include <qdom.h>

#include <sstream>


QSpinBox_accessor::QSpinBox_accessor( QWidget* widget ) 
    : spin_box_(dynamic_cast<QSpinBox*>(widget) ) {}

    
bool QSpinBox_accessor::initialize( QWidget* widget ) {

  spin_box_ = dynamic_cast<QSpinBox*>(widget);
  if( spin_box_ == 0 )
    return false;
  
  return true;
}


std::string QSpinBox_accessor::value() const {
  std::string widget_name = String_Op::qstring2string(spin_box_->objectName());

  // Use a QString to convert a number to a string.
  QString qstr_value;

  qstr_value = qstr_value.setNum( spin_box_->value() );
  
  std::string str_value( String_Op::qstring2string(qstr_value) );


  return "<" + widget_name + "  value=\"" + str_value + "\" /> \n";
}


bool QSpinBox_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
    
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  QString qstr_value = elem.attribute( "value" );

  int val = qstr_value.toInt();
  spin_box_->setValue( val );
  return true;
}


void QSpinBox_accessor::clear() {
  spin_box_->setValue( spin_box_->minimum() );
}


Named_interface* QSpinBox_accessor::create_new_interface(std::string&) {
  return new QSpinBox_accessor;
}

/*
  ------------------------------------
*/


QDoubleSpinBox_accessor::QDoubleSpinBox_accessor( QWidget* widget ) 
    : spin_box_(dynamic_cast<QDoubleSpinBox*>(widget) ) {}

    
bool QDoubleSpinBox_accessor::initialize( QWidget* widget ) {

  spin_box_ = dynamic_cast<QDoubleSpinBox*>(widget);
  if( spin_box_ == 0 )
    return false;
  
  return true;
}


std::string QDoubleSpinBox_accessor::value() const {
  std::string widget_name = String_Op::qstring2string(spin_box_->objectName());

  // Use a QString to convert a number to a string.
  QString qstr_value;

  qstr_value = qstr_value.setNum( spin_box_->value() );
  
  std::string str_value( String_Op::qstring2string(qstr_value) );


  return "<" + widget_name + "  value=\"" + str_value + "\" /> \n";
}


bool QDoubleSpinBox_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
    
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  QString qstr_value = elem.attribute( "value" );

  int val = qstr_value.toInt();
  spin_box_->setValue( val );
  return true;
}


void QDoubleSpinBox_accessor::clear() {
  spin_box_->setValue( spin_box_->minimum() );
}


Named_interface* QDoubleSpinBox_accessor::create_new_interface(std::string&) {
  return new QDoubleSpinBox_accessor;
}
