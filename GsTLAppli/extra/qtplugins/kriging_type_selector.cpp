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






#include <GsTLAppli/extra/qtplugins/kriging_type_selector.h>


#include <QPushButton>
#include <QLayout>
#include <utility>

#include <iostream>



Drift_components::Drift_components( QWidget* parent ):QWidget(parent){
	QVBoxLayout* layout = new QVBoxLayout(this);
	drifts_button_ = new QPushButton("External drifts",this);
	layout->addWidget(drifts_button_);
	this->setLayout(layout);

	QObject::connect(drifts_button_,SIGNAL(clicked()),this,SLOT(show_selection_dialog()));

}
Drift_components::~Drift_components() {}

void Drift_components::set_drift_components( const std::vector<std::string>& components ){
	drifts_.clear();
	for(int i=0; i<components.size(); ++i) drifts_.append(QString::fromStdString(components[i]));

	this->set_selection_tip();
}

void Drift_components::set_drift_components( const QStringList& components ){
	drifts_ = components;

	this->set_selection_tip();
}

QStringList Drift_components::drift_components() const{
	return drifts_;

}

QString Drift_components::grid_name() const{
  return grid_name_;
}

void Drift_components::set_button_text(const QString& text){
  if(text.isEmpty() )
    drifts_button_->setText("External drifts");
  else
    drifts_button_->setText("External drifts (" + text+")");
  
}


void Drift_components::set_selection_tip(){
	this->setToolTip(drifts_.join(" || "));
}

void Drift_components::set_grid_name(const QString& grid_name){
	grid_name_ = grid_name;
}

void Drift_components::show_selection_dialog(){
	  Ordered_property_selector_dialog* dialog =
	    new Ordered_property_selector_dialog( this, "Choose" );
	  dialog->setWindowTitle( "Select Properties" );
	  dialog->show_properties( grid_name_ );

	  if( drifts_.size() > 0 ) {
	    dialog->set_current_selection( drifts_ );
	  }

	  if( dialog->exec() == QDialog::Rejected ) return;

	  drifts_ = dialog->selection();
	  std::cout<<drifts_.at(0).toStdString();
	  this->set_selection_tip();

}



//===============



TrendComponents::TrendComponents( QWidget* parent, const char* name )
  : QWidget( parent ) {

  if (name)
    setObjectName(name);

  QVBoxLayout *layout = new QVBoxLayout(this);

  QGridLayout* trend_layout = new QGridLayout( this );
  trend_layout->setAlignment( Qt::AlignTop );

  x_ = new QCheckBox( "X", this);
  y_ = new QCheckBox( "Y", this);
  z_ = new QCheckBox( "Z", this);
  xy_ = new QCheckBox( "XY", this);
  xz_ = new QCheckBox( "XZ", this);
  yz_ = new QCheckBox( "YZ", this);
  x2_ = new QCheckBox( "X^2", this);
  y2_ = new QCheckBox( "Y^2", this);
  z2_ = new QCheckBox( "Z^2", this);
  drifts_ = new Drift_components(this);
  hd_drifts_ = new Drift_components(this);
  hd_drifts_->set_button_text("Data");
  hd_drifts_->hide();
  QHBoxLayout* drift_layout = new QHBoxLayout(this);
  drift_layout->addWidget(drifts_);
  drift_layout->addWidget(hd_drifts_);
  layout->addLayout(drift_layout);

//  layout->addWidget(drifts_);

  trend_layout->addWidget( x_, 0, 0 );
  trend_layout->addWidget( y_, 0, 1 );
  trend_layout->addWidget( z_, 0, 2 );
  trend_layout->addWidget( xy_, 1, 0 );
  trend_layout->addWidget( xz_, 1, 1 );
  trend_layout->addWidget( yz_, 1, 2 );
  trend_layout->addWidget( x2_, 2, 0 );
  trend_layout->addWidget( y2_, 2, 1 );
  trend_layout->addWidget( z2_, 2, 2 );
  layout->addItem(trend_layout);

}
TrendComponents::~TrendComponents(){}

