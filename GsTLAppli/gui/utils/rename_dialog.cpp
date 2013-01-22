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
 * rename_dialog.cpp
 *
 *  Created on: May 27, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/gui/utils/rename_dialog.h>

#include <QVBoxLayout>
#include <QPushButton>

Rename_dialog::
Rename_dialog(QString object_type, QString old_name,  QWidget* parent)
  : QDialog(parent) {

//	this->setModal(true);
	this->setWindowIconText(QString("Rename %1 %2").arg(object_type).arg(old_name) );
	line_new_name_ = new QLineEdit(old_name,this);

	QPushButton* accept_button = new QPushButton("Apply", this);
	accept_button->setDefault(true);
	QPushButton* reject_button = new QPushButton("Cancel", this);

	QHBoxLayout* button_layout = new QHBoxLayout(this);
	button_layout->addWidget(accept_button);
	button_layout->addWidget(reject_button);

  QFrame* frame = new QFrame(this);
  frame->setLayout(button_layout);

	this->setLayout(new QVBoxLayout(this));
	this->layout()->addWidget(line_new_name_);
  this->layout()->addWidget(frame);
//	this->layout()->addItem(button_layout);

	QObject::connect(accept_button, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(reject_button, SIGNAL(clicked()), this, SLOT(reject()));

}

Rename_dialog::~Rename_dialog() {
}

QString Rename_dialog::new_name(){
	return  line_new_name_->text();
}
