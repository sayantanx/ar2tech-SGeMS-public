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



#ifndef CHART_SCATTER_PLOT_H
#define CHART_SCATTER_PLOT_H

#include <GsTLAppli/charts/chart_base.h>
#include <GsTLAppli/charts/common.h>

#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>

#include <GsTLAppli/charts/scatter_plot_proxy_model.h>
#include <GsTLAppli/charts/chart_display_control.h>

#include <vtkMath.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>
#include <vtkQtTableView.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPlotPoints.h>
#include <vtkVariantArray.h>

#include <QColor>
#include <QToolBox>
#include <QTabWidget>
#include <QModelIndexList>
#include <QDropEvent>


class CHARTS_DECL Chart_scatter_plot : public Chart_base
{
  Q_OBJECT

public:
    Chart_scatter_plot(int nbins = 55,QWidget *parent=0);
    virtual ~Chart_scatter_plot();


public slots :

  void load_data(QModelIndex index_x, QModelIndexList indexes_y, QModelIndexList indexes_filters);
  void load_data(GsTL_object_item* item_x, std::vector<GsTL_object_item*> items_y, std::vector<GsTL_object_item*> items_filter);

  void add_data(Scatter_plot_item* prop_item);
  void update_data_display(Scatter_plot_item* item);

//  void add_data( GsTLGridProperty* prop, GsTLGridRegion* region=0, GsTLGridProperty* weigths=0);

  //Add the data to the model first which then send it to the chart
  /*
  void add_data( GsTLGridProperty* prop);
  void add_data( GsTLGridProperty* prop, GsTLGridWeightProperty* weigths);
  void add_data( GsTLGridProperty* prop, GsTLGridRegion* region);
  void add_data( GsTLGridPropertyGroup* group);
  void add_data( GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weigths);
  void add_data( GsTLGridPropertyGroup* group, GsTLGridRegion* region);
  */
  void remove_data( int id);

  void set_visibility(Scatter_plot_item* item);
  void set_color(Scatter_plot_item* item);
  void set_data_filter(Scatter_plot_item* item);
  //void set_data_display_style(Scatter_plot_item* item);
  void set_marker_style(Scatter_plot_item* item);
  void set_marker_size(Scatter_plot_item* item) ;

  void set_axis(float min_x, float min_y, float max_x, float max_y);
  void set_x_axis(float min_x, float max_x);
  void set_y_axis(float min_y, float max_y);
  void reset_axis();

  void set_x_axis_label(const QString& text);
  void set_y_axis_label(const QString& text);
  void set_title(const QString& text);
  void set_legend(bool);
  void set_grid(bool);
  void set_x_grid(bool);
  void set_y_grid(bool);

  void set_xaxis_min(double min);
  void set_xaxis_max(double max);
  void set_xaxis_precision(int digits);
  void set_xaxis_nticks(int nticks);
  void set_xaxis_logscale(bool on);
  void set_xaxis_autoscale();

  void set_yaxis_min(double min);
  void set_yaxis_max(double max);
  void set_yaxis_precision(int digits);
  void set_yaxis_nticks(int nticks);
  void set_yaxis_logscale(bool on);
  void set_yaxis_autoscale();

  void set_clipping_values(float min_x, float min_y, float max_x, float max_y);
  void set_x_clipping_values(float min_x, float max_x);
  void set_y_clipping_values(float min_y, float max_y);
  void reset_clipping_values();

  void set_x_axis_font_size(int size);
  void set_y_axis_font_size(int size);
  void set_x_label_font_size(int size);
  void set_y_label_font_size(int size);
  void set_legend_font_size(int size);
  void set_title_font_size(int size);

  void set_numbers_of_bins(int nbins);


//	virtual void dragEnterEvent(QDragEnterEvent *event);
//	virtual void dropEvent(QDropEvent *event);

private:

  enum CorelativeStats {  DATAX, DATAY, GRID, FILTER, N, MEANX, MEANY, VARIANCEX, VARIANCEY, COVAR, CORREL, REGINTERCEPT, REGSLOPE};


  struct Scatter_data {
    std::string name;
    std::string name_x;
    std::string name_y;
    std::string filter_name;
    GsTLGridProperty* prop_x;
    GsTLGridProperty* prop_y;
    GsTLGridWeightProperty* weight;
    GsTLGridRegion* region;
    GsTLGridCategoricalProperty* cprop;
    Geostat_grid* grid;
    float mean;
    float variance;
    float skewness;
    float median;
    float p10;
    float p90;
    float coef_correl;

    vtkSmartPointer<vtkVariantArray> correl_stats_array;
    vtkSmartPointer<vtkTable> scatter_table;
    vtkSmartPointer<vtkPlotPoints> plot_points;
    int id;
  };

  void initialize_data(Scatter_plot_property_item* item);
  void initialize_plot(Scatter_plot_item* item);

  std::string get_item_base_name(Scatter_data& data);

  void process_data(Scatter_data& data);
  void compute_stats(Scatter_data& data);
  void compute_stats_with_weights(Scatter_data& data);
  void remove_plot(vtkSmartPointer<vtkPlotPoints> plot);

  void intialize_stats_tables();
  void add_data_to_stats_table(Scatter_data& data);
  void remove_data_to_stats_table(Scatter_data& data);

  void initialize_default_colors();

//  void manage_plot_display(Scatter_data& data, QString display_style );

  void update_chart_display_control();

private:

  Scatter_plot_proxy_model* model_;


  float min_values_;
  float max_values_;

  //std::map<GsTLGridProperty*, histo_data> data_stats_;
  std::map<int, Scatter_data> data_stats_;

  vtkSmartPointer<vtkTable> scatter_table_;
  vtkSmartPointer<vtkQtTableView> correlative_stats_view_;

  vtkSmartPointer<vtkTable> correlative_stats_table_;

  //QToolBox* stats_view_;
  QTabWidget* information_view_;
  Chart_display_control* chart_control_;

  QList<QColor> default_colors_;
  int default_color_id_;
  int max_index_default_colors_;
    
};


class accumulate_pairs {
public:
  template<typename T>
  float operator()(float sum, const std::pair<T,T>& pair) {
    return sum + pair.first;
  }
};


#endif // CHART_CONTINUOUS_HISTOGRAM_H
