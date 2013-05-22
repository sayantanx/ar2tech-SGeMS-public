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



#include <charts/categorical_histogram_creator.h>
#include <charts/chart_continuous_histogram.h>
#include <charts/chart_categorical_histogram.h>

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




Categorical_histogram_creator::Categorical_histogram_creator(Chart_mdi_area* mdi_area,QWidget *parent):Chart_creator(mdi_area,parent){

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

  cat_histo_chart_model_ = new Categorical_histo_chart_model(histo_tree);
  cat_histo_selection_ = new QItemSelectionModel(cat_histo_chart_model_);
  histo_tree->setModel(cat_histo_chart_model_);
  histo_tree->setRootIndex(cat_histo_chart_model_->root_index());
  histo_tree->setSelectionModel(cat_histo_selection_);
  histo_tree->setSelectionMode(QAbstractItemView::MultiSelection);
    

  connect( clear_button, SIGNAL(clicked()),cat_histo_selection_, SLOT(clearSelection ())   );
  connect( show_button, SIGNAL(clicked()),this, SLOT(show_categorical_histogram ())   );

}



void Categorical_histogram_creator::show_categorical_histogram(){

  //Build list of indexes that are compatible regarding their categorical definition
  QModelIndexList indexes = cat_histo_chart_model_->mapSelectionToSource(cat_histo_selection_->selection()).indexes();


  std::map<std::string, QModelIndexList> cdef_indexes;
  QModelIndexList region_indexes;
  QModelIndex index;
  foreach(index, indexes) {
    GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
    if(item->item_type() == "Region") {
      region_indexes.append(index);
    }
    else if(item->item_type() == "CategoricalProperty") {
      Grid_categorical_property* cprop = static_cast<Grid_categorical_property*>(item->data_pointer());
      CategoricalPropertyDefinition* cdef = cprop->get_category_definition();
      std::map<std::string, QModelIndexList>::iterator it = cdef_indexes.find(cdef->name());
      if( it == cdef_indexes.end() ) {
        it = cdef_indexes.insert(std::make_pair(cdef->name(), QModelIndexList())).first;
      }
      it->second.append(index);
    }
    else if(item->item_type() == "Group") {
      GsTLGridPropertyGroup* group = static_cast<GsTLGridPropertyGroup*>(index.internalPointer());
      std::map<std::string,Grid_continuous_property*>::iterator it_prop = group->begin_property();
      Grid_categorical_property* cprop = dynamic_cast<Grid_categorical_property*>(it_prop->second);
      if(!cprop) continue;
      CategoricalPropertyDefinition* cdef_group = cprop->get_category_definition();
      for( ; it_prop != group->end_property(); ++it_prop ) {   //Check that all properties are categorical and have the same cdef
        Grid_categorical_property* cprop = dynamic_cast<Grid_categorical_property*>(it_prop->second);
        if( cdef_group!= cprop->get_category_definition() ) {
          cdef_group=0;
          break;
        }
      }
      if(cdef_group==0) continue;
      std::map<std::string, QModelIndexList>::iterator it = cdef_indexes.find(cdef_group->name());
      if( it == cdef_indexes.end() ) {
        it = cdef_indexes.insert(std::make_pair(cdef_group->name(), QModelIndexList())).first;
      }
      it->second.append(index);
    }
  }


  std::map<std::string, QModelIndexList>::iterator it = cdef_indexes.begin();
  for(; it != cdef_indexes.end(); ++it) {
    it->second.append(region_indexes);
    Chart_categorical_histogram* chart = new Chart_categorical_histogram();
    chart->load_data(indexes);

    QMdiSubWindow* sub_window = mdi_area_->addSubWindow(chart);
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->show();
    cat_histo_selection_->clearSelection();
  }
}