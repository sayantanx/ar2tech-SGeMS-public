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



#ifndef GRID_FILTER_WIDGETS_H
#define GRID_FILTER_WIDGETS_H

#include <GsTLAppli/extra/qtplugins/common.h>
#include <GsTLAppli/extra/qtplugins/grid_filter_model.h>
#include <GsTLAppli/grid/grid_filter.h>

#include <QTreeView>
#include <QModelIndex>
#include <QPushButton>
#include <QDialog>

class QTPLUGINS_DECL Grid_filter_selector : public QTreeView
{
  Q_OBJECT

public:
    Grid_filter_selector(QWidget *parent=0);

    ~Grid_filter_selector();

    Grid_filter* create_filter() const { return  model_->create_filter();}

    QStringList filter_selected() const;


public slots:
  void set_grid(const QString& grid_name);

signals:
  void grid_modified(const QString&);

private:
   void assign_delegate();

private:
    Grid_filter_model* model_;
};


class QTPLUGINS_DECL Grid_filter_selector_button : public QPushButton 
{
  Q_OBJECT
public :
  Grid_filter_selector_button(QWidget *parent=0);
  ~Grid_filter_selector_button(){}

  Grid_filter* create_filter() const;

public slots:
  void set_grid(const QString& grid_name);
  void open_filters_selector();
  void set_filter_selection(QStringList parameters);

signals:
    void filters_changed(QStringList);
    void grid_modified(QString);

private:
  Grid_filter_selector* filter_selector_;
  QDialog* filter_dialog_;

};

class QTPLUGINS_DECL Grid_filter_selector_collapsing : public QWidget 
{
  Q_OBJECT
public :
  Grid_filter_selector_collapsing(QWidget *parent=0);
  ~Grid_filter_selector_collapsing(){}

  Grid_filter* create_filter() const;

public slots:
  void set_grid(const QString& grid_name);
  void update_filter_selection(bool);
  void toggle_selection_visibility();

signals:
  void filters_changed(QStringList);
  void grid_modified(QString);

private:
  Grid_filter_selector* filter_selector_;
  QPushButton* filter_button_;

};

#endif // HISTOGRAM_ITEM_TREE_VIEW_H
