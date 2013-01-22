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



/*
 * colormap_controller.cpp
 *
 *  Created on: May 19, 2011
 *      Author: aboucher
 */

#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <GsTLAppli/gui/viewer/colormap_controller.h>
#include <GsTLAppli/gui/viewer/colormap_widget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

Colormap_controller::Colormap_controller(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop,QWidget *parent) :
	QWidget(parent),prop_(prop), vtk_prop_(vtk_prop)
{
}

Colormap_controller::~Colormap_controller() {
	// TODO Auto-generated destructor stub
	
}






/*
 *
 *
 */



Colormap_continuous_controller::
Colormap_continuous_controller(const GsTLGridProperty* prop, GsTL_vtkProp* vtk_prop,QWidget* parent)
	: Colormap_controller(prop,vtk_prop,parent), cmap_(0)
{

	QVBoxLayout *cmapFrameLayout = new QVBoxLayout(this);

	colormap_selector_ = new QComboBox(this);
	this->init_colormap_selector();

	QFrame *cmapSelectorFrame = new QFrame(this);
	QHBoxLayout *cmapLayout = new QHBoxLayout(this);
	cmapLayout->addWidget(new QLabel("Colormap",this));
	cmapLayout->addWidget(colormap_selector_);
	cmapSelectorFrame->setLayout(cmapLayout);


	Qcolormap_bitmap* cmap_bitmap = new Qcolormap_bitmap( this,
							"cmap_bitmap");


	QSpinBox *alpha_spin = new QSpinBox;
	alpha_spin->setRange(0,255);
	alpha_spin->setValue(255);
	QSlider* alpha_slider = new QSlider(Qt::Horizontal, this);
	alpha_slider->setRange(0,255);
	alpha_slider->setValue(255);
	alpha_slider->setTracking(false);

	QHBoxLayout *alpha_layout = new QHBoxLayout;
	alpha_layout->addWidget(new QLabel("Opacity",this));
	alpha_layout->addWidget(alpha_slider);
	alpha_layout->addWidget(alpha_spin);


//	cmapFrameLayout->addLayout(cmapLayout);
	cmapFrameLayout->addWidget(cmapSelectorFrame);
	cmapFrameLayout->addWidget(cmap_bitmap);
	cmapFrameLayout->addLayout(alpha_layout);
	this->setLayout(cmapFrameLayout);

	QObject::connect(alpha_spin, SIGNAL(valueChanged(int)),
			alpha_slider, SLOT(setValue(int)));
	QObject::connect(alpha_slider, SIGNAL(valueChanged(int)),
			alpha_spin, SLOT(setValue(int)));

	QObject::connect(this, SIGNAL(colormap_changed(const Colormap*)),
						cmap_bitmap, SLOT(draw_color_scale( const Colormap*) ) );

//	QObject::connect(alpha_slider,SIGNAL(valueChanged(int)), this, SLOT(set_alpha(int)));
	QObject::connect(alpha_slider,SIGNAL(valueChanged(int)), this, SLOT(set_alpha(int)));


	min_cmap_value_edit_ = new QLineEdit(this);
	max_cmap_value_edit_ = new QLineEdit(this);
	QFrame *min_max_frame = new QFrame(this);
	QHBoxLayout *min_max_layout = new QHBoxLayout(min_max_frame);
	min_max_layout->addWidget(new QLabel("Min", this));
	min_max_layout->addWidget(min_cmap_value_edit_);
	min_max_layout->addSpacing(2);
	min_max_layout->addWidget(new QLabel("Max",this));
	min_max_layout->addWidget(max_cmap_value_edit_);
	min_max_frame->setLayout(min_max_layout);

	reset_cmap_bounds_button_ = new QPushButton("Recompute Min/max", this);

	this->layout()->addWidget(min_max_frame);
	this->layout()->addWidget(reset_cmap_bounds_button_);

	log_scale_checkbox_ = new QCheckBox(this);
	QHBoxLayout *log_layout = new QHBoxLayout(this);
	log_layout->addWidget(log_scale_checkbox_);
	log_layout->addWidget(new QLabel("  Log 10 scale",this));
	log_layout->addStretch();
	this->layout()->addItem(log_layout);



	QObject::connect( colormap_selector_, SIGNAL( activated( const QString& ) ),
			this, SLOT( set_colorscale( const QString&  ) ) );
	QObject::connect( min_cmap_value_edit_, SIGNAL( returnPressed() ),
			    this, SLOT( set_colormap_min_value() ) );
	QObject::connect( max_cmap_value_edit_, SIGNAL( returnPressed() ),
			    this, SLOT( set_colormap_max_value() ) );
	bool ok = QObject::connect( reset_cmap_bounds_button_, SIGNAL( clicked(bool) ),
			    this, SLOT( reset_min_max() ) );

	QObject::connect( this, SIGNAL( colormap_changed( const Colormap* ) ),
			this, SLOT( enable_log_scale( ) ) );
	QObject::connect( log_scale_checkbox_, SIGNAL( toggled( bool ) ),
			this, SLOT( set_log_scale(bool) ) );

	this->create_colormap();
	emit colormap_changed(cmap_);
//	set_colormap(colormap_selector_->currentText());

}

