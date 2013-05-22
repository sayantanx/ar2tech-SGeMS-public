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



#ifndef CHART_BASE_H
#define CHART_BASE_H

#include <charts/common.h>

#include <QVTKWidget.h>
#include <vtkFloatArray.h>
#include <vtkMath.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

#include <vtkQtTableView.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkChartXY.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>

#include <QMdiSubWindow>
#include <QString>
#include <QStringList>

#include <QFrame>
#include <QWidget>

class CHARTS_DECL Chart_base : public QFrame
{
  Q_OBJECT

public:
    Chart_base(QWidget *parent);
    virtual ~Chart_base();

    vtkSmartPointer<vtkChartXY> chart(){return chart_;}


protected:
  QVTKWidget *qvtkWidget_;
	vtkSmartPointer<vtkChartXY> chart_;
	vtkSmartPointer<vtkContextView> context_view_;

  //Data_analysis_control_widget
//	vtkSmartPointer<vtkTable> table_;
//	vtkSmartPointer<vtkQtTableView> tableView_;
};

#endif // CHART_BASE_H
