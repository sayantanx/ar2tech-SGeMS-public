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



#include "chart_distribution.h"

#include <vtkContextScene.h>
#include <vtkDoubleArray.h>
#include <vtkAxis.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <QVBoxLayout>

Chart_distribution::Chart_distribution(QWidget *parent)
  : QWidget(parent)
{

  QVBoxLayout* layout = new QVBoxLayout(this);

  //Add the vtk rendering window
  qvtkWidget_ = new QVTKWidget(this);

  qvtkWidget_ = new QVTKWidget(this);
  context_view_ = vtkSmartPointer<vtkContextView>::New();
  context_view_->SetInteractor(qvtkWidget_->GetInteractor());
  qvtkWidget_->SetRenderWindow(context_view_->GetRenderWindow());
	chart_ = vtkSmartPointer<vtkChartXY>::New();
	context_view_->GetScene()->AddItem(chart_); 

  qvtkWidget_->setMinimumSize(QSize(200, 150));

  chart_->GetAxis(vtkAxis::BOTTOM)->SetTitle("Quantiles");
  chart_->GetAxis(vtkAxis::BOTTOM)->SetBehavior(1);

  layout->addWidget(qvtkWidget_);

  this->setLayout(layout);
  table_ = vtkSmartPointer<vtkTable>::New();

}

Chart_distribution::~Chart_distribution()
{

}

void Chart_distribution::set_distribution( Continuous_distribution* distribution ){

  table_ = vtkSmartPointer<vtkTable>::New();

  vtkSmartPointer<vtkDoubleArray> cdf = vtkSmartPointer<vtkDoubleArray>::New();
  vtkSmartPointer<vtkDoubleArray> pdf = vtkSmartPointer<vtkDoubleArray>::New();
  cdf->SetName("CDF");
  pdf->SetName("PDF");
  vtkSmartPointer<vtkDoubleArray> quantiles = vtkSmartPointer<vtkDoubleArray>::New();
  quantiles->SetName("z");


  double q = distribution->quantile(0.001);
  double f_max = -1;
  if( boost::math::isfinite(q)) {
    cdf->InsertNextValue(0.001);
    quantiles->InsertNextValue(q);
    double f = distribution->pdf(q);
    if(f > f_max) f_max = f;
    pdf->InsertNextValue( f );
  }

  for(double p=0.005; p<1.0; p+=0.005 ) {
    q = distribution->quantile(p);
    cdf->InsertNextValue(p);
    quantiles->InsertNextValue(q);
    double f = distribution->pdf(q);
    if(f > f_max) f_max = f;
    pdf->InsertNextValue( f );
  }

  q = distribution->quantile(0.999);
  if( boost::math::isfinite(q)) {
    cdf->InsertNextValue(0.999);
    quantiles->InsertNextValue(q);
    double f = distribution->pdf(q);
    if(f > f_max) f_max = f;
    pdf->InsertNextValue( f );
  }

  table_->AddColumn(quantiles);
  table_->AddColumn(pdf);  
  table_->AddColumn(cdf); 

  chart_->ClearPlots();
  vtkPlot* plot_cdf = chart_->AddPlot(vtkChartXY::LINE);
  vtkPlot* plot_pdf = chart_->AddPlot(vtkChartXY::STACKED);
  plot_cdf->GetYAxis()->SetTitle("cdf");
  plot_pdf->GetYAxis()->SetTitle("pdf");

  chart_->SetPlotCorner(plot_pdf, 0);
  chart_->SetPlotCorner(plot_cdf, 1);

  plot_pdf->SetInputData(table_, 0,1);
  plot_pdf->SetColor(150,150,120,100);
  plot_cdf->SetInputData(table_, 0,2);
  plot_cdf->SetWidth(2);


  plot_pdf->GetYAxis()->SetMaximumLimit(5e60);  //Need to remove the top max limit
  plot_pdf->GetYAxis()->SetMinimumLimit(0.00);
  plot_pdf->GetYAxis()->SetRange(0,f_max);
  plot_pdf->GetYAxis()->AutoScale();
  plot_pdf->GetYAxis()->SetMaximumLimit(plot_pdf->GetYAxis()->GetMaximum());
  
  chart_->GetAxis(vtkAxis::RIGHT)->SetMinimumLimit(0.0);
  chart_->GetAxis(vtkAxis::RIGHT)->SetMaximumLimit(1.0);
  chart_->GetAxis(vtkAxis::RIGHT)->SetRange(0.0,1.0);
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimumLimit(quantiles->GetValue(0));
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximumLimit(quantiles->GetValue(table_->GetNumberOfRows()-1));
  chart_->GetAxis(vtkAxis::BOTTOM)->SetRange(quantiles->GetValue(0),quantiles->GetValue(table_->GetNumberOfRows()-1));
  chart_->Update();
  qvtkWidget_->update();

  
}
