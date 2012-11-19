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



#include <GsTLAppli/charts/continuous_histogram_creator.h>
#include <GsTLAppli/charts/chart_continuous_histogram.h>
#include <GsTLAppli/charts/chart_categorical_histogram.h>

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


Continuous_histogram_creator::Continuous_histogram_creator(Chart_mdi_area* mdi_area,QWidget *parent):Chart_creator(mdi_area,parent){

  //QGroupBox* histo_selector = new QGroupBox(parent);
  
  QTreeView* histo_tree = new QTreeView(this);
  histo_tree->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  histo_tree->setDragEnabled(true);

  QHBoxLayout* button_layout = new QHBoxLayout(this);
  QPushButton* clear_button = new QPushButton("Clear",this);
  QPushButton* show_button = new QPushButton("Display",this);
  button_layout->addWidget(clear_button);
  button_layout->addWidget(show_button);
  QWidget* button_holder = new QWidget(this);
  button_holder->setLayout(button_layout);

  
  QVBoxLayout* histo_layout = new QVBoxLayout(this);
  histo_layout->addWidget(histo_tree);
  histo_layout->addWidget(button_holder);
  //histo_layout->addItem(button_layout);
  //histo_layout->addStretch();
  //histo_selector->setLayout(histo_layout);
  this->setLayout(histo_layout);

  //set the model to the view and the selection model

  cont_histo_chart_model_ = new Continuous_histo_chart_model(histo_tree);
  cont_histo_selection_ = new QItemSelectionModel(cont_histo_chart_model_);
  histo_tree->setModel(cont_histo_chart_model_);
  histo_tree->setRootIndex(cont_histo_chart_model_->root_index());
  histo_tree->setSelectionModel(cont_histo_selection_);
  histo_tree->setSelectionMode(QAbstractItemView::MultiSelection);
    

  connect( clear_button, SIGNAL(clicked()),cont_histo_selection_, SLOT(clearSelection ())   );
  connect( show_button, SIGNAL(clicked()),this, SLOT(show_continuous_histogram ())   );

}

void Continuous_histogram_creator::show_continuous_histogram(){

  QModelIndexList indexes = cont_histo_chart_model_->mapSelectionToSource(cont_histo_selection_->selection()).indexes();
  if(indexes.isEmpty()) return;
  Chart_continuous_histogram* chart = new Chart_continuous_histogram(55);
  chart->load_data(indexes);

  QMdiSubWindow* sub_window = mdi_area_->addSubWindow(chart);
  sub_window->setAttribute( Qt::WA_DeleteOnClose );
  sub_window->show();
  cont_histo_selection_->clearSelection();
}

