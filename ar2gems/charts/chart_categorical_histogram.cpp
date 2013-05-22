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


#include <charts/chart_categorical_histogram.h>
#include <appli/manager_repository.h>
#include <grid/grid_model/grid_region_temp_selector.h>
#include <qtplugins/color_delegate.h>
#include <charts/region_weight_delegate.h>
#include <charts/histogram_style_delegate.h>
#include <charts/histogram_item_tree_view.h>

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkAxis.h>
#include <vtkChartLegend.h>
#include <vtkTextProperty.h>
#include <vtkColorSeries.h>
#include <vtkColor.h>

#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QModelIndex>
#include <QAbstractItemView>



Chart_categorical_histogram::Chart_categorical_histogram( QWidget *parent)
  :  Chart_base(parent), ncat_(0),current_cdef_(0)
{

  this->setWindowTitle("Categorical Histogram");
  this->setWindowIcon(QIcon(":/icons/appli/Pixmaps/histo-bars.svg"));


    //Add the tree the control the selection
  QSplitter* mainSplitter = new QSplitter(Qt::Vertical, this);
	
  // Top part of the view
  QSplitter* chartSplitter = new QSplitter(Qt::Horizontal, mainSplitter);
  Histogram_item_tree_view* tree = new Histogram_item_tree_view(this);
  model_ = new Categorical_histogram_proxy_model(this);
  tree->setModel(model_);

  Color_delegate* color_delegate = new Color_delegate (this);
  tree->setItemDelegateForColumn(2,color_delegate);

  Region_weight_delegate* region_weight_delegate = new Region_weight_delegate (this);
  tree->setItemDelegateForColumn(1,region_weight_delegate);

  chartSplitter->addWidget(tree);

  //Add the vtk rendering window
  //There are two rendering window within a tab
  QTabWidget* tab_chart = new QTabWidget(this);
  tab_chart->setTabPosition(QTabWidget::South);

  qvtkWidget_ = new QVTKWidget(this);
	context_view_ = vtkSmartPointer<vtkContextView>::New();
	context_view_->SetInteractor(qvtkWidget_->GetInteractor());
	qvtkWidget_->SetRenderWindow(context_view_->GetRenderWindow());
	chart_ = vtkSmartPointer<vtkChartXY>::New();
	context_view_->GetScene()->AddItem(chart_);  
//  chartSplitter->addWidget(qvtkWidget_);
  tab_chart->addTab(qvtkWidget_,"Per category");


  qvtkWidget_stacked_ = new QVTKWidget(this);
	context_view_stacked_ = vtkSmartPointer<vtkContextView>::New();
	context_view_stacked_->SetInteractor(qvtkWidget_stacked_->GetInteractor());
	qvtkWidget_stacked_->SetRenderWindow(context_view_stacked_->GetRenderWindow());
	chart_stacked_ = vtkSmartPointer<vtkChartXY>::New();
	context_view_stacked_->GetScene()->AddItem(chart_stacked_);  
  tab_chart->addTab(qvtkWidget_stacked_,"Per property");
  
  chartSplitter->addWidget(tab_chart);
  mainSplitter->addWidget(chartSplitter);

  information_view_ = new QTabWidget(mainSplitter);
 // chart_control_ = new Chart_display_control(this);
 // information_view_->addTab(chart_control_,"Chart controls");

 // information_view_->addTab(descriptive_stats_view_->GetWidget(),"Descriptive statistics");
 // information_view_->addTab(ordered_stats_view_->GetWidget(),"Ordered statistics");



  mainSplitter->addWidget(information_view_);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(mainSplitter);
  this->setLayout(layout);
    
  histo_table_ = vtkSmartPointer<vtkTable>::New();

  this->initialize_default_colors();

  bool ok;
  ok = connect( model_, SIGNAL(data_added(Categorical_histogram_item*)), this, SLOT(add_data(Categorical_histogram_item*)) );
  ok = connect( model_, SIGNAL(data_removed(Categorical_histogram_item*)), this, SLOT(remove_data(Categorical_histogram_item*)) );
  ok = connect( model_, SIGNAL(data_filter_changed(Categorical_histogram_item*)), this, SLOT(set_data_filter(Categorical_histogram_item*)) );
  ok = connect( model_, SIGNAL(data_color_changed(Categorical_histogram_item*)), this, SLOT(set_color(Categorical_histogram_item*)) );
  ok = connect( model_, SIGNAL(data_visibility_changed(Categorical_histogram_item*)), this, SLOT(set_visibility(Categorical_histogram_item*)) );
/*
  connect( chart_control_, SIGNAL(xaxis_changed(const QString&)), this, SLOT(set_x_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(yaxis_changed(const QString&)), this, SLOT(set_y_axis_label(const QString&)) );
  connect( chart_control_, SIGNAL(title_changed(const QString&)), this, SLOT(set_title_label(const QString&)) );
  connect( chart_control_, SIGNAL(legend_display_changed(bool)), this, SLOT(set_legend(bool)) );

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
  */
  //QObject::connect( tree, SIGNAL(doubleClicked ( const QModelIndex&)), tree, SLOT(show_color_editor(const QModelIndex&)) );
}

