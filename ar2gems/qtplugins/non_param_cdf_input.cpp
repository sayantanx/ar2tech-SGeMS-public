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



#include <GsTLAppli/qtplugins/non_param_cdf_input.h>
#include <GsTLAppli/qtplugins/selectors.h>
#include <GsTLAppli/qtplugins/filechooser.h>
#include <GsTLAppli/qtplugins/variogram_input.h>
#include <GsTLAppli/qtplugins/distribution_selectors.h>

#include <qcombobox.h>
#include <qlineedit.h>
#include <QGroupBox>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>


TailCdfInput::TailCdfInput(QWidget* parent, const char* name)
: QWidget(parent ) {
  if (name)
    setObjectName(name);

//  QVBoxLayout * layout = new QVBoxLayout;
//  this->setLayout(layout);

 
  QGridLayout* main_layout = new QGridLayout( this);
  main_layout->setRowStretch( 0, 0 );
  main_layout->setRowStretch( 1, 0 );
  main_layout->setRowStretch( 2, 1 );

  QLabel* function_label = new QLabel( "Function", this);

  function_type = new QComboBox( this);
  function_type->setEditable(false);

//  layout->addWidget( function_label );
//  layout->addWidget( function_type );

  main_layout->addWidget( function_label, 0, 0 );
  main_layout->addWidget( function_type, 1, 0 );
  main_layout->setColumnStretch( 1, 6 );

  min_max_label_ = new QLabel( this);
  extremeValue = new QLineEdit( this);

//  layout->addWidget( min_max_label_);
// layout->addWidget( extremeValue);

  main_layout->addWidget( min_max_label_, 0, 2 );
  main_layout->addWidget( extremeValue, 1, 2 );
  main_layout->setColumnStretch( 3, 6 );

  omega_label = new QLabel( "omega", this);
  omega = new QLineEdit( this);
//  layout->addWidget( omega_label);
//  layout->addWidget( omega );

  
  main_layout->addWidget( omega_label, 0, 4 );
  main_layout->addWidget( omega, 1, 4 );


	QObject::connect( function_type, SIGNAL( activated( int ) ),
		    this, SLOT( setExtremeBoxEnabbled( int ) ) );
	QObject::connect( function_type, SIGNAL( activated( int ) ),
		    this, SLOT( setOmegaBoxEnabbled( int ) ) );
}

QString TailCdfInput::getFunctionType() const {
	return function_type->currentText();
}

float TailCdfInput::getExtremeValue() const {
	return extremeValue->text().toFloat();
}

float TailCdfInput::getOmega() const {
	return omega->text().toFloat();
}

QString TailCdfInput::getOmegaText() const{
  return omega->text();
}
QString TailCdfInput::getExtremeValueText() const{
  return extremeValue->text();
}

void TailCdfInput::setFunctionType(QString text ) {
  int i = function_type->findText(text);
  function_type->setCurrentIndex(i);
}

void TailCdfInput::setExtremeValue( QString extremeStr ) {
	extremeValue->setText(extremeStr);
}

void TailCdfInput::setOmega( QString omegaStr ) {
	omega->setText(omegaStr);
}


void TailCdfInput::setExtremeBoxEnabbled(int index){
//	bool on = function_type->currentText() == "Power";
  bool on = function_type_index[index] == "Power";
	min_max_label_->setEnabled( on );
  extremeValue->setEnabled( on );
}

void TailCdfInput::setOmegaBoxEnabbled(int index){
	//bool on = function_type->currentText() != "Exponential";
  QByteArray tmp = function_type_index[index].toLatin1();
  std::string function_type = tmp.constData();
  bool on = (function_type != "Exponential") && (function_type != "No extrapolation");
	omega_label->setEnabled( on );
	omega->setEnabled( on );
}

LowerTailCdfInput::LowerTailCdfInput(QWidget* parent, const char* name)
: TailCdfInput(parent, name) {
	function_type->clear();
  function_type->addItem( tr( "Power" ) );
  function_type->addItem( tr( "Exponential" ) );
  function_type->addItem( tr( "No extrapolation" ) );
  omega->setText( "3");

  function_type_index.push_back("Power");
  function_type_index.push_back("Exponential");
  function_type_index.push_back("No extrapolation");

	min_max_label_->setText( "Min" );
}

UpperTailCdfInput::UpperTailCdfInput(QWidget* parent, const char* name) 
: TailCdfInput(parent, name) {
	function_type->clear();
  function_type->addItem( tr( "Power" ) );
  function_type->addItem( tr( "Hyperbolic" ) );
  function_type->addItem( tr( "No extrapolation" ) );
  omega->setText("0.333");

  function_type_index.push_back("Power");
  function_type_index.push_back("Hyperbolic");
  function_type_index.push_back("No extrapolation");

	min_max_label_->setText( "Max" );

}


//=====================================

