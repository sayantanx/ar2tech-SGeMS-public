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



#include <GsTLAppli/charts/chart_continuous_histogram.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/grid_region_temp_selector.h>
#include <GsTLAppli/qtplugins/color_delegate.h>
#include <GsTLAppli/charts/region_weight_delegate.h>
#include <GsTLAppli/charts/histogram_style_delegate.h>
#include <GsTLAppli/charts/histogram_item_tree_view.h>

#include <vtkDescriptiveStatistics.h> 
#include <vtkOrderStatistics.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkAxis.h>
#include <vtkColorSeries.h>
#include <vtkColor.h>
#include <vtkTextProperty.h>
#include <vtkChartLegend.h>
#include <QVTKInteractor.h>
#include <vtkPlotLine.h>

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QAbstractItemView>

Chart_continuous_histogram::Chart_continuous_histogram(int nbins, QWidget *parent)
  : nbins_(nbins), Chart_base(parent)
{
  this->setWindowTitle("Continuous Histogram");
  this->setWindowIcon(QIcon(":/icons/appli/Pixmaps/histo-bars.svg"));


    //Add the tree the control the selection
  QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);
	
  // Top part of the view
  QSplitter* chartSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
  Histogram_item_tree_view* tree = new Histogram_item_tree_view(this);
  model_ = new Histogram_proxy_model(this);
  tree->setModel(model_);

  Color_delegate* color_delegate = new Color_delegate (this);
  tree->setItemDelegateForColumn(2,color_delegate);

  Region_weight_delegate* region_weight_delegate = new Region_weight_delegate (this);
  tree->setItemDelegateForColumn(1,region_weight_delegate);

  Continuous_histo_style_delegate* chart_style_delegate = new Continuous_histo_style_delegate (this);
  tree->setItemDelegateForColumn(3,chart_style_delegate);

  chartSplitter->addWidget(tree);

  //Add the vtk rendering window
  qvtkWidget_ = new QVTKWidget(this);
  context_view_ = vtkSmartPointer<vtkContextView>::New();
  context_view_->SetInteractor(qvtkWidget_->GetInteractor());
  qvtkWidget_->SetRenderWindow(context_view_->GetRenderWindow());
	chart_ = vtkSmartPointer<vtkChartXY>::New();
	context_view_->GetScene()->AddItem(chart_); 

  chart_->GetAxis(0)->SetNumberOfTicks(10);
  chart_->GetAxis(1)->SetNumberOfTicks(10);

  chartSplitter->addWidget(qvtkWidget_);

  mainSplitter->addWidget(chartSplitter);


  //Add the vtk table view for the stats
