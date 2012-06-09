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



#ifndef CHART_DISTRIBUTION_H
#define CHART_DISTRIBUTION_H

#include <GsTLAppli/math/continuous_distribution.h>
#include <QWidget>
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#include <vtkContextView.h>
#include <vtkTable.h>
#include <vtkPlot.h>

class Chart_distribution : public QWidget
{
  Q_OBJECT

public:
    Chart_distribution(QWidget *parent);
    ~Chart_distribution();

public slots:
    void set_distribution( Continuous_distribution* distribution );

private:
  QVTKWidget *qvtkWidget_;
	vtkSmartPointer<vtkChartXY> chart_;
	vtkSmartPointer<vtkContextView> context_view_;

  vtkSmartPointer<vtkPlot> plot_cdf_;
  vtkSmartPointer<vtkPlot> plot_pdf_;
  vtkSmartPointer<vtkTable> table_;
};

#endif // CHART_DISTRIBUTION_H
