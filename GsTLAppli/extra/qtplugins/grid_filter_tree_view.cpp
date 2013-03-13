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


#include <GsTLAppli/extra/qtplugins/grid_filter_tree_view.h>
#include <GsTLAppli/extra/qtplugins/grid_filter_model.h>
#include <GsTLAppli/extra/qtplugins/grid_filter_delegate.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/extra/qtplugins/grid_filter_named_interface.h>

#include <QColorDialog>
#include <QHeaderView>


Grid_filter_tree_view::Grid_filter_tree_view(QWidget *parent)
  : QTreeView(parent), model_(0)
{

  this->header()->setResizeMode(QHeaderView::ResizeToContents);



}

Grid_filter_tree_view::~Grid_filter_tree_view()
{

}

void Grid_filter_tree_view::assign_delegate()
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

void Grid_filter_tree_view::set_grid(const QString& grid_name)
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