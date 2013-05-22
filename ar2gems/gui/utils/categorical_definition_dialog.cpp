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

#include <gui/utils/categorical_definition_dialog.h>
#include <qtplugins/selectors.h>
#include <qtplugins/categorical_selectors.h>
#include <utils/error_messages_handler.h>
#include <utils/gstl_messages.h>
#include <appli/manager_repository.h>
#include <grid/grid_model/grid_property.h>
#include <grid/grid_model/grid_categorical_property.h>
#include <actions/defines.h>

#include <qtplugins/color_delegate.h>

#include <set>

#include <qpushbutton.h>
#include <qstringlist.h>
#include <qlistwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QRadioButton>
#include <QTextBrowser>
#include <QTextEdit>
#include <QTableView>
#include <QSpinBox>
#include <QColorDialog>


View_category_definition_dialog::
View_category_definition_dialog( GsTL_project* proj, QWidget* parent, const char* name )
  : QDialog( parent ) {

  if (name)
    setObjectName(name);

  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);

  main_layout->addWidget(new QLabel("Select definition",this) );
  def_selector_ = new CategoricalDefinitionSelector(this,"Categorical_definition");
  def_selector_->initiliaze();
  

  CategoricalDefinitionTable* cdef_table = new CategoricalDefinitionTable(this,"Definition_viewer");
  cdef_table->initialize();
  cdef_table->set_read_only();
  //def_viewer_ = new CategoricalDefinitionViewer(this,"Definition_viewer");
  
  
  properties_viewer_ = new QTextBrowser(this);
  properties_viewer_->setReadOnly(true);
//  properties_viewer_->setAutoFormatting(QTextEdit::AutoBulletList);
  properties_viewer_->setUndoRedoEnabled(false);
//  properties_viewer_->setMaximumHeight(50);
  
  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* close = new QPushButton( "Close", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget( close );
  
  main_layout->addWidget(def_selector_);
  main_layout->addStretch();
  main_layout->addWidget( new QLabel("Code and name of values") );
  //main_layout->addWidget(def_viewer_);
  main_layout->addWidget(cdef_table);
  main_layout->addStretch();
  main_layout->addWidget( new QLabel("Associated properties") );
  main_layout->addWidget(properties_viewer_);
  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );


  QObject::connect( def_selector_, SIGNAL( activated( const QString& ) ),
                    cdef_table, SLOT( show_definition( const QString& ) ) );
    
  QObject::connect( def_selector_, SIGNAL( activated( const QString& ) ),
                    this, SLOT( display_properties( const QString& ) ) );

  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );
//  QObject::connect( close, SIGNAL( clicked() ),
//                    this, SLOT( reject() ) );

}


void View_category_definition_dialog::display_properties(const QString& def_name){
  if(current_definition_ == def_name) return;
  current_definition_ = def_name;

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+def_name.toStdString() );
  appli_assert( ni );

  properties_viewer_->document()->clear();
	CategoricalPropertyDefinition* cat_def =
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());

	CategoricalPropertyDefinition::property_set::const_iterator it = cat_def->begin_property();
	for( ; it != cat_def->end_property(); ++it) {
			properties_viewer_->append(QString((*it)->name().c_str()));
	}

}

/*
  ------------------------------------------------------
*/

New_category_definition_dialog::
New_category_definition_dialog(GsTL_project* project, QWidget* parent, const char* name )
  : project_(project), QDialog( parent ) {

  if (name)
    setObjectName(name);

  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);
  
  cat_def_name_ = new QLineEdit(this);

  QTableView* view = new QTableView(this);
  model_ = new Categorical_table_model(this);
  view->setModel(model_);
  Color_delegate* c_delagate =  new Color_delegate(this);
  view->setItemDelegateForColumn(2,c_delagate);
  view->setAlternatingRowColors ( true );
  //view->horizontalHeader()->resizeSections(QHeaderView::Stretch);

  ncat_ = new QSpinBox(this);
  QHBoxLayout* ncat_layout = new QHBoxLayout(this);
  ncat_layout->addWidget(new QLabel("Number of categories : ", this));
  ncat_layout->addWidget(ncat_);
  ncat_layout->addStretch();


  
  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* close = new QPushButton( "Create and Close", this);
  QPushButton* ok = new QPushButton( "Create", this);
  QPushButton* clear = new QPushButton( "Clear", this);
  QPushButton* cancel = new QPushButton( "Exit", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget( clear ); 
  bottom_layout->addStretch();
  bottom_layout->addWidget( ok ); 
  bottom_layout->addStretch();
  bottom_layout->addWidget( close ); 
  bottom_layout->addStretch();
  bottom_layout->addWidget( cancel ); 
  bottom_layout->addStretch();

  main_layout->addWidget(new QLabel("New definition name",this) );
  main_layout->addWidget(cat_def_name_);
//  main_layout->addStretch();
  main_layout->addLayout(ncat_layout);
//  main_layout->addStretch();
  main_layout->addWidget( new QLabel("Enter category names") );
  main_layout->addWidget(view);
//  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );


  QObject::connect( ncat_, SIGNAL( valueChanged ( int  ) ),
                    model_, SLOT( set_number_of_categories( int ) ) );
  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( create_definition() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( create_definition_close() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );
  QObject::connect( clear, SIGNAL( clicked() ),
                    this, SLOT( clear() ) );

  QObject::connect( view, SIGNAL(doubleClicked ( const QModelIndex&)), this, SLOT(show_color_editor(const QModelIndex&)) );

}


void New_category_definition_dialog::clear(){
  cat_def_name_->clear();
  ncat_->setValue(0);
//  cat_names_text_->clear();
}


void New_category_definition_dialog::create_definition(){
    
  int ncat = model_->rowCount();
  std::map<int, QString> code_names;
  for(int i=0;i<ncat;++i) {
    code_names[model_->data( model_->index(i,0) ).toInt()] = model_->data( model_->index(i,1) ).toString();
  }
  if(code_names.size() != ncat) {
    GsTLcerr << "Could not create new definition:  One code is repeated. \n";
    return;
  }

  QStringList list_code;
  QStringList list_name;
  std::map<int, QString>::iterator it = code_names.begin();
  for( ; it != code_names.end(); ++it) {
    list_code.append( QString("%1").arg( it->first));
    list_name.append( it->second );
  }


  QStringList parameter_list;
  parameter_list<<cat_def_name_->text()<<QString("%1").arg(ncat)<<list_name<<list_code;
//  QStringList list  = cat_names_text_->toPlainText().split("\n");
//  QStringList parameters  = names.split("\n");

  QString sep = Actions::separator.c_str();
  std::string parameters = parameter_list.join( sep ).toStdString();

  QApplication::setOverrideCursor( Qt::WaitCursor );
 
  // call the CopyProperty action
  Error_messages_handler error_messages;

  std::string command( "NewCategoricalCodeDefinition" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    QApplication::restoreOverrideCursor();
    return;
  }

  //Set the color
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_def_name_->text().toStdString() );
  appli_assert( ni );
	CategoricalPropertyDefinitionName* cat_def =
    dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());

  for(int i=0;i<ncat;++i) {
    cat_def->color(model_->data( model_->index(i,0)).value<int>() , model_->data( model_->index(i,2) ).value<QColor>() );
  }

  QApplication::restoreOverrideCursor();

}

