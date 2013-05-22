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



#ifndef CHART_OPTION_CONTROLS_H
#define CHART_OPTION_CONTROLS_H

#include <charts/common.h>

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QFrame>
#include <QSpinBox>

class CHARTS_DECL Chart_option_controls : public QWidget
{
  Q_OBJECT

public:
    Chart_option_controls(QWidget *parent);
    ~Chart_option_controls();

    void set_xaxis(const QString& text);
    void set_yaxis(const QString& text);
    void set_title(const QString& text);
    void set_legend_display(bool on);

signals :
    void xaxis_changed(const QString& axis);
    void yaxis_changed(const QString& axis);
    void title_changed(const QString& axis);
    void legend_display_changed(bool on);

    void xaxis_min_changed(const QString& axis);
    void xaxis_max_changed(const QString& axis);
    void yaxis_min_changed(const QString& axis);
    void yaxis_max_changed(const QString& axis);
    void xaxis_nticks_changed(int);
    void yaxis_nticks_changed(int);
    void xaxis_precision_changed(int);
    void yaxis_precision_changed(int);

private:

  QFrame* labels_control_;
 
  QLineEdit* x_line_edit_;
  QLineEdit* y_line_edit_;
  QLineEdit* title_line_edit_;
  QCheckBox* legend_check_;

  QCheckBox* auto_scale_check_;

  QFrame* xaxis_control_;
  QLineEdit* x_min_;
  QLineEdit* x_max_;
  QSpinBox* x_nticks_;
  QSpinBox* x_precision_;


};

#endif // CHART_OPTION_CONTROLS_H
