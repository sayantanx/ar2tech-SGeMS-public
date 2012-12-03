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



#include <GsTLAppli/charts/chart_scatter_plot.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/grid_region_temp_selector.h>
#include <GsTLAppli/extra/qtplugins/color_delegate.h>
#include <GsTLAppli/charts/region_weight_delegate.h>
#include <GsTLAppli/charts/scatter_plot_point_style_delegate.h>
#include <GsTLAppli/charts/scatter_plot_item_tree_view.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>

#include <vtkCorrelativeStatistics.h> 
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkAxis.h>
#include <vtkColorSeries.h>
#include <vtkColor.h>
#include <vtkTextProperty.h>
#include <vtkChartLegend.h>

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QAbstractItemView>

Chart_scatter_plot::Chart_scatter_plot(int nbins, QWidget *parent)
  :  Chart_base(parent)
{
  this->setWindowTitle("Scatter plot");
  this->setWindowIcon(QIcon(":/icons/appli/Pixmaps/histo-bars.svg"));


    //Add the tree the control the selection
  QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);
	
  // Top part of the view
  QSplitter* chartSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
  Scatter_plot_item_tree_view* tree = new Scatter_plot_item_tree_view(this);
  model_ = new Scatter_plot_proxy_model(this);
  tree->setModel(model_);
  tree->setExpandsOnDoubleClick(false);

  Color_delegate* color_delegate = new Color_delegate (this);
  tree->setItemDelegateForColumn(3,color_delegate);

  Region_weight_delegate* region_weight_delegate = new Region_weight_delegate (this);
  tree->setItemDelegateForColumn(2,region_weight_delegate);

  Scatter_plot_point_style_delegate* chart_style_delegate = new Scatter_plot_point_style_delegate (this);
  tree->setItemDelegateForColumn(4,chart_style_delegate);

  chartSplitter->addWidget(tree);

  //Add the vtk rendering window
  qvtkWidget_ = new QVTKWidget(this);
	context_view_ = vtkSmartPointer<vtkContextView>::New();
	context_view_->SetInteractor(qvtkWidget_->GetInteractor());
	qvtkWidget_->SetRenderWindow(context_view_->GetRenderWindow());
	chart_ = vtkSmartPointer<vtkChartXY>::New();
	context_view_->GetScene()->AddItem(chart_);  
  chartSplitter->addWidget(qvtkWidget_);

  mainSplitter->addWidget(chartSplitter);


  //Add the vtk table view for the stats
//	mainSplitter->addWidget(tableView_->GetWidget());

  correlative_stats_view_ = vtkSmartPointer<vtkQtTableView>::New();

  information_view_ = new QTabWidget(mainSplitter);
  chart_control_ = new Chart_display_control(this);
  information_view_->addTab(chart_control_,"Chart controls");

  information_view_->addTab(correlative_stats_view_->GetWidget(),"Statistics");

  mainSplitter->addWidget(information_view_);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mainSplitter);
  this->setLayout(layout);
    
  scatter_table_ = vtkSmartPointer<vtkTable>::New();
  correlative_stats_table_ = vtkSmartPointer<vtkTable>::New();
  this->intialize_stats_tables();
  this->initialize_default_colors();

  correlative_stats_view_->SetRepresentationFromInput(correlative_stats_table_);

  connect( model_, SIGNAL(data_added(Scatter_plot_item*)), this, SLOT(add_data(Scatter_plot_item*)) );
  connect( model_, SIGNAL(data_removed(Scatter_plot_item*)), this, SLOT(remove_data(Scatter_plot_item*)) );
  connect( model_, SIGNAL(data_filter_changed(Scatter_plot_item*)), this, SLOT(set_data_filter(Scatter_plot_item*)) );
  connect( model_, SIGNAL(data_display_changed(Scatter_plot_item*)), this, SLOT(update_data_display(Scatter_plot_item*)) );
  connect( model_, SIGNAL(data_color_changed(Scatter_plot_item*)), this, SLOT(set_color(Scatter_plot_item*)) );
  connect( model_, SIGNAL(data_visibility_changed(Scatter_plot_item*)), this, SLOT(set_visibility(Scatter_plot_item*)) );
  connect( model_, SIGNAL(data_marker_style_changed(Scatter_plot_item*)), this, SLOT(set_marker_style(Scatter_plot_item*)) );
  connect( model_, SIGNAL(data_marker_size_changed(Scatter_plot_item*)), this, SLOT(set_marker_size(Scatter_plot_item*)) );
 // connect( model_, SIGNAL(display_format_changed(Scatter_plot_item*)), this, SLOT(set_data_display_style(Scatter_plot_item*)) );

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


