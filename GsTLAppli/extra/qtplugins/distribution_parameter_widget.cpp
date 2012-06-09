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



#include "distribution_parameter_widget.h"

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/distribution_utilities.h>

#include <QRadioButton>
#include <QPushButton>

Distribution_parameter_widget::Distribution_parameter_widget(QWidget *parent)
  : QWidget(parent)
{

}

Distribution_parameter_widget::~Distribution_parameter_widget()
{

}

Continuous_distribution_parameter_widget::Continuous_distribution_parameter_widget(QWidget *parent)
  : QWidget(parent), current_param_widget_(0), current_distribution_(0)
{
  this->setLayout(new QVBoxLayout(this));
  QLabel* label = new QLabel("Name : ",this);
  name_edit_ = new QLineEdit(this);

  QHBoxLayout* name_layout = new QHBoxLayout (this);
  name_layout->addWidget(label);
  name_layout->addWidget(name_edit_);
  name_layout->addStretch(1);

  this->layout()->addItem(name_layout);

  non_param_widget_ = new Non_parameter_distribution_widget(this);
  this->layout()->addWidget(non_param_widget_);
  current_param_widget_ = non_param_widget_;

  connect(non_param_widget_, SIGNAL(distribution_ready()), this, SLOT(forward_distribution()));

}

Continuous_distribution_parameter_widget::~Continuous_distribution_parameter_widget()
{

}


void Continuous_distribution_parameter_widget::initialize(Continuous_distribution* distribution){
  
  current_distribution_ = distribution;
  if( current_param_widget_ != 0 ) current_param_widget_->setVisible(false);

  if( distribution->is_parametric() ) {
    QString d_name = QString::fromStdString(distribution->type());
    QHash<QString, Numbered_parameter_distribution*>::iterator it = params_widgets_.find(d_name);
    if( it ==  params_widgets_.end()) {
      Numbered_parameter_distribution* param_widget=0;
    
    
      Parametric_continuous_distribution* param_dist = dynamic_cast<Parametric_continuous_distribution*>(distribution);
      std::vector<std::string> pnames_str = param_dist->parameter_names();
      QStringList pnames;
      for(int i=0; i<pnames_str.size(); ++i) {
        pnames.append(pnames_str[i].c_str());
      }
      
      if(pnames.size() == 1)
        param_widget = new One_parameter_continous_distribution(pnames, this);
      else if(pnames.size() == 2) 
        param_widget = new Two_parameter_continous_distribution(pnames, this);
      else if(pnames.size() == 3) 
        param_widget = new Three_parameter_continous_distribution(pnames, this);

      std::vector<float> params = param_dist->parameter_values();
      param_widget->initialize(params);

      if(param_widget == 0) return;
      it = params_widgets_.insert(d_name, param_widget);
      this->layout()->addWidget(param_widget);
      connect(param_widget, SIGNAL(distribution_ready()), this, SLOT(forward_distribution()) );

    }
    
    current_param_widget_ = it.value();
    current_param_widget_->setVisible(true);
  }
  else {  // Non parametric distribution; built by the constructor
    non_param_widget_->setVisible(true);
    non_param_widget_->set_distribution(dynamic_cast<Non_parametric_distribution*>(distribution));
    non_param_widget_->setVisible(true);
    current_param_widget_ = non_param_widget_;
  }

  name_edit_->setText(distribution->name().c_str());





}

void  Continuous_distribution_parameter_widget::forward_distribution(){
  
  if(current_distribution_->is_parametric()) {
    Numbered_parameter_distribution* param_widget = dynamic_cast<Numbered_parameter_distribution*>(current_param_widget_);
    dynamic_cast<Parametric_continuous_distribution*>(current_distribution_)->initialize( param_widget->parameters() );
  }
  else {  // Nothing to do since the non_param distribution is updated when clicking on the "Compute" button
    

  }

  emit this->distribution_changed(current_distribution_);
}


/*
  ----------------------------------------
*/



Categorical_distribution_parameter_widget::Categorical_distribution_parameter_widget(QWidget *parent)
  : QWidget(parent)
{

}

Categorical_distribution_parameter_widget::~Categorical_distribution_parameter_widget()
{

}


