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



#ifndef SCATTER_PLOT_MODEL_H
#define SCATTER_PLOT_MODEL_H

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/charts/common.h>
#include <GsTLAppli/charts/scatter_plot_model_item.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>

#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QAbstractItemModel>
#include <QWidget>
#include <QHash>
#include <QPair>



class CHARTS_DECL Scatter_plot_proxy_model : public QAbstractItemModel
{
  Q_OBJECT

public:
    Scatter_plot_proxy_model(QObject *parent);
    Scatter_plot_proxy_model(QList< QPair<GsTL_object_item*,GsTL_object_item*>> items, QObject *parent);
    ~Scatter_plot_proxy_model();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

//    virtual QModelIndex	mapFromSource ( const QModelIndex & sourceIndex ) const;
//    virtual QModelIndex	mapToSource ( const QModelIndex & proxyIndex ) const;

    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    bool insert_row(GsTLGridProperty* prop_x,GsTLGridProperty* prop_y,QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop_x, GsTLGridProperty* prop_y,GsTLGridWeightProperty* weights, QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop_x, GsTLGridProperty* prop_y, GsTLGridRegion* region, QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop_x, GsTLGridProperty* prop_y, GsTLGridCategoricalProperty* cprop, QColor color = QColor());

    bool insert_row(GsTLGridProperty* prop_x,GsTLGridPropertyGroup* group_y,QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop_x, GsTLGridPropertyGroup* group_y,GsTLGridWeightProperty* weights, QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop_x, GsTLGridPropertyGroup* group_y, GsTLGridRegion* region, QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop_x, GsTLGridPropertyGroup* group_y, GsTLGridCategoricalProperty* cprop, QColor color = QColor());


    bool insert_row(GsTLGridPropertyGroup* group_x,GsTLGridPropertyGroup* group_y,QColor color = QColor());
    bool insert_row(GsTLGridPropertyGroup* group_x, GsTLGridPropertyGroup* group_y,GsTLGridWeightProperty* weights, QColor color = QColor());
    bool insert_row(GsTLGridPropertyGroup* group_x, GsTLGridPropertyGroup* group_y, GsTLGridRegion* region, QColor color = QColor());
    bool insert_row(GsTLGridPropertyGroup* group_x, GsTLGridPropertyGroup* group_y, GsTLGridCategoricalProperty* cprop, QColor color = QColor());

    bool insert_row(Scatter_plot_item* item);
    bool remove_row(Scatter_plot_item* item);
    
    bool insert_rows(std::vector< GsTL_object_item*> items);
//    bool remove_rows(std::vector< GsTL_object_item*> items);

//	virtual QStringList mimeTypes() const;
//	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

signals :
  void data_added(Scatter_plot_item* item);
  void data_removed(int id); 

  void data_display_changed(Scatter_plot_item* item);
  void data_color_changed(Scatter_plot_item* item);
  void data_visibility_changed(Scatter_plot_item* item);
  void data_filter_changed(Scatter_plot_item* item);
  void display_format_changed(Scatter_plot_item* item);
  void data_marker_style_changed(Scatter_plot_item*);
  void data_marker_size_changed(Scatter_plot_item*);
  //void distribution_display_changed(Histogram_distribution_item* item);

//  void distribution_added(Histogram_distribution_item* dist);
//  void distribution_removed(Continuous_distribution* dist);


private slots :

//	void begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
//	void end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);

private :
  void set_connections();


private :

	Manager* grid_manager_;
  Root_model* model_;
  //QList< Histogram_item*> items_;
  //QHash<const GsTL_object_item*,Histogram_item*> lookup_items_;
  std::set< Scatter_plot_item*> items_;
  //std::map<const GsTL_object_item*,Histogram_item*> lookup_items_;  
 // std::map<GsTL_object_item*,Histogram_item*> lookup_items_; 

  void find_items_to_be_removed(GsTL_object_item*, std::set< Scatter_plot_item*>& items_to_be_removed);
  /*
  bool is_item_exist(GsTLGridProperty* prop);
  bool is_item_exist(GsTLGridProperty* prop, GsTLGridRegion* region);
  bool is_item_exist(GsTLGridProperty* prop, GsTLGridWeightProperty* weights);
  bool is_item_exist(GsTLGridPropertyGroup* prop);
  bool is_item_exist(GsTLGridPropertyGroup* prop, GsTLGridRegion* region);
  bool is_item_exist(GsTLGridPropertyGroup* prop, GsTLGridWeightProperty* weights);
  */

  int current_id_; //id for each element in the histogram
};

#endif // HISTOGRAM_MODEL_H
