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



#include "chart_display_control.h"

Chart_display_control::Chart_display_control(QWidget *parent)
    : QWidget(parent)
{
  ui.setupUi(this);

  connect(ui.title_label_edit, SIGNAL(textChanged(const QString &)), this, SIGNAL(title_changed(const QString &)));
  connect( ui.x_label_edit, SIGNAL(textChanged(const QString &)), this, SIGNAL(xaxis_label_changed(const QString &)));
  connect( ui.y_label_edit, SIGNAL(textChanged(const QString &)), this, SIGNAL(yaxis_label_changed(const QString &)));
  connect(ui.legend_display, SIGNAL(	toggled(bool)), this, SIGNAL(legend_display_changed(bool)));
  //connect(ui.grid_display, SIGNAL(	toggled(bool)), this, SIGNAL(grid_display_changed(bool)));
  connect(ui.x_grid_display, SIGNAL(	toggled(bool)), this, SIGNAL(x_grid_display_changed(bool)));
  connect(ui.y_grid_display, SIGNAL(	toggled(bool)), this, SIGNAL(y_grid_display_changed(bool)));

  bool ok = connect(ui.x_axis_font_size, SIGNAL(	valueChanged (int)), this, SIGNAL( x_axis_font_size(int)) );
  ok = connect(ui.y_axis_font_size, SIGNAL(	valueChanged (int)), this, SIGNAL( y_axis_font_size(int)) );
  ok = connect(ui.x_label_font_size, SIGNAL(	valueChanged (int)), this, SIGNAL( x_label_font_size(int)) );
  ok = connect(ui.y_label_font_size, SIGNAL(	valueChanged (int)), this, SIGNAL( y_label_font_size(int)) );
  ok = connect(ui.legend_font_size, SIGNAL(	valueChanged (int)), this, SIGNAL( legend_font_size(int)) );
  ok = connect(ui.title_font_size, SIGNAL(	valueChanged (int)), this, SIGNAL( title_font_size(int)) );


  connect(ui.x_min, SIGNAL(editingFinished()), this,  SLOT( forward_x_min_changed() ) );
  connect(ui.x_max, SIGNAL(editingFinished()), this,  SLOT( forward_x_max_changed() ) );
  connect(ui.x_precision, SIGNAL(	valueChanged ( int )), this,  SIGNAL( xaxis_precision_changed(int) ) );
  connect(ui.x_nticks, SIGNAL(	valueChanged ( int )), this,  SIGNAL( xaxis_nticks_changed(int) ) );
  connect(ui.x_logscale, SIGNAL(	toggled(bool)), this,  SIGNAL( xaxis_logscale_changed(bool) ) );
  connect(ui.x_autoscale, SIGNAL(	clicked (bool)), this,  SIGNAL( xaxis_autoscale_changed() ) );

  connect(ui.y_min, SIGNAL(editingFinished()), this,  SLOT( forward_y_min_changed() ) );
  connect(ui.y_max, SIGNAL(editingFinished()), this,  SLOT( forward_y_max_changed() ) );
  connect(ui.y_precision, SIGNAL(	valueChanged ( int )), this,  SIGNAL( yaxis_precision_changed(int) ) );
  connect(ui.y_nticks, SIGNAL(	valueChanged ( int )), this,  SIGNAL( yaxis_nticks_changed(int) ) );
  connect(ui.y_logscale, SIGNAL(	toggled(bool)), this,  SIGNAL( yaxis_logscale_changed(bool) ) );
  connect(ui.y_autoscale, SIGNAL(	clicked (bool)), this,  SIGNAL( yaxis_autoscale_changed() ) );



}

void Chart_display_control::send_axis_signals(){
  QString str_par;
  str_par = ui.title_label_edit->text();
  emit title_changed(str_par);
  str_par = ui.x_label_edit->text();
  emit xaxis_label_changed(str_par);
  str_par = ui.y_label_edit->text();
  emit yaxis_label_changed(str_par);
  emit legend_display_changed(ui.legend_display->isChecked());
  emit x_grid_display_changed(ui.x_grid_display->isChecked());
  emit y_grid_display_changed(ui.y_grid_display->isChecked());

  emit title_font_size(ui.title_font_size->value());
  emit legend_font_size(ui.legend_font_size->value());
  emit x_axis_font_size(ui.x_axis_font_size->value());
  emit y_axis_font_size(ui.y_axis_font_size->value());
  emit x_label_font_size(ui.x_label_font_size->value());
  emit y_label_font_size(ui.y_label_font_size->value());

}


Chart_display_control::~Chart_display_control()
{

}

void Chart_display_control::forward_x_min_changed() {
  double v = ui.x_min->value();
  double vmax = ui.x_max->value();
  ui.x_min->setValue(v >  vmax? vmax:v );
  emit this->xaxis_min_changed(ui.x_min->value());
}

void Chart_display_control::forward_x_max_changed(){
  double v = ui.x_max->value();
  double vmin = ui.x_min->value();
  ui.x_max->setValue(v <  vmin? vmin:v );

  emit this->xaxis_max_changed(ui.x_max->value());

}
void Chart_display_control::forward_y_min_changed(){
  double v = ui.y_min->value();
  double vmax = ui.y_max->value();
  ui.y_min->setValue(v >  vmax? vmax:v );
  emit this->yaxis_min_changed(ui.y_min->value());

}
void Chart_display_control::forward_y_max_changed(){
  double v = ui.y_max->value();
  double vmin = ui.y_min->value();
  ui.y_max->setValue(v <  vmin? vmin:v );
  emit this->yaxis_max_changed(ui.y_max->value());

}

void Chart_display_control::set_title(const QString& label){
  ui.title_label_edit->setText( label );
}

void Chart_display_control::set_xaxis_min(double min){
  ui.x_min->setValue(min);
}
void Chart_display_control::set_xaxis_max(double max){
  ui.x_max->setValue(max);
}
void Chart_display_control::set_xaxis_precision(int precision){
  ui.x_precision->setValue(precision);
}
void Chart_display_control::set_xaxis_nticks(int nticks){
  ui.x_nticks->setValue(nticks);
}

void Chart_display_control::set_xaxis_label(const QString& label) {
  ui.x_label_edit->setText( label );
}

void Chart_display_control::set_yaxis_min(double min){
  ui.y_min->setValue(min);
}
void Chart_display_control::set_yaxis_max(double max){
  ui.y_max->setValue(max);
}
void Chart_display_control::set_yaxis_precision(int precision){
  ui.y_precision->setValue(precision);
}
void Chart_display_control::set_yaxis_nticks(int nticks){
  ui.y_nticks->setValue(nticks);
}

void Chart_display_control::set_yaxis_label(const QString& label) {
  ui.y_label_edit->setText( label );
}