One_parameter_continous_distribution::One_parameter_continous_distribution(QWidget* parent)
  :Numbered_parameter_distribution(parent){
  QGridLayout* layout = new QGridLayout(this);

  param1_name_ = new QLabel(this);
  param1_value_ = new QLineEdit(this);
  layout->addWidget(param1_name_,0,0);
  layout->addWidget(param1_value_,0,1);
  this->setLayout(layout);

  connect(param1_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));

}

One_parameter_continous_distribution::One_parameter_continous_distribution(const QStringList& param_names, QWidget* parent)  
  :Numbered_parameter_distribution(parent)
{
  QGridLayout* layout = new QGridLayout(this);

  param1_name_ = new QLabel(this);
  param1_value_ = new QLineEdit(this);
  layout->addWidget(param1_name_,0,0);
  layout->addWidget(param1_value_,0,1);
  this->setLayout(layout);

  connect(param1_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));

  this->initialize(param_names);
}
One_parameter_continous_distribution::~One_parameter_continous_distribution(){

}

std::vector<float> One_parameter_continous_distribution::parameters() const{
  std::vector<float> vals;
  bool ok;
  float v = param1_value_->text().toFloat(&ok);
  if(ok) vals.push_back(  v );
  return vals;

}
void One_parameter_continous_distribution::initialize(const QStringList& param_names){
  if( param_names.empty() ) return;

  param1_name_->setText(param_names.at(0));

}

void One_parameter_continous_distribution::initialize(const std::vector<float>& params){
  if( params.empty() ) 
    param1_value_->clear();
  else
    param1_value_->setText( QString("%1").arg(params[0]) );
}

void One_parameter_continous_distribution::check_distribution_parameters(){
  if (param1_value_->text().isEmpty()) return;
  bool ok = false; 
  float p1 = param1_value_->text().toFloat(&ok);
  if(ok) {
    emit this->distribution_ready();
  }
}


/*
  --------------------------
*/

Two_parameter_continous_distribution::Two_parameter_continous_distribution(QWidget* parent)
  :Numbered_parameter_distribution(parent){
  QGridLayout* layout = new QGridLayout(this);

  param1_name_ = new QLabel(this);
  param1_value_ = new QLineEdit(this);
  layout->addWidget(param1_name_,0,0);
  layout->addWidget(param1_value_,0,1);

  param2_name_ = new QLabel(this);
  param2_value_ = new QLineEdit(this);
  layout->addWidget(param2_name_,1,0);
  layout->addWidget(param2_value_,1,1);

  this->setLayout(layout);

  connect(param1_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));
  connect(param2_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));
}


Two_parameter_continous_distribution::Two_parameter_continous_distribution(const QStringList& param_names, QWidget* parent)  
  :Numbered_parameter_distribution(parent)
{
  QGridLayout* layout = new QGridLayout(this);

  param1_name_ = new QLabel(this);
  param1_value_ = new QLineEdit(this);
  layout->addWidget(param1_name_,0,0);
  layout->addWidget(param1_value_,0,1);
  
  param2_name_ = new QLabel(this);
  param2_value_ = new QLineEdit(this);
  layout->addWidget(param2_name_,1,0);
  layout->addWidget(param2_value_,1,1);  
  
  this->setLayout(layout);

  connect(param1_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));
  connect(param2_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));

  this->initialize(param_names);
}

Two_parameter_continous_distribution::~Two_parameter_continous_distribution(){

}

std::vector<float> Two_parameter_continous_distribution::parameters() const{
  std::vector<float> vals;
  bool ok;
  float v = param1_value_->text().toFloat(&ok);
  if(ok) 
    vals.push_back(  v );
  else
    return vals;

  v = param2_value_->text().toFloat(&ok);
  if(ok) 
    vals.push_back(  v );
  else {
    vals.clear();
    return vals;
  }

  return vals;


}
void Two_parameter_continous_distribution::initialize(const QStringList& param_names){
  if( param_names.size() !=2 ) return;

  param1_name_->setText(param_names.at(0));
  param2_name_->setText(param_names.at(1));

}

void Two_parameter_continous_distribution::initialize(const std::vector<float>& params){
  if( params.size() !=2 ) {
    param1_value_->clear();
    param2_value_->clear();
  }
  else {
    param1_value_->setText( QString("%1").arg(params[0]) );
    param2_value_->setText( QString("%1").arg(params[1]) );

  }
}

