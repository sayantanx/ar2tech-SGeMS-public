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



#include <charts/scatter_plot_creator.h>
#include <charts/chart_scatter_plot.h>


#include <QDialog>
#include <QTreeView>
#include <QToolBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QModelIndexList>
#include <QGroupBox>
#include <QLabel>
#include <QDockWidget>
#include <QToolBar>
#include <QAction>
#include <QListWidgetItem>

Scatter_plot_creator::Scatter_plot_creator(Chart_mdi_area* mdi_area,QWidget *parent):Chart_creator(mdi_area,parent){

  //QGroupBox* histo_selector = new QGroupBox(parent);
  
  grid_selector_ = new GridSelector( this );
  x_prop_selector_ = new SinglePropertySelector(this);

  y_props_selector_ = new OrderedPropertySelector(this);
  y_props_selector_->setTitle("");
  y_groups_selector_ = new MultiPropertyGroupSelector(this);


  QHBoxLayout* button_layout = new QHBoxLayout(this);
  QPushButton* clear_button = new QPushButton("Clear",this);
  QPushButton* show_button = new QPushButton("Display",this);
  button_layout->addWidget(clear_button);
  button_layout->addWidget(show_button);
  QWidget* button_holder = new QWidget(this);
  button_holder->setLayout(button_layout);

  
  QVBoxLayout* main_layout = new QVBoxLayout(this);
  main_layout->addWidget(grid_selector_);
  main_layout->addWidget(new QLabel("Slect X-axis property",this));
  main_layout->addWidget(x_prop_selector_);
  main_layout->addWidget(new QLabel("Slect Y-axis property (if any)",this));
  main_layout->addWidget(y_props_selector_);
  main_layout->addWidget(new QLabel("Slect Y-axis group (if any)",this));
  main_layout->addWidget(y_groups_selector_);
  main_layout->addWidget(button_holder);
  this->setLayout(main_layout);

  connect( grid_selector_, SIGNAL(activated(const QString&)),x_prop_selector_, SLOT(show_properties (const QString&))   );
  connect( grid_selector_, SIGNAL(activated(const QString&)),y_props_selector_, SLOT(show_properties (const QString&))   );
  connect( grid_selector_, SIGNAL(activated(const QString&)),y_groups_selector_, SLOT(show_property_groups (const QString&))   );
  
  connect( show_button, SIGNAL(clicked()),this, SLOT(show_scatter_plot ())   );

}

void Scatter_plot_creator::show_scatter_plot(){
/*
  QModelIndexList indexes_x = cont_histo_chart_model_->mapSelectionToSource(x_selection_->selection()).indexes();
  QModelIndexList indexes_y = cont_histo_chart_model_->mapSelectionToSource(y_selections_->selection()).indexes();
  QModelIndexList indexes_filter = cont_histo_chart_model_->mapSelectionToSource(filter_selections_->selection()).indexes();

  if(indexes_x.isEmpty() || indexes_y.isEmpty() ) return;
  
  Chart_scatter_plot* chart = new Chart_scatter_plot();  
  chart->load_data(indexes_x.at(0),indexes_y,indexes_filter);
  */

  QString grid_name = grid_selector_->selectedGrid();
  QString region_name = grid_selector_->selectedRegion();

  QString x_prop_name = x_prop_selector_->currentText();

  if(grid_name.isEmpty() || x_prop_name.isEmpty()) return;
  //Get the grid
  Geostat_grid* grid = grid_selector_->selected_grid_object();
  if(grid == 0 ) return;

  Grid_continuous_property* x_prop = grid->property(x_prop_name.toStdString());
  if(x_prop == 0 ) return;

  QStringList y_prop_names = y_props_selector_->selected_properties();
  QList<QListWidgetItem *> y_groups_items =	y_groups_selector_->selectedItems();
  if(y_prop_names.isEmpty() && y_groups_items.isEmpty()) return;

  std::vector<GsTL_object_item*> filter_items;
  filter_items.push_back( grid->region( region_name.toStdString() ) );

  std::vector<GsTL_object_item*> y_items;
  for(int i=0; i<y_prop_names.size(); ++i) {
    Grid_continuous_property* prop = grid->property( y_prop_names.at(i).toStdString() );
    if( prop ) y_items.push_back( prop );
  }
  for(int i=0; i<y_groups_items.size(); ++i) {
    GsTLGridPropertyGroup* prop = grid->get_group( y_groups_items.at(i)->text().toStdString() );
    if( prop ) y_items.push_back( prop );
  }
  if(y_items.empty()) return;

  Chart_scatter_plot* chart = new Chart_scatter_plot();  
  chart->load_data( x_prop, y_items,  filter_items );
  QMdiSubWindow* sub_window = mdi_area_->addSubWindow(chart);
  sub_window->setAttribute( Qt::WA_DeleteOnClose );
  sub_window->show();

  //cont_histo_selection_->clearSelection();
}