Chart_scatter_plot::~Chart_scatter_plot()
{

}


void Chart_scatter_plot::intialize_stats_tables(){

  vtkSmartPointer<vtkStringArray>  name_x_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkStringArray>  name_y_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkStringArray>  grid_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkStringArray>  filter_name_array = vtkSmartPointer<vtkStringArray>::New();
  vtkSmartPointer<vtkIntArray>  n_array = vtkSmartPointer<vtkIntArray>::New();
  vtkSmartPointer<vtkFloatArray>  mean_x_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  mean_y_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  var_x_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  var_y_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  covar_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  correl_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  reg_slope_array = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray>  reg_intersect_array = vtkSmartPointer<vtkFloatArray>::New();

  name_x_array->SetName("X");
  name_y_array->SetName("Y");
  grid_name_array->SetName("Grid");
  filter_name_array->SetName("Filter");
  n_array->SetName("N");
  mean_x_array->SetName("Mean X");
  mean_y_array->SetName("Mean Y");
  var_x_array->SetName("Variance X");
  var_y_array->SetName("Variance Y");
  covar_array->SetName("Covariance");
  correl_array->SetName("Correlation Coef");
  reg_slope_array->SetName("Slope Y/X");
  reg_intersect_array->SetName("Intercept Y/X");

  correlative_stats_table_->AddColumn(name_x_array);
  correlative_stats_table_->AddColumn(name_y_array);
  correlative_stats_table_->AddColumn(grid_name_array);
  correlative_stats_table_->AddColumn(filter_name_array);
  correlative_stats_table_->AddColumn(n_array);
  correlative_stats_table_->AddColumn(mean_x_array);
  correlative_stats_table_->AddColumn(mean_y_array);
  correlative_stats_table_->AddColumn(var_x_array);
  correlative_stats_table_->AddColumn(var_y_array);
  correlative_stats_table_->AddColumn(covar_array);
  correlative_stats_table_->AddColumn(correl_array);
  correlative_stats_table_->AddColumn(reg_intersect_array);
  correlative_stats_table_->AddColumn(reg_slope_array);
  
}

