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



#ifndef CHART_DISPLAY_CONTROL_H
#define CHART_DISPLAY_CONTROL_H

#include <GsTLAppli/charts/common.h>
#include <QWidget>
#include <GsTLAppli/charts/GeneratedFiles/ui_chart_display_control.h>

class CHARTS_DECL Chart_display_control : public QWidget
{
    Q_OBJECT

public:
    Chart_display_control(QWidget *parent = 0);
    ~Chart_display_control();

    void send_axis_signals();

public slots:
  void set_xaxis_min(double min);
  void set_xaxis_max(double max);
  void set_xaxis_precision(int precision);
  void set_xaxis_nticks(int nticks);
  void set_xaxis_label(const QString& label);

  void set_yaxis_min(double min);
  void set_yaxis_max(double max);
  void set_yaxis_precision(int precision);
  void set_yaxis_nticks(int nticks);
  void set_yaxis_label(const QString& label);

  void set_title(const QString& label);

signals :
    void xaxis_label_changed(const QString& axis);
    void yaxis_label_changed(const QString& axis);
    void title_changed(const QString& axis);
    void legend_display_changed(bool on);
    void grid_display_changed(bool);
    void x_grid_display_changed(bool);
    void y_grid_display_changed(bool);

    void xaxis_min_changed(double);
    void xaxis_max_changed(double);
    void xaxis_precision_changed(int);
    void xaxis_nticks_changed(int);
    void xaxis_logscale_changed(bool);
    void xaxis_autoscale_changed();

    void yaxis_min_changed(double);
    void yaxis_max_changed(double);
    void yaxis_precision_changed(int);
    void yaxis_nticks_changed(int);
    void yaxis_logscale_changed(bool);
    void yaxis_autoscale_changed();

    void x_axis_font_size(int);
    void y_axis_font_size(int);
    void x_label_font_size(int);
    void y_label_font_size(int);
    void legend_font_size(int);
    void title_font_size(int);

private:
    Ui::Chart_display_control ui;

private slots:
  void forward_x_min_changed();
  void forward_x_max_changed();
  void forward_y_min_changed();
  void forward_y_max_changed();
};

#endif // CHART_DISPLAY_CONTROL_H
