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



#ifndef DISTRIBUTION_PARAMETER_WIDGET_H
#define DISTRIBUTION_PARAMETER_WIDGET_H

#include <QWidget>
#include <math/continuous_distribution.h>
#include <math/non_parametric_distribution.h>
#include <grid/grid_model/grid_property.h>
#include <grid/grid_model/grid_weight_property.h>
#include <grid/grid_model/grid_region.h>
#include <grid/grid_model/geostat_grid.h>

#include <qtplugins/common.h>
#include <qtplugins/selectors.h>
#include <qtplugins/weight_property_selector.h>
#include <qtplugins/non_param_cdf_input.h>

#include<QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QStringList>
#include <QGroupBox>
#include <QHash>


class QTPLUGINS_DECL Non_parameter_distribution_widget :public QGroupBox
  
{
  Q_OBJECT

public:
  Non_parameter_distribution_widget(QWidget* parent=0);
  virtual ~Non_parameter_distribution_widget();

  void set_distribution(Non_parametric_distribution* dist){dist_ = dist;}

signals :
  void distribution_ready();
  void clear_plot();

private slots:
  void check_distribution_parameters();

private :
  bool initialize_lower_tail();
  bool initialize_upper_tail();

  QStringList lti_parameters();
  QStringList uti_parameters();


private :
  PropertySelectorNoRegion* prop_selector_;
  SingleRegionSelector* region_selector_;
  SingleWeightPropertySelector* weight_selector_;

  LowerTailCdfInput* lower_tail_input_;
  UpperTailCdfInput* upper_tail_input_;

  Geostat_grid* grid_;  
  Grid_continuous_property* prop_;
  Grid_weight_property* weight_;
  Grid_region* region_;

  QString current_grid_name_;
  QString current_prop_name_;
  QString current_region_name_;
  QString current_weight_name_;
  QStringList current_lti_;
  QStringList current_uti_;

  QString current_filter_;

  Non_parametric_distribution* dist_;

};

class QTPLUGINS_DECL Numbered_parameter_distribution :public QGroupBox
{
  Q_OBJECT

public:
  Numbered_parameter_distribution(QWidget* parent=0):QGroupBox(parent){}
  virtual ~Numbered_parameter_distribution(){}
  
  virtual std::vector<float> parameters() const=0;
  virtual void initialize(const QStringList& param_names)=0;
  virtual void initialize(const std::vector<float>& params)=0;

signals:
  void distribution_ready();

private slots :
  virtual void check_distribution_parameters()=0;

private :

};


class QTPLUGINS_DECL One_parameter_continous_distribution : public Numbered_parameter_distribution
{
  Q_OBJECT
public:

  One_parameter_continous_distribution(QWidget* parent=0);
  One_parameter_continous_distribution(const QStringList& param_names, QWidget* parent=0);
  virtual ~One_parameter_continous_distribution();

  virtual std::vector<float> parameters() const;
  virtual void initialize(const QStringList& param_names);
  virtual void initialize(const std::vector<float>& params);

signals:
  void distribution_ready();

private slots :
  virtual void check_distribution_parameters();

protected:
  QLabel* param1_name_;
  QLineEdit* param1_value_;
};

class QTPLUGINS_DECL Two_parameter_continous_distribution : public Numbered_parameter_distribution
{
  Q_OBJECT
public:
  Two_parameter_continous_distribution( QWidget* parent=0);
  Two_parameter_continous_distribution(const QStringList& param_names, QWidget* parent=0);
  virtual ~Two_parameter_continous_distribution();

  virtual std::vector<float> parameters() const;
  virtual void initialize(const QStringList& param_names);
  virtual void initialize(const std::vector<float>& params);

signals:
  void distribution_ready();

private slots :
  virtual void check_distribution_parameters();

protected:
  QLabel* param1_name_;
  QLineEdit* param1_value_;
  QLabel* param2_name_;
  QLineEdit* param2_value_;
};

class QTPLUGINS_DECL Three_parameter_continous_distribution : public Numbered_parameter_distribution
{
  Q_OBJECT
public:
  Three_parameter_continous_distribution(QWidget* parent=0);
  Three_parameter_continous_distribution(const QStringList& param_names, QWidget* parent=0);
  virtual ~Three_parameter_continous_distribution();

  virtual std::vector<float> parameters() const;
  virtual void initialize(const QStringList& param_names);
  virtual void initialize(const std::vector<float>& params);

signals:
  void distribution_ready();

private slots :
  virtual void check_distribution_parameters();

protected:
  QLabel* param1_name_;
  QLineEdit* param1_value_;
  QLabel* param2_name_;
  QLineEdit* param2_value_;
  QLabel* param3_name_;
  QLineEdit* param3_value_;

};



class QTPLUGINS_DECL Continuous_distribution_parameter_widget : public QWidget
{
  Q_OBJECT

public:
    Continuous_distribution_parameter_widget(QWidget *parent);
    ~Continuous_distribution_parameter_widget();

    void are_parameters_editable(bool on);

    void initialize(Continuous_distribution* distribution);

    Continuous_distribution* get_distribution();

    QString name() {return name_edit_->text();}



signals :
    void distribution_changed(Continuous_distribution*);

private slots:
  void forward_distribution();

private:
  //Numbered_parameter_distribution* current_param_widget_;
  QWidget* current_param_widget_;
  Continuous_distribution* current_distribution_;

  QLineEdit* name_edit_;

  QHash<QString, Numbered_parameter_distribution*>  params_widgets_;

  Non_parameter_distribution_widget* non_param_widget_;


    
};



class QTPLUGINS_DECL Categorical_distribution_parameter_widget : public QWidget
{
  Q_OBJECT

public:
    Categorical_distribution_parameter_widget(QWidget *parent);
    ~Categorical_distribution_parameter_widget();

    //void initialize(Categorical_distribution* distribution);

private:
    
};


class QTPLUGINS_DECL Distribution_parameter_widget : public QWidget
{
  Q_OBJECT

public:
    Distribution_parameter_widget(QWidget *parent);
    ~Distribution_parameter_widget();

private:
    Categorical_distribution_parameter_widget* cont_params_;
    Categorical_distribution_parameter_widget* cat_params_;
};





#endif // DISTRIBUTION_PARAMETER_WIDGET_H
