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



#include "chart_option_controls.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
Chart_option_controls::Chart_option_controls(QWidget *parent)
  : QWidget(parent)
{
  QLabel* x_label = new QLabel("X axis",this);
  QLabel* y_label = new QLabel("Y axis",this);
  QLabel* title_label = new QLabel("Title",this);
  
  x_line_edit_ = new QLineEdit("X",this);
  y_line_edit_ = new QLineEdit("Y",this);
  title_line_edit_ = new QLineEdit("",this);

  legend_check_ = new QCheckBox("Display legend",this);

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(x_label, 0, 0);
  layout->addWidget(x_line_edit_, 0, 1);
  layout->addWidget(y_label, 1, 0);
  layout->addWidget(y_line_edit_, 1, 1);
  layout->addWidget(title_label, 2, 0);
  layout->addWidget(title_line_edit_, 2, 1);
  layout->addWidget(legend_check_, 3, 0);

  this->setLayout(layout);

  //Forward signal
  connect(x_line_edit_, SIGNAL(textChanged(const QString &)), this, SIGNAL(xaxis_changed(const QString &)));
  connect(y_line_edit_, SIGNAL(textChanged(const QString &)), this, SIGNAL(yaxis_changed(const QString &)));
  connect(title_line_edit_, SIGNAL(textChanged(const QString &)), this, SIGNAL(title_changed(const QString &)));
  connect(legend_check_, SIGNAL(	toggled(bool)), this, SIGNAL(legend_display_changed(bool)));


}

Chart_option_controls::~Chart_option_controls()
{

}
void Chart_option_controls::set_xaxis(const QString& text){
  x_line_edit_->setText(text);
}
void Chart_option_controls::set_yaxis(const QString& text){
  y_line_edit_->setText(text);
}
void Chart_option_controls::set_title(const QString& text){
  title_line_edit_->setText(text);
}
void Chart_option_controls::set_legend_display(bool on){
  legend_check_->setChecked(on);
}