void Two_parameter_continous_distribution::check_distribution_parameters(){
  if(param1_value_->text().isEmpty()) return;
  if(param2_value_->text().isEmpty()) return;
  bool ok = false; 
  float p1 = param1_value_->text().toFloat(&ok);
  if(!ok) return;
  float p2 = param2_value_->text().toFloat(&ok);
  if(ok) {
    emit this->distribution_ready();
  }
}

/*
  --------------------------
*/

Three_parameter_continous_distribution::Three_parameter_continous_distribution(QWidget* parent)
  :Numbered_parameter_distribution(parent){
  QGridLayout* layout = new QGridLayout(this);

  param1_name_ = new QLabel(this);
  param1_value_ = new QLineEdit(this);
  layout->addWidget(param1_name_,0,0);
  layout->addWidget(param1_value_,0,1);

  param2_name_ = new QLabel(this);
  param2_value_ = new QLineEdit(this);
  layout->addWidget(param2_name_,1,0);
  layout->addWidget(param2_value_,1,1);

  param3_name_ = new QLabel(this);
  param3_value_ = new QLineEdit(this);
  layout->addWidget(param3_name_,2,0);
  layout->addWidget(param3_value_,2,1);

  this->setLayout(layout);

  connect(param1_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));
  connect(param2_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));
  connect(param3_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));


}

Three_parameter_continous_distribution::Three_parameter_continous_distribution(const QStringList& param_names, QWidget* parent)  
  :Numbered_parameter_distribution(parent)
{
  QGridLayout* layout = new QGridLayout(this);

  param1_name_ = new QLabel(this);
  param1_value_ = new QLineEdit(this);
  layout->addWidget(param1_name_,0,0);
  layout->addWidget(param1_value_,0,1);
  
  param2_name_ = new QLabel(this);
  param2_value_ = new QLineEdit(this);
  layout->addWidget(param2_name_,1,0);
  layout->addWidget(param2_value_,1,1);  
  

  param3_name_ = new QLabel(this);
  param3_value_ = new QLineEdit(this);
  layout->addWidget(param3_name_,2,0);
  layout->addWidget(param3_value_,2,1);

  this->setLayout(layout);

  connect(param1_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));
  connect(param2_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));
  connect(param3_value_, SIGNAL(editingFinished ()), this, SLOT(check_distribution_parameters()));

  this->initialize(param_names);
}


Three_parameter_continous_distribution::~Three_parameter_continous_distribution(){

}

std::vector<float> Three_parameter_continous_distribution::parameters() const{
  std::vector<float> vals;
  bool ok;
  float v = param1_value_->text().toFloat(&ok);
  if(ok) 
    vals.push_back(  v );
  else
    return vals;

  v = param2_value_->text().toFloat(&ok);
  if(ok) 
    vals.push_back(  v );
  else {
    vals.clear();
    return vals;
  }

  v = param3_value_->text().toFloat(&ok);
  if(ok) 
    vals.push_back(  v );
  else {
    vals.clear();
    return vals;
  }

  return vals;


}
void Three_parameter_continous_distribution::initialize(const QStringList& param_names){
  if( param_names.size() !=3 ) return;

  param1_name_->setText(param_names.at(0));
  param2_name_->setText(param_names.at(1));
  param3_name_->setText(param_names.at(2));

}

void Three_parameter_continous_distribution::initialize(const std::vector<float>& params){
  if( params.size() !=3 ) {
    param1_value_->clear();
    param2_value_->clear();
    param3_value_->clear();
  }
  else {
    param1_value_->setText( QString("%1").arg(params[0]) );
    param2_value_->setText( QString("%1").arg(params[1]) );
    param3_value_->setText( QString("%1").arg(params[2]) );

  }
}

void Three_parameter_continous_distribution::check_distribution_parameters(){
  if(param1_value_->text().isEmpty()) return;
  if(param2_value_->text().isEmpty()) return;
  if(param3_value_->text().isEmpty()) return;

  bool ok = false; 
  float p1 = param1_value_->text().toFloat(&ok);
  if(!ok) return;
  float p2 = param2_value_->text().toFloat(&ok);
  if(!ok) return;
  float p3 = param3_value_->text().toFloat(&ok);
  if(ok) {
    emit this->distribution_ready();
  }
}




