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

#include <GsTLAppli/gui/QWidget_value_accessors/qcombobox_accessor.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <qstring.h>
#include <qdom.h>
#include <qtimer.h>



QComboBox_accessor::QComboBox_accessor( QWidget* widget ) 
    : combo_box_(dynamic_cast<QComboBox*>(widget) ) {
  helper_ = new ComboBoxHelper(0);
}

    
bool QComboBox_accessor::initialize( QWidget* widget ) {

  combo_box_ = dynamic_cast<QComboBox*>(widget);
  if( combo_box_ == 0 )
    return false;
  
  return true;
}


std::string QComboBox_accessor::value() const {
  std::string widget_name = String_Op::qstring2string(combo_box_->objectName());
  std::string val;

  if( combo_box_->currentText().isEmpty() )
     val="";
  else 
    val = String_Op::qstring2string(combo_box_->currentText());

  return "<" + widget_name + "  value=\"" + val + "\"  /> \n";
}


bool QComboBox_accessor::set_value( const std::string& str ) {
  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  QString value = elem.attribute( "value" );
  
  // select entry "value" in the combobox. If there is no such entry,
  // either add it if the combobox is editable or return false
  int id = combo_box_->findText(value);
  if(id >= 0) {
    combo_box_->setCurrentIndex(id);
    helper_->activateComboBox(combo_box_,value);
    return true;
  }
  else if( combo_box_->isEditable() ){
    combo_box_->addItem( elem.attribute( "value" ) );
    helper_->activateComboBox(combo_box_,value);
    return true;
  }
  else
    return false;

  /*
  int entry_id = 0;
  while( combo_box_->itemText(entry_id) != value ) {
    entry_id++;
    if( entry_id >= combo_box_->count() ) break;
  }

  if( entry_id >= combo_box_->count() ) {
    // the entry wasn't found
    if( combo_box_->isEditable() ) {
      combo_box_->addItem( elem.attribute( "value" ) );
      return true;
    }
    else
      return false;
  }

//  combo_box_->setCurrentItem( entry_id );
  helper_->activateComboBox( combo_box_, entry_id );
  */
  return true;
}

 

void QComboBox_accessor::clear() {
  if( combo_box_->isEditable() ) 
    combo_box_->setItemText( combo_box_->currentIndex(),"" );
}



Named_interface* QComboBox_accessor::create_new_interface(std::string&) {
  return new QComboBox_accessor;
}




//===================================

ComboBoxHelper::ComboBoxHelper( QObject* parent ) : QObject( parent ) {}

void ComboBoxHelper::activateComboBox( QComboBox* box, int  i ) {
  QObject::connect( this, SIGNAL( doInternalActivate( int ) ),
                    box, SLOT( internalActivate(int) ) );
  emit doInternalActivate(i);
  QObject::disconnect( this, SIGNAL( doInternalActivate( int ) ), 0, 0 );
}

void ComboBoxHelper::activateComboBox( QComboBox* box, QString  text ) {
  bool ok = QObject::connect( this, SIGNAL( activateText( QString ) ),
                    box, SIGNAL( activated(QString) ) );
  emit activateText(text);
  QObject::disconnect( this, SIGNAL( activateText( QString ) ), 0, 0 );
}