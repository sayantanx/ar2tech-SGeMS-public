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


#ifndef EDA_CONTINUOUS_HISTOGRAMS_H
#define EDA_CONTINUOUS_HISTOGRAMS_H


#include <charts/chart_creator.h>
#include <charts/histogram_chart_model.h>
#include <charts/chart_mdi_area.h>
#include <charts/common.h>

#include <QDialog>
#include <QMainWindow>
#include <QItemSelectionModel>

class CHARTS_DECL Continuous_histogram_creator : public Chart_creator 
{
  Q_OBJECT

public:
    Continuous_histogram_creator(Chart_mdi_area* mdi_area, QWidget *parent=0);
    ~Continuous_histogram_creator(){}


  private slots:
  void show_continuous_histogram();

private :
  Continuous_histo_chart_model* cont_histo_chart_model_;
  QItemSelectionModel* cont_histo_selection_;

};

class CHARTS_DECL Continuous_histogram_creator_factory : public Chart_creator_factory 
{

public:

  static Named_interface* create_new_interface(std::string&) {
    return new Continuous_histogram_creator_factory;
  }

  Continuous_histogram_creator_factory(){}
  ~Continuous_histogram_creator_factory(){}

  virtual QString title_name() const {return "Histogram (Continuous)";}
  virtual QString tab_name() const{return "Statistics";}
  std::string name() const {return "ContinuousHistogram";}

  virtual Chart_creator* get_interface(Chart_mdi_area* mdi_area){return new Continuous_histogram_creator(mdi_area);}

};



#endif