Colormap_continuous_controller::
~Colormap_continuous_controller(){
  delete cmap_;
}


void Colormap_continuous_controller::init_colormap_selector(){

	SmartPtr<Named_interface> ni =
	Root::instance()->interface( colorscale_manager );
	Manager* cmap_manager = dynamic_cast<Manager*>( ni.raw_ptr() );
	appli_assert( cmap_manager );

	for( Manager::interface_iterator cmap_it = cmap_manager->begin_interfaces();
	   cmap_it != cmap_manager->end_interfaces(); ++cmap_it ) {
	QString cmap_name( cmap_manager->name( cmap_it->raw_ptr() ).c_str() );
	colormap_selector_->addItem( cmap_name );
	}
  int id = colormap_selector_->findText("rainbow");
  if(id >= 0 ) {
    colormap_selector_->setCurrentIndex(id);
  }
}

void Colormap_continuous_controller::set_colormap_min_value() {
  QString min_str = min_cmap_value_edit_->text();
  float min = min_cmap_value_edit_->text().toFloat();
  float max = max_cmap_value_edit_->text().toFloat();

  if( min > cmap_->upper_bound() ) {
    min = cmap_->upper_bound();
    min_cmap_value_edit_->setText( min_str.setNum( min ) );
  }

  // Override if min < 0  and log scale is on
  if(this->is_log_scale() && min <= 0 ) {
	  min = 0.0000001;
	  max = std::max( max, min );
	  min_cmap_value_edit_->setText(QString("%1").arg(min));
	  max_cmap_value_edit_->setText(QString("%1").arg(max));
  }

  cmap_->set_bounds(min,max);

  this->enable_log_scale();
  emit colormap_changed(cmap_);
  //emit display_modified();

}

void Colormap_continuous_controller::set_colorscale( const QString& colorscale_name ) {

	SmartPtr<Named_interface> ni =
	Root::instance()->interface( colorscale_manager + "/" + colorscale_name.toStdString() );
	Color_scale* colors = dynamic_cast<Color_scale*>( ni.raw_ptr() );
	appli_assert( colors );
	cmap_->color_scale(colors);
	cmap_->refresh();

	emit colormap_changed(cmap_);
//	emit display_modified();

}

void Colormap_continuous_controller::set_alpha( int alpha ){
	if(alpha< 0 || alpha > 255) return;
	cmap_->set_alpha(static_cast<float>(alpha)/255);
	emit colormap_changed(cmap_);
//	emit display_modified();
}


void Colormap_continuous_controller::set_colormap_max_value() {
  QString max_str = max_cmap_value_edit_->text();

  float max = max_str.toFloat();
  if( max < cmap_->lower_bound() ) {
    max =  cmap_->lower_bound();
    max_cmap_value_edit_->setText( QString("%1").arg(max));
  }
  cmap_->upper_bound(max);
  emit colormap_changed(cmap_);
  //emit display_modified();

}


void Colormap_continuous_controller::enable_log_scale(){
	log_scale_checkbox_->setEnabled(cmap_->lower_bound() > 0);
}


void Colormap_continuous_controller::set_log_scale(bool on) {
	//vtk_prop_->colormap()->setLogScale(on);
	/*
	Colormap_continuous* cmap_cont = dynamic_cast<Colormap_continuous*>(vtk_prop_->colormap());
	if( cmap_cont ) cmap_cont->setLogScale(on);
	*/
	Colormap_continuous* cmap_cont = dynamic_cast<Colormap_continuous*>(cmap_);
	if(cmap_->lower_bound() <= 0) {
		float min = 0.000001;
		float max = cmap_->upper_bound();
		min_cmap_value_edit_->setText( QString("%1").arg(min) );
		if( max < min ) {
			max = min;
			max_cmap_value_edit_->setText( QString("%1").arg(max) );
		}
		cmap_->set_bounds(min,max);
	}

	cmap_cont->setLogScale(on);
  emit colormap_changed(cmap_);
	//emit display_modified();
}

