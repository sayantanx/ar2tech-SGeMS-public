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


#ifndef CHART_CATEGORICAL_HISTOGRAM_H
#define CHART_CATEGORICAL_HISTOGRAM_H

#include <charts/common.h>
#include <math/categorical_distribution.h>
#include <charts/chart_base.h>
#include <grid/grid_model/grid_property.h>
#include <grid/grid_model/grid_categorical_property.h>
#include <grid/grid_model/grid_weight_property.h>
#include <grid/grid_model/grid_region.h>
#include <charts/categorical_histogram_proxy_model.h>


#include <QObject>
#include <QTabWidget>

#include <vtkPlotBar.h>

class CHARTS_DECL Chart_categorical_histogram : public Chart_base
{
  Q_OBJECT

public:
    Chart_categorical_histogram(QWidget *parent=0);
    virtual ~Chart_categorical_histogram();

public slots :

  void load_data(QModelIndexList indexes);

  void add_data(Categorical_histogram_item* prop_item);
  void update_data_display(Categorical_histogram_item* item);

//  void add_data( Grid_continuous_property* prop, Grid_region* region=0, Grid_continuous_property* weigths=0);

  //Add the data to the model first which then send it to the chart
  void add_data( Grid_categorical_property* prop);
  void add_data( Grid_categorical_property* prop, Grid_weight_property* weigths);
  void add_data( Grid_categorical_property* prop, Grid_region* region);
  void add_data( GsTLGridPropertyGroup* group);
  void add_data( GsTLGridPropertyGroup* group, Grid_weight_property* weigths);
  void add_data( GsTLGridPropertyGroup* group, Grid_region* region);
  void add_data( Categorical_distribution* dist);
  void remove_data( int id);

  void set_visibility(Categorical_histogram_item* item);
  void set_color(Categorical_histogram_item* item);
  void set_data_filter(Categorical_histogram_item* item);


private:
    
  struct histo_data {
    std::string name;
    Grid_categorical_property* prop;
    Grid_weight_property* weight;
    Grid_region* region;
    vtkSmartPointer<vtkTable> histo_table;
    vtkSmartPointer<vtkPlot> plot_bar;
    int id;
  };

  void compute_stats(histo_data& data);
  void compute_stats_with_weights(histo_data& data);
  void compute_stats_from_distribution(histo_data& data);

  void initialize_data(Categorical_histogram_property_item* item);
  void initialize_data(Categorical_histogram_distribution_item* item);
  void initialize_plot(Categorical_histogram_item* item);
  
  void initialize_default_colors();

  void remove_plot(vtkSmartPointer<vtkPlot> plot);

  void intialize_stats_tables();
  void add_data_to_stats_table(histo_data& data);
  void remove_data_to_stats_table(histo_data& data);
  void update_chart_display_control();

  void initialize_proportion_table();
  void initialize_stacked_chart();


private slots:
    void set_x_axis_label();
    void manage_plot_display( histo_data& data );

private :

  QVTKWidget *qvtkWidget_stacked_;
	vtkSmartPointer<vtkChartXY> chart_stacked_;
	vtkSmartPointer<vtkContextView> context_view_stacked_;


  vtkPlotBar* stacked_bar_;

  int ncat_in_plot_;
  CategoricalPropertyDefinition* current_cdef_;

  vtkSmartPointer<vtkTable> histo_table_;

  int ncat_;
  vtkSmartPointer<vtkTable> proportion_table_;
  std::vector<std::string> cprop_names_;


  std::map<int, histo_data> data_stats_;

  Categorical_histogram_proxy_model* model_;
  QTabWidget* information_view_;

  QList<QColor> default_colors_;
  int default_color_id_;
  int max_index_default_colors_;
};

#endif // CHART_CATEGORICAL_HISTOGRAM_H