void New_category_definition_dialog::create_definition_close(){
  create_definition();
  accept();
}


void New_category_definition_dialog::show_color_editor(const QModelIndex& index){
  if ( index.column() != 2 ) return;

  //get the current color
  QColor color = index.data().value<QColor>();
  QString category_name = model_->index(index.row(),1).data().toString();
  color = QColorDialog::getColor(color, this, QString("Choose color for %1").arg(category_name),QColorDialog::ShowAlphaChannel);
  model_->setData(index, color);
}


/*
-------------------------------
*/


Assign_category_definition_dialog::
Assign_category_definition_dialog( GsTL_project* proj, QWidget* parent, const char* name )
  : QDialog( parent ), project_(proj) {

  if (name)
    setObjectName(name);

  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);
  
 
  def_selector_ = new CategoricalDefinitionSelector(this,"Categorical_definition");
  def_selector_->initiliaze();
  grid_selector_ = new GridSelectorBasic(this,"grid selector",proj);
  props_selector_ = new MultiCategoricalPropertySelector(this,"Prop");

  
  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* assign_close = new QPushButton( "Assign and Close", this);
  QPushButton* assign = new QPushButton( "Assign", this);
  QPushButton* close = new QPushButton( "Close", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget( assign ); 
  bottom_layout->addStretch();
  bottom_layout->addWidget( assign_close ); 
  bottom_layout->addStretch();
  bottom_layout->addWidget( close ); 
  bottom_layout->addStretch();

  main_layout->addWidget(new QLabel("Select definition to assign",this) );
  main_layout->addWidget(def_selector_);
  main_layout->addStretch();
  main_layout->addWidget( new QLabel("Select grid and properties") );
  main_layout->addWidget(grid_selector_);
  main_layout->addWidget( props_selector_ );
  main_layout->addStretch();
  main_layout->addLayout(bottom_layout);

  QObject::connect( grid_selector_, SIGNAL(  activated( const QString& ) ),
                    props_selector_, SLOT( show_properties(const QString&) ) );


  QObject::connect( assign, SIGNAL( clicked() ),
                    this, SLOT( assign_definition() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );
  QObject::connect( assign_close, SIGNAL( clicked() ),
                    this, SLOT( assign_definition_close() ) );


}



void Assign_category_definition_dialog::assign_definition(){

  Error_messages_handler error_messages;
  QString def_name = def_selector_->currentText();
  if(def_name.isEmpty()) {
    GsTLcerr << "A definition must be selected\n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
    return;
  }

  QString grid_name = grid_selector_->currentText();
  if(grid_name.isEmpty())  {
    GsTLcerr << "A grid must be selected\n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
    return;
  }

  QList<QListWidgetItem*> listWidget = props_selector_->selectedItems();
  if(listWidget.isEmpty())  {
    GsTLcerr << "At least one property must be selected\n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
    return;
  }
  QStringList list;
  list.append(def_name);
  list.append(grid_name);
  for (int i = 0; i < listWidget.size(); ++i)
    list.append(listWidget.at(i)->text());


  QString sep = Actions::separator.c_str();
  std::string parameters = list.join( sep ).toStdString();

  QApplication::setOverrideCursor( Qt::WaitCursor );
 
  std::string command( "AssignCategoricalDefinition" );
  bool ok = project_->execute( command, parameters, &error_messages );

  if( !ok ) {
    GsTLcerr << "Command " << command << " could not be performed. \n";
    if( !error_messages.empty() ) {
      GsTLcerr << error_messages.errors();
    }
    GsTLcerr << gstlIO::end;
  }
//update the property field
  props_selector_->show_properties(grid_name);
  QApplication::restoreOverrideCursor();

}

void Assign_category_definition_dialog::assign_definition_close(){
	assign_definition();
	accept();

}
