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




#include <charts/chart_dialog.h>
#include <charts/chart_creator.h>
#include <appli/manager_repository.h>
#include <utils/named_interface.h>


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


Chart_dialog::Chart_dialog(QWidget *parent)
  : QMainWindow (parent)
{
  this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
  QVBoxLayout* main_layout = new QVBoxLayout(this);

//  QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);

  //QToolBox* tool_box = new QToolBox(mainSplitter);
  QToolBox* tool_box = new QToolBox(this);
  mdi_area_ = new Chart_mdi_area(this);

  //Load the control for the charts
  Manager* dir = dynamic_cast<Manager*>(Root::instance()->interface(eda_charts_manager).raw_ptr());
  if(dir) {
    Manager::type_iterator it_chart = dir->begin();
    for( ; it_chart != dir->end(); ++it_chart ) {
      SmartPtr<Named_interface> ni = Root::instance()->new_interface(*it_chart, eda_charts_manager+"/");
      Chart_creator_factory* chart_factory = dynamic_cast<Chart_creator_factory*>(ni.raw_ptr());
      if(chart_factory ==0) continue;

      QString chart_name = chart_factory->title_name();
      QString tab_name = chart_factory->tab_name();
      Chart_creator* chart = chart_factory->get_interface(mdi_area_);
      QTabWidget* tab = 0;
      for(int i=0; i< tool_box->count(); ++i ) {
        if( tool_box->itemText(i) == tab_name ) {
          tab = dynamic_cast<QTabWidget*>(tool_box->widget(i));
          break;
        }
      }
      if(tab ==0 ) {  //Need to create a tab with the proper name
        tab = new QTabWidget(tool_box);
        tab->setTabPosition(QTabWidget::West);
        tool_box->addItem(tab,tab_name);
      }
      chart->setParent(tab);
      tab->addTab(chart,chart_name);

      
//      tool_box->addItem(chart, chart->title_name());

    }
  }

/*
  //Build tree view for continuous histogram
  QWidget* cont_histo_selector = this->build_continuous_histogram_selector(tool_box);
  tool_box->addItem(cont_histo_selector, "Histogram (Continuous)");

  QWidget* cat_histo_selector = this->build_categorical_histogram_selector(tool_box);
  tool_box->addItem(cat_histo_selector, "Histogram (Categorical)");
  */
  
  //mdi_area_ = new QMdiArea(this);

 // mainSplitter->addWidget(tool_box);
 // mainSplitter->addWidget(mdi_area_);
  
 // main_layout->addWidget(mainSplitter);

  QDockWidget *dock_toolbox = new QDockWidget(tr("Data Selection"), this);
  dock_toolbox->setAllowedAreas(Qt::LeftDockWidgetArea |
                              Qt::RightDockWidgetArea);
  dock_toolbox->setWidget(tool_box);
  this->addDockWidget(Qt::LeftDockWidgetArea, dock_toolbox);

  this->setCentralWidget(mdi_area_);

  QToolBar* chart_toolbar= addToolBar(tr("Tile"));
  QAction* tile_action  = new QAction(QIcon(), tr("&Tile..."), this);
  tile_action->setShortcut(Qt::Key_T);
  tile_action->setStatusTip(tr("Organize charts in window"));
  connect(tile_action, SIGNAL(triggered()), mdi_area_, SLOT(tileSubWindows()));
  chart_toolbar->addAction(tile_action);

  QAction* toggle_view = dock_toolbox->toggleViewAction();
  toggle_view ->setShortcut(Qt::Key_P);
  chart_toolbar->addAction(toggle_view);
  
 // this->setLayout(main_layout);

}

Chart_dialog::~Chart_dialog()
{

}

/*
QWidget* Chart_dialog::build_continuous_histogram_selector(QWidget* parent){
  QGroupBox* histo_selector = new QGroupBox(parent);
  
  QTreeView* histo_tree = new QTreeView(histo_selector);
  histo_tree->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  histo_tree->setDragEnabled(true);

  QHBoxLayout* button_layout = new QHBoxLayout(histo_selector);
  QPushButton* clear_button = new QPushButton("Clear",histo_selector);
  QPushButton* show_button = new QPushButton("Display",histo_selector);
  button_layout->addWidget(clear_button);
  button_layout->addWidget(show_button);
  QWidget* button_holder = new QWidget(histo_selector);
  button_holder->setLayout(button_layout);

  
  QVBoxLayout* histo_layout = new QVBoxLayout(histo_selector);
  histo_layout->addWidget(histo_tree);
  histo_layout->addWidget(button_holder);
  //histo_layout->addItem(button_layout);
  //histo_layout->addStretch();
  histo_selector->setLayout(histo_layout);

  //set the model to the view and the selection model

  cont_histo_chart_model_ = new Continuous_histo_chart_model(histo_tree);
  cont_histo_selection_ = new QItemSelectionModel(cont_histo_chart_model_);
  histo_tree->setModel(cont_histo_chart_model_);
  histo_tree->setRootIndex(cont_histo_chart_model_->root_index());
  histo_tree->setSelectionModel(cont_histo_selection_);
  histo_tree->setSelectionMode(QAbstractItemView::MultiSelection);
    

  connect( clear_button, SIGNAL(clicked()),cont_histo_selection_, SLOT(clearSelection ())   );
  connect( show_button, SIGNAL(clicked()),this, SLOT(show_continuous_histogram ())   );

  return histo_selector;

   //Need to set up model and selectionModel for the continuous attribute only (+ weights and regions)
}

QWidget* Chart_dialog::build_categorical_histogram_selector(QWidget* parent){
  QWidget* histo_selector = new QWidget(parent);
  
  QTreeView* histo_tree = new QTreeView(histo_selector);
  QHBoxLayout* button_layout = new QHBoxLayout(histo_selector);
  QPushButton* clear_button = new QPushButton("Clear",histo_selector);
  QPushButton* show_button = new QPushButton("Display",histo_selector);
  button_layout->addWidget(clear_button);
  button_layout->addWidget(show_button);

  QVBoxLayout* histo_layout = new QVBoxLayout(histo_selector);
  histo_layout->addWidget(histo_tree);
  histo_layout->addItem(button_layout);
  histo_layout->addStretch();
  histo_selector->setLayout(histo_layout);


  Categorical_histo_chart_model* model = new Categorical_histo_chart_model(histo_tree);
  cat_histo_selection_ = new QItemSelectionModel(model);
  histo_tree->setModel(model);
  histo_tree->setRootIndex(model->root_index());
  histo_tree->setSelectionModel(cat_histo_selection_);
  histo_tree->setSelectionMode(QAbstractItemView::MultiSelection);

  connect( clear_button, SIGNAL(clicked()),cat_histo_selection_, SLOT(clearSelection ())   );
  connect( show_button, SIGNAL(clicked()),this, SLOT(show_categorical_histogram ())   );

  return histo_selector;
}

void Chart_dialog::show_continuous_histogram(){

  Chart_continuous_histogram* chart = new Chart_continuous_histogram(55,mdi_area_);

  QModelIndexList indexes = cont_histo_chart_model_->mapSelectionToSource(cont_histo_selection_->selection()).indexes();
  chart->load_data(indexes);

  QMdiSubWindow* sub_window = mdi_area_->addSubWindow(chart);
  sub_window->show();

}

void Chart_dialog::show_categorical_histogram(){

}*/