NonParamCdfInput::NonParamCdfInput( QWidget* parent, const char* name) 
: QWidget(parent) {

  QVBoxLayout* main_layout = new QVBoxLayout( this );
  main_layout->setSpacing(1);

  if (name) {
	  this->setObjectName(name);
  }


  QGroupBox * options_group = new QGroupBox( "Reference distribution from:",this);

  QHBoxLayout * hbox = new QHBoxLayout;
  options_group->setLayout(hbox);
  
  from_grid = 
    new QRadioButton( "Object", options_group);
  from_file = 
    new QRadioButton( "File", options_group);
  from_distribution  =
      new QRadioButton( "Distribution", options_group);

  from_grid->setChecked(false);
  from_file->setChecked(false);
  from_distribution->setChecked(true);

  hbox->addWidget(from_distribution);
  hbox->addWidget(from_grid);
  hbox->addWidget(from_file);

  distributionSelector = new Continuous_distribution_selector(this);


  QFrame* non_param_frame = new QFrame(this);
  non_param_frame->setLayout(new QVBoxLayout());

  break_ties = 
    new QRadioButton( "Break ties", this);



  Line_separator* file_sep = 
    new Line_separator( "File with ref. distribution", non_param_frame, "filesep" );
	refFileChooser = new FileChooser( non_param_frame, "refFilename" );

  Line_separator* prop_sep = 
    new Line_separator( "Property with ref. distribution", non_param_frame, "propsep" );  
  refPropSelector = new PropertySelector( non_param_frame, "refGrid" );
 // refPropSelector->setMinimumHeight( 180 );



//  Line_separator* lti_sep = 
//    new Line_separator( "Lower Tail Extrapolation", this, "ltisep" );
  QLabel* lti_sep = new QLabel("Lower Tail Extrapolation",non_param_frame);

  LTI = new LowerTailCdfInput( non_param_frame, "LTI_type" );

//  Line_separator* uti_sep = 
//    new Line_separator( "Upper Tail Extrapolation", this, "utisep" );  
  QLabel* uti_sep = new QLabel("Upper Tail Extrapolation",non_param_frame);
	UTI = new UpperTailCdfInput( non_param_frame, "UTI_type" );

  non_param_frame->layout()->addWidget( break_ties );
  non_param_frame->layout()->addWidget( file_sep );
  non_param_frame->layout()->addWidget( refFileChooser );

  non_param_frame->layout()->addWidget( prop_sep );
  non_param_frame->layout()->addWidget( refPropSelector);
//  main_layout->addSpacing( 8 );

  non_param_frame->layout()->addWidget( lti_sep );
  non_param_frame->layout()->addWidget( LTI );

  non_param_frame->layout()->addWidget( uti_sep );
  non_param_frame->layout()->addWidget( UTI );
  non_param_frame->setVisible(false);


  main_layout->addWidget( options_group );
  main_layout->addWidget(distributionSelector);
  main_layout->addWidget(non_param_frame);


  main_layout->addStretch();

  this->setLayout( main_layout );
  this->setMaximumHeight(480);


  QObject::connect( from_distribution, SIGNAL( toggled(bool) ),
                    non_param_frame, SLOT( setHidden(bool) ) );
  QObject::connect( from_distribution, SIGNAL( toggled(bool) ),
                    distributionSelector, SLOT( setVisible(bool) ) );
  QObject::connect( from_file, SIGNAL( toggled(bool) ),
                    file_sep, SLOT( setShown(bool) ) );
  QObject::connect( from_file, SIGNAL( toggled(bool) ),
                    refFileChooser, SLOT( setShown(bool) ) );
  QObject::connect( from_grid, SIGNAL( toggled(bool) ),
                    prop_sep, SLOT( setShown(bool) ) );
  QObject::connect( from_grid, SIGNAL( toggled(bool) ),
                    refPropSelector, SLOT( setShown(bool) ) );


  from_grid->toggle();
  file_sep->setShown( false );
  refFileChooser->setShown( false );
}

void NonParamCdfInput::setReferenceGrid(const QString& grid_name){
  refPropSelector->setSelectedGrid( grid_name );
}
void NonParamCdfInput::setReferenceProperty(const QString& prop_name){
  refPropSelector->setSelectedProperty( prop_name );
}

bool NonParamCdfInput::isRefInDistribution() const { return from_distribution->isChecked(); }
bool NonParamCdfInput::isRefOnFile() const{ return from_file->isChecked(); }
bool NonParamCdfInput::isRefOnGrid() const{ return from_grid->isChecked(); }

QString NonParamCdfInput::getRefDistributionName() const{return distributionSelector->currentText();}
QString NonParamCdfInput::getRefGridName() const{return refPropSelector->selectedGrid();}
QString NonParamCdfInput::getRefPropName() const{ return refPropSelector->selectedProperty();}
QString NonParamCdfInput::getRefFileName() const{return refFileChooser->fileName();}
QString NonParamCdfInput::getRefRegionName() const {return refPropSelector->selectedRegion();}

void NonParamCdfInput::setRefDistributionName(QString distributionName){
  int id = distributionSelector->findText(distributionName);
  if(id < 0 ) return;
  distributionSelector->setCurrentIndex(id);
}

void NonParamCdfInput::setRefGridName(QString gridName){
  refPropSelector->setSelectedGrid(gridName);
}
void NonParamCdfInput::setRefPropName(QString propName){
  refPropSelector->setSelectedProperty(propName);
}
void NonParamCdfInput::setRefRegionName(QString regionName){
  refPropSelector->setSelectedRegion(regionName);
}

void NonParamCdfInput::setRefFileName(QString fileName){
  refFileChooser->setFileName(fileName);
}
void NonParamCdfInput::setRefOnFile(bool on ){ 
  from_file->setChecked(on);
//  from_grid->setChecked(!on);
}

void NonParamCdfInput::setRefOnGrid(bool on ){ 
//  from_file->setChecked(!on);
  from_grid->setChecked(on);
}

void NonParamCdfInput::setRefInDistribution(bool on ){ 
  from_distribution->setChecked(on);
}

bool NonParamCdfInput::isTieBreaking() const { return break_ties->isChecked(); }

void  NonParamCdfInput::setTieBreaking(bool on ){
  break_ties->setChecked(on);
}


