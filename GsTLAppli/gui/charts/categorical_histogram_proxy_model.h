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

#ifndef CATEGORICAL_HISTOGRAM_MODEL_H
#define CATEGORICAL_HISTOGRAM_MODEL_H

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>
#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/math/categorical_distribution.h>

#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QAbstractProxyModel>
#include <QWidget>
#include <QHash>

class GUI_DECL Categorical_histogram_item {
public :
  Categorical_histogram_item(int& id):id_(id){}
  virtual ~Categorical_histogram_item(){}

  virtual int children_count() const =0;
  virtual Categorical_histogram_item* children(int row) =0;  
  virtual Categorical_histogram_item* parent() =0;  

  virtual QString type() const =0;

 // void id(int id) const {id_ = id;}
  int id() const {return id_;}

  QString grid_name() const {return grid_name_;}
  QString item_name() const {return item_name_;}
  virtual GsTL_object_item* object_item() = 0;

  Geostat_grid* grid() const {return grid_;}

  virtual GsTLGridWeightProperty* weights() {return 0;}
  virtual GsTLGridRegion* region() {return 0;}
  virtual void weights(GsTLGridWeightProperty* weights) {}
  virtual void region(GsTLGridRegion* region) {}
  virtual void weights(QString weights_name) {}
  virtual void region(QString region_name) {}

  virtual bool operator<(const Categorical_histogram_item& rhs ) const {

    if(this->grid_name_ < rhs.grid_name()) return true;
    else if(this->grid_name() > rhs.grid_name()) return false;
    else return this->item_name_ <  rhs.item_name();

  }

  virtual bool operator==(const Categorical_histogram_item& rhs ) const {
    return item_name_ == rhs.item_name() &&  rhs.grid_name() == grid_name_;
  }

  virtual bool operator!=(const Categorical_histogram_item& rhs ) const {
    return !this->operator==(rhs);
  }
  /*
  float r_color() const {return color_.redF();}
  float g_color() const {return color_.greenF();}
  float b_color() const {return color_.blueF();}
  */
  QColor color() const {return color_;}
  bool is_visible() {return is_visible_;}

  virtual void color(const QColor &color) {color_ = color;}
  virtual void set_visibility( bool on ) {is_visible_ = on;}

protected :

  const int id_;

  QString grid_name_;
  QString item_name_;  // Either the name of a property of a group

  Geostat_grid* grid_;

  QColor color_;
  bool is_visible_;

};


class GUI_DECL Categorical_histogram_property_item :public Categorical_histogram_item {
public :
  Categorical_histogram_property_item( GsTLGridCategoricalProperty* prop, int& id, Categorical_histogram_item* parent=0);
  virtual ~Categorical_histogram_property_item(){}

  virtual QString type() const {return "CategoricalProperty";}

  
  GsTLGridCategoricalProperty* prop() {return prop_;}
  virtual GsTL_object_item* object_item() {return dynamic_cast<GsTL_object_item*>(prop_);}

  virtual GsTLGridWeightProperty* weights() {return weights_;}
  virtual GsTLGridRegion* region() {return region_;}
  virtual void weights(GsTLGridWeightProperty* weights) {weights_ = weights;}
  virtual void region(GsTLGridRegion* region) {region_ = region;}
  virtual void weights(QString weights_name) {weights_ = grid_->weight_property(weights_name.toStdString());}
  virtual void region(QString region_name) {region_ = grid_->region(region_name.toStdString());}

  virtual int children_count() const {return 0;}
  virtual Categorical_histogram_item* children(int row) {return 0;}
  virtual Categorical_histogram_item* parent() {return parent_;};  

  /*
  virtual bool operator==(const Categorical_histogram_property_item& rhs ) const {
    return this->prop_ == rhs.prop();
  }

  virtual bool operator!=(const Categorical_histogram_property_item& rhs ) const {
    return this->prop_ != rhs.prop();
  }
  */
private :
  
  Categorical_histogram_item* parent_;
  GsTLGridCategoricalProperty* prop_;
  GsTLGridRegion* region_;
  GsTLGridWeightProperty* weights_;


};


class GUI_DECL Categorical_histogram_group_item :public Categorical_histogram_item {
public :
  Categorical_histogram_group_item( GsTLGridPropertyGroup* group, int& id);
  virtual ~Categorical_histogram_group_item();

  virtual QString type() const {return "Group";}
  GsTLGridPropertyGroup* group() {return group_;}
  virtual GsTL_object_item* object_item() {return group_;}

  virtual GsTLGridWeightProperty* weights(){return weights_;}
  virtual GsTLGridRegion* region(){return region_;}
  virtual void weights(GsTLGridWeightProperty* weights);
  virtual void region(GsTLGridRegion* region);
  virtual void weights(QString weights_name);
  virtual void region(QString region_name) ;



