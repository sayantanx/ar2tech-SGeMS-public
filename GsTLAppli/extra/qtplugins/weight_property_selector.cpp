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



#include "weight_property_selector.h"

SingleWeightPropertySelector::SingleWeightPropertySelector(QWidget *parent)
  : QComboBox(parent), model_(0)
{
  current_grid_ = "";
}

SingleWeightPropertySelector::~SingleWeightPropertySelector()
{

}



void SingleWeightPropertySelector::show_properties( const QString& grid_name ) {
  if(model_ == 0) {
    model_ = new Weight_property_proxy_model_no_selection(this);
    this->setModel(model_);
  }
	if(grid_name == current_grid_) {

	}
	else {
    model_->set_grid(grid_name);
    current_grid_ = grid_name;
	}

	emit activated( this->currentText() );

}

void SingleWeightPropertySelector::set_selected_property( const QString& property_name ){

	int id = this->findText(property_name);
	if(id>=0) {
		this->setCurrentIndex(id);
	}

}

QString SingleWeightPropertySelector::grid_name() const{
  return current_grid_;
}
