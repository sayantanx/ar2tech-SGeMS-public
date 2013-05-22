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



#include <gui/utils/copy_property_dialog.h>

#include <actions/action.h>
#include <actions/defines.h>
#include <utils/error_messages_handler.h>

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

//=========================================


Copy_property_dialog::
Copy_property_dialog( GsTL_project* proj,
                     QWidget* parent, const char* name ) 
                     : QDialog( parent ) {
  project_ = proj;

  if (name)
    setObjectName(name);
  
  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);
  
  QGroupBox* source_box = new QGroupBox( this);
  QGroupBox* dest_box = new QGroupBox( this);
  QGroupBox* options_box = new QGroupBox( this );

  QVBoxLayout * vs, * vd, * vo;
  vs = new QVBoxLayout(source_box);
  vd = new QVBoxLayout(dest_box);
  vo = new QVBoxLayout(options_box);
  
  source_box->setLayout(vs);
  dest_box->setLayout(vd);
  options_box->setLayout(vo);

  vs->addWidget(new QLabel( "Copy property from object:", source_box ));
  source_ = new GridSelectorBasic( source_box, "source", proj );
  vs->addWidget(source_);
  vs->addWidget(new QLabel( "Select property to copy:", source_box ));
  source_property_ = new SinglePropertySelector( source_box, "source_prop" );
  vs->addWidget(source_property_);

  vd->addWidget(new QLabel( "Copy to object:", dest_box ));
  destination_ = new GridSelectorBasic( dest_box, "dest", proj );
  vd->addWidget(destination_);
  vd->addWidget(new QLabel( "Select or type-in a destination\nproperty name", dest_box ));
  destination_property_ = new SinglePropertySelector( dest_box, "dest_prop" );
  vd->addWidget(destination_property_);
//  destination_property_ = new QComboBox( dest_box, "dest_prop" );
  destination_property_->setEditable( true );

  overwrite_ = new QCheckBox( "Overwrite", options_box );
  vo->addWidget(overwrite_);
  mark_as_hard_ = new QCheckBox( "Mark as Hard Data", options_box );
  vo->addWidget(mark_as_hard_);
  
  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* ok = new QPushButton( "Copy", this);  
  QPushButton* cancel = new QPushButton( "Close", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget( ok );
  bottom_layout->addWidget( cancel );

  main_layout->addWidget( source_box );
  main_layout->addStretch();
  main_layout->addWidget( dest_box );
  main_layout->addStretch();
  main_layout->addWidget( options_box );
  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );

  
  QObject::connect( source_, SIGNAL( activated( const QString& ) ),
                    source_property_, SLOT( show_properties( const QString& ) ) );
  QObject::connect( destination_, SIGNAL( activated( const QString& ) ),
                    destination_property_, SLOT( show_properties( const QString& ) ) );
/*
  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );
*/
  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( copy_property() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );

  setSizeGripEnabled( true );
}


std::string Copy_property_dialog::parameters() const {
  QString overwrite_str;
  overwrite_str.setNum( overwrite_->isChecked() );
  QString mark_hard_str;
  mark_hard_str.setNum( mark_as_hard_->isChecked() );

  QString sep = Actions::separator.c_str();
  QStringList list;
  list << source_->currentText()
       << source_property_->currentText() 
       << destination_->currentText() 
       << destination_property_->currentText() 
       << overwrite_str << mark_hard_str;
  
  QString res = list.join( sep );
  if( res.isEmpty() ) return std::string( "" );

  return std::string( res.toStdString() );
}

void Copy_property_dialog::copy_property() {
  if( !project_ ) return;

  QString overwrite_str;
  overwrite_str.setNum( overwrite_->isChecked() );
  QString mark_hard_str;
  mark_hard_str.setNum( mark_as_hard_->isChecked() );

  QString sep = Actions::separator.c_str();
  QStringList list;
  list << source_->currentText()
       << source_property_->currentText() 
       << destination_->currentText() 
       << destination_property_->currentText() 
       << overwrite_str << mark_hard_str;
  
  std::string parameters = std::string( list.join( sep ).toStdString() );
  if( parameters.empty() ) return;


  QApplication::setOverrideCursor( Qt::WaitCursor );
 
  // call the CopyProperty action
  Error_messages_handler error_messages;

  std::string command( "CopyProperty" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }

  QApplication::restoreOverrideCursor();
}


QString Copy_property_dialog::source() const { 
  return source_->currentText(); 
}
QString Copy_property_dialog::source_property() const { 
  return source_property_->currentText();
}
QString Copy_property_dialog::destination() const { 
  return destination_->currentText(); 
}
QString Copy_property_dialog::destination_property() const { 
  return destination_property_->currentText(); 
}
bool Copy_property_dialog::overwrite() const { 
  return overwrite_->isChecked(); 
}
bool Copy_property_dialog::mark_as_hard() const { 
  return mark_as_hard_->isChecked(); 
}