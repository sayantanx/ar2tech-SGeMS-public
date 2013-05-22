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


#include <GsTLAppli/qtplugins/grid_filter_widgets.h>
#include <GsTLAppli/qtplugins/grid_filter_model.h>
#include <GsTLAppli/qtplugins/grid_filter_delegate.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/qtplugins/grid_filter_named_interface.h>

#include <QColorDialog>
#include <QHeaderView>


Grid_filter_selector::Grid_filter_selector(QWidget *parent)
  : QTreeView(parent), model_(0)
{

  this->header()->setResizeMode(QHeaderView::ResizeToContents);



}

Grid_filter_selector::~Grid_filter_selector()
{

}

void Grid_filter_selector::assign_delegate()
{

  int nrow = model_->rowCount();

  for( int i=0; i< nrow; ++i) {

    QString grid_filter_name = model_->get_grid_filter_item( i )->filter_name();
    SmartPtr<Named_interface> ni = Root::instance()->new_interface(grid_filter_name.toStdString()+"://", qitem_grid_filter_model_manager+"/");
    Grid_filter_named_interface* grid_filter_ni = dynamic_cast<Grid_filter_named_interface*>(ni.raw_ptr());
    Grid_filter_abstract_delegate* filter_delegate = grid_filter_ni->filter_delegate();
    filter_delegate->setParent(this);
    this->setItemDelegateForRow(i,filter_delegate);
    bool ok = QObject::connect( this, SIGNAL(grid_modified ( const QString&)), filter_delegate, SLOT(set_grid(const QString&)) );
  }


  

}

void Grid_filter_selector::set_grid(const QString& grid_name)
{
  if(model_ == 0) {
    model_ = new Grid_filter_model(this);
    model_->initialize();
    this->setModel(model_);
    this->assign_delegate();
  }

  model_->set_grid(grid_name);
  emit this->grid_modified( grid_name );

}


QStringList Grid_filter_selector::filter_selected() const{
  return model_->get_grid_filter_name();
}

//
// ---------------------------------------
//

Grid_filter_selector_button::Grid_filter_selector_button(QWidget* parent) : QPushButton(parent) {

  filter_selector_ = new Grid_filter_selector();

  this->setText("No filters");
  this->setToolTip("No Filter Selected");

  filter_dialog_ = new QDialog(this);
  QVBoxLayout* filter_dialog_layout = new QVBoxLayout(filter_dialog_);
  filter_dialog_layout->addWidget(filter_selector_);
  
//  QPushButton* clear_selection_button = new QPushButton("Clear Selection",filter_dialog_ );
  QPushButton* close_selection_button = new QPushButton("Close",filter_dialog_ );
  QHBoxLayout* button_layout = new QHBoxLayout(filter_dialog_);
//  button_layout->addWidget(clear_selection_button);
  button_layout->addWidget(close_selection_button);
  filter_dialog_layout->addItem(button_layout);
  filter_dialog_->setLayout(filter_dialog_layout);
  filter_dialog_->setWindowTitle( "Select Filters" );

/*
  QObject::connect( clear_selection_button, SIGNAL( clicked() ),
                    filter_dialog_, SLOT( clearSelection() ) );
*/

  bool ok = QObject::connect( this, SIGNAL( clicked() ),
                    this, SLOT( open_filters_selector( ) ) );

  ok = QObject::connect( close_selection_button, SIGNAL( clicked( ) ),
                    filter_dialog_, SLOT( accept( ) ) );

  ok = QObject::connect( close_selection_button, SIGNAL( clicked( ) ),
                    this, SLOT( toggle( ) ) );

  ok = QObject::connect( this, SIGNAL( filters_changed( QStringList ) ),
                    this, SLOT( set_filter_selection( QStringList ) ) );

}


void Grid_filter_selector_button::open_filters_selector(){

	filter_dialog_->exec();
  QStringList filters = filter_selector_->filter_selected();
	emit this->filters_changed(filters);
}


void Grid_filter_selector_button::set_filter_selection(QStringList filters){
  if(filters.isEmpty()) {
    this->setText("No filters");
    this->setToolTip("No Filter Selected");
  }
  else {
    if(filters.size()==1) this->setText("One Filter Selected");
    else {
      QString filter_text = QString("%1 Filters Selected").arg(filters.size());
      this->setText(filter_text);
    }
    
    QString parameters = filters.join("\n");
    this->setToolTip(parameters);
  }
}

void Grid_filter_selector_button::set_grid(const QString& grid_name)
{
  filter_selector_->set_grid(grid_name);
  emit this->grid_modified( grid_name );

}

//
// ---------------------------------------
//

Grid_filter_selector_collapsing::Grid_filter_selector_collapsing(QWidget* parent) : QWidget(parent) {

  filter_button_ = new QPushButton("Select Filter (No Filter Selected)",this );
  filter_selector_ = new Grid_filter_selector();
  filter_selector_->setVisible(false);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(filter_button_);
  layout->addWidget(filter_selector_);


  bool ok = QObject::connect( filter_button_, SIGNAL( clicked( ) ),
                    this, SLOT( toggle_selection_visibility(  ) ) );

}

void Grid_filter_selector_collapsing::update_filter_selection(bool on){
  if(on) { // the button is pressed
    filter_button_->setText("Select Filter");
  }
  else {
    QStringList filters = filter_selector_->filter_selected();
    if(filters.isEmpty()) {
      filter_button_->setText("No filters");
      filter_button_->setToolTip("No Filter Selected");
    }
    else {
      if(filters.size()==1) filter_button_->setText("One Filter Selected");
      else {
        QString filter_text = QString("%1 Filters Selected").arg(filters.size());
        filter_button_->setText(filter_text);
      }
    
      QString parameters = filters.join("\n");
      this->setToolTip(parameters);
    }
  }


}

void Grid_filter_selector_collapsing::set_grid(const QString& grid_name)
{
  filter_selector_->set_grid(grid_name);
  emit this->grid_modified( grid_name );

}

void Grid_filter_selector_collapsing::toggle_selection_visibility(){
  bool ok = filter_selector_->isVisible();
  filter_selector_->setVisible(!ok);
  this->update_filter_selection( !ok );
}