std::vector<bool> TrendComponents::trendComponents() const {
  std::vector<bool> flags;
  flags.push_back( x_->isChecked() );
  flags.push_back( y_->isChecked() );
  flags.push_back( z_->isChecked() );
  flags.push_back( xy_->isChecked() );
  flags.push_back( xz_->isChecked() );
  flags.push_back( yz_->isChecked() );
  flags.push_back( x2_->isChecked() );
  flags.push_back( y2_->isChecked() );
  flags.push_back( z2_->isChecked() );

  return flags;
}

void TrendComponents::setTrendComponents( const std::vector<bool>& flags ) {
  x_->setChecked( flags[0] );
  y_->setChecked( flags[1] );
  z_->setChecked( flags[2] );
  xy_->setChecked( flags[3] );
  xz_->setChecked( flags[4] );
  yz_->setChecked( flags[5] );
  x2_->setChecked( flags[6] );
  y2_->setChecked( flags[7] );
  z2_->setChecked( flags[8] );
}

void TrendComponents::set_grid_name(const QString& grid_name){
	drifts_->set_grid_name(grid_name);
}

void TrendComponents::set_hd_grid_name(const QString& grid_name){
	hd_drifts_->set_grid_name(grid_name);
  if(!hd_drifts_->grid_name().isEmpty() &&  
    hd_drifts_->grid_name() != drifts_->grid_name()) {
      hd_drifts_->show();
      drifts_->set_button_text("Grid");

  }
  else {
    hd_drifts_->hide();
    drifts_->set_button_text("");
  }
}

QStringList TrendComponents::drift_components() const{
  return drifts_->drift_components();
}

QStringList TrendComponents::hd_drift_components() const{
  return hd_drifts_->drift_components();
}


void TrendComponents::set_drift_components(const QStringList& drifts) const{
  drifts_->set_drift_components(drifts);
}
void TrendComponents::set_hd_drift_components(const QStringList& drifts) const{
  hd_drifts_->set_drift_components(drifts);
}

//======================================================

KrigingTypeSelector::KrigingTypeSelector( QWidget* parent, const char* name )
  : QWidget(parent) {

    resize( 209, 421 );
    setWindowTitle( trUtf8( "Kriging Type Selection" ) );
    QVBoxLayout* Kriging_type_selector_baseLayout = new QVBoxLayout( this);
    Kriging_type_selector_baseLayout->setMargin(11);
    Kriging_type_selector_baseLayout->setSpacing(6);

    kriging_type_ = new QComboBox( this);
    kriging_type_->setEditable(false);
    kriging_type_->addItem( "Simple Kriging (SK)" );
    kriging_type_->addItem( "Ordinary Kriging (OK)" );
    kriging_type_->addItem( "Kriging with Trend (KT)" );
    kriging_type_->addItem( "SK with Locally Varying Mean (LVM)" );
    Kriging_type_selector_baseLayout->addWidget( kriging_type_ );

    sk_mean_box_ = new QGroupBox( this);
    sk_mean_box_->setFlat(true);
    sk_mean_box_->setTitle( "" );

    QHBoxLayout* sk_mean_box_Layout = new QHBoxLayout( );
    sk_mean_box_Layout->setAlignment( Qt::AlignTop );

    sk_mean_box_->setLayout(sk_mean_box_Layout);
    sk_mean_box_->layout()->setSpacing( 6 );
    sk_mean_box_->layout()->setMargin( 11 );

    sk_mean_box_Layout->addWidget(new QLabel("Mean", sk_mean_box_) );

    sk_mean_edit_ = new QLineEdit( sk_mean_box_);
    sk_mean_box_Layout->addWidget( sk_mean_edit_ );
    Kriging_type_selector_baseLayout->addWidget( sk_mean_box_ );

    // Widgets for KT/KED
    trends_ = new TrendComponents(this);
    trends_->hide();
    Kriging_type_selector_baseLayout->addWidget(trends_);


    // Widgets for LVM
    lvm_box_ = new QFrame(this);
    lvm_grid_box_ = new QFrame(lvm_box_);
    QHBoxLayout* hbox_grid_lvm = new QHBoxLayout(lvm_grid_box_);
    local_mean_property_ = new SinglePropertySelector(this);
    hbox_grid_lvm->addWidget(new QLabel("LVM for unknowm",lvm_grid_box_));
    hbox_grid_lvm->addWidget(local_mean_property_);
    lvm_grid_box_->setLayout(hbox_grid_lvm);

    lvm_hd_box_ = new QFrame(lvm_box_);
    QHBoxLayout* hbox_hd_lvm = new QHBoxLayout(lvm_hd_box_);
    hd_local_mean_property_ = new SinglePropertySelector(this);
    hbox_hd_lvm->addWidget(new QLabel("LVM for data",lvm_hd_box_));
    hbox_hd_lvm->addWidget(hd_local_mean_property_);
    lvm_hd_box_->setLayout(hbox_hd_lvm);

    QVBoxLayout* lvm_layout = new QVBoxLayout(this);
    lvm_layout->addWidget(lvm_grid_box_);
    lvm_layout->addWidget(lvm_hd_box_);
    lvm_box_->setLayout(lvm_layout);

    lvm_box_->hide();

    Kriging_type_selector_baseLayout->addWidget(lvm_box_);

/*
    trend_components_box_Layout = new QGridLayout( );
    trend_components_box_Layout->setAlignment( Qt::AlignTop );
    trend_components_box_->setLayout(trend_components_box_Layout);
    trend_components_box_->layout()->setSpacing( 6 );
    trend_components_box_->layout()->setMargin( 11 );
*/

  QObject::connect( kriging_type_, SIGNAL( activated( int ) ),
		    this, SLOT( promptForParameters( int ) ) );
  QObject::connect( kriging_type_, SIGNAL( activated( const QString& ) ),
		    this, SIGNAL( krigingTypeChanged( const QString& ) ) );
}


void KrigingTypeSelector::promptForParameters( int id ) {
  switch( id ) {
  case 0:   // Simple kriging
    sk_mean_box_->show();
    trends_->hide();
    lvm_box_->hide();
    break;

  case 1:  // Ordinary kriging
    sk_mean_box_->hide();
    trends_->hide();
    lvm_box_->hide();
    break;

  case 2:   // Kriging with trend
    sk_mean_box_->hide();
    trends_->show();
    lvm_box_->hide();
    break;

  case 3:   // Simple kriging with varying mean
    sk_mean_box_->hide();
    trends_->hide();
    lvm_box_->show();
    if(hd_local_mean_property_->grid_name().isEmpty() || 
       hd_local_mean_property_->grid_name() == local_mean_property_->grid_name()) {
         lvm_hd_box_->hide();
    }
    else
      lvm_hd_box_->show();

    break;

  }
}


float KrigingTypeSelector::skMean() const {
  QString val = sk_mean_edit_->text();
  return val.toFloat();
}

void KrigingTypeSelector::setSkMean( float m ) {
  QString val;
  val.setNum( m );
  sk_mean_edit_->setText( val );
  emit krigingParametersChanged();
}


std::vector<bool> KrigingTypeSelector::trendComponents() const {
	return trends_->trendComponents();
}

void KrigingTypeSelector::setTrendComponents( const std::vector<bool>& flags ) {
	trends_->setTrendComponents(flags);
  
  emit krigingParametersChanged();
}

std::pair<QString,QString> KrigingTypeSelector::LocalMeanProperty() const {
  return std::make_pair(  grid_name_, local_mean_property_->currentText() );
}

QString KrigingTypeSelector::local_mean_property() const {
  return local_mean_property_->currentText();
}
QString KrigingTypeSelector::hard_data_local_mean_property() const{
  return hd_local_mean_property_->currentText();
}

void KrigingTypeSelector::set_local_mean_property(const QString& prop){
  local_mean_property_->set_selected_property(prop);
}
void KrigingTypeSelector::set_hard_data_local_mean_property(const QString& prop){
  hd_local_mean_property_->set_selected_property(prop);
}

QStringList KrigingTypeSelector::drift_components() const{
  return trends_->drift_components();
}
QStringList KrigingTypeSelector::hd_drift_components() const{
  return trends_->hd_drift_components();
}


void KrigingTypeSelector::set_drift_components(const QStringList& drifts) const{
  trends_->set_drift_components(drifts);
}
void KrigingTypeSelector::set_hd_drift_components(const QStringList& drifts) const{
  trends_->set_hd_drift_components(drifts);
}


void
KrigingTypeSelector::setLocalMeanProperty( const std::pair<QString,QString>& prop ) {
  local_mean_property_->show_properties(prop.first);

  int id = local_mean_property_->findText(prop.second);
  if(id<0) id=0;
  local_mean_property_->setCurrentIndex(id);

  emit krigingParametersChanged();
}


QString KrigingTypeSelector::get_grid() const{
  return grid_name_;
}
QString KrigingTypeSelector::get_hd_grid() const{
  return hd_grid_name_;
}


void KrigingTypeSelector::set_grid( const QString& grid_name ){
	grid_name_= grid_name;
	trends_->set_grid_name(grid_name);
	local_mean_property_->show_properties(grid_name);
}

void KrigingTypeSelector::set_hd_grid( const QString& grid_name ){
  hd_grid_name_ = grid_name;
	trends_->set_hd_grid_name(grid_name);
  
	hd_local_mean_property_->show_properties(grid_name);
  if(!grid_name.isEmpty() && grid_name != local_mean_property_->grid_name())
    lvm_hd_box_->show();
  else
    lvm_hd_box_->hide();

}


void KrigingTypeSelector::setKrigingType( const QString& type ) {

  if( type.contains( "(SK)", Qt::CaseInsensitive ) || type.contains( "simple", Qt::CaseInsensitive ) ) {
    kriging_type_->setCurrentIndex( 0 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 0 );
    return;
  }

  if( type.contains( "(OK)", Qt::CaseInsensitive ) || type.contains( "ordinary", Qt::CaseInsensitive ) ) {
    kriging_type_->setCurrentIndex( 1 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 1 );
    return;
  }

  if( type.contains( "(KT)", Qt::CaseInsensitive ) || type.contains( "trend", Qt::CaseInsensitive ) ) {
    kriging_type_->setCurrentIndex( 2 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 2 );
    return;
  }
  
  if( type.contains( "(LVM)", Qt::CaseInsensitive ) || type.contains( "local", Qt::CaseInsensitive ) ) {
    kriging_type_->setCurrentIndex( 3 );
    emit krigingTypeChanged( kriging_type_->currentText() );
    promptForParameters( 3 );
    return;
  }
}


QString KrigingTypeSelector::krigingType() const {
  return kriging_type_->currentText();
/*  int id = kriging_type_->currentIndex();
  if(id == 0) return "SK";
  if(id==1) return "OK";
  if(id==2) return "KT";
  if(id==3) return "LVM";

  return "";
  */
}

/*
 * --------------------------------------------------------------
 */


HardDataKrigingPropertySelector::HardDataKrigingPropertySelector(QWidget* parent)
 : PropertySelector(parent){

	lvm_box_ = new QGroupBox("Local Varying Mean",this);
	QVBoxLayout* layout = new QVBoxLayout(this);
    local_mean_property_ = new SinglePropertySelector(lvm_box_);
    layout->addWidget(local_mean_property_);
    lvm_box_->setLayout(layout);
    lvm_box_->hide();


	drifts_ = new Drift_components(this);
	drifts_->hide();
	this->layout()->addWidget(lvm_box_);
	this->layout()->addWidget(drifts_);


}
HardDataKrigingPropertySelector::~HardDataKrigingPropertySelector(){}

Drift_components* HardDataKrigingPropertySelector::get_drift_widget(){
	return drifts_;
}

SinglePropertySelector* HardDataKrigingPropertySelector::get_lvm_widget(){
	return local_mean_property_;
}

void HardDataKrigingPropertySelector::set_kriging_type(const QString& kriging_type){

	kriging_type_ = kriging_type;
	if(domain_grid_name_ == this->selectedGrid()) return;

	if(kriging_type_ == "Kriging with Trend (KT)") {
		drifts_->show();
		lvm_box_->hide();
	}
	else if(kriging_type_ == "SK with Locally Varying Mean (LVM)") {
		drifts_->hide();
		lvm_box_->show();
	}

}

void HardDataKrigingPropertySelector::set_domain_grid(const QString& grid_name){
	if(domain_grid_name_ == grid_name) return;

	domain_grid_name_ = grid_name;
	if(this->selectedGrid() == domain_grid_name_) {
		drifts_->hide();
		lvm_box_->hide();
	}
}

QString HardDataKrigingPropertySelector::get_kriging_type(){
	return kriging_type_;
}

QString HardDataKrigingPropertySelector::get_domain_grid(){
	return domain_grid_name_;
}

bool HardDataKrigingPropertySelector::is_kriging_field_needed(){
	return (this->selectedGrid() != domain_grid_name_);
}
