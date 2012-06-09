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


#ifndef EDA_HISTOGRAMS_H
#define EDA_HISTOGRAMS_H

#include <GsTLAppli/gui/models/histogram_chart_model.h>
#include <GsTLAppli/gui/charts/chart_mdi_area.h>
#include <GsTLAppli/gui/common.h>

#include <QDialog>
#include <QMainWindow>
#include <QItemSelectionModel>

class GUI_DECL EDA_chart : public QWidget 
{
    Q_OBJECT
public:
  EDA_chart(Chart_mdi_area* mdi_area,QWidget *parent=0):QWidget(parent), mdi_area_(mdi_area){}
  virtual ~EDA_chart(){}

   

protected :
  Chart_mdi_area* mdi_area_;

};


class GUI_DECL EDA_chart_factory : public Named_interface 
{
public:
  EDA_chart_factory(){}
  virtual ~EDA_chart_factory(){}

  virtual QString title_name() const = 0;
  virtual QString tab_name() const=0;
  virtual EDA_chart* get_interface(Chart_mdi_area* mdi_area)=0;

};


class GUI_DECL EDA_continuous_histogram : public EDA_chart 
{
  Q_OBJECT

public:
    EDA_continuous_histogram(Chart_mdi_area* mdi_area, QWidget *parent=0);
    ~EDA_continuous_histogram(){}


  private slots:
  void show_continuous_histogram();

private :
  Continuous_histo_chart_model* cont_histo_chart_model_;
  QItemSelectionModel* cont_histo_selection_;

};

class GUI_DECL EDA_continuous_histogram_factory : public EDA_chart_factory 
{

public:

  static Named_interface* create_new_interface(std::string&) {
    return new EDA_continuous_histogram_factory;
  }

  EDA_continuous_histogram_factory(){}
  ~EDA_continuous_histogram_factory(){}

  virtual QString title_name() const {return "Histogram (Continuous)";}
  virtual QString tab_name() const{return "Statistics";}
  std::string name() const {return "ContinuousHistogram";}

  virtual EDA_chart* get_interface(Chart_mdi_area* mdi_area){return new EDA_continuous_histogram(mdi_area);}

};



class GUI_DECL EDA_categorical_histogram : public EDA_chart 
{
  Q_OBJECT

public:
    EDA_categorical_histogram(Chart_mdi_area* mdi_area, QWidget *parent=0);
    ~EDA_categorical_histogram(){}


  private slots:
  void show_categorical_histogram();

private :
  Categorical_histo_chart_model* cat_histo_chart_model_;
  QItemSelectionModel* cat_histo_selection_;

};

class GUI_DECL EDA_categorical_histogram_factory : public EDA_chart_factory 
{

public:

  static Named_interface* create_new_interface(std::string&) {
    return new EDA_categorical_histogram_factory;
  }

  EDA_categorical_histogram_factory(){}
  ~EDA_categorical_histogram_factory(){}

  virtual QString title_name() const {return "Histogram (Categorical)";}
  virtual QString tab_name() const{return "Statistics";}
  std::string name() const {return "CategoricalHistogram";}

  virtual EDA_chart* get_interface(Chart_mdi_area* mdi_area){return new EDA_categorical_histogram(mdi_area);}

};


#endif