void Chart_scatter_plot::initialize_default_colors(){


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

void Chart_scatter_plot::load_data(GsTL_object_item* item_x, std::vector<GsTL_object_item*> items_y, std::vector<GsTL_object_item*> items_filter){
  
  //All data are wihtin the same grid
  if( item_x == 0 || items_y.empty()) return;

  // Check if some filters has been selected
  GsTLGridProperty* prop_x = dynamic_cast<GsTLGridProperty*>(item_x);
  if( !prop_x ) return;


  for( int i=0; i<items_y.size(); ++i ) {
    GsTL_object_item* item_y = items_y[i];
    if(item_y->item_type() == "ContinuousProperty") {
	  GsTLGridProperty* prop_y = static_cast<GsTLGridProperty*>(item_y->data_pointer());
      if(prop_y == prop_x) continue;
      std::vector<GsTL_object_item*>::iterator it = items_filter.begin();
      for( ; it != items_filter.end(); ++it) {
        if( (*it) == 0) {
          model_->insert_row(prop_x,prop_y, 
                             default_colors_.at(default_color_id_%max_index_default_colors_) );
          ++default_color_id_;
        }
        else if( (*it)->item_type() == "GsTLGridRegion" ) {
          model_->insert_row(prop_x,prop_y,dynamic_cast<GsTLGridRegion*>(*it), 
                             default_colors_.at(default_color_id_%max_index_default_colors_) );
          ++default_color_id_;
        }
        else if( (*it)->item_type() == "CategoricalProperty" ) {
          model_->insert_row(prop_x,prop_y,dynamic_cast<GsTLGridCategoricalProperty*>(*it), 
                             default_colors_.at(default_color_id_%max_index_default_colors_) );
          ++default_color_id_;
        }
      }


    }
    else if(item_y->item_type().startsWith("Group") ) {
      GsTLGridPropertyGroup* group_y = dynamic_cast<GsTLGridPropertyGroup*>(item_y);
      std::vector<GsTL_object_item*>::iterator it = items_filter.begin();
      for( ; it != items_filter.end(); ++it) {
        if( (*it) == 0) {
          model_->insert_row(prop_x,group_y, 
                             default_colors_.at(default_color_id_%max_index_default_colors_) );
          ++default_color_id_;
        }
        else if( (*it)->item_type() == "GsTLGridRegion" ) {
          model_->insert_row(prop_x,group_y,dynamic_cast<GsTLGridRegion*>(*it), 
                             default_colors_.at(default_color_id_%max_index_default_colors_) );
          ++default_color_id_;
        }
        else if( (*it)->item_type() == "CategoricalProperty" ) {
          model_->insert_row(prop_x,group_y,dynamic_cast<GsTLGridCategoricalProperty*>(*it), 
                             default_colors_.at(default_color_id_%max_index_default_colors_) );
          ++default_color_id_;
        }
      }

    }

  }  // for each y index

}


void Chart_scatter_plot::load_data(QModelIndex index_x, QModelIndexList indexes_y, QModelIndexList indexes_filters){
  
  //All data are wihtin the same grid
  if( !index_x.isValid() || indexes_y.isEmpty()) return;

  // Check if some filters has been selected
  GsTL_object_item* item_x = static_cast<GsTL_object_item*>(index_x.internalPointer());

  std::vector<GsTL_object_item*> items_y;
  QModelIndex index;
  foreach(index, indexes_filters) {
    GsTL_object_item* item_y = static_cast<GsTL_object_item*>(index.internalPointer());
    if( item_y ==0 ) continue;
    items_y.push_back(item_y);
  }

  //Get the filters
  std::vector<GsTL_object_item*> filter_items;
  foreach(index, indexes_filters) {
    GsTL_object_item* item_filter = static_cast<GsTL_object_item*>(index.internalPointer());
    if( item_filter ==0 ) continue;
    QString item_type = item_filter->item_type();
    if( item_type == "CategoricalProperty"  || item_type == "GsTLGridRegion"  ) {
      filter_items.push_back( item_filter );
    }

  }

  this->load_data(item_x, items_y, filter_items);

}
/*
void Chart_scatter_plot::add_data( GsTLGridProperty* prop_x){
  model_->insert_row(prop, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_scatter_plot::add_data( GsTLGridProperty* prop, GsTLGridWeightProperty* weigths){
  model_->insert_row(prop,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_scatter_plot::add_data( GsTLGridProperty* prop, GsTLGridRegion* region){
  model_->insert_row(prop,region, default_colors_.at(default_color_id_%max_index_default_colors_) );
  default_color_id_++;
}
void Chart_scatter_plot::add_data( GsTLGridPropertyGroup* group){
  model_->insert_row(group, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_scatter_plot::add_data( GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weigths){
  model_->insert_row(group,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_scatter_plot::add_data( GsTLGridPropertyGroup* group, GsTLGridRegion* region){
  model_->insert_row(group,region, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
*/
void Chart_scatter_plot::add_data(Scatter_plot_item* item){


    if(item->type() == "Property") {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(item);
      this->initialize_data(prop_item);
      this->initialize_plot(prop_item);

      std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
      std::cout<<"set color and style"<<std::endl;
      it->second.plot_points->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
    }
    else if(item->type() == "Property-Group") {
      Scatter_plot_property_group_item* prop_group_item = dynamic_cast<Scatter_plot_property_group_item*>(item);
      int n_props = prop_group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(prop_group_item->children(j));
        this->add_data(prop_item);
      }
    }
    else if(item->type() == "Group") {
      Scatter_plot_group_item* group_item = dynamic_cast<Scatter_plot_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(group_item->children(j));
        this->add_data(prop_item);
      }
    }
    
}