bool Colormap_continuous_controller::is_log_scale(){
	Colormap_continuous* cmap_cont = dynamic_cast<Colormap_continuous*>(cmap_);
	return cmap_cont->is_log_scale();
}


void Colormap_continuous_controller::create_colormap(){

	std::string colorscale_name = colormap_selector_->currentText().toStdString();
	SmartPtr<Named_interface> ni =
	Root::instance()->interface( colorscale_manager + "/" + colorscale_name );
	Color_scale* colors = dynamic_cast<Color_scale*>( ni.raw_ptr() );
	appli_assert( colors );

	cmap_ = new Colormap_continuous( colors );
	this->reset_min_max();
}

bool Colormap_continuous_controller::reset_min_max() {
  int start=0;
  while( !prop_->is_informed( start ) ) {
    start++;
    // If we reach the end of the property
    if( start >= prop_->size() ) return false;
  }

  float min = prop_->get_value( start );
  float max = prop_->get_value( start );
  for( int i=start+1; i < prop_->size() ; i++ ) {
    if( !prop_->is_informed(i) ) continue;

    float val = prop_->get_value( i );
    min = std::min( min, val );
    max = std::max( max, val );
  }

  // Override if min < 0  and log scale is on
  if(this->is_log_scale() && min <= 0 ) {
	  min = 0.0000001;
	  max = std::max( max, min );
  }

  cmap_->set_bounds(min,max);

  QString min_str, max_str;
  min_str.setNum( cmap_->lower_bound() );
  max_str.setNum( cmap_->upper_bound() );
  min_cmap_value_edit_->setText( min_str );
  max_cmap_value_edit_->setText( max_str );

  this->enable_log_scale();

  this->set_colormap_min_value();
  this->set_colormap_max_value();



  return true;
}

/*
 *     Colormap_categorical_controller
 */



Colormap_categorical_controller::
Colormap_categorical_controller(const GsTLGridCategoricalProperty* prop, GsTL_vtkProp* vtk_prop,QWidget* parent)
	: Colormap_controller(prop,vtk_prop,parent), cprop_(prop), cmap_(0)
{
  cmap_ = new Colormap_categorical(cprop_->get_category_definition(), cprop_->get_number_of_category());
  cdef_table_ = new CategoricalDefinitionTable(this);
  cdef_table_->initialize();
  cdef_table_->set_flags_name(Qt::ItemIsEnabled );
  cdef_table_->set_flags_color(Qt::ItemIsEnabled | Qt::ItemIsEditable);

  const CategoricalPropertyDefinition* cdef_const = cprop_->get_category_definition();

  //Need to a non-const cat definition
  Named_interface* ni = Root::instance()->interface(categoricalDefinition_manager+"/"+cdef_const->name()).raw_ptr();
  CategoricalPropertyDefinition* cdef = dynamic_cast<CategoricalPropertyDefinition*>(ni);
  
  CategoricalPropertyDefinitionName* cdef_named = dynamic_cast<CategoricalPropertyDefinitionName*>(cdef);
  if(cdef_named) {
    cdef_table_->show_definition(cdef_named);
  }
  else {
    cdef_table_->show_definition(cprop_->get_number_of_category());
  }

  display_button_ = new QPushButton("Display/Update",this);


  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(cdef_table_);
  layout->addWidget(display_button_);

	QObject::connect( display_button_, SIGNAL( clicked() ),
			this, SLOT( update_colormap() ) );


//	this->update_colormap();
	emit colormap_changed(cmap_);


}

Colormap_categorical_controller::
~Colormap_categorical_controller(){

}
/*
void Colormap_categorical_controller::create_colormap(){

    cmap_ = new Colormap_categorical(cprop_->get_category_definition(), cprop_->get_number_of_category());

}
*/
void Colormap_categorical_controller::update_colormap(){

  // May need to update the displays
  if( cmap_->get_categorical_definition() != cprop_->get_category_definition()) {

    //Need to a non-const cat definition
    Named_interface* ni = Root::instance()->interface(categoricalDefinition_manager+"/"+cprop_->get_category_definition()->name()).raw_ptr();
    CategoricalPropertyDefinition* cdef = dynamic_cast<CategoricalPropertyDefinition*>(ni);

    CategoricalPropertyDefinitionName* cdef_named = dynamic_cast<CategoricalPropertyDefinitionName*>(cdef);
    if(cdef_named) {
      cdef_table_->show_definition(cdef_named);
    }
    else {
      cdef_table_->show_definition(cprop_->get_number_of_category());
    }

  }

  cmap_->set_categorical_definition(cprop_->get_category_definition(), cprop_->get_number_of_category());

  emit colormap_changed(cmap_);

}