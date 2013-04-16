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


#include <GsTLAppli/charts/chart_base_display_controls.h>


#include <vtkAxis.h>
#include <vtkTextProperty.h>
#include <vtkChartLegend.h>

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QAbstractItemView>
#include <QFileDialog>

#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>

Chart_base_display_controls::Chart_base_display_controls( QWidget *parent)
  :  Chart_base(parent)
{

  //Add the vtk rendering window
  qvtkWidget_ = new QVTKWidget(this);
	context_view_ = vtkSmartPointer<vtkContextView>::New();
	context_view_->SetInteractor(qvtkWidget_->GetInteractor());
	qvtkWidget_->SetRenderWindow(context_view_->GetRenderWindow());
	chart_ = vtkSmartPointer<vtkChartXY>::New();
	context_view_->GetScene()->AddItem(chart_);  

  chart_control_ = new Chart_display_control( this );

  report_frame_ = new QFrame(this);
  view_report_button_ = new QPushButton("View Report",this);
  save_report_button_ = new QPushButton("Save Report",this);
  save_figure_button_ = new QPushButton("Save Figure",this);
  QHBoxLayout* report_layout = new QHBoxLayout(report_frame_);
  report_layout->addStretch();
  report_layout->addWidget(save_figure_button_);
  report_layout->addWidget(save_report_button_);
  report_layout->addWidget(view_report_button_);
  report_frame_->setLayout(report_layout);

  connect( chart_control_, SIGNAL(xaxis_label_changed(const QString&)), this, SLOT(set_x_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(yaxis_label_changed(const QString&)), this, SLOT(set_y_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(title_changed(const QString&)), this, SLOT(set_title(const QString&)) );
  connect( chart_control_, SIGNAL(legend_display_changed(bool)), this, SLOT(set_legend(bool)) );
  connect( chart_control_, SIGNAL(grid_display_changed(bool)), this, SLOT(set_grid(bool)) );
  connect( chart_control_, SIGNAL(x_grid_display_changed(bool)), this, SLOT(set_x_grid(bool)) );
  connect( chart_control_, SIGNAL(y_grid_display_changed(bool)), this, SLOT(set_y_grid(bool)) );

  bool ok = connect( chart_control_, SIGNAL(x_axis_font_size(int)), this, SLOT(set_x_axis_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(y_axis_font_size(int)), this, SLOT(set_y_axis_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(x_label_font_size(int)), this, SLOT(set_x_label_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(y_label_font_size(int)), this, SLOT(set_y_label_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(legend_font_size(int)), this, SLOT(set_legend_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(title_font_size(int)), this, SLOT(set_title_font_size(int)) );

  connect( chart_control_, SIGNAL(xaxis_min_changed(double)), this, SLOT(set_xaxis_min(double)) );
  connect( chart_control_, SIGNAL(xaxis_max_changed(double)), this, SLOT(set_xaxis_max(double)) );
  connect( chart_control_, SIGNAL(xaxis_precision_changed(int)), this, SLOT(set_xaxis_precision(int)) );
  connect( chart_control_, SIGNAL(xaxis_nticks_changed(int)), this, SLOT(set_xaxis_nticks(int)) );
  connect( chart_control_, SIGNAL(xaxis_logscale_changed(bool)), this, SLOT(set_xaxis_logscale(bool)) );
  connect( chart_control_, SIGNAL(xaxis_autoscale_changed()), this, SLOT(set_xaxis_autoscale()) );

  connect( chart_control_, SIGNAL(yaxis_min_changed(double)), this, SLOT(set_yaxis_min(double)) );
  connect( chart_control_, SIGNAL(yaxis_max_changed(double)), this, SLOT(set_yaxis_max(double)) );
  connect( chart_control_, SIGNAL(yaxis_precision_changed(int)), this, SLOT(set_yaxis_precision(int)) );
  connect( chart_control_, SIGNAL(yaxis_nticks_changed(int)), this, SLOT(set_yaxis_nticks(int)) );
  connect( chart_control_, SIGNAL(yaxis_logscale_changed(bool)), this, SLOT(set_yaxis_logscale(bool)) );
  connect( chart_control_, SIGNAL(yaxis_autoscale_changed()), this, SLOT(set_yaxis_autoscale()) );

  connect( view_report_button_, SIGNAL(clicked()), this, SLOT(view_report()) );
  connect( save_report_button_, SIGNAL(clicked()), this, SLOT(save_report()) );
  connect( save_figure_button_, SIGNAL(clicked()), this, SLOT(save_figure()) );

  

}


Chart_base_display_controls::~Chart_base_display_controls()
{

}


void Chart_base_display_controls::set_report_control_visibility(bool ok){
  report_frame_->setVisible(ok);
}

void Chart_base_display_controls::set_save_report_visibility(bool ok){
  save_report_button_->setVisible( ok );
}
void Chart_base_display_controls::set_save_figure_visibility(bool ok){
  save_figure_button_->setVisible( ok );

}
void Chart_base_display_controls::set_view_report_visibility(bool ok){
  view_report_button_->setVisible( ok );
}


void Chart_base_display_controls::update_chart_axis_display_control(){
  vtkAxis* xaxis = chart_->GetAxis(vtkAxis::BOTTOM);

  double xmin = xaxis->GetMinimum();
  double xmax = xaxis->GetMaximum();
  if( chart_control_->is_x_axis_log_scale() ) {
    xmin = std::pow(xmin,10);
    xmax = std::pow(xmax,10);
  }
  chart_control_->set_xaxis_min(xmin);
  chart_control_->set_xaxis_max(xmax);
  chart_control_->set_xaxis_precision(xaxis->GetPrecision());
  chart_control_->set_xaxis_nticks(xaxis->GetNumberOfTicks());

  vtkAxis* yaxis = chart_->GetAxis(vtkAxis::LEFT);
  double ymin = yaxis->GetMinimum();
  double ymax = yaxis->GetMaximum();
  if( chart_control_->is_y_axis_log_scale() ) {
    ymin = std::pow(ymin,10);
    ymax = std::pow(ymax,10);
  }
  chart_control_->set_yaxis_min(ymin);
  chart_control_->set_yaxis_max(ymax);
  chart_control_->set_yaxis_precision(yaxis->GetPrecision());
  chart_control_->set_yaxis_nticks(yaxis->GetNumberOfTicks());
}


void Chart_base_display_controls::set_x_axis_label(const QString& text){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetTitle(text.toStdString());
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_y_axis_label(const QString& text){
  chart_->GetAxis(vtkAxis::LEFT)->SetTitle(text.toStdString());
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_title(const QString& text){
  chart_->SetTitle(text.toStdString().c_str());
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_legend(bool on){
  chart_->SetShowLegend(on);
  qvtkWidget_->update();
}

void Chart_base_display_controls::set_grid(bool on){
  chart_->GetAxis(0)->SetGridVisible(on);
  chart_->GetAxis(1)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_base_display_controls::set_x_grid(bool on){
  chart_->GetAxis(0)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_base_display_controls::set_y_grid(bool on){
  chart_->GetAxis(1)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_base_display_controls::set_xaxis_min(double min){
  if( chart_control_->is_x_axis_log_scale() ) {
    if(min <= 0) min = 0.0000001;
    min = std::log10( min );
  }
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(min);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_xaxis_max(double max){
  if( chart_control_->is_x_axis_log_scale() ) {
    if(max <= 0) max = 0.0000001;
    max = std::log10( max );
  }
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(max);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_xaxis_precision(int digits){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetPrecision(digits);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_xaxis_nticks(int nticks){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetNumberOfTicks(nticks);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_xaxis_logscale(bool on){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetLogScale(on);
  chart_control_->send_axis_signals();
  qvtkWidget_->update();
  //this->update_chart_axis_display_control();
}
void Chart_base_display_controls::set_xaxis_autoscale(){
  chart_->GetAxis(vtkAxis::BOTTOM)->AutoScale();
  chart_->Update();
  qvtkWidget_->update();
  this->update_chart_axis_display_control();
}

void Chart_base_display_controls::set_yaxis_min(double min){
  if( chart_control_->is_y_axis_log_scale() ) {
    if(min <= 0) min = 0.0000001;
    min = std::log10( min );
  }
  chart_->GetAxis(vtkAxis::LEFT)->SetMinimum(min);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_yaxis_max(double max){
  if( chart_control_->is_y_axis_log_scale() ) {
    if(max <= 0) max = 0.0000001;
    max = std::log10( max );
  }
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(max);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_yaxis_precision(int digits){
  chart_->GetAxis(vtkAxis::LEFT)->SetPrecision(digits);
  chart_->Update();
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_yaxis_nticks(int nticks){
  chart_->GetAxis(vtkAxis::LEFT)->SetNumberOfTicks(nticks);
  chart_->Update();
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_yaxis_logscale(bool on){
  chart_->GetAxis(vtkAxis::LEFT)->SetLogScale(on);
  chart_control_->send_axis_signals();
  chart_->Update();
  qvtkWidget_->update();
//  this->update_chart_axis_display_control();
}
void Chart_base_display_controls::set_yaxis_autoscale(){
  chart_->GetAxis(vtkAxis::LEFT)->AutoScale();
  chart_->Update();
  qvtkWidget_->update();
  this->update_chart_axis_display_control();
}

void Chart_base_display_controls::set_x_axis_font_size(int size){
  chart_->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_y_axis_font_size(int size){
  chart_->GetAxis(vtkAxis::LEFT)->GetLabelProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_x_label_font_size(int size){
  chart_->GetAxis(vtkAxis::BOTTOM)->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_y_label_font_size(int size){
  chart_->GetAxis(vtkAxis::LEFT)->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_legend_font_size(int size){
  chart_->GetLegend()->SetLabelSize(size);
  qvtkWidget_->update();
}
void Chart_base_display_controls::set_title_font_size(int size){
  chart_->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}


void Chart_base_display_controls::reset_axis(){

}

void Chart_base_display_controls::save_figure() {

  QString filename = 
    QFileDialog::getSaveFileName( this,"Save figure",chart_control_->get_title());

  if(filename.isEmpty()) return;

  if( !filename.endsWith(".png",Qt::CaseInsensitive) ||
      !filename.endsWith(".jpeg",Qt::CaseInsensitive) ||
      !filename.endsWith(".jpg",Qt::CaseInsensitive) ) 
  {
    filename.append(".png");
  }

  this->save_figure(filename);

}

void Chart_base_display_controls::save_figure(QString& filename ) {
  QApplication::setOverrideCursor( Qt::WaitCursor );

  vtkSmartPointer<vtkWindowToImageFilter> w2i = vtkSmartPointer<vtkWindowToImageFilter>::New();
  w2i->SetInput(context_view_->GetRenderWindow());
  w2i->Modified();
  
  QByteArray ba_filename = filename.toLatin1();


  if(  filename.endsWith( "PNG", Qt::CaseInsensitive ) ) {
    
    vtkSmartPointer<vtkPNGWriter> png = vtkSmartPointer<vtkPNGWriter>::New();
    png->SetInputConnection(w2i->GetOutputPort());
    png->SetFileName(ba_filename.data());
    png->Write();
  }

  if( filename.endsWith( "JPEG", Qt::CaseInsensitive ) || filename.endsWith( "JPG", Qt::CaseInsensitive ) ) {

    vtkSmartPointer<vtkJPEGWriter> jpeg = vtkSmartPointer<vtkJPEGWriter>::New();
    jpeg->SetInputConnection(w2i->GetOutputPort());
    jpeg->SetFileName(ba_filename.data());
    jpeg->Write();
  }

  QApplication::restoreOverrideCursor();

}
void Chart_base_display_controls::save_report(){


}

void Chart_base_display_controls::view_report(){


}