void Chart_scatter_plot::update_data_display(Scatter_plot_item* item){
    if(item->type() == "Property" ) {
      //Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(item);

      std::map<int, Scatter_data>::iterator it = data_stats_.find(item->id());
      it->second.plot_points->SetColor( item->color().red(),item->color().green(),item->color().blue(), item->color().alpha());
      
      if( !item->is_visible() ) {
        chart_->RemovePlotInstance(it->second.plot_points);
      }
      else {
        chart_->RemovePlotInstance(it->second.plot_points);  //Want to be sure that is not added twice, must a smarter way of doing that
        chart_->AddPlot(it->second.plot_points);
      }
    }
    else if(item->type() == "Group") {
      Scatter_plot_group_item* group_item = dynamic_cast<Scatter_plot_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(group_item->children(j));
        this->update_data_display(prop_item);

      }
    }
}

void Chart_scatter_plot::remove_plot(vtkSmartPointer<vtkPlotPoints> plot){
  // Has to be a better way
  int n = chart_->GetNumberOfPlots();
  for(int i=0; i<n; ++i) {
    if(plot == vtkPlotPoints::SafeDownCast(chart_->GetPlot(i))) { 
      chart_->RemovePlot(i);
      break;
    }
  }
}


void Chart_scatter_plot::initialize_data(Scatter_plot_property_item* item){
  
  std::map<int, Scatter_data>::iterator it = data_stats_.find(item->id());

  if(it == data_stats_.end()) {

    Scatter_data data;
    data.name_x = item->prop_x()->name();
    data.name_y = item->prop_y()->name();
    data.name =  this->get_item_base_name(data); 
    data.grid = static_cast<Geostat_grid*>(item->prop_x()->parent()->parent());
    data.scatter_table = vtkSmartPointer<vtkTable>::New();

    data.correl_stats_array = vtkSmartPointer<vtkVariantArray>::New();
    data.correl_stats_array->SetName("Statistics");
    data.correl_stats_array->SetNumberOfValues(12);

    data.prop_x = item->prop_x();
    data.prop_y = item->prop_y();
    data.region = item->region();
    data.weight =item->weights();
    data.cprop = item->categorical_property();
    data.id = -1;
    data_stats_.insert(std::make_pair(item->id(),data));
    it = data_stats_.find(item->id());
  }
  else {  // Assume that we need to recompute
    it->second.name = it->second.name_x +" || "+ it->second.name_y;
    it->second.region = item->region();
    it->second.weight = item->weights();
    it->second.cprop = item->categorical_property();
  }

  //Find how to compute the stats
  if( it->second.weight ==0 ) {
    if(it->second.region !=0 ) {
      it->second.name += " ("+it->second.region->name()+")";
      it->second.filter_name = it->second.region->name();
    }
    else {
      it->second.filter_name = "None";
    }
    this->compute_stats(it->second);
  }
  else {
    it->second.name += " ("+it->second.weight->name()+")";
    it->second.filter_name = it->second.weight->name();
    this->compute_stats_with_weights(it->second);
  }

  this->add_data_to_stats_table(it->second);

  //TODO: Need to add the line format and color
 // data_stats_[item->id()] = data;
}

void Chart_scatter_plot::initialize_plot(Scatter_plot_item* item){

  std::map<int, Scatter_data>::iterator it = data_stats_.find(item->id());

  it->second.plot_points = vtkPlotPoints::SafeDownCast(chart_->AddPlot(vtkChart::POINTS)); // LINE, POINTS, BAR, STACKED

  it->second.plot_points->SetInputData(it->second.scatter_table, 0, 1);
  QColor color = default_colors_.at( default_color_id_%max_index_default_colors_ );
  it->second.plot_points->SetColor(color.redF(), color.greenF(), color.blueF());
  chart_->AddPlot(it->second.plot_points);
  it->second.plot_points->Update();
  

  this->update_chart_display_control();
}

