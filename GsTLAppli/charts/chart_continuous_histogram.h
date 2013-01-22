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



#ifndef CHART_CONTINUOUS_HISTOGRAM_H
#define CHART_CONTINUOUS_HISTOGRAM_H

#include <GsTLAppli/charts/chart_base.h>
#include <GsTLAppli/charts/common.h>

#include <GsTLAppli/math/continuous_distribution.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/grid/grid_model/grid_region.h>
#include <GsTLAppli/math/continuous_distribution.h>

#include <GsTLAppli/charts/histogram_proxy_model.h>
#include <GsTLAppli/charts/chart_display_control.h>

#include <vtkMath.h>
#include <vtkTable.h>
#include <vtkStringArray.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>
#include <vtkQtTableView.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkPlotBar.h>
#include <vtkPlotLine.h>
#include <vtkVariantArray.h>

#include <QColor>
#include <QToolBox>
#include <QTabWidget>
#include <QModelIndexList>
#include <QDropEvent>


class CHARTS_DECL Chart_continuous_histogram : public Chart_base
{
  Q_OBJECT

public:
    Chart_continuous_histogram(int nbins = 55,QWidget *parent=0);
    virtual ~Chart_continuous_histogram();


public slots :

  void load_data(QModelIndexList indexes);

  void add_data(Histogram_item* prop_item);
  void update_data_display(Histogram_item* item);

//  void add_data( GsTLGridProperty* prop, GsTLGridRegion* region=0, GsTLGridProperty* weigths=0);

  //Add the data to the model first which then send it to the chart
  void add_data( GsTLGridProperty* prop);
  void add_data( GsTLGridProperty* prop, GsTLGridWeightProperty* weigths);
  void add_data( GsTLGridProperty* prop, GsTLGridRegion* region);
  void add_data( GsTLGridPropertyGroup* group);
  void add_data( GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weigths);
  void add_data( GsTLGridPropertyGroup* group, GsTLGridRegion* region);
  void add_data( Continuous_distribution* dist);
  void remove_data( int id);

  void set_visibility(Histogram_item* item);
  void set_color(Histogram_item* item);
  void set_data_filter(Histogram_item* item);
  void set_data_display_style(Histogram_item* item);

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


	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

private:

  enum DescriptiveStats {  DATA, GRID, N, MEAN, VARIANCE, MIN, MAX, SUM, SKEWNESS, KURTOSIS };


  struct histo_data {
    std::string name;
    GsTLGridProperty* prop;
    GsTLGridWeightProperty* weight;
    GsTLGridRegion* region;
    Continuous_distribution* dist;
    Geostat_grid* grid;
    float mean;
    float variance;
    float skewness;
    float median;
    float p10;
    float p90;
    //vtkTable* descriptive_table;
    //vtkTable* order_table;
    //vtkPlot* plot;
    vtkSmartPointer<vtkVariantArray> desc_stats_array;
    vtkSmartPointer<vtkVariantArray> ord_stats_array;
    vtkSmartPointer<vtkTable> order_table;
    vtkSmartPointer<vtkTable> histo_table;
    vtkSmartPointer<vtkTable> histo_line_table;
//    vtkSmartPointer<vtkTable> oder_cardinality_table;
    vtkSmartPointer<vtkPlot> plot_bar;
    vtkSmartPointer<vtkPlot> plot_line;
    int id;
  };

  void initialize_data(Histogram_property_item* item);
  void initialize_data(Histogram_distribution_item* item);
  void initialize_plot(Histogram_item* item);


  void process_data(histo_data& data);
  void compute_stats(histo_data& data);
  void compute_stats_with_weights(histo_data& data);
  void compute_stats_from_distribution(histo_data& data);
  void remove_plot(vtkSmartPointer<vtkPlot> plot);

  void intialize_stats_tables();
  void add_data_to_stats_table(histo_data& data);
  void remove_data_to_stats_table(histo_data& data);

  void initialize_default_colors();

  void manage_plot_display(histo_data& data, QString display_style );

  void update_chart_display_control();

private:

  Histogram_proxy_model* model_;

  int nbins_;

  float min_values_;
  float max_values_;

  //std::map<GsTLGridProperty*, histo_data> data_stats_;
  std::map<int, histo_data> data_stats_;

  vtkSmartPointer<vtkTable> histo_table_;
  vtkSmartPointer<vtkQtTableView> descriptive_stats_view_;
  vtkSmartPointer<vtkQtTableView> ordered_stats_view_;

  vtkSmartPointer<vtkTable> descriptive_stats_table_;
  vtkSmartPointer<vtkTable> ordered_stats_table_;

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
