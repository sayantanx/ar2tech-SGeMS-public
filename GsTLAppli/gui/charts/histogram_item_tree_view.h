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



#ifndef HISTOGRAM_ITEM_TREE_VIEW_H
#define HISTOGRAM_ITEM_TREE_VIEW_H

#include <GsTLAppli/gui/common.h>

#include <QTreeView>
#include <QModelIndex>

class GUI_DECL Histogram_item_tree_view : public QTreeView
{
  Q_OBJECT

public:
    Histogram_item_tree_view(QWidget *parent);
    ~Histogram_item_tree_view();

public slots:
  void show_color_editor(const QModelIndex& index);


private:
    
};

#endif // HISTOGRAM_ITEM_TREE_VIEW_H