void Chart_scatter_plot::remove_data( int id){

  std::map<int, Scatter_data>::iterator it = data_stats_.find(id);

  chart_->RemovePlotInstance(it->second.plot_points);
  scatter_table_->RemoveColumnByName (it->second.name.c_str());
  data_stats_.erase(it);
}


void Chart_scatter_plot::process_data(Scatter_data& data){

}


void Chart_scatter_plot::compute_stats(Scatter_data& data){

  vtkSmartPointer<vtkFloatArray> x = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> y = vtkSmartPointer<vtkFloatArray>::New();
  
  bool need_memory_swap_x = !data.prop_x->is_in_memory();
  if(need_memory_swap_x) {
    data.prop_x->swap_to_memory();
  }
  bool need_memory_swap_y = !data.prop_y->is_in_memory();
  if(need_memory_swap_y) {
    data.prop_y->swap_to_memory();
  }

  int ndata=0;
  // always build the array manually since vtk 
  // output nan if any data is a nan

  if(data.region == 0) { 
 //   x->SetArray(prop->data(),prop->size(),1);

    for( int i=0 ; i <data.prop_x->size(); ++i) {
      if( data.prop_x->is_informed(i) && data.prop_y->is_informed(i)) ++ndata;
    }
    x->Allocate(ndata);
    y->Allocate(ndata);

    for( int i=0 ; i <data.prop_x->size(); ++i) {
      if( data.prop_x->is_informed(i) && data.prop_y->is_informed(i)) {
        x->InsertNextValue(data.prop_x->get_value_no_check(i));
        y->InsertNextValue(data.prop_y->get_value_no_check(i));
      }
    }
      
  }
  else {
    for( int i=0 ; i <data.prop_x->size(); ++i) {
      if( data.region->is_inside_region(i) && data.prop_x->is_informed(i) && data.prop_y->is_informed(i)) ++ndata;
    }
    x->Allocate(ndata);
    y->Allocate(ndata);
    for( int i=0 ; i <data.prop_x->size(); ++i) {
      if( data.region->is_inside_region(i) && data.prop_x->is_informed(i) && data.prop_y->is_informed(i)) {
        x->InsertNextValue(data.prop_x->get_value_no_check(i));
        y->InsertNextValue(data.prop_y->get_value_no_check(i));
      }
    }
  }


  x->SetName(data.prop_x->name().c_str());
  y->SetName(data.prop_y->name().c_str());

  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New(); 
  table->AddColumn(x);
  table->AddColumn(y);

  //Compute statistics

  vtkSmartPointer<vtkCorrelativeStatistics> correlative_stats  = vtkSmartPointer<vtkCorrelativeStatistics>::New();
  correlative_stats->SetInputData(0, table );
  correlative_stats->AddColumnPair(data.prop_x->name().c_str(),data.prop_y->name().c_str());
  correlative_stats->SetDeriveOption(true);
  correlative_stats->SetAssessOption( false );
  correlative_stats->Update();

  vtkSmartPointer<vtkMultiBlockDataSet> mblock_correlative = vtkMultiBlockDataSet::SafeDownCast( correlative_stats->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  vtkSmartPointer<vtkTable> ctable_primary = vtkTable::SafeDownCast( mblock_correlative->GetBlock( 0 ) );
  vtkSmartPointer<vtkTable> ctable_derived = vtkTable::SafeDownCast( mblock_correlative->GetBlock( 1 ) );

  data.correl_stats_array->SetValue(DATAX, data.name_x.c_str());
  data.correl_stats_array->SetValue(DATAY, data.name_y.c_str());
  data.correl_stats_array->SetValue(GRID, data.prop_x->parent()->parent()->item_name().toStdString().c_str());
  data.correl_stats_array->SetValue(FILTER, data.filter_name.c_str());
  data.correl_stats_array->SetValue(N, vtkVariant(ndata));
  data.correl_stats_array->SetValue(MEANX, ctable_primary->GetValueByName(0,"Mean X"));
  data.correl_stats_array->SetValue(MEANY, ctable_primary->GetValueByName(0,"Mean Y"));
  data.correl_stats_array->SetValue(VARIANCEX, ctable_derived->GetValueByName(0,"Variance X"));
  data.correl_stats_array->SetValue(VARIANCEY, ctable_derived->GetValueByName(0,"Variance Y"));
  data.correl_stats_array->SetValue(COVAR, ctable_derived->GetValueByName(0,"Covariance"));
  data.correl_stats_array->SetValue(CORREL, ctable_derived->GetValueByName(0,"Pearson r"));
  data.correl_stats_array->SetValue(REGINTERCEPT, ctable_derived->GetValueByName(0,"Intercept Y/X"));
  data.correl_stats_array->SetValue(REGSLOPE, ctable_derived->GetValueByName(0,"Slope Y/X"));

  data.scatter_table->ShallowCopy(table);

  if(need_memory_swap_x) {
    data.prop_x->swap_to_disk();
  }
  if(need_memory_swap_y) {
    data.prop_y->swap_to_disk();
  }
}


void Chart_scatter_plot::compute_stats_with_weights(Scatter_data& data){
/*  

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
  */
}

void Chart_scatter_plot::add_data_to_stats_table(Scatter_data& data){

  //check if the entry already exist in the table
  int irow = data.id; 
  bool create_row = false;
  if(data.id == -1) {
      irow = correlative_stats_table_->InsertNextBlankRow();
      data.id= irow;
      create_row = true;
  }
  correlative_stats_table_->SetRow(irow, data.correl_stats_array);
  correlative_stats_table_->Modified();

  // Does not update by iteslf;  have to reset the Representation, something strange here
  correlative_stats_view_->SetRepresentationFromInput(correlative_stats_table_);
  correlative_stats_view_->Update();
  QAbstractItemView* correl_view = dynamic_cast<QAbstractItemView*>(correlative_stats_view_->GetWidget());
  correl_view->reset();

}


void Chart_scatter_plot::remove_data_to_stats_table(Scatter_data& data){

}



void Chart_scatter_plot::set_visibility( Scatter_plot_item* item){
  if(item->type() == "Property") {
    Scatter_plot_property_item* prop_item = dynamic_cast< Scatter_plot_property_item*>(item);
    std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    if(prop_item->is_visible()) {
      chart_->RemovePlotInstance(it->second.plot_points);
      chart_->AddPlot(it->second.plot_points);
    }
    else {
      chart_->RemovePlotInstance(it->second.plot_points);
    }

  }
  else if(item->type() == "Property-Group") {
    Scatter_plot_property_group_item* prop_group_item = dynamic_cast<Scatter_plot_property_group_item*>(item);
    int n_props = prop_group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(prop_group_item->children(j));
      this->set_visibility(prop_item);
    }
  }
  else if(item->type() == "Group") {
    Scatter_plot_group_item* group_item = dynamic_cast<Scatter_plot_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(group_item->children(j));
      this->set_visibility(prop_item);
    }
  }
}
void Chart_scatter_plot::set_color( Scatter_plot_item* item){
  if(item->type() == "Property") {
    Scatter_plot_property_item* prop_item = dynamic_cast< Scatter_plot_property_item*>(item);
    std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_points->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
    it->second.plot_points->Update();
  }
  else if(item->type() == "Property-Group") {
    Scatter_plot_property_group_item* prop_group_item = dynamic_cast<Scatter_plot_property_group_item*>(item);
    int n_props = prop_group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(prop_group_item->children(j));
      std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
      if( it == data_stats_.end() ) return;
      it->second.plot_points->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_points->Update();
    }
  }
  else if(item->type() == "Group") {
    Scatter_plot_group_item* group_item = dynamic_cast<Scatter_plot_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(group_item->children(j));
      std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
      if( it == data_stats_.end() ) return;
      it->second.plot_points->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_points->Update();
    }
  }
}