  virtual int children_count() const {return prop_items_.size();}
  virtual Categorical_histogram_item* children(int row);
  virtual Categorical_histogram_item* parent() {return 0;}  
  int row_for_children(Categorical_histogram_item*) const;

  //Need to propagate the change to underlying properties
  virtual void color(const QColor &color);
  virtual void set_visibility( bool on );

private :

  GsTLGridPropertyGroup* group_;
  GsTLGridRegion* region_;
  GsTLGridWeightProperty* weights_;

  std::set< Categorical_histogram_property_item*> prop_items_;

};




class GUI_DECL Categorical_histogram_distribution_item :public Categorical_histogram_item {
public :
  Categorical_histogram_distribution_item( Categorical_distribution* dist, int& id );
  virtual ~Categorical_histogram_distribution_item(){}

  virtual QString type() const {return "CategoricalDistribution";}

  
  GsTLGridCategoricalProperty* prop() {return 0;}
  virtual GsTL_object_item* object_item() {return dist_;}

  virtual GsTLGridWeightProperty* weights() {return 0;}
  virtual GsTLGridRegion* region() {return 0;}
  virtual void weights(GsTLGridWeightProperty* weights) {}
  virtual void region(GsTLGridRegion* region) {}
  virtual void weights(QString weights_name) {}
  virtual void region(QString region_name) {}

  virtual int children_count() const {return 0;}
  virtual Categorical_histogram_item* children(int row) {return 0;}
  virtual Categorical_histogram_item* parent() {return 0;}  


private :
  
  Categorical_histogram_item* parent_;
  Categorical_distribution* dist_;
};


class GUI_DECL Categorical_histogram_proxy_model : public QAbstractProxyModel
{
  Q_OBJECT

public:
    Categorical_histogram_proxy_model(QObject *parent);
    Categorical_histogram_proxy_model(QList< GsTL_object_item*> items, QObject *parent);
    ~Categorical_histogram_proxy_model();

    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    virtual QModelIndex	mapFromSource ( const QModelIndex & sourceIndex ) const;
    virtual QModelIndex	mapToSource ( const QModelIndex & proxyIndex ) const;

    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    bool insert_row(GsTLGridCategoricalProperty* prop,QColor color = QColor());
    bool insert_row(GsTLGridCategoricalProperty* prop, GsTLGridWeightProperty* weights, QColor color = QColor());
    bool insert_row(GsTLGridCategoricalProperty* prop, GsTLGridRegion* region, QColor color = QColor());

    bool insert_row(GsTLGridPropertyGroup* group, QColor color = QColor());
    bool insert_row(GsTLGridPropertyGroup* group, GsTLGridRegion* region, QColor color = QColor());
    bool insert_row(GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weights, QColor color = QColor());

    bool insert_row(Categorical_distribution* dist,QColor color = QColor());

    bool insert_row(Categorical_histogram_item* item);
    bool remove_row(Categorical_histogram_item* item);
    
    bool insert_rows(std::vector< GsTL_object_item*> items);
//    bool remove_rows(std::vector< GsTL_object_item*> items);

//	virtual QStringList mimeTypes() const;
//	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

signals :
  void data_added(Categorical_histogram_item* item);
  void data_removed(int id); 

  void data_display_changed(Categorical_histogram_item* item);
  void data_color_changed(Categorical_histogram_item* item);
  void data_visibility_changed(Categorical_histogram_item* item);
  void data_filter_changed(Categorical_histogram_item* item);
  //void distribution_display_changed(Categorical_histogram_distribution_item* item);

//  void distribution_added(Categorical_histogram_distribution_item* dist);
//  void distribution_removed(Categorical_distribution* dist);


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
  //QList< Categorical_histogram_item*> items_;
  //QHash<const GsTL_object_item*,Categorical_histogram_item*> lookup_items_;
  std::set< Categorical_histogram_item*> items_;
  //std::map<const GsTL_object_item*,Categorical_histogram_item*> lookup_items_;  
 // std::map<GsTL_object_item*,Categorical_histogram_item*> lookup_items_; 

  void find_items_to_be_removed(GsTL_object_item*, std::set< Categorical_histogram_item*>& items_to_be_removed);
  
  bool is_item_exist(GsTLGridCategoricalProperty* prop);
  bool is_item_exist(GsTLGridCategoricalProperty* prop, GsTLGridRegion* region);
  bool is_item_exist(GsTLGridCategoricalProperty* prop, GsTLGridWeightProperty* weights);
  bool is_item_exist(GsTLGridPropertyGroup* prop);
  bool is_item_exist(GsTLGridPropertyGroup* prop, GsTLGridRegion* region);
  bool is_item_exist(GsTLGridPropertyGroup* prop, GsTLGridWeightProperty* weights);


  int current_id_; //id for each element in the histogram
};

#endif // HISTOGRAM_MODEL_H
