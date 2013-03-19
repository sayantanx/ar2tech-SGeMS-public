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


#include <GsTLAppli/gui/utils/categorical_property_dialog.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/utils/error_messages_handler.h>

#include <QStringList>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QRadioButton>


Convert_to_categorical_dialog::Convert_to_categorical_dialog( GsTL_project* proj,QWidget* parent, const char* name )
: QDialog( parent ), project_(proj){

  QVBoxLayout* layout = new QVBoxLayout(this);

  grid_selector_ = new GridSelectorBasic(this);
  props_selector_ = new OrderedPropertySelector(this);

  layout->addWidget(grid_selector_);
  layout->addWidget(props_selector_);

  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  QPushButton* create = new QPushButton( "Create", this);
  QPushButton* create_close = new QPushButton( "Create and Close", this);
  QPushButton* cancel = new QPushButton( "Cancel", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget(create);
  bottom_layout->addWidget(create_close);
  bottom_layout->addWidget(cancel);


  layout->addStretch();
  layout->addLayout( bottom_layout );

  QObject::connect( cancel, SIGNAL( clicked() ), this, SLOT( accept() ) );
  QObject::connect( create, SIGNAL( clicked() ), this, SLOT( execute() ) );
  QObject::connect( create_close, SIGNAL( clicked() ), this, SLOT( execute_close() ) );
  QObject::connect( grid_selector_, SIGNAL( activated( const QString& ) ), props_selector_, SLOT( show_properties( const QString&) ) );
}

void Convert_to_categorical_dialog::execute(){

  QString grid_name = grid_selector_->currentText();
  if(grid_name.isEmpty()) return;

  QStringList prop_names = props_selector_->selected_properties();
  if(prop_names.isEmpty()) return;

  QString sep = Actions::separator.c_str();
  QStringList param_list;
  param_list<<grid_name<<prop_names;
  std::string parameters =  param_list.join(sep).toStdString();

  QApplication::setOverrideCursor( Qt::WaitCursor );
 
  // call the CopyProperty action
  Error_messages_handler error_messages;

  std::string command( "ConvertToCategoricalProperty" );
  bool ok = project_->execute( command, parameters, &error_messages );   

  QApplication::restoreOverrideCursor();
  return;
  

}
void Convert_to_categorical_dialog::execute_close(){

  this->execute();

  this->accept();
}

//
// ---------------------------------------------
//

Sequentialize_categorical_property_dialog::
  Sequentialize_categorical_property_dialog( GsTL_project* proj,QWidget* parent, const char* name  )
: QDialog( parent ),project_(proj){

  QVBoxLayout* layout = new QVBoxLayout(this);

  grid_selector_ = new GridSelectorBasic(this);
  cprop_selector_ = new SingleCategoricalPropertySelector(this);

  layout->addWidget(grid_selector_);
  layout->addWidget(cprop_selector_);

  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  QPushButton* create = new QPushButton( "Create", this);
  QPushButton* create_close = new QPushButton( "Create and Close", this);
  QPushButton* cancel = new QPushButton( "Cancel", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget(create);
  bottom_layout->addWidget(create_close);
  bottom_layout->addWidget(cancel);


  layout->addStretch();
  layout->addLayout( bottom_layout );

  QObject::connect( cancel, SIGNAL( clicked() ), this, SLOT( accept() ) );
  QObject::connect( create, SIGNAL( clicked() ), this, SLOT( execute() ) );
  QObject::connect( create_close, SIGNAL( clicked() ), this, SLOT( execute_close() ) );
  QObject::connect( grid_selector_, SIGNAL( activated( const QString& ) ), cprop_selector_, SLOT( show_properties( const QString&) ) );

}


void Sequentialize_categorical_property_dialog::execute(){

  QString grid_name = grid_selector_->currentText();
  if(grid_name.isEmpty()) return;

  QString prop_name = cprop_selector_->currentText();
  if(prop_name.isEmpty()) return;

  QString sep = Actions::separator.c_str();
  QStringList param_list;
  param_list<<grid_name<<prop_name;
  std::string parameters =  param_list.join(sep).toStdString();

  QApplication::setOverrideCursor( Qt::WaitCursor );
 
  // call the CopyProperty action
  Error_messages_handler error_messages;

  std::string command( "ConvertCategoricalPropertyToSequential" );
  bool ok = project_->execute( command, parameters, &error_messages );   

  QApplication::restoreOverrideCursor();
  return;
  

}
void Sequentialize_categorical_property_dialog::execute_close(){

  this->execute();

  this->accept();
}

//
// ------------------------------------------
//

Sequentialize_categorical_definition_dialog::
  Sequentialize_categorical_definition_dialog( GsTL_project* proj,QWidget* parent , const char* name )
: QDialog( parent ), project_(proj){
  

  QVBoxLayout* layout = new QVBoxLayout(this);

  cdef_selector_ = new CategoricalDefinitionSelector(this);
  cdef_selector_->initiliaze();

  layout->addWidget(cdef_selector_);

  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  QPushButton* create = new QPushButton( "Create", this);
  QPushButton* create_close = new QPushButton( "Create and Close", this);
  QPushButton* cancel = new QPushButton( "Cancel", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget(create);
  bottom_layout->addWidget(create_close);
  bottom_layout->addWidget(cancel);


  layout->addStretch();
  layout->addLayout( bottom_layout );

  QObject::connect( cancel, SIGNAL( clicked() ), this, SLOT( accept() ) );
  QObject::connect( create, SIGNAL( clicked() ), this, SLOT( execute() ) );
  QObject::connect( create_close, SIGNAL( clicked() ), this, SLOT( execute_close() ) );

  
}


void Sequentialize_categorical_definition_dialog::execute(){

  QString cdef_name = cdef_selector_->currentText();

  QApplication::setOverrideCursor( Qt::WaitCursor );
 
  // call the CopyProperty action
  Error_messages_handler error_messages;

  std::string command( "ConvertPropertyInDefinitionToSequential" );
  bool ok = project_->execute( command, cdef_name.toStdString(), &error_messages );   

  QApplication::restoreOverrideCursor();
  return;
  

}
void Sequentialize_categorical_definition_dialog::execute_close(){

  this->execute();

  this->accept();
}
