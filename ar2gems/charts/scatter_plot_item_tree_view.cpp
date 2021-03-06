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





#include <charts/scatter_plot_item_tree_view.h>
#include <charts/scatter_plot_proxy_model.h>
#include <qtplugins/color_delegate.h>


#include <QColorDialog>
#include <QHeaderView>

Scatter_plot_item_tree_view::Scatter_plot_item_tree_view(QWidget *parent)
  : QTreeView(parent)
{

  this->header()->setResizeMode(QHeaderView::ResizeToContents);

  QObject::connect( this, SIGNAL(doubleClicked ( const QModelIndex&)), this, SLOT(show_color_editor(const QModelIndex&)) );


}

Scatter_plot_item_tree_view::~Scatter_plot_item_tree_view()
{

}

// Need to be connected to the double click of the view
void Scatter_plot_item_tree_view::show_color_editor(const QModelIndex& index) {
  if ( index.column() != 3 ) return;

  //get the current color
  Scatter_plot_item* item = static_cast<Scatter_plot_item*>(index.internalPointer());
  QColor color = QColorDialog::getColor(item->color(), this, 
                                        QString("Choose color for pair %1 - %2").arg(item->item_name_x()).arg(item->item_name_y()),
                                        QColorDialog::ShowAlphaChannel);
  this->model()->setData(index, color);
  //item->color(color);
 
}