Non_parameter_distribution_widget::Non_parameter_distribution_widget(QWidget* parent)
 : QGroupBox(parent), grid_(0),prop_(0),weight_(0),region_(0),dist_(0), current_filter_("None")   
{
  QGridLayout* layout = new QGridLayout(this);

  prop_selector_ = new PropertySelectorNoRegion(this);

  QGroupBox* button_group = new QGroupBox("Filtering options",this);
  QRadioButton* none_button = new QRadioButton("None",button_group);
  QRadioButton* region_button = new QRadioButton("Region",button_group);
  QRadioButton* weight_button = new QRadioButton("Weight",button_group);
  none_button->setChecked(true);

  QHBoxLayout* button_layout = new QHBoxLayout();
  button_layout->addWidget(none_button);
  button_layout->addWidget(region_button);
  button_layout->addWidget(weight_button);
  button_group->setLayout(button_layout);
    
  region_selector_ = new SingleRegionSelector(this);
  weight_selector_ = new SingleWeightPropertySelector(this);
  region_selector_->setEnabled(false);
  weight_selector_->setEnabled(false);

  lower_tail_input_ =  new LowerTailCdfInput(this);
  upper_tail_input_ = new UpperTailCdfInput(this);

  QPushButton* execute = new QPushButton("Compute", this);

  layout->addWidget(prop_selector_,0,0);
  layout->addWidget(button_group,0,1);
  layout->addWidget(region_selector_,1,0);
  layout->addWidget(weight_selector_,1,1);
  layout->addWidget(lower_tail_input_,2,0);
  layout->addWidget(upper_tail_input_,2,1);
  layout->addWidget(execute);


  connect(prop_selector_, SIGNAL(object_changed( const QString&)), 
          region_selector_, SLOT(show_regions( const QString&)) );

  connect(prop_selector_, SIGNAL(object_changed( const QString&)), 
          weight_selector_, SLOT(show_properties( const QString&)) );

  connect(region_button, SIGNAL(	toggled ( bool)), 
          region_selector_, SLOT(setEnabled ( bool )) );

  connect(weight_button, SIGNAL(	toggled ( bool)), 
          weight_selector_, SLOT(setEnabled ( bool )) );

    connect(execute, SIGNAL(	clicked ()), 
          this, SLOT(check_distribution_parameters ( )) );

}
Non_parameter_distribution_widget::~Non_parameter_distribution_widget(){

}


void Non_parameter_distribution_widget::check_distribution_parameters(){

  bool grid_changed = false;
  bool data_changed = false;

  if( prop_selector_->selectedGrid().isEmpty() || prop_selector_->selectedProperty().isEmpty() ){
    //emit this->clear_plot();
    return;
  }

  if( current_grid_name_ != prop_selector_->selectedGrid() ) {
    current_grid_name_ = prop_selector_->selectedGrid();
    grid_ = dynamic_cast<Geostat_grid*>(Root::instance()->interface( gridModels_manager+"/"+ current_grid_name_.toStdString()).raw_ptr());
    
    current_prop_name_ = prop_selector_->selectedProperty();
    prop_ = grid_->property(current_prop_name_.toStdString());
    grid_changed = true;
    data_changed = true;

  }
  else if( current_prop_name_ != prop_selector_->selectedProperty() ) {
    current_prop_name_ = prop_selector_->selectedProperty();
    prop_ = grid_->property(current_prop_name_.toStdString());
    data_changed = true;
  }

  if( weight_selector_->isEnabled()  ) {
    current_region_name_.clear();

    if( grid_changed || (weight_selector_->currentText() != current_weight_name_)  ) {
      current_weight_name_ = weight_selector_->currentText();
      weight_ = grid_->weight_property(current_weight_name_.toStdString());
      data_changed = true;
    }
  
  }
  else if( region_selector_->isEnabled()  ) {
    current_weight_name_.clear();

    if( grid_changed || (region_selector_->currentText() != current_region_name_)  ) {
      current_region_name_ = region_selector_->currentText();
      region_ = grid_->region(current_region_name_.toStdString());
      data_changed = true;
    }
  
  }
  else if( !region_selector_->isEnabled() && !weight_selector_->isEnabled() ) {
     current_region_name_.clear();
     current_weight_name_.clear();
     data_changed = true;
  }

  if(data_changed) {

    if(!current_weight_name_.isEmpty()) {
      Non_parametric_distribution* dist = build_non_param_distribution(std::string(""),prop_,weight_);
      dist_->copy(dist);
      delete dist;
    }
    else if( !current_region_name_.isEmpty() ) {
      Non_parametric_distribution* dist = build_non_param_distribution(std::string(""),prop_,region_);
      dist_->copy(dist);
      delete dist;
    }
    else {
      Non_parametric_distribution* dist = build_non_param_distribution(std::string(""),prop_);
      dist_->copy(dist);
      delete dist;
    }

    if( this->lti_parameters() != current_lti_ ) {
      this->initialize_lower_tail();
    }
    if( this->uti_parameters() != current_uti_ ) {
      this->initialize_upper_tail();
    }

    emit this->distribution_ready();

  }

}


bool Non_parameter_distribution_widget::initialize_lower_tail(){

  current_lti_ = this->lti_parameters();
  if( lower_tail_input_->getFunctionType() == "Power")  {
    double z_min = *(dist_->z_begin());
   
    if( lower_tail_input_->getExtremeValueText().isEmpty()) {
      lower_tail_input_->setExtremeValue(QString("%1").arg(z_min));
    }
    
    float t_min = lower_tail_input_->getExtremeValue();

    if(t_min > z_min) {
      lower_tail_input_->setExtremeValue(QString("%1").arg(z_min));
      t_min=z_min;
    }

    if( lower_tail_input_->getOmegaText().isEmpty()) {
      lower_tail_input_->setOmega(QString("%1").arg(1.5));
    }

    float omega = lower_tail_input_->getOmega();

    dist_->lower_tail_interpolator( Tail_interpolator( new Power_LTI(t_min,omega) ) );
    return true;
	}
  else if(lower_tail_input_->getFunctionType() == "No extrapolation") {
    dist_->lower_tail_interpolator( Tail_interpolator( new No_TI() ) );
    return true;
  }

  return false;
}



bool Non_parameter_distribution_widget::initialize_upper_tail(){

  current_uti_ = this->uti_parameters();

  if( upper_tail_input_->getFunctionType() == "Power")  {
    double z_max = *(dist_->z_end()-1);
    
    if( upper_tail_input_->getExtremeValueText().isEmpty()) {
      upper_tail_input_->setExtremeValue(QString("%1").arg(z_max));
    }
    
    float t_max = upper_tail_input_->getExtremeValue();

    if(t_max < z_max) {
      upper_tail_input_->setExtremeValue(QString("%1").arg(z_max));
      t_max=z_max;
    }

    if( upper_tail_input_->getOmegaText().isEmpty()) {
      upper_tail_input_->setOmega(QString("%1").arg(0.5));
    }

    float omega = upper_tail_input_->getOmega();

    dist_->upper_tail_interpolator( Tail_interpolator( new Power_UTI(t_max,omega) ) );
    return true;
	}
  else if( upper_tail_input_->getFunctionType() == "Hyperbolic" ) {

    if( upper_tail_input_->getOmegaText().isEmpty()) {
      upper_tail_input_->setOmega(QString("%1").arg(0.5));
    }
    float omega = upper_tail_input_->getOmega();
    dist_->upper_tail_interpolator( Tail_interpolator( new Hyperbolic_UTI(omega) ) );
    return true;
  }
  else if(upper_tail_input_->getFunctionType() == "No extrapolation") {
    dist_->upper_tail_interpolator( Tail_interpolator( new No_TI() ) );
    return true;
  }

  return false;
}


QStringList Non_parameter_distribution_widget::lti_parameters(){

  QStringList params;
  
  params.append(lower_tail_input_->getFunctionType());

  if( lower_tail_input_->getFunctionType() == "Power")  {
    params.append(lower_tail_input_->getExtremeValueText());
    params.append(lower_tail_input_->getOmegaText());

	}
  return params;
}

QStringList Non_parameter_distribution_widget::uti_parameters(){

  QStringList params;
  params.append(lower_tail_input_->getFunctionType());

  if( upper_tail_input_->getFunctionType() == "Power")  {
    params.append(upper_tail_input_->getExtremeValueText());
    params.append(upper_tail_input_->getOmegaText());

	}
  else if( upper_tail_input_->getFunctionType() == "Hyperbolic" ) {
    params.append(upper_tail_input_->getOmegaText());
  }

  return params;
}