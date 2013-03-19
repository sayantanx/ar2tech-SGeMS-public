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



#include <GsTLAppli/extra/qtplugins/categorical_selectors.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <QKeySequence>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/manager.h>

#include <GsTLAppli/extra/qtplugins/categorical_definition_proxy_model.h>
#include <GsTLAppli/extra/qtplugins/color_delegate.h>

#include <qstring.h>
#include <QGroupBox>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qobject.h>
#include <qlabel.h>
#include <qevent.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <QtUiTools/QUiLoader>
#include <algorithm>
#include <QTextBrowser>
#include <QColorDialog>
#include <QHeaderView>

SingleCategoricalPropertySelector::SingleCategoricalPropertySelector( QWidget* parent, const char* name )
  : SinglePropertySelector( parent, name ) {
}

void SingleCategoricalPropertySelector::show_properties( const QString& grid_name ) {
  QString current_text;
  if( grid_name == current_grid_ )
    current_text = currentText();  
  
  current_grid_ = grid_name;
  QComboBox::clear();

  if( grid_name.isEmpty() || grid_name == GridSelectorBasic::no_selection ) return;
  

  QByteArray tmp = grid_name.toLatin1();
  std::string name( tmp.constData() );

  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + name );
  Geostat_grid* grid = (Geostat_grid*) ni.raw_ptr() ;
  // no dynamic_cast is used on the previous line because it makes 
  // designer and uic segfault when they try to load the plugin...
  // It used to work before, don't know what changed.  

  if( !grid ) return;

  int selected_item = 0;
  int i=0;
  std::list<std::string> properties = grid->categorical_property_list();
  std::list<std::string>::iterator begin = properties.begin();
  for( ; begin != properties.end() ; ++begin, i++ ) {
    QString prop_name( begin->c_str() );
    addItem( prop_name );

    if( current_text == prop_name ) selected_item = i;
  }

  // a dirty hack to have the combobox resize itself to the size of 
  // its longest element
  setFont( font() );
  updateGeometry();
  
  setCurrentIndex( selected_item );


  emit activated( currentText() );
  // Don't know why "0". Should be "currentItem". remove if everything works fine
    emit activated( 0 );
//  emit activated( currentItem() );
}


//==========================================================================

MultiCategoricalPropertySelector::MultiCategoricalPropertySelector(
  QWidget* parent, const char* name) : MultiPropertySelector(parent,name) {}

void MultiCategoricalPropertySelector::show_properties( const QString& grid_name ) {

  QListWidget::clear();

  if( grid_name.isEmpty() || grid_name == GridSelectorBasic::no_selection ) return;

  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + grid_name.toStdString() );
  if(ni.raw_ptr() == 0) return;

  Geostat_grid* grid = (Geostat_grid*) ni.raw_ptr() ;
  // no dynamic_cast is used on the previous line because it makes 
  // designer and uic segfault when they try to load the plugin...
  // It used to work before, don't know what changed.  

  if( !grid ) return;

  std::list<std::string> properties = grid->categorical_property_list();
  std::list<std::string>::iterator begin = properties.begin();
  for( ; begin != properties.end() ; ++begin ) {
    QString prop_name( begin->c_str() );
     std::string name = *begin;
    //const CategoricalPropertyDefinition* cdef;// = grid->categorical_property(name)->get_category_definition();
//    const GsTLGridCategoricalProperty* prop = grid->categorical_property(name);
    const CategoricalPropertyDefinition* cdef = grid->categorical_property(name)->get_category_definition();
    QString def_name( cdef->name().c_str() );
    //addItem( prop_name + "  ("+def_name+")" );
    addItem( prop_name );
  }
  
}
//==========================================================================

Ordered_categorical_property_selector_dialog::
Ordered_categorical_property_selector_dialog( QWidget* parent, const char* name)
:Ordered_property_selector_dialog(parent, name){
  
  selector_box_->layout()->removeWidget(property_selector_);
  delete property_selector_;

  property_selector_ = 
    new MultiCategoricalPropertySelector( selector_box_, "prop_select" );
  selector_box_->layout()->addWidget(property_selector_);

}


void Ordered_categorical_property_selector_dialog::
show_properties( const QString& grid_name ) {
  property_selector_->show_properties( grid_name );
}


//==========================================================================
OrderedCategoricalPropertySelector::
OrderedCategoricalPropertySelector( QWidget* parent, const char* name)
: GsTLGroupBox( parent, name ) {
  
  //setColumnLayout( 2, Qt::Vertical );
  QVBoxLayout * vbox = new QVBoxLayout;
  setLayout(vbox);
  
  selected_properties_ = new MultiCategoricalPropertySelector();
  QPushButton* choose_button = 
    new QPushButton( "Choose Properties...");
  vbox->addWidget(selected_properties_);
  vbox->addWidget(choose_button);

  selected_properties_->setSelectionMode( QAbstractItemView::NoSelection );

  QObject::connect( choose_button, SIGNAL( clicked() ),
                    this, SLOT( show_selection_dialog() ) );

  QObject::connect( this, SIGNAL( selected_count( int ) ),
                    this, SLOT( forwardSelection(int) ) );
}


void OrderedCategoricalPropertySelector::show_properties( const QString& grid_name ) {
  if( grid_name_ == grid_name ) return;

  selected_properties_->clear();
  grid_name_ = grid_name;
}

void OrderedCategoricalPropertySelector::show_selection_dialog() {
  Ordered_categorical_property_selector_dialog* dialog =
    new Ordered_categorical_property_selector_dialog( this, "Choose" );
  dialog->setWindowTitle( "Select Properties" );
  dialog->show_properties( grid_name_ );

  if( selected_properties_->count() != 0 ) {
    QStringList current_selection;
    for( int i = 0; i < selected_properties_->count() ; i++ )
      current_selection.push_back( selected_properties_->item(i)->text() );
    dialog->set_current_selection( current_selection );
  }

  if( dialog->exec() == QDialog::Rejected ) return;
  
  QStringList selection = dialog->selection();
  selected_properties_->clear();
  selected_properties_->addItems( selection );

  emit selected_count( selected_properties_->count() );
}

QStringList OrderedCategoricalPropertySelector::selected_properties() const {
  QStringList list;
  for( int i = 0 ; i < selected_properties_->count() ; i++ ) 
    list << selected_properties_->item(i)->text( );

  return list;
}

void OrderedCategoricalPropertySelector::set_selected_properties( QStringList name_list ) {

  if( grid_name_.isEmpty() || grid_name_ == GridSelectorBasic::no_selection ) return;
  

  QByteArray tmp = grid_name_.toLatin1();
  std::string name( tmp.constData() );

  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + name );
  Geostat_grid* grid = (Geostat_grid*) ni.raw_ptr() ;
  // no dynamic_cast is used on the previous line because it makes 
  // designer and uic segfault when they try to load the plugin...
  // It used to work before, don't know what changed.  

  if( !grid ) return;
    
  selected_properties_->clear();

  std::list<std::string> properties = grid->categorical_property_list();
  properties.sort();

  for( unsigned int i=0; i < name_list.size() ; i++ ) {
    if( name_list[i] == QString::null ) continue;

    QByteArray tt = name_list[i].toLatin1();
    bool found = std::binary_search( properties.begin(), properties.end(), 
                                     std::string( tt.constData() ) );
    if( found )
      selected_properties_->addItem( name_list[i] );
  }

}

void OrderedCategoricalPropertySelector::forwardSelection(int i){
	QStringList selection = this->selected_properties();
	emit this->forwardSelectedProperties(selection);
}

//======================================================================
CategoricalDefinitionSelector::CategoricalDefinitionSelector( QWidget* parent, const char* name )
  : QComboBox( parent ) {
  if (name)
    setObjectName(name);
  
}

void CategoricalDefinitionSelector::initiliaze(){
  this->show_definitons();
}

void CategoricalDefinitionSelector::show_definitons( ) {

  Categorical_definition_proxy_model* model = new Categorical_definition_proxy_model(this);

  this->setModel(model);
/*
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager );
  if(!ni) return;
  appli_assert( ni );
  Manager* cat_manager = dynamic_cast<Manager*>(ni.raw_ptr());
  
  int selected_item = 0;
  int i=0;
  Manager::names_iterator begin = cat_manager->begin_names();
  for( ; begin != cat_manager->end_names() ; ++begin, i++ ) {
    QString catdef_name( begin->c_str() );
    addItem( catdef_name );
  }

  // a dirty hack to have the combobox resize itself to the size of 
  // its longest element
  setFont( font() );
  updateGeometry();
  
  setCurrentIndex( selected_item );
  */

//  emit activated( currentText() );
  // Don't know why "0". Should be "currentItem". remove if everything works fine
//    emit activated( 0 );
//  emit activated( currentItem() );
}


