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


#ifndef SCATTER_PLOT_CREATOR_H
#define SCATTER_PLOT_CREATOR_H


#include <GsTLAppli/charts/chart_creator.h>
#include <GsTLAppli/charts/scatter_plot_proxy_model.h>
#include <GsTLAppli/charts/chart_mdi_area.h>
#include <GsTLAppli/charts/common.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/group_selectors.h>

#include <QDialog>
#include <QMainWindow>
#include <QItemSelectionModel>

class CHARTS_DECL Scatter_plot_creator : public Chart_creator 
{
  Q_OBJECT

public:
    Scatter_plot_creator(Chart_mdi_area* mdi_area, QWidget *parent=0);
    ~Scatter_plot_creator(){}


  private slots:
  void show_scatter_plot();

private :

  GridSelector* grid_selector_;
  SinglePropertySelector *x_prop_selector_;

  OrderedPropertySelector* y_props_selector_;
  MultiPropertyGroupSelector* y_groups_selector_;


};

class CHARTS_DECL Scatter_plot_creator_factory : public Chart_creator_factory 
{

public:

  static Named_interface* create_new_interface(std::string&) {
    return new Scatter_plot_creator_factory;
  }

  Scatter_plot_creator_factory(){}
  ~Scatter_plot_creator_factory(){}

  virtual QString title_name() const {return "Scatter plot (Continuous)";}
  virtual QString tab_name() const{return "Statistics";}
  std::string name() const {return "Scatter-plot";}

  virtual Chart_creator* get_interface(Chart_mdi_area* mdi_area){return new Scatter_plot_creator(mdi_area);}

};



#endif