void Chart_scatter_plot::set_data_filter(Scatter_plot_item* item){

  if(item->type() == "Property") {
    Scatter_plot_property_item* prop_item = dynamic_cast< Scatter_plot_property_item*>(item);
    std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
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
        it->second.name = this->get_item_base_name(it->second);
      } else {
        it->second.name = this->get_item_base_name(it->second) + " ("+it->second.region->name()+")";
      }
      this->compute_stats(it->second);
    }
    else {  // has weights
      it->second.name = this->get_item_base_name(it->second) + " ("+it->second.weight->name()+")";
      this->compute_stats_with_weights(it->second);

    }
    this->add_data_to_stats_table(it->second);

    it->second.plot_points->SetInputData(it->second.scatter_table, 0, 1);
    it->second.plot_points->Update();
    qvtkWidget_->update();

    }

  }
  else if(item->type() == "Property-Group") {
    Scatter_plot_property_group_item* prop_group_item = dynamic_cast<Scatter_plot_property_group_item*>(item);
    int n_props = prop_group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(prop_group_item->children(j));
      this->set_data_filter(prop_item);
    }
  }
  else if(item->type() == "Group") {
    Scatter_plot_group_item* group_item = dynamic_cast<Scatter_plot_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(group_item->children(j));
      this->set_data_filter(prop_item);
    }
  }
}