//===============================================

MultiCategoricalDefinitionSelector::MultiCategoricalDefinitionSelector( QWidget* parent, const char* name )
  : QListWidget( parent ) {
  if (name)
    setObjectName(name);

  setSelectionMode( QAbstractItemView::ExtendedSelection );

  QObject::connect( this, SIGNAL( itemSelectionChanged() ),
		    this, SLOT( selection_size() ) );
}

void MultiCategoricalDefinitionSelector::show_definitions( ) {
  QListWidget::clear();
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager );
  appli_assert( ni );
  Manager* cat_manager = dynamic_cast<Manager*>(ni.raw_ptr());
  
  int selected_item = 0;
  int i=0;
  Manager::names_iterator begin = cat_manager->begin_names();
  for( ; begin != cat_manager->end_names() ; ++begin, i++ ) {
    QString catdef_name( begin->c_str() );
    addItem( catdef_name );
  }

}


void MultiCategoricalDefinitionSelector::selection_size() {
  int size=0;
  for( unsigned int i = 0; i < count() ; i++ ) {
    QListWidgetItem * it = item(i);
    if( it->isSelected( ) && !it->text().isEmpty() ) {
      size++;
    }
  }
  emit( selected_count(size) );
}

//===============================================

CategoricalDefinitionViewer::CategoricalDefinitionViewer(QWidget *parent, const char *name)
: QGroupBox(parent), current_definition_(""), baseLayout_(0) {
}

void CategoricalDefinitionViewer::show_definition( QString cat_def_name) {
  if(current_definition_ == cat_def_name ) return;
  current_definition_ = cat_def_name ;

  delete baseLayout_;
  baseLayout_ = new QVBoxLayout(this);

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_def_name.toStdString() );
  appli_assert( ni );

	CategoricalPropertyDefinitionName* cat_def = 
    dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
  QTextBrowser* def_text = new QTextBrowser(this);
  def_text->setReadOnly(true);
//  def_text->setAutoFormatting(QTextEdit::AutoBulletList);
  def_text->setUndoRedoEnabled(false);
//  def_text->setMaximumHeight(50);
  baseLayout_->addWidget(def_text);

  if(cat_def != 0) {
    for(unsigned int i=0; i<cat_def->number_of_category(); i++) {
      std::ostringstream def_name;
      def_name<<i<<": "<<cat_def->get_category_name(i);
      def_text->append( def_name.str().c_str() );
//      QLabel* nameLabel = new QLabel(def_text.str().c_str(),this);
//      baseLayout_->addWidget(nameLabel);
    }
  } 
  else {
    def_text->append( "Default naming convention" );
//    QLabel* nameLabel = new QLabel("Default naming convention",this);
//    baseLayout_->addWidget(nameLabel);    
  }
}



//===============================================

CategoricalDefinitionTable::CategoricalDefinitionTable(QWidget *parent, const char *name)
: QTableView(parent), current_definition_("") {

  this->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(show_color_editor(const QModelIndex&)));


}

void CategoricalDefinitionTable::initialize( ) {
  model_ = new Category_proxy_model(this);
  this->setModel(model_);
}

void CategoricalDefinitionTable::set_read_only() {
  if(model_) model_->set_read_only();
}

void CategoricalDefinitionTable::show_definition( QString cat_def_name) {
  if(current_definition_ == cat_def_name ) return;
  current_definition_ = cat_def_name ;

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_def_name.toStdString() );
  appli_assert( ni );

	CategoricalPropertyDefinitionName* cat_def = 
    dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());
  if(cat_def == 0) return;

  this->show_definition(cat_def);

}