Chart_categorical_histogram::~Chart_categorical_histogram()
{

}




void Chart_categorical_histogram::load_data(QModelIndexList indexes){
  std::map<GsTL_object_item*,Grid_region*> grid_to_region;
  QModelIndex index;

// Need to compute the numbers of categories

  std::vector<Grid_categorical_property*> cprops;
  foreach(index, indexes) {
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    if(item->item_type() == "CategoricalProperty") {
      cprops.push_back(static_cast<Grid_categorical_property*>(item->data_pointer()));
    }
    else if(item->item_type() == "Group") {
      GsTLGridPropertyGroup* group = static_cast<GsTLGridPropertyGroup*>(index.internalPointer());
      std::map<std::string,Grid_continuous_property*>::const_iterator  it_p_in_group = group->begin_property();
      for( ; it_p_in_group != group->end_property(); ++it_p_in_group ) {
        Grid_categorical_property* cprop = dynamic_cast<Grid_categorical_property*>(it_p_in_group->second);
        if(cprop) cprops.push_back(cprop);
      }
    }
  }
  ncat_ = -1;
  for( int i=0; i< cprops.size(); ++i) {
    int ncat = cprops[i]->get_number_of_category();
    if(ncat>ncat_) ncat_ = ncat;
  }


  foreach(index, indexes) {
//  for(int i=0; i<indexes.size(); ++i) {
  //  QModelIndex index =     indexes.at(i);
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    if(item->item_type() == "Region") {
      Grid_region* region = static_cast<Grid_region*>(index.internalPointer());
      grid_to_region[region->parent()->parent()] = region;
    }
  }

//  Send the data to the chart
//  foreach(index, indexes) {
  for(int i=0; i<indexes.size(); ++i) {
    QModelIndex index = indexes.at(i);
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    if(item->item_type() == "CategoricalProperty" ) {
      Grid_categorical_property* prop = static_cast<Grid_categorical_property*>(item->data_pointer());
      std::map<GsTL_object_item*,Grid_region*>::iterator it = grid_to_region.find(prop->parent()->parent());
      Grid_region* region = 0;
      if(it != grid_to_region.end()) region = it->second;
    
      CategoricalPropertyDefinition* cdef = prop->get_category_definition();
      model_->insert_row(prop,region,default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;
    }
    else if(item->item_type().contains("Group:")) {
      GsTLGridPropertyGroup* group = static_cast<GsTLGridPropertyGroup*>(index.internalPointer());
      std::map<GsTL_object_item*,Grid_region*>::iterator it = grid_to_region.find(group->parent());
      Grid_region* region = 0;
      if(it != grid_to_region.end()) region = it->second;
      model_->insert_row(group,region,default_colors_.at(default_color_id_%max_index_default_colors_));
      default_color_id_++;
    }
    else if(item->item_type() == "CategoricalDistribution") {
      Categorical_distribution* dist = static_cast<Categorical_distribution*>(index.internalPointer());
//      model_->insert_row(dist,);
    }
  }
  if(!data_stats_.empty()) current_cdef_ = data_stats_.begin()->second.prop->get_category_definition();
  this->initialize_proportion_table();
  this->set_x_axis_label();
  this->initialize_stacked_chart();

}

void Chart_categorical_histogram::add_data( Grid_categorical_property* prop){
  model_->insert_row(prop, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_categorical_histogram::add_data( Grid_categorical_property* prop, GsTLGridWeightProperty* weigths){
  model_->insert_row(prop,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_categorical_histogram::add_data( Grid_categorical_property* prop, Grid_region* region){
  model_->insert_row(prop,region, default_colors_.at(default_color_id_%max_index_default_colors_) );
  default_color_id_++;
}
void Chart_categorical_histogram::add_data( GsTLGridPropertyGroup* group){
  model_->insert_row(group, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_categorical_histogram::add_data( GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weigths){
  model_->insert_row(group,weigths, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_categorical_histogram::add_data( GsTLGridPropertyGroup* group, Grid_region* region){
  model_->insert_row(group,region, default_colors_.at(default_color_id_%max_index_default_colors_));
  default_color_id_++;
}
void Chart_categorical_histogram::add_data( Categorical_distribution* dist){
//  model_->insert_row(dist);
}

void Chart_categorical_histogram::add_data(Categorical_histogram_item* item){
    if(item->type() == "CategoricalProperty") {
      Categorical_histogram_property_item* prop_item = dynamic_cast<Categorical_histogram_property_item*>(item);
      this->initialize_data(prop_item);
      this->initialize_plot(prop_item);

      std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
      it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
//      it->second.plot_line->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());

    }
    else if(item->type() == "Group") {
      Categorical_histogram_group_item* group_item = dynamic_cast<Categorical_histogram_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Categorical_histogram_property_item* prop_item = dynamic_cast<Categorical_histogram_property_item*>(group_item->children(j));
        this->add_data(prop_item);
      }
    }
    else if(item->type() == "CategoricalDistribution") {
      Categorical_histogram_distribution_item* dist_item = dynamic_cast<Categorical_histogram_distribution_item*>(item);
      this->initialize_data(dist_item);
      this->initialize_plot(dist_item);

      std::map<int, histo_data>::iterator it = data_stats_.find(dist_item->id());
      it->second.plot_bar->SetColor( dist_item->color().red(),dist_item->color().green(),dist_item->color().blue(), dist_item->color().alpha());
 //     it->second.plot_line->SetColor( dist_item->color().red(),dist_item->color().green(),dist_item->color().blue(), dist_item->color().alpha());

    }
}



void Chart_categorical_histogram::update_data_display(Categorical_histogram_item* item){
    if(item->type() == "CategoricalProperty" || item->type() == "CategoricalDistribution") {
      //Categorical_histogram_property_item* prop_item = dynamic_cast<Categorical_histogram_property_item*>(item);

      std::map<int, histo_data>::iterator it = data_stats_.find(item->id());
      it->second.plot_bar->SetColor( item->color().red(),item->color().green(),item->color().blue(), item->color().alpha());
      it->second.plot_bar->SetVisible(item->is_visible());

    }
    else if(item->type() == "Group") {
      Categorical_histogram_group_item* group_item = dynamic_cast<Categorical_histogram_group_item*>(item);
      int n_props = group_item->children_count();
      for(int j=0; j<n_props;++j) {
        Categorical_histogram_property_item* prop_item = dynamic_cast<Categorical_histogram_property_item*>(group_item->children(j));
        this->update_data_display(prop_item);

      }
    }
    qvtkWidget_->update();
}

void Chart_categorical_histogram::remove_plot(vtkSmartPointer<vtkPlot> plot){
  // Has to be a better way
  int n = chart_->GetNumberOfPlots();
  for(int i=0; i<n; ++i) {
    if(plot == chart_->GetPlot(i)) { 
      chart_->RemovePlot(i);
      break;
    }
  }
}

void Chart_categorical_histogram::initialize_data(Categorical_histogram_distribution_item* item){
  
  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  histo_data data;
  data.name = item->item_name().toStdString();
  data.histo_table = vtkSmartPointer<vtkTable>::New();

  if(it == data_stats_.end()) {
    data.id = -1;
    data_stats_.insert(std::make_pair(item->id(),data));
    it = data_stats_.find(item->id());
  }

//  this->compute_stats_from_distribution(it->second);


  this->add_data_to_stats_table(it->second);

}


void Chart_categorical_histogram::initialize_data(Categorical_histogram_property_item* item){
  
  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  histo_data data;
  QString grid_name = item->prop()->parent()->parent()->item_name();
  data.name = item->prop()->name() + "  ( Grid: "+ grid_name.toStdString()+" )";
  data.histo_table = vtkSmartPointer<vtkTable>::New();

  if(it == data_stats_.end()) {
    data.prop = dynamic_cast<Grid_categorical_property*>(item->prop());
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
  if( it->second.weight ==0 )
    this->compute_stats(it->second);
  else 
    this->compute_stats_with_weights(it->second);

  this->add_data_to_stats_table(it->second);

  //TODO: Need to add the line format and color
 // data_stats_[item->id()] = data;


}

void Chart_categorical_histogram::initialize_plot(Categorical_histogram_item* item){

  std::map<int, histo_data>::iterator it = data_stats_.find(item->id());

  it->second.plot_bar = chart_->AddPlot(vtkChart::BAR); // LINE, POINTS, BAR, STACKED
  it->second.plot_bar->GetXAxis()->SetMinimumLimit(-1);
  it->second.plot_bar->GetXAxis()->SetMaximumLimit(ncat_);
  it->second.plot_bar->GetYAxis()->SetMinimumLimit(0.00);
  it->second.plot_bar->GetYAxis()->SetMaximumLimit(1.00);

//  this->manage_plot_display(it->second, item->display_format());

//  vtkPlot* plt = chart_->AddPlot(vtkChart::BAR);
//  plt->SetInput(it->second.histo_table);
//  plt->SetInputArray(0,"Category");
//  plt->SetInputArray(1,it->second.prop->name());
  //it->second.plot_bar->SetInput(it->second.histo_table);
    it->second.plot_bar->SetInputData(it->second.histo_table, 0, 1);
//  it->second.plot_bar->SetInput(it->second.histo_table);
//  it->second.plot_bar->SetInputArray(0,"Category");
//  it->second.plot_bar->SetInputArray(1,it->second.prop->name());

  QColor color = item->color();
//  QColor color = default_colors_.at( default_color_id_%max_index_default_colors_ );
  it->second.plot_bar->SetColor(color.redF(), color.greenF(), color.blueF());
  it->second.plot_bar->Update();


  this->update_chart_display_control();
}

void Chart_categorical_histogram::remove_data( int id){

  std::map<int, histo_data>::iterator it = data_stats_.find(id);

  this->remove_plot(it->second.plot_bar);
  histo_table_->RemoveColumnByName (it->second.name.c_str());
  data_stats_.erase(it);
}

void Chart_categorical_histogram::compute_stats(histo_data& data){
  
 
  Grid_categorical_property* prop = data.prop;
  CategoricalPropertyDefinition* cdef = prop->get_category_definition();
  
  bool need_memory_swap = !prop->is_in_memory();
  if(need_memory_swap) {
    prop->swap_to_memory();
  }
  
  int ndata=0;
  // always build the array manually since vtk 
  // output nan if nay data is a nan

  //std::vector<int> count(ncat_,0);
  std::map<int,int> count;

  {
    Temporary_propRegion_Selector temp_region(data.region, prop);
    Grid_categorical_property::const_iterator it = data.prop->begin();
    for( ; it != data.prop->end(); ++it, ++ndata) {
      count[*it]++;
    }
  }

  vtkSmartPointer<vtkFloatArray> p = vtkSmartPointer<vtkFloatArray>::New();
  vtkSmartPointer<vtkFloatArray> id = vtkSmartPointer<vtkFloatArray>::New();
  p->SetName(prop->name().c_str());
  id->SetName("Category");
  p->SetNumberOfValues(ncat_);
  id->SetNumberOfValues(ncat_);
  for(int i=0;i<ncat_;++i) {
    int cat_id = cdef->category_id_from_index(i);
    p->SetValue(i,static_cast<float>(count[cat_id])/ndata);
    id->SetValue(i,i);
  }

  vtkSmartPointer<vtkTable> histo_table =  vtkSmartPointer<vtkTable>::New();
  histo_table->AddColumn(id);
  histo_table->AddColumn(p);

  data.histo_table->ShallowCopy(histo_table);
  histo_table_->AddColumn(p);
}

//It is either a region or weight, not  both together.
void Chart_categorical_histogram::compute_stats_with_weights(histo_data& data){
  
  Grid_categorical_property* prop = data.prop;
  GsTLGridWeightProperty* weight = data.weight;
  CategoricalPropertyDefinition* cdef = prop->get_category_definition();
  
  bool need_memory_swap = !prop->is_in_memory();
  if(need_memory_swap) {
    prop->swap_to_memory();
  }
  
  int ndata=0;
  // always build the array manually since vtk 
  // output nan if nay data is a nan

  //std::vector<int> count(ncat_,0);
  std::map<int,int> count;

  for(int i=0 ; i<prop->size(); ++i) {
    if( prop->is_informed(i) && weight->is_informed(i)) {
      count[static_cast<int>(prop->get_value(i))] += weight->get_value(i) ;
    }
  }


  vtkSmartPointer<vtkFloatArray> x = vtkSmartPointer<vtkFloatArray>::New();
  x->SetName(prop->name().c_str());
  x->Allocate(ncat_);
  for(int i=0;i<ncat_;++i) {
    int cat_id = cdef->category_id_from_index(i);
    x->SetValue(i,static_cast<float>(count[cat_id]/ndata));
  }

  histo_table_->AddColumn(x);
}
void Chart_categorical_histogram::compute_stats_from_distribution(histo_data& data){

}


void Chart_categorical_histogram::initialize_default_colors(){


  vtkSmartPointer<vtkColorSeries> colors = vtkSmartPointer<vtkColorSeries>::New();
  colors->SetColorScheme(vtkColorSeries::WILD_FLOWER);
  for( int i=0; i<colors->GetNumberOfColors(); ++i ) {
    vtkColor3ub c3ub	= colors->GetColor (i);
    const unsigned char r = c3ub.GetRed();
    const unsigned char g = c3ub.GetGreen();
    const unsigned char b = c3ub.GetBlue();

    default_colors_.append(QColor(c3ub.GetRed(),int(g),int(b)));
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
  */

}

void Chart_categorical_histogram::manage_plot_display(histo_data& data ){

}

void Chart_categorical_histogram::update_chart_display_control(){

}

void Chart_categorical_histogram::add_data_to_stats_table(histo_data& data){

}


void Chart_categorical_histogram::initialize_proportion_table(){

  if(data_stats_.empty() || current_cdef_==0) return;
  
  proportion_table_ = vtkSmartPointer<vtkTable>::New();

   //Build a 0-ncat array for the property name
   vtkSmartPointer<vtkFloatArray> pname = vtkSmartPointer<vtkFloatArray>::New();
   pname->SetName("Properties");
   pname->SetNumberOfValues(data_stats_.size());
   for(int i=0; i<data_stats_.size(); ++i) {
     pname->SetValue(i,i);
   }
   proportion_table_->AddColumn(pname);


  for(int c= 0; c< ncat_; ++c) {
    vtkSmartPointer<vtkFloatArray> p = vtkSmartPointer<vtkFloatArray>::New();
    p->SetName(current_cdef_->get_category_name_from_index(c).c_str());
    p->SetNumberOfValues(data_stats_.size());

    std::map<int, histo_data>::iterator it  = data_stats_.begin();
    for(int i=0 ; it != data_stats_.end(); ++it, ++i) {
      p->SetValue(i,it->second.histo_table->GetValue(c,1).ToFloat());
    }
    proportion_table_->AddColumn(p);
  }


/*
  ncat_ = -1;
  std::map<int, histo_data>::iterator it  = data_stats_.begin();
  for( ; it != data_stats_.end(); ++it) {
    cprop_names_.push_back(it->second.prop->name());
    Grid_categorical_property* cprop = it->second.prop;
    if(cprop==0) continue;
    int ncat = cprop->get_number_of_category();
    if (ncat > ncat_) ncat_ = ncat;
  }


   proportion_table_ = vtkSmartPointer<vtkTable>::New();
   std::vector<std::vector<float>> proportions;
  
   cprop_names_.clear();
   for( it  = data_stats_.begin() ; it != data_stats_.end(); ++it) {
     cprop_names_.push_back(it->second.prop->name());
     Grid_categorical_property* cprop = it->second.prop;
     Grid_categorical_property::const_iterator it_cat = cprop->begin();
     std::vector<float> p(ncat_, 0.0);
     for( ; it_cat != cprop->end(); ++it_cat) {
        p[static_cast<int>(*it_cat)]++;
	    }
     float sum = std::accumulate(p.begin(), p.end(), 0.0);
     for(int i=0; i<ncat_;++i) p[i] /= sum;
     proportions.push_back(p);
   }

   // Check if the number of categories matches the catdef
   current_cdef_ = data_stats_.begin()->second.prop->get_category_definition();
   CategoricalPropertyDefinitionName* cdef_name = dynamic_cast<CategoricalPropertyDefinitionName*>(current_cdef_);

   //Build a 0-ncat array for the property name
   vtkSmartPointer<vtkFloatArray> pname = vtkSmartPointer<vtkFloatArray>::New();
   pname->SetName("Properties");
   pname->Allocate(proportions.size());
   for(int i=0; i<proportions.size(); ++i) {
     pname->SetValue(i,i);
   }
   proportion_table_->AddColumn(pname);

   for(int c= 0; c< ncat_; ++c) {
     vtkSmartPointer<vtkFloatArray> p = vtkSmartPointer<vtkFloatArray>::New();
     p->SetName(current_cdef_->get_category_name(c).c_str());
     p->Allocate(data_stats_.size());
     for(int i=0; i<proportions.size(); ++i) {
       p->SetValue(i,proportions[i][c]);
     }
     proportion_table_->AddColumn(p);
   }
*/
}


void Chart_categorical_histogram::initialize_stacked_chart(){

 if( current_cdef_ == 0 ) return;
 stacked_bar_ = vtkPlotBar::SafeDownCast(chart_stacked_->AddPlot(vtkChart::BAR));
 //stacked_bar_->SetInput(proportion_table_, "Properties", current_cdef_->get_category_name(0));
 stacked_bar_->SetInputData(proportion_table_, 0,1);
 for(int i=1; i<ncat_; ++i) {
  stacked_bar_->SetInputArray(i+1,current_cdef_->get_category_name_from_index(i));
 }

 vtkSmartPointer<vtkColorSeries> color_series = vtkSmartPointer<vtkColorSeries>::New();
 color_series->ClearColors();
 for(int i=0; i<ncat_; ++i) {
   QColor color = current_cdef_->color_from_index(i);
   vtkColor3ub vtk_color( color.red(), color.green(), color.blue() );
   color_series->AddColor(vtk_color);
 } 
 stacked_bar_->SetColorSeries(color_series);

 chart_stacked_->SetShowLegend(true);

  vtkAxis *y_axis = chart_stacked_->GetAxis(vtkAxis::LEFT);
 y_axis->SetBehavior(1);
 y_axis->SetMinimum(0);
 y_axis->SetMaximum(1.0);
 y_axis->SetTitle("Proportions");

 // Set up the legend to be off to the top right of the viewport.
 chart_stacked_->GetLegend()->SetInline(false);
 chart_stacked_->GetLegend()->SetHorizontalAlignment(vtkChartLegend::RIGHT);
 chart_stacked_->GetLegend()->SetVerticalAlignment(vtkChartLegend::TOP);


 // Set up some custom labels for months.
 vtkSmartPointer<vtkDoubleArray> prop_id =
 vtkSmartPointer<vtkDoubleArray>::New();
 vtkSmartPointer<vtkStringArray> prop_names =
 vtkSmartPointer<vtkStringArray>::New();
 std::map<int, histo_data>::iterator it  = data_stats_.begin();
 for(int i=0 ; it != data_stats_.end(); ++it, ++i) {
   prop_id->InsertNextValue(i);
   prop_names->InsertNextValue(it->second.prop->name());
 }

 vtkAxis *x_axis = chart_stacked_->GetAxis(vtkAxis::BOTTOM);
 x_axis->SetBehavior(1);
 x_axis->SetMinimum(-1);
 x_axis->SetMaximum(data_stats_.size());
 x_axis->SetTitle("Properties");

 x_axis->SetCustomTickPositions(prop_id,prop_names);
// x_axis->SetTickPositions(prop_id);
// x_axis->SetTickLabels(prop_names);
 x_axis->GetLabelProperties()->SetOrientation(90);
 x_axis->GetLabelProperties()->SetVerticalJustification(VTK_TEXT_CENTERED);
 x_axis->GetLabelProperties()->SetJustification(VTK_TEXT_RIGHT);


 stacked_bar_->Update();

}


void Chart_categorical_histogram::set_x_axis_label(){
 vtkAxis *axis = chart_->GetAxis(vtkAxis::BOTTOM);
 axis->SetBehavior(1);
 axis->SetMinimum(-1);
 axis->SetMaximum(ncat_);
 axis->SetTitle("Categories");
 chart_->GetAxis(vtkAxis::LEFT)->SetTitle("Proportions");
 
 // Set up the legend to be off to the top right of the viewport.
 chart_->SetShowLegend(true);
 chart_->GetLegend()->SetInline(false);
 chart_->GetLegend()->SetHorizontalAlignment(vtkChartLegend::RIGHT);
 chart_->GetLegend()->SetVerticalAlignment(vtkChartLegend::TOP);
 
 // Set up some custom labels for months.
 vtkSmartPointer<vtkDoubleArray> prop_id =
        vtkSmartPointer<vtkDoubleArray>::New();
 vtkSmartPointer<vtkStringArray> prop_names =
        vtkSmartPointer<vtkStringArray>::New();
 for(int i=0; i<ncat_; ++i) {
   prop_id->InsertNextValue(i);
   prop_names->InsertNextValue(current_cdef_->get_category_name_from_index(i));
 }

 axis->SetCustomTickPositions(prop_id,prop_names);
 //axis->SetTickPositions(prop_id);
 //axis->SetTickLabels(prop_names);
 axis->GetLabelProperties()->SetOrientation(90);
 axis->GetLabelProperties()->SetVerticalJustification(VTK_TEXT_CENTERED);
 axis->GetLabelProperties()->SetJustification(VTK_TEXT_RIGHT);

 
}



void Chart_categorical_histogram::set_data_filter(Categorical_histogram_item* item){

  if(item->type() == "CategoricalProperty") {
    Categorical_histogram_property_item* prop_item = dynamic_cast< Categorical_histogram_property_item*>(item);
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
    if(it->second.weight==0) this->compute_stats(it->second);
    else this->compute_stats_with_weights(it->second);
    this->add_data_to_stats_table(it->second);
    qvtkWidget_->update();
    }

  }
  else if(item->type() == "Group") {
    Categorical_histogram_group_item* group_item = dynamic_cast<Categorical_histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Categorical_histogram_property_item* prop_item = dynamic_cast<Categorical_histogram_property_item*>(group_item->children(j));
      this->set_data_filter(prop_item);
    }
  }

}


void Chart_categorical_histogram::set_color( Categorical_histogram_item* item){
  if(item->type() == "CategoricalProperty") {
    Categorical_histogram_property_item* prop_item = dynamic_cast< Categorical_histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
    it->second.plot_bar->Update();
  }
  else if(item->type() == "Group") {
    Categorical_histogram_group_item* group_item = dynamic_cast<Categorical_histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Categorical_histogram_property_item* prop_item = dynamic_cast<Categorical_histogram_property_item*>(group_item->children(j));
      std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
      if( it == data_stats_.end() ) return;
      it->second.plot_bar->SetColor( prop_item->color().red(),prop_item->color().green(),prop_item->color().blue(), prop_item->color().alpha());
      it->second.plot_bar->Update();
    }
  }
}


void Chart_categorical_histogram::set_visibility( Categorical_histogram_item* item){
  if(item->type() == "CategoricalProperty") {
    Categorical_histogram_property_item* prop_item = dynamic_cast< Categorical_histogram_property_item*>(item);
    std::map<int, histo_data>::iterator it = data_stats_.find(prop_item->id());
    if( it == data_stats_.end() ) return;
    it->second.plot_bar->SetVisible(prop_item->is_visible());
  }
  else if(item->type() == "Group") {
    Categorical_histogram_group_item* group_item = dynamic_cast<Categorical_histogram_group_item*>(item);
    int n_props = group_item->children_count();
    for(int j=0; j<n_props;++j) {
      Categorical_histogram_property_item* prop_item = dynamic_cast<Categorical_histogram_property_item*>(group_item->children(j));
      this->set_visibility(prop_item);
    }
  }
  qvtkWidget_->update();
}