//	mainSplitter->addWidget(tableView_->GetWidget());

  descriptive_stats_view_ = vtkSmartPointer<vtkQtTableView>::New();
  ordered_stats_view_ = vtkSmartPointer<vtkQtTableView>::New();

  information_view_ = new QTabWidget(mainSplitter);
  chart_control_ = new Chart_display_control(this);
  information_view_->addTab(chart_control_,"Chart controls");

  information_view_->addTab(descriptive_stats_view_->GetWidget(),"Descriptive statistics");
  information_view_->addTab(ordered_stats_view_->GetWidget(),"Ordered statistics");



  mainSplitter->addWidget(information_view_);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mainSplitter);
  this->setLayout(layout);
    
  histo_table_ = vtkSmartPointer<vtkTable>::New();

  descriptive_stats_table_ = vtkSmartPointer<vtkTable>::New();
  ordered_stats_table_ = vtkSmartPointer<vtkTable>::New();

  this->intialize_stats_tables();
  this->initialize_default_colors();

  descriptive_stats_view_->SetRepresentationFromInput(descriptive_stats_table_);
  ordered_stats_view_->SetRepresentationFromInput(ordered_stats_table_);

  connect( model_, SIGNAL(data_added(Histogram_item*)), this, SLOT(add_data(Histogram_item*)) );
  connect( model_, SIGNAL(data_removed(Histogram_item*)), this, SLOT(remove_data(Histogram_item*)) );
  connect( model_, SIGNAL(data_filter_changed(Histogram_item*)), this, SLOT(set_data_filter(Histogram_item*)) );
  connect( model_, SIGNAL(data_display_changed(Histogram_item*)), this, SLOT(update_data_display(Histogram_item*)) );
  connect( model_, SIGNAL(data_color_changed(Histogram_item*)), this, SLOT(set_color(Histogram_item*)) );
  connect( model_, SIGNAL(data_visibility_changed(Histogram_item*)), this, SLOT(set_visibility(Histogram_item*)) );
  connect( model_, SIGNAL(display_format_changed(Histogram_item*)), this, SLOT(set_data_display_style(Histogram_item*)) );

  connect( chart_control_, SIGNAL(xaxis_label_changed(const QString&)), this, SLOT(set_x_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(yaxis_label_changed(const QString&)), this, SLOT(set_y_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(title_changed(const QString&)), this, SLOT(set_title(const QString&)) );
  connect( chart_control_, SIGNAL(legend_display_changed(bool)), this, SLOT(set_legend(bool)) );
  connect( chart_control_, SIGNAL(grid_display_changed(bool)), this, SLOT(set_grid(bool)) );
  connect( chart_control_, SIGNAL(x_grid_display_changed(bool)), this, SLOT(set_x_grid(bool)) );
  connect( chart_control_, SIGNAL(y_grid_display_changed(bool)), this, SLOT(set_y_grid(bool)) );

  bool ok = connect( chart_control_, SIGNAL(x_axis_font_size(int)), this, SLOT(set_x_axis_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(y_axis_font_size(int)), this, SLOT(set_y_axis_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(x_label_font_size(int)), this, SLOT(set_x_label_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(y_label_font_size(int)), this, SLOT(set_y_label_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(legend_font_size(int)), this, SLOT(set_legend_font_size(int)) );
  ok = connect( chart_control_, SIGNAL(title_font_size(int)), this, SLOT(set_title_font_size(int)) );

  connect( chart_control_, SIGNAL(xaxis_min_changed(double)), this, SLOT(set_xaxis_min(double)) );
  connect( chart_control_, SIGNAL(xaxis_max_changed(double)), this, SLOT(set_xaxis_max(double)) );
  connect( chart_control_, SIGNAL(xaxis_precision_changed(int)), this, SLOT(set_xaxis_precision(int)) );
  connect( chart_control_, SIGNAL(xaxis_nticks_changed(int)), this, SLOT(set_xaxis_nticks(int)) );
  connect( chart_control_, SIGNAL(xaxis_logscale_changed(bool)), this, SLOT(set_xaxis_logscale(bool)) );
  connect( chart_control_, SIGNAL(xaxis_autoscale_changed()), this, SLOT(set_xaxis_autoscale()) );

  connect( chart_control_, SIGNAL(yaxis_min_changed(double)), this, SLOT(set_yaxis_min(double)) );
  connect( chart_control_, SIGNAL(yaxis_max_changed(double)), this, SLOT(set_yaxis_max(double)) );
  connect( chart_control_, SIGNAL(yaxis_precision_changed(int)), this, SLOT(set_yaxis_precision(int)) );
  connect( chart_control_, SIGNAL(yaxis_nticks_changed(int)), this, SLOT(set_yaxis_nticks(int)) );
  connect( chart_control_, SIGNAL(yaxis_logscale_changed(bool)), this, SLOT(set_yaxis_logscale(bool)) );
  connect( chart_control_, SIGNAL(yaxis_autoscale_changed()), this, SLOT(set_yaxis_autoscale()) );

  chart_control_->send_axis_signals();

  //QObject::connect( tree, SIGNAL(doubleClicked ( const QModelIndex&)), tree, SLOT(show_color_editor(const QModelIndex&)) );
}


Chart_continuous_histogram::~Chart_continuous_histogram()
{

}


void Chart_continuous_histogram::intialize_stats_tables(){

  vtkSmartPointer<vtkStringArray>  name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkStringArray>  grid_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkIntArray>  n_array = vtkSmartPointer<vtkIntArray>::New();
  vtkSmartPointer<vtkFloatArray>  mean_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  var_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  min_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  max_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  sum_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  skew_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  kurt_array = vtkSmartPointer<vtkFloatArray>::New();
  name_array->SetName("Data");
  grid_name_array->SetName("Grid");
  n_array->SetName("N");
  mean_array->SetName("Mean");
  var_array->SetName("Variance");
  min_array->SetName("Min");
  max_array->SetName("Max");
  sum_array->SetName("Sum");
  skew_array->SetName("Skewness");
  kurt_array->SetName("Kurtosis");
  descriptive_stats_table_->AddColumn(name_array);
  descriptive_stats_table_->AddColumn(grid_name_array);
  descriptive_stats_table_->AddColumn(n_array);
  descriptive_stats_table_->AddColumn(mean_array);
  descriptive_stats_table_->AddColumn(var_array);
  descriptive_stats_table_->AddColumn(min_array);
  descriptive_stats_table_->AddColumn(max_array);
  descriptive_stats_table_->AddColumn(sum_array);
  descriptive_stats_table_->AddColumn(skew_array);
  descriptive_stats_table_->AddColumn(kurt_array);

  vtkSmartPointer<vtkStringArray> p_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkStringArray> g_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkFloatArray>  p_min_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p_max_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p10_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p20_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p30_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p40_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p50_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p60_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p70_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p80_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  p90_array = vtkSmartPointer<vtkFloatArray>::New();
  p_name_array->SetName("Data");
  g_name_array->SetName("Grid");
  p_min_array->SetName("Min");
  p_max_array->SetName("Max");
  p10_array->SetName("P10");
  p20_array->SetName("P20");
  p30_array->SetName("P30");
  p40_array->SetName("P40");
  p50_array->SetName("Median");
  p60_array->SetName("P60");
  p70_array->SetName("P70");
  p80_array->SetName("P80");
  p90_array->SetName("P90");
  ordered_stats_table_->AddColumn(p_name_array);
  ordered_stats_table_->AddColumn(g_name_array);
  ordered_stats_table_->AddColumn(p_min_array);
  ordered_stats_table_->AddColumn(p10_array);
  ordered_stats_table_->AddColumn(p20_array);
  ordered_stats_table_->AddColumn(p30_array);
  ordered_stats_table_->AddColumn(p40_array);
  ordered_stats_table_->AddColumn(p50_array);
  ordered_stats_table_->AddColumn(p60_array);
  ordered_stats_table_->AddColumn(p70_array);
  ordered_stats_table_->AddColumn(p80_array);
  ordered_stats_table_->AddColumn(p90_array);
  ordered_stats_table_->AddColumn(p_max_array);

}

void Chart_continuous_histogram::initialize_default_colors(){


  vtkSmartPointer<vtkColorSeries> colors = vtkSmartPointer<vtkColorSeries>::New();
  colors->SetColorScheme(vtkColorSeries::WILD_FLOWER);
  for( int i=0; i<colors->GetNumberOfColors(); ++i ) {
    vtkColor3ub c3ub	= colors->GetColor (i);
    const unsigned char r = c3ub.GetRed();
    const unsigned char g = c3ub.GetGreen();
    const unsigned char b = c3ub.GetBlue();

    default_colors_.append(QColor(int(r),int(g),int(b)));
  }

  
  colors->SetColorScheme(vtkColorSeries::CITRUS);
  for( int i=0; i<colors->GetNumberOfColors(); ++i ) {
    vtkColor3ub c3ub	= colors->GetColor (i);
    const unsigned char r = c3ub.GetRed();
    const unsigned char g = c3ub.GetGreen();
    const unsigned char b = c3ub.GetBlue();

    default_colors_.append(QColor((int)r,int(g),int(b)));
  }

  colors->SetColorScheme(vtkColorSeries::SPECTRUM);

  for( int i=0; i<colors->GetNumberOfColors(); ++i ) {
    vtkColor3ub c3ub	= colors->GetColor (i);
    const unsigned char r = c3ub.GetRed();
    const unsigned char g = c3ub.GetGreen();
    const unsigned char b = c3ub.GetBlue();

    default_colors_.append(QColor((int)r,int(g),int(b)));
  }

  default_color_id_ = 0;
  max_index_default_colors_ = default_colors_.size()-1;
  /*
  default_colors_.append(QColor(141,211,199));
  default_colors_.append(QColor(255, 255, 179));
  default_colors_.append(QColor(190, 186, 218));
  default_colors_.append(QColor(251, 128, 114));
  default_colors_.append(QColor(128, 177, 211));
  default_colors_.append(QColor(253, 180, 98));
  default_colors_.append(QColor(179, 222, 105));
  default_colors_.append(QColor(252, 205, 229));
  default_colors_.append(QColor(217, 217, 217));
  default_colors_.append(QColor(188, 128, 189));
  default_colors_.append(QColor(204, 235, 197));
  default_colors_.append(QColor(255, 237, 111));

  default_color_id_ = 0;
  max_index_default_colors_ = default_colors_.size()-1;
  */

}


void Chart_continuous_histogram::load_data(QModelIndexList indexes){
      //Check if a region has been selected for a specific grid

  std::map<GsTL_object_item*,GsTLGridRegion*> grid_to_region;

  typedef std::multimap<GsTL_object_item*,GsTLGridWeightProperty*> map_grid_to_weights;
  typedef std::multimap<GsTL_object_item*,GsTLGridWeightProperty*>::iterator map_iterator_grid_to_weights;
  map_grid_to_weights grid_to_weights;
  QModelIndex index;
  foreach(index, indexes) {
//  for(int i=0; i<indexes.size(); ++i) {
  //  QModelIndex index =     indexes.at(i);
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    if(item->item_type() == "Region") {
      GsTLGridRegion* region = static_cast<GsTLGridRegion*>(index.internalPointer());
      grid_to_region[region->parent()->parent()] = region;
    }
    else if(item->item_type() == "WeightsProperty") {
      GsTLGridWeightProperty* weigth = static_cast<GsTLGridWeightProperty*>(index.internalPointer());
      grid_to_weights.insert(std::make_pair(weigth->parent()->parent(),weigth));
    }
  }


//  Send the data to the chart
//  foreach(index, indexes) {
  for(int i=0; i<indexes.size(); ++i) {
    QModelIndex index = indexes.at(i);
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    //if(item->item_type() == "ContinuousProperty" || item->item_type() == "WeightsProperty") {
    if(item->item_type() == "ContinuousProperty" ) {
      GsTLGridProperty* prop = static_cast<GsTLGridProperty*>(item->data_pointer());
      GsTL_object_item* parent_grid = prop->parent()->parent();
  
      std::map<GsTL_object_item*,GsTLGridRegion*>::iterator it = grid_to_region.find(parent_grid);
      GsTLGridRegion* region = 0;
      if(it != grid_to_region.end()) region = it->second;
      model_->insert_row(prop,region, default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;

      if(grid_to_weights.count(parent_grid) != 0) {
        //load one set of property per weights
        std::pair<map_iterator_grid_to_weights,map_iterator_grid_to_weights> it_pair = grid_to_weights.equal_range(parent_grid);
        for( ; it_pair.first != it_pair.second; ++ it_pair.first ) {

          model_->insert_row(prop,it_pair.first->second, default_colors_.at(default_color_id_%max_index_default_colors_));
          default_color_id_++;

        }

      }


      //this->add_data(prop,region);
    }
    else if(item->item_type().contains("Group:")) {
      GsTLGridPropertyGroup* group = static_cast<GsTLGridPropertyGroup*>(index.internalPointer());
      std::map<GsTL_object_item*,GsTLGridRegion*>::iterator it = grid_to_region.find(group->parent());
      GsTLGridRegion* region = 0;
      if(it != grid_to_region.end()) region = it->second;
      model_->insert_row(group,region,default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;
    }
    else if(item->item_type() == "ContinuousDistribution") {
      Continuous_distribution* dist = static_cast<Continuous_distribution*>(index.internalPointer());
      model_->insert_row(dist,default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;
    }
  }
}

void Chart_continuous_histogram::add_data( GsTLGridProperty* prop){
  model_->insert_row(prop, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( GsTLGridProperty* prop, GsTLGridWeightProperty* weigths){
  model_->insert_row(prop,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( GsTLGridProperty* prop, GsTLGridRegion* region){
  model_->insert_row(prop,region, default_colors_.at(default_color_id_%max_index_default_colors_) );
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( GsTLGridPropertyGroup* group){
  model_->insert_row(group, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weigths){
  model_->insert_row(group,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( GsTLGridPropertyGroup* group, GsTLGridRegion* region){
  model_->insert_row(group,region, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data( Continuous_distribution* dist){
  model_->insert_row(dist, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_continuous_histogram::add_data(Histogram_item* item){
    if(item->type() == "Property") {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(item);
      this->initialize_data(prop_item);
      this->initialize_plot(prop_item);

      std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
      std::cout<<"set color and style"<<std::endl;
      it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_line->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());

    }
    else if(item->type() == "Group") {
      Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
        this->add_data(prop_item);
      }
    }
    else if(item->type() == "ContinuousDistribution") {
      Histogram_distribution_item* dist_item = dynamic_cast<Histogram_distribution_item*>(item);
      this->initialize_data(dist_item);
      this->initialize_plot(dist_item);

      std::map<int, histo_data>::iterator it = data_stats_.find(dist_item->id());
      it->second.plot_bar->SetColor( dist_item->color().red(),dist_item->color().green(),dist_item->color().blue(), dist_item->color().alpha());
      it->second.plot_line->SetColor( dist_item->color().red(),dist_item->color().green(),dist_item->color().blue(), dist_item->color().alpha());

    }
}



void Chart_continuous_histogram::update_data_display(Histogram_item* item){
    if(item->type() == "Property" || item->type() == "ContinuousDistribution") {
      //Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(item);

      std::map<int, histo_data>::iterator it = data_stats_.find(item->id());
      it->second.plot_bar->SetColor( item->color().red(),item->color().green(),item->color().blue(), item->color().alpha());
      it->second.plot_line->SetColor( item->color().red(),item->color().green(),item->color().blue(), item->color().alpha());
      
      if( !item->is_visible() ) {
        it->second.plot_bar->SetVisible(false);
        it->second.plot_line->SetVisible(false);
      }
      else {
        this->manage_plot_display(it->second, item->display_format());
      }
    }
    else if(item->type() == "Group") {
      Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
        this->update_data_display(prop_item);

      }
    }
    qvtkWidget_->update();
}

void Chart_continuous_histogram::initialize_data(Histogram_distribution_item* item){

  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  histo_data data;
  data.name = item->item_name().toStdString();
  data.order_table = vtkSmartPointer<vtkTable>::New();
  data.histo_table = vtkSmartPointer<vtkTable>::New();
  data.histo_line_table = vtkSmartPointer<vtkTable>::New();

  data.desc_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  data.desc_stats_array->SetName("Descriptive Statistics");
  data.desc_stats_array->SetNumberOfValues(9);

  data.ord_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  data.ord_stats_array->SetName("Ordered Statistics");
  data.ord_stats_array->SetNumberOfValues(12);

  if(it == data_stats_.end()) {
    data.id = -1;
    data_stats_.insert(std::make_pair(item->id(),data));
    it = data_stats_.find(item->id());
  }

  this->compute_stats_from_distribution(it->second);


  this->add_data_to_stats_table(it->second);

}


void Chart_continuous_histogram::initialize_data(Histogram_property_item* item){
  
  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  histo_data data;
  data.name = item->prop()->name();
  data.grid = static_cast<Geostat_grid*>(item->prop()->parent()->parent());
  data.order_table = vtkSmartPointer<vtkTable>::New();
  data.histo_table = vtkSmartPointer<vtkTable>::New();
  data.histo_line_table = vtkSmartPointer<vtkTable>::New();

  data.desc_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  data.desc_stats_array->SetName("Descriptive Statistics");
  data.desc_stats_array->SetNumberOfValues(10);

  data.ord_stats_array = vtkSmartPointer<vtkVariantArray>::New();
  data.ord_stats_array->SetName("Ordered Statistics");
  data.ord_stats_array->SetNumberOfValues(13);

  if(it == data_stats_.end()) {
    data.prop = item->prop();
    data.region = item->region();
    data.weight =item->weights();
    data.id = -1;
    data_stats_.insert(std::make_pair(item->id(),data));
    it = data_stats_.find(item->id());
  }
  else {  // Assume that we need to recompute
    it->second.region = item->region();
    it->second.weight = item->weights();
  }

  //Find how to compute the stats
  if( it->second.weight ==0 ) {
    if(data.region !=0 ) {
      it->second.name += " ("+data.region->name()+")";
    }
    this->compute_stats(it->second);
  }
  else {
    it->second.name += " ("+data.weight->name()+")";
    this->compute_stats_with_weights(it->second);
  }

  this->add_data_to_stats_table(it->second);

  //TODO: Need to add the line format and color
 // data_stats_[item->id()] = data;
}

void Chart_continuous_histogram::initialize_plot(Histogram_item* item){

  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  it->second.plot_bar = chart_->AddPlot(vtkChart::BAR); // LINE, POINTS, BAR, STACKED
  it->second.plot_line = chart_->AddPlot(vtkChart::LINE);
  it->second.plot_bar->GetYAxis()->SetMinimumLimit(0.00);
  it->second.plot_line->GetYAxis()->SetMinimumLimit(0.00);
  it->second.plot_bar->GetYAxis()->SetMaximumLimit(1.00);
  it->second.plot_line->GetYAxis()->SetMaximumLimit(1.00);

//  this->manage_plot_display(it->second, item->display_format());

  it->second.plot_bar->SetInputData(it->second.histo_table, 0, 2);
  it->second.plot_line->SetInputData(it->second.histo_line_table, 0, 1);
  QColor color = default_colors_.at( default_color_id_%max_index_default_colors_ );
  it->second.plot_bar->SetColor(color.redF(), color.greenF(), color.blueF());
  it->second.plot_line->SetColor(color.redF(), color.greenF(), color.blueF());
  it->second.plot_bar->Update();
  it->second.plot_line->Update();

  this->update_chart_display_control();
}

void Chart_continuous_histogram::remove_data( int id){

  std::map<int, histo_data>::iterator it = data_stats_.find(id);

  chart_->RemovePlotInstance(it->second.plot_bar);
  chart_->RemovePlotInstance(it->second.plot_line);
  histo_table_->RemoveColumnByName (it->second.name.c_str());
  data_stats_.erase(it);
}


void Chart_continuous_histogram::process_data(histo_data& data){

}


void Chart_continuous_histogram::compute_stats(histo_data& data){

  vtkSmartPointer<vtkFloatArray> x = vtkSmartPointer<vtkFloatArray>::New();

  
  GsTLGridProperty* prop = data.prop;
  
  bool need_memory_swap = !prop->is_in_memory();
  if(need_memory_swap) {
    prop->swap_to_memory();
  }
  
  int ndata=0;
  // always build the array manually since vtk 
  // output nan if nay data is a nan

  if(data.region == 0) { 
 //   x->SetArray(prop->data(),prop->size(),1);
    GsTLGridProperty::const_iterator it = data.prop->begin();
    for( ; it != data.prop->end(); ++it, ++ndata) {}
    x->Allocate(ndata);
    for(it = data.prop->begin() ; it != prop->end(); ++it) {
      x->InsertNextValue(*it);
    }
  }
  else {
    Temporary_propRegion_Selector temp_region(data.region, prop);
    GsTLGridProperty::const_iterator it = prop->begin();
    for( ; it != data.prop->end(); ++it, ++ndata) {}
    x->Allocate(ndata);
    for(it = prop->begin() ; it != prop->end(); ++it) {
      x->InsertNextValue(*it);
    }
  }


  x->SetName(prop->name().c_str());

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New(); 
  table->AddColumn(x);

  //Compute statistics

  vtkSmartPointer<vtkDescriptiveStatistics> descriptive_stats  = vtkSmartPointer<vtkDescriptiveStatistics>::New();
  descriptive_stats->SetInputData(0, table );
  descriptive_stats->AddColumn( prop->name().c_str() );
  descriptive_stats->SetColumnStatus( prop->name().c_str(), 1 );
  descriptive_stats->RequestSelectedColumns();
  descriptive_stats->SetDeriveOption(true);
  descriptive_stats->SetAssessOption( false );
  descriptive_stats->Update();

  vtkSmartPointer<vtkMultiBlockDataSet> mblock_descriptive = vtkMultiBlockDataSet::SafeDownCast( descriptive_stats->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  vtkSmartPointer<vtkTable> dtable_primary = vtkTable::SafeDownCast( mblock_descriptive->GetBlock( 0 ) );
  vtkSmartPointer<vtkTable> dtable_derived = vtkTable::SafeDownCast( mblock_descriptive->GetBlock( 1 ) );


  data.desc_stats_array->SetValue(DATA, data.name.c_str());
  data.desc_stats_array->SetValue(GRID, data.prop->parent()->parent()->item_name().toStdString().c_str());
//  data.desc_stats_array->SetValue(FILTER, data.name.c_str());
  data.desc_stats_array->SetValue(N, vtkVariant(ndata));
  data.desc_stats_array->SetValue(MEAN, dtable_primary->GetValueByName(0,"Mean"));
  data.desc_stats_array->SetValue(VARIANCE, dtable_derived->GetValueByName(0,"Variance"));
  data.desc_stats_array->SetValue(MIN, dtable_primary->GetValueByName(0,"Minimum"));
  data.desc_stats_array->SetValue(MAX, dtable_primary->GetValueByName(0,"Maximum"));
  data.desc_stats_array->SetValue(SUM, data.desc_stats_array->GetValue(MEAN).ToFloat()*ndata);
  data.desc_stats_array->SetValue(SKEWNESS, dtable_derived->GetValueByName(0,"Skewness"));
  data.desc_stats_array->SetValue(KURTOSIS, dtable_derived->GetValueByName(0,"Kurtosis"));

  std::cout<<"Computing Order Statistics"<<std::endl;
  vtkSmartPointer<vtkOrderStatistics> decile_stats = vtkSmartPointer<vtkOrderStatistics>::New();
  decile_stats->SetInputData( 0, table );
  decile_stats->AddColumn( prop->name().c_str() );
  decile_stats->SetColumnStatus( prop->name().c_str(), 1 );
  decile_stats->RequestSelectedColumns();
  decile_stats->SetQuantileDefinition( vtkOrderStatistics::InverseCDFAveragedSteps );
  decile_stats->SetNumberOfIntervals(10);
//  decile_stats->SetNumberOfIntervals(nbins_);
//  decile_stats->SetMaximumHistogramSize(nbins_);
  decile_stats->SetQuantize(false);
  decile_stats->SetDeriveOption(true);
  decile_stats->SetAssessOption( false );
  decile_stats->Update();

  std::cout<<"Retrieving block"<<std::endl;
  vtkSmartPointer<vtkMultiBlockDataSet> mblock_ordered = vtkMultiBlockDataSet::SafeDownCast( decile_stats->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  unsigned nbq = mblock_ordered->GetNumberOfBlocks() - 1;

  vtkSmartPointer<vtkTable> ordered_stats_table = vtkTable::SafeDownCast( mblock_ordered->GetBlock( nbq ) );
  vtkAbstractArray* quantiles = ordered_stats_table->GetColumn(1);
  data.ord_stats_array->SetValue(0, data.name.c_str());
  data.ord_stats_array->SetValue(1, data.grid->item_name().toStdString().c_str());
  for(int i=0;i<11;++i) {
    data.ord_stats_array->SetValue(i+2, quantiles->GetVariantValue(i));
  }

  // Compute the histogram with n bins
  vtkSmartPointer<vtkFloatArray> histo_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmid = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmean = vtkSmartPointer<vtkFloatArray>::New();
  histo_p->SetName(data.name.c_str());
  histo_vmid->SetName("Mid binning");
  histo_vmean->SetName("Mean binning");
  histo_p->SetNumberOfValues(nbins_);
  histo_vmid->SetNumberOfValues(nbins_);
  histo_vmean->SetNumberOfValues(nbins_);

  //Find the max for each range
  float  min = data.desc_stats_array->GetValue(MIN).ToFloat();
  float  max = data.desc_stats_array->GetValue(MAX).ToFloat();
  float range= max-min;
  float bin_width = range/nbins_;

  std::cout<<"Computing histo"<<std::endl;
  vtkTable* full_histo = vtkTable::SafeDownCast( mblock_ordered->GetBlock( nbq - 2 ));
  vtkDoubleArray* d = vtkDoubleArray::SafeDownCast(full_histo->GetColumn(0));
  vtkDoubleArray* p = vtkDoubleArray::SafeDownCast( full_histo->GetColumn(2));
  int histo_bin_id=0;
  float bin_value=0;
  float bin_p=0;
  int count=0;
  for(int i=0; i<d->GetNumberOfTuples(); ++i) {
    float val = d->GetValue(i);
    if( val <= min+bin_width  ) {
      bin_value += val;
      bin_p += p->GetValue(i);
      count++;
    } else {  //Reached the next bin
      histo_p->SetValue( histo_bin_id, bin_p );
      histo_vmid->SetValue( histo_bin_id, min+bin_width/2 );
      histo_vmean->SetValue( histo_bin_id, bin_value/count );
  

      //Initialize the variables for the next bin
      bin_value = val;
      bin_p = p->GetValue(i);
      count=1;
      //Find the next bin
      while( val > min+bin_width ) {
        histo_bin_id++;
        min+=bin_width;
        histo_p->SetValue(histo_bin_id,0);
        histo_vmid->SetValue( histo_bin_id, min+bin_width/2 );
        histo_vmean->SetValue( histo_bin_id, min+static_cast<float>(bin_width)/2 );
      }

    }
  
  }
  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(histo_vmid);
  histo_table->AddColumn(histo_vmean);
  histo_table->AddColumn(histo_p);


  //Set the table for the line plot
  // same as histo_p but with min and max prepended and appended
  vtkSmartPointer<vtkFloatArray> histo_line_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_line_bin = vtkSmartPointer<vtkFloatArray>::New();
  histo_line_p->SetName(data.name.c_str());
  histo_line_bin->SetName("Mean binning");
  histo_line_p->SetNumberOfValues(nbins_+2);
  histo_line_bin->SetNumberOfValues(nbins_+2);
  histo_line_p->SetValue(0,0);
  histo_line_bin->SetValue(0,data.desc_stats_array->GetValue(MIN).ToFloat());
  histo_line_p->SetValue(nbins_+1,0);
  histo_line_bin->SetValue(nbins_+1,data.desc_stats_array->GetValue(MAX).ToFloat());
  for(int i=0; i<nbins_; ++i) {
    histo_line_p->SetValue(i+1,histo_p->GetValue(i));
    histo_line_bin->SetValue(i+1,histo_vmean->GetValue(i));
  }
  vtkSmartPointer<vtkTable> histo_line_table =  vtkSmartPointer<vtkTable>::New();
  histo_line_table->AddColumn(histo_line_bin);
  histo_line_table->AddColumn(histo_line_p);


  data.histo_table->ShallowCopy(histo_table);
  data.histo_line_table->ShallowCopy(histo_line_table);


  if(need_memory_swap) {
    prop->swap_to_disk();
  }
}


void Chart_continuous_histogram::compute_stats_with_weights(histo_data& data){
  

  //Compute the first pass
  // Mean, min, max

  std::vector< std::pair<float, float> > data_weights;

  GsTLGridProperty* prop = data.prop;
  bool need_memory_swap = !prop->is_in_memory();
  if(need_memory_swap) {
    prop->swap_to_memory();
  }


  GsTLGridWeightProperty* weights = data.weight;
  if(weights == 0) {
    this->compute_stats(data);
    return;
  }
  float mean = 0.0;
  float min = 9e20; 
  float max = -9e20;
  float sum=0;
  int n=0;
  for(int i=0; i<prop->size(); ++i) {
    if( !weights->is_informed(i) || !prop->is_informed(i) ) continue;
    float v = prop->get_value(i);
    float w = weights->get_value(i);
    mean += w*v;
    sum += v;
    if(v<min) min = v;
    if(v>max) max = v;
    n++;
    data_weights.push_back( std::make_pair(v,w) );
  }
  if(need_memory_swap) {
    prop->swap_to_disk();
  }

  //Second pass compute
  double var = 0.0;
  double skew = 0.0; 
  double kurt = 0.0;
  for(int i=0; i<prop->size(); ++i) {
    if( !weights->is_informed(i) || !prop->is_informed(i) ) continue;
    float v = prop->get_value(i);
    float res = v - mean;
    float w = weights->get_value(i);
    var += w*res*res;
    skew += w*res*res*res;
    kurt += w*res*res*res*res;
  }
  skew /= std::pow((double)var,1.5);
  kurt = kurt/std::pow((double)var,2) -3;

  data.desc_stats_array->SetValue(DATA, data.name.c_str());
  data.desc_stats_array->SetValue(GRID, data.grid->item_name().toStdString().c_str());
  data.desc_stats_array->SetValue(N, vtkVariant(n));
  data.desc_stats_array->SetValue(MEAN, mean);
  data.desc_stats_array->SetValue(VARIANCE, var);
  data.desc_stats_array->SetValue(MIN, min);
  data.desc_stats_array->SetValue(MAX, max);
  data.desc_stats_array->SetValue(SUM, sum);
  data.desc_stats_array->SetValue(SKEWNESS, skew);
  data.desc_stats_array->SetValue(KURTOSIS, kurt);


  // Compute the order stats
  std::sort(data_weights.begin(), data_weights.end());
  //Loop over to compute the cumulative weight
  float w_cumul = 0;
  std::vector<float> cweights;
  cweights.reserve(data_weights.size());
  std::vector< std::pair<float, float> >::iterator it =  data_weights.begin();
  for( ; it != data_weights.end(); ++it ) {
//    it->second += w_cumul;
//    w_cumul = it->second;
    w_cumul += it->second;
    cweights.push_back(w_cumul );
  }

  //Find the decile
  vtkSmartPointer<vtkTable> ordered_table = vtkSmartPointer<vtkTable>::New();
  data.ord_stats_array->SetValue(DATA, data.name.c_str());
  data.ord_stats_array->SetValue(GRID, data.grid->item_name().toStdString().c_str());
  int ip = 3;
  for(float p = 0.1; p<1.0; p+=0.1, ++ip) {
    std::vector<float>::iterator it = std::lower_bound(cweights.begin(),cweights.end(), p );
    int i = std::distance(cweights.begin(), it);
    float low_v = (i==0) ?  min : data_weights[i-1].first;
    float up_v = (it==cweights.end()) ?  max : data_weights[i].first;
    float low_p = (i==0) ?  0.0 : cweights[i-1];
    float up_p = (it==cweights.end()) ?  1.0 : cweights[i];
    float perc = (p-low_p)/(up_p - low_p)*(up_v-low_v) + low_v;
    
    data.ord_stats_array->SetValue(ip,perc);
  }
  data.ord_stats_array->SetValue(2,min);
  data.ord_stats_array->SetValue(12,max);

  // Compute the histogram with n bins
  vtkSmartPointer<vtkFloatArray> histo_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmid = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmean = vtkSmartPointer<vtkFloatArray>::New();
  histo_p->SetName(data.name.c_str());
  histo_vmid->SetName("Mid binning");
  histo_vmean->SetName("Mean binning");
  histo_p->SetNumberOfValues(nbins_);
  histo_vmid->SetNumberOfValues(nbins_);
  histo_vmean->SetNumberOfValues(nbins_);

  //Find the max for each range
  float range= max-min;
  float bin_width = range/nbins_;

  //Get the first bin

  std::vector<std::pair<float,float> >::iterator low_it_cw = data_weights.begin();
  std::vector<std::pair<float,float> >::iterator up_it_cw;
  int low_index = 0;
  int up_index;
  
  for( int b = 0; b<nbins_; ++b ) {
    float lower_v = (float)(b+1)*bin_width + min;
    //up_it_cw = std::lower_bound(data_weights.begin(),data_weights.end(), std::make_pair(lower_v,(float)0.0));
    up_it_cw = std::lower_bound(low_it_cw,data_weights.end(), std::make_pair(lower_v,(float)0.0));

    //The itertator is one pass the last item in the bin
    up_index = std::distance(data_weights.begin(),up_it_cw);
    up_index = up_index <= low_index ? low_index : up_index-1;
    histo_p->SetValue( b, (up_index==0) ? 0 : cweights[up_index]-cweights[low_index] );
    histo_vmid->SetValue( b, (float)(b+0.5)*bin_width + min );
    int n_v = std::distance(low_it_cw,up_it_cw);
    if(n_v == 0)
      histo_vmean->SetValue( b, lower_v );
    else
      histo_vmean->SetValue( b, std::accumulate(low_it_cw,up_it_cw, 0.0, accumulate_pairs() )/n_v );

    low_it_cw = up_it_cw;
    low_index = up_index;
  }
  
  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(histo_vmid);
  histo_table->AddColumn(histo_vmean);
  histo_table->AddColumn(histo_p);


  //Set the table for the line plot
  // same as histo_p but with min and max prepended and appended
  vtkSmartPointer<vtkFloatArray> histo_line_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_line_bin = vtkSmartPointer<vtkFloatArray>::New();
  histo_line_p->SetName(data.name.c_str());
  histo_line_bin->SetName("Mean binning");
  histo_line_p->SetNumberOfValues(nbins_+2);
  histo_line_bin->SetNumberOfValues(nbins_+2);
  histo_line_p->SetValue(0,0);
  histo_line_bin->SetValue(0,min);
  histo_line_p->SetValue(nbins_+1,0);
  histo_line_bin->SetValue(nbins_+1,max);
  for(int i=0; i<nbins_; ++i) {
    histo_line_p->SetValue(i+1,histo_p->GetValue(i));
    histo_line_bin->SetValue(i+1,histo_vmean->GetValue(i));
  }
  vtkSmartPointer<vtkTable> histo_line_table =  vtkSmartPointer<vtkTable>::New();
  histo_line_table->AddColumn(histo_line_bin);
  histo_line_table->AddColumn(histo_line_p);


  data.histo_table->ShallowCopy(histo_table);
  data.histo_line_table->ShallowCopy(histo_line_table);

}


void Chart_continuous_histogram::compute_stats_from_distribution(histo_data& data){

  float min = data.dist->quantile(0.0000001);
  float max = data.dist->quantile(0.9999999);

  data.desc_stats_array->SetValue(DATA, data.name.c_str());
  data.desc_stats_array->SetValue(N, vtkVariant(""));
  data.desc_stats_array->SetValue(MEAN, data.dist->mean());
  data.desc_stats_array->SetValue(VARIANCE, data.dist->variance());
  data.desc_stats_array->SetValue(MIN, min);
  data.desc_stats_array->SetValue(MAX, max);
  data.desc_stats_array->SetValue(SUM, vtkVariant(""));
  data.desc_stats_array->SetValue(SKEWNESS, data.dist->skewness());
  data.desc_stats_array->SetValue(KURTOSIS, data.dist->kurtosis());


  int ip = 2;
  for(float p = 0.1; p<1.0; p+=0.1, ++ip) {
    data.ord_stats_array->SetValue(ip,data.dist->quantile(p));
  }
  data.ord_stats_array->SetValue(0, data.name.c_str());
  data.ord_stats_array->SetValue(1,min);
  data.ord_stats_array->SetValue(11,max);


// Get the array for the histogram plotting
  vtkSmartPointer<vtkFloatArray> histo_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmid = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_vmean = vtkSmartPointer<vtkFloatArray>::New();
  histo_p->SetName(data.name.c_str());
  histo_vmid->SetName("Mid binning");
  histo_vmean->SetName("Mean binning");  

  vtkSmartPointer<vtkFloatArray> histo_line_p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> histo_line_bin = vtkSmartPointer<vtkFloatArray>::New();
  histo_line_p->SetName(data.name.c_str());
  histo_line_bin->SetName("Mean binning");

  for(double p=0.001; p<1.0; p+=0.001) {
    double q = data.dist->quantile(p);
    histo_vmid->InsertNextValue(q);
    histo_vmean->InsertNextValue(q);
    histo_p->InsertNextValue(p);

    histo_line_bin->InsertNextValue(q);
    histo_line_p->InsertNextValue(p);
  }



  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(histo_vmid);
  histo_table->AddColumn(histo_vmean);
  histo_table->AddColumn(histo_p);

  vtkSmartPointer<vtkTable> histo_line_table =  vtkSmartPointer<vtkTable>::New();
  histo_line_table->AddColumn(histo_line_bin);
  histo_line_table->AddColumn(histo_line_p);

  data.histo_table->ShallowCopy(histo_table);
  data.histo_line_table->ShallowCopy(histo_line_table);
}

void Chart_continuous_histogram::add_data_to_stats_table(histo_data& data){

  //check if the entry already exist in the table
  int irow = data.id; 
  bool create_row = false;
  if(data.id == -1) {
      irow = descriptive_stats_table_->InsertNextBlankRow();
      data.id= irow;
      create_row = true;
  }
  descriptive_stats_table_->SetRow(irow, data.desc_stats_array);
  descriptive_stats_table_->Modified();


  //check if the entry already exist in the table
  if(create_row) {
      ordered_stats_table_->InsertNextBlankRow();
  }
  ordered_stats_table_->SetRow(irow, data.ord_stats_array);
  ordered_stats_table_->Modified();

  // Does not update by iteslf;  have to reset the Representation, something strange here
  descriptive_stats_view_->SetRepresentationFromInput(descriptive_stats_table_);
  descriptive_stats_view_->Update();
  QAbstractItemView* desc_view = dynamic_cast<QAbstractItemView*>(descriptive_stats_view_->GetWidget());
  desc_view->reset();
  
  ordered_stats_view_->SetRepresentationFromInput(ordered_stats_table_);
  ordered_stats_view_->Update();
  ordered_stats_view_->GetWidget()->update();


}


void Chart_continuous_histogram::remove_data_to_stats_table(histo_data& data){

}



void Chart_continuous_histogram::set_visibility( Histogram_item* item){
  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_bar->SetVisible(prop_item->is_visible());
    it->second.plot_line->SetVisible(prop_item->is_visible());

  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      this->set_visibility(prop_item);
    }
  }
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_color( Histogram_item* item){
  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
    it->second.plot_line->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
    it->second.plot_bar->Update();
    it->second.plot_line->Update();
  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
      if( it == data_stats_.end() ) return;
      it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_line->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_bar->Update();
      it->second.plot_line->Update();
    }
  }
}

void Chart_continuous_histogram::set_data_filter(Histogram_item* item){

  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    bool changed = false;
    if( it->second.region != item->region() ) {
      it->second.region = item->region();
      changed = true;
    }
    if( it->second.weight != item->weights() ) {
      it->second.weight = item->weights();
      changed = true;
    }
    // Get the stats recomputed
    if(changed) {
    if(it->second.weight==0) {
      if(it->second.region == 0) {
        it->second.name = it->second.prop->name();
      } else {
        it->second.name = it->second.prop->name() + " ("+it->second.region->name()+")";
      }
      this->compute_stats(it->second);
    }
    else {  // has weights
      it->second.name = it->second.prop->name() + " ("+it->second.weight->name()+")";
      this->compute_stats_with_weights(it->second);

    }
    this->add_data_to_stats_table(it->second);

    it->second.plot_bar->SetInputData(it->second.histo_table, 1, 2);
    it->second.plot_bar->Update();
    it->second.plot_line->SetInputData(it->second.histo_line_table, 0, 1);
    it->second.plot_line->Update();
    qvtkWidget_->update();

    }

  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      this->set_data_filter(prop_item);
    }
  }

}

void Chart_continuous_histogram::set_data_display_style(Histogram_item* item){

  if(item->type() == "Property") {
    Histogram_property_item* prop_item = dynamic_cast< Histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    this->manage_plot_display(it->second, prop_item->display_format());

  }
  else if(item->type() == "Group") {
    Histogram_group_item* group_item = dynamic_cast<Histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Histogram_property_item* prop_item = dynamic_cast<Histogram_property_item*>(group_item->children(j));
      this->set_data_display_style(prop_item);
    }
  }

}

void Chart_continuous_histogram::manage_plot_display(histo_data& data, QString display_style ){
  bool changed = false;
  if(display_style == "Bars") {
    data.plot_line->SetVisible(false);
    data.plot_bar->SetVisible(true);
    changed = true;
  }
  else if(display_style == "Lines") {
    data.plot_line->SetVisible(true);
    data.plot_bar->SetVisible(false);
    changed = true;
  }
  else if(display_style == "Bars and Lines") {
    data.plot_line->SetVisible(true);
    data.plot_bar->SetVisible(true);
    changed = true;
  }
  if(changed) {
    this->update_chart_display_control();
    qvtkWidget_->update();
  }
}

void Chart_continuous_histogram::update_chart_display_control(){
  vtkAxis* xaxis = chart_->GetAxis(vtkAxis::BOTTOM);
  chart_control_->set_xaxis_min(xaxis->GetMinimum());
  chart_control_->set_xaxis_max(xaxis->GetMaximum());
  chart_control_->set_xaxis_precision(xaxis->GetPrecision());
  chart_control_->set_xaxis_nticks(xaxis->GetNumberOfTicks());

  vtkAxis* yaxis = chart_->GetAxis(vtkAxis::LEFT);
  chart_control_->set_yaxis_min(yaxis->GetMinimum());
  chart_control_->set_yaxis_max(yaxis->GetMaximum());
  chart_control_->set_yaxis_precision(yaxis->GetPrecision());
  chart_control_->set_yaxis_nticks(yaxis->GetNumberOfTicks());
}


void Chart_continuous_histogram::set_x_axis_label(const QString& text){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetTitle(text.toStdString());
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_y_axis_label(const QString& text){
  chart_->GetAxis(vtkAxis::LEFT)->SetTitle(text.toStdString());
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_title(const QString& text){
  chart_->SetTitle(text.toStdString().c_str());
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_legend(bool on){
  chart_->SetShowLegend(on);
  qvtkWidget_->update();
}

void Chart_continuous_histogram::set_grid(bool on){
  chart_->GetAxis(0)->SetGridVisible(on);
  chart_->GetAxis(1)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_continuous_histogram::set_x_grid(bool on){
  chart_->GetAxis(0)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_continuous_histogram::set_y_grid(bool on){
  chart_->GetAxis(1)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_continuous_histogram::set_xaxis_min(double min){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(min);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_xaxis_max(double max){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(max);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_xaxis_precision(int digits){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetPrecision(digits);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_xaxis_nticks(int nticks){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetNumberOfTicks(nticks);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_xaxis_logscale(bool on){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetLogScale(on);
  qvtkWidget_->update();
  this->update_chart_display_control();
}
void Chart_continuous_histogram::set_xaxis_autoscale(){
  chart_->GetAxis(vtkAxis::BOTTOM)->AutoScale();
  chart_->Update();
  qvtkWidget_->update();
  this->update_chart_display_control();
}


void Chart_continuous_histogram::set_yaxis_min(double min){
  chart_->GetAxis(vtkAxis::LEFT)->SetMinimum(min);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_yaxis_max(double max){
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(max);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_yaxis_precision(int digits){
  chart_->GetAxis(vtkAxis::LEFT)->SetPrecision(digits);
  chart_->Update();
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_yaxis_nticks(int nticks){
  chart_->GetAxis(vtkAxis::LEFT)->SetNumberOfTicks(nticks);
  chart_->Update();
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_yaxis_logscale(bool on){
  chart_->GetAxis(vtkAxis::LEFT)->SetLogScale(on);
  chart_->Update();
  qvtkWidget_->update();
  this->update_chart_display_control();
}
void Chart_continuous_histogram::set_yaxis_autoscale(){
  chart_->GetAxis(vtkAxis::LEFT)->AutoScale();
  chart_->Update();
  qvtkWidget_->update();
  this->update_chart_display_control();
}

void Chart_continuous_histogram::set_x_axis_font_size(int size){
  chart_->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_y_axis_font_size(int size){
  chart_->GetAxis(vtkAxis::LEFT)->GetLabelProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_x_label_font_size(int size){
  chart_->GetAxis(vtkAxis::BOTTOM)->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_y_label_font_size(int size){
  chart_->GetAxis(vtkAxis::LEFT)->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_legend_font_size(int size){
  chart_->GetLegend()->SetLabelSize(size);
  qvtkWidget_->update();
}
void Chart_continuous_histogram::set_title_font_size(int size){
  chart_->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}




void Chart_continuous_histogram::set_axis(float min_x, float min_y, float max_x, float max_y){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(min_x);
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(max_x);
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(min_y);
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(max_y);
  qvtkWidget_->update();
}

void Chart_continuous_histogram::set_x_axis(float min_x, float max_x){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(min_x);
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(max_x);
  qvtkWidget_->update();
}

void Chart_continuous_histogram::set_y_axis(float min_y, float max_y){
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(min_y);
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(max_y);
  qvtkWidget_->update();
}

void Chart_continuous_histogram::reset_axis(){

}

void Chart_continuous_histogram::set_clipping_values(float min_x, float min_y, float max_x, float max_y){

}

void Chart_continuous_histogram::set_x_clipping_values(float min_x, float max_x){

}

void Chart_continuous_histogram::set_y_clipping_values(float min_y, float max_y){

}

void Chart_continuous_histogram::reset_clipping_values(){

}

void Chart_continuous_histogram::set_numbers_of_bins(int nbins){

}


void Chart_continuous_histogram::dragEnterEvent(QDragEnterEvent *event)
{

	if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		event->acceptProposedAction();
	}
}

void Chart_continuous_histogram::dropEvent(QDropEvent *event)
{
	QByteArray encoded = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);

	QStringList objectNames;
	while (!stream.atEnd())
	{
	  int row, col;
	  QMap<int,  QVariant> roleDataMap;
	  stream >> row >> col >> roleDataMap;

    QString name = roleDataMap.value(0).toString();
    QString type = roleDataMap.value(1).toString();

    Geostat_grid* grid = 0;
    if(type != "ContinuousDistribution") {
      QString grid_name = roleDataMap.value(2).toString();
      QString s4 = roleDataMap.value(3).toString();

      grid = dynamic_cast<Geostat_grid*>( 
		              Root::instance()->interface( 
                  gridModels_manager + "/" + grid_name.toStdString()
					        ).raw_ptr()
		  );
      if(grid == 0) continue;
    }

    

    if(type == "ContinuousProperty" || type == "WeightProperty" ) {
      this->add_data( grid->property(name.toStdString()) );
    }
    else if(type.startsWith("Group:")) {
      this->add_data( grid->get_group(name.toStdString()) );
    }
    else if(type == "ContinuousDistribution") {
      Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>( 
		              Root::instance()->interface( 
                  continuous_distributions_manager + "/" + name.toStdString()
					        ).raw_ptr());
      if(dist == 0) continue;
      this->add_data(  dist );
    }
	}
  
}