//Definition for the default
void CategoricalDefinitionTable::show_definition( int ncat) {
  if(current_definition_ == "Default" ) return;
  current_definition_ = "Default";

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( categoricalDefinition_manager+"/Default" );
  appli_assert( ni );

	CategoricalPropertyDefinition* cat_def = 
    dynamic_cast<CategoricalPropertyDefinition*>(ni.raw_ptr());
  if(cat_def == 0) return;
  
  this->setModel(0);
  model_->initialize(cat_def, ncat);
  this->setModel(model_);
  this->reset();

//  color_delegate

  this->setItemDelegateForColumn(1,new Color_delegate(this));

}

void CategoricalDefinitionTable::show_definition( CategoricalPropertyDefinition* cdef) {

  CategoricalPropertyDefinitionName* cdef_named = dynamic_cast<CategoricalPropertyDefinitionName*>(cdef);
  if(cdef_named==0) return;
  this->setModel(0);
  model_->initialize(cdef_named);
  this->setModel(model_);
  this->reset();

//  color_delegate

  this->setItemDelegateForColumn(1,new Color_delegate(this));

}

void CategoricalDefinitionTable::show_color_editor(const QModelIndex& index){
  if ( index.column() != 1 ) return;

  //Check if the the index is editable
  if(  this->model()->flags(index) & Qt::ItemIsEditable ) {
    //get the current color
    QColor color = index.data().value<QColor>();
    QString category_name = model_->index(index.row(),0).data().toString();
    color = QColorDialog::getColor(color, this, QString("Choose color for %1").arg(category_name),QColorDialog::ShowAlphaChannel);
    model_->setData(index, color);
  }
}


//===============================================


//===============================================

MultipleCategorySelector::MultipleCategorySelector( QWidget* parent, const char* name )
  : QListWidget( parent ) {
  if (name)
    setObjectName(name);

  setSelectionMode( QAbstractItemView::ExtendedSelection );

  QObject::connect( this, SIGNAL( itemSelectionChanged() ),
		    this, SLOT( selection_size() ) );
}

void MultipleCategorySelector::show_categories( const GsTLGridProperty* prop){
	const GsTLGridCategoricalProperty* cprop = dynamic_cast<const GsTLGridCategoricalProperty*>(prop);
	if(cprop == 0) return;

//	const CategoricalPropertyDefinition* cdef = cprop->get_category_definition();

	const CategoricalPropertyDefinitionName* cdef =
	  dynamic_cast<const CategoricalPropertyDefinitionName*>( cprop->get_category_definition());

	if(cdef) {
		this->show_categories(cdef);
	}
	else
		this->show_default_categories(cprop->get_number_of_category());


}

void MultipleCategorySelector::show_categories( const QString& cat_def_name ) {
  QListWidget::clear();

  if( cat_def_name.isEmpty() || cat_def_name == GridSelectorBasic::no_selection ) return;

  SmartPtr<Named_interface> ni =
    Root::instance()->interface( categoricalDefinition_manager+"/"+cat_def_name.toStdString() );

	CategoricalPropertyDefinitionName* cat_def =
	  dynamic_cast<CategoricalPropertyDefinitionName*>(ni.raw_ptr());


	if(cat_def != 0) {
	  for(unsigned int i=0; i<cat_def->number_of_category(); i++) {
		  addItem( cat_def->get_category_name(i).c_str() );
	  }
	}


}

void MultipleCategorySelector::show_default_categories( int number_of_category ) {
  QListWidget::clear();

  if(number_of_category <= 0) return;

  SmartPtr<Named_interface> ni =
    Root::instance()->interface( categoricalDefinition_manager+"/Default" );
  CategoricalPropertyDefinitionDefault* cat_definitions_ =
    dynamic_cast<CategoricalPropertyDefinitionDefault*>(ni.raw_ptr());


	if(cat_definitions_ != 0) {
	  for(unsigned int i=0; i<number_of_category; i++) {
		  addItem( cat_definitions_->get_category_name(i).c_str() );
	  }
	}


}

void MultipleCategorySelector::show_categories( const CategoricalPropertyDefinition* cdef){

}


void MultipleCategorySelector::selection_size() {
  int size=0;
  for( unsigned int i = 0; i < count() ; i++ ) {
    QListWidgetItem * it = item(i);
    if( it->isSelected( ) && !it->text().isEmpty() ) {
      size++;
    }
  }
  emit( selected_count(size) );
}