void Chart_scatter_plot::set_marker_style(Scatter_plot_item* item) {

  if(item->type() == "Property") {
    Scatter_plot_property_item* prop_item = dynamic_cast< Scatter_plot_property_item*>(item);
    std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_points->SetMarkerStyle( item->marker_style() );

  }
  else if(item->type() == "Property-Group") {
    Scatter_plot_property_group_item* prop_group_item = dynamic_cast<Scatter_plot_property_group_item*>(item);
    int n_props = prop_group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(prop_group_item->children(j));
      this->set_marker_style(prop_item);
    }
  }
  else if(item->type() == "Group") {
    Scatter_plot_group_item* group_item = dynamic_cast<Scatter_plot_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(group_item->children(j));
      this->set_marker_style(prop_item);
    }
  }

}

void Chart_scatter_plot::set_marker_size(Scatter_plot_item* item) {

  if(item->type() == "Property") {
    Scatter_plot_property_item* prop_item = dynamic_cast< Scatter_plot_property_item*>(item);
    std::map<int, Scatter_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_points->SetMarkerSize( item->marker_size() );

  }
  else if(item->type() == "Property-Group") {
    Scatter_plot_property_group_item* prop_group_item = dynamic_cast<Scatter_plot_property_group_item*>(item);
    int n_props = prop_group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(prop_group_item->children(j));
      this->set_marker_size(prop_item);
    }
  }
  else if(item->type() == "Group") {
    Scatter_plot_group_item* group_item = dynamic_cast<Scatter_plot_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Scatter_plot_property_item* prop_item = dynamic_cast<Scatter_plot_property_item*>(group_item->children(j));
      this->set_marker_size(prop_item);
    }
  }

}

/*
void Chart_scatter_plot::manage_plot_display(Scatter_data& data, QString display_style ){
  bool changed = false;
  if(display_style == "Bars") {
    chart_->RemovePlotInstance(data.plot_line);
    chart_->RemovePlotInstance(data.plot_bar);  //Want to be sure that is not added twice, must a smarter way of doing that
    chart_->AddPlot(data.plot_bar);
    changed = true;
  }
  else if(display_style == "Lines") {
    chart_->RemovePlotInstance(data.plot_bar);
    chart_->RemovePlotInstance(data.plot_line);
    chart_->AddPlot(data.plot_line);
    changed = true;
  }
  else if(display_style == "Bars and Lines") {
    chart_->RemovePlotInstance(data.plot_bar);
    chart_->AddPlot(data.plot_bar);
    chart_->RemovePlotInstance(data.plot_line);
    chart_->AddPlot(data.plot_line);    
    changed = true;
  }
  if(changed)
    this->update_chart_display_control();
}
*/
void Chart_scatter_plot::update_chart_display_control(){
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


void Chart_scatter_plot::set_x_axis_label(const QString& text){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetTitle(text.toStdString());
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_y_axis_label(const QString& text){
  chart_->GetAxis(vtkAxis::LEFT)->SetTitle(text.toStdString());
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_title(const QString& text){
  chart_->SetTitle(text.toStdString().c_str());
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_legend(bool on){
  chart_->SetShowLegend(on);
  qvtkWidget_->update();
}

void Chart_scatter_plot::set_grid(bool on){
  chart_->GetAxis(0)->SetGridVisible(on);
  chart_->GetAxis(1)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_scatter_plot::set_x_grid(bool on){
  chart_->GetAxis(0)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_scatter_plot::set_y_grid(bool on){
  chart_->GetAxis(1)->SetGridVisible(on);
  qvtkWidget_->update();
}

void Chart_scatter_plot::set_xaxis_min(double min){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(min);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_xaxis_max(double max){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(max);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_xaxis_precision(int digits){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetPrecision(digits);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_xaxis_nticks(int nticks){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetNumberOfTicks(nticks);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_xaxis_logscale(bool on){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetLogScale(on);
  qvtkWidget_->update();
  this->update_chart_display_control();
}
void Chart_scatter_plot::set_xaxis_autoscale(){
  chart_->GetAxis(vtkAxis::BOTTOM)->AutoScale();
  qvtkWidget_->update();
  this->update_chart_display_control();
}


void Chart_scatter_plot::set_yaxis_min(double min){
  chart_->GetAxis(vtkAxis::LEFT)->SetMinimum(min);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_yaxis_max(double max){
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(max);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_yaxis_precision(int digits){
  chart_->GetAxis(vtkAxis::LEFT)->SetPrecision(digits);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_yaxis_nticks(int nticks){
  chart_->GetAxis(vtkAxis::LEFT)->SetNumberOfTicks(nticks);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_yaxis_logscale(bool on){
  chart_->GetAxis(vtkAxis::LEFT)->SetLogScale(on);
  qvtkWidget_->update();
  this->update_chart_display_control();
}
void Chart_scatter_plot::set_yaxis_autoscale(){
  chart_->GetAxis(vtkAxis::LEFT)->AutoScale();
  qvtkWidget_->update();
  this->update_chart_display_control();
}

void Chart_scatter_plot::set_x_axis_font_size(int size){
  chart_->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_y_axis_font_size(int size){
  chart_->GetAxis(vtkAxis::LEFT)->GetLabelProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_x_label_font_size(int size){
  chart_->GetAxis(vtkAxis::BOTTOM)->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_y_label_font_size(int size){
  chart_->GetAxis(vtkAxis::LEFT)->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_legend_font_size(int size){
  chart_->GetLegend()->SetLabelSize(size);
  qvtkWidget_->update();
}
void Chart_scatter_plot::set_title_font_size(int size){
  chart_->GetTitleProperties()->SetFontSize(size);
  qvtkWidget_->update();
}




void Chart_scatter_plot::set_axis(float min_x, float min_y, float max_x, float max_y){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(min_x);
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(max_x);
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(min_y);
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(max_y);
  qvtkWidget_->update();
}

void Chart_scatter_plot::set_x_axis(float min_x, float max_x){
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMinimum(min_x);
  chart_->GetAxis(vtkAxis::BOTTOM)->SetMaximum(max_x);
  qvtkWidget_->update();
}

void Chart_scatter_plot::set_y_axis(float min_y, float max_y){
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(min_y);
  chart_->GetAxis(vtkAxis::LEFT)->SetMaximum(max_y);
  qvtkWidget_->update();
}

void Chart_scatter_plot::reset_axis(){

}

void Chart_scatter_plot::set_clipping_values(float min_x, float min_y, float max_x, float max_y){

}

void Chart_scatter_plot::set_x_clipping_values(float min_x, float max_x){

}

void Chart_scatter_plot::set_y_clipping_values(float min_y, float max_y){

}

void Chart_scatter_plot::reset_clipping_values(){

}

void Chart_scatter_plot::set_numbers_of_bins(int nbins){

}

/*
void Chart_scatter_plot::dragEnterEvent(QDragEnterEvent *event)
{

	if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		event->acceptProposedAction();
	}
}

void Chart_scatter_plot::dropEvent(QDropEvent *event)
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
*/
std::string Chart_scatter_plot::get_item_base_name(Scatter_data& data){
  return data.name_x +" || "+ data.name_y;

}