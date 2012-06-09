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



#ifndef HISTOGRAM_MODEL_H
#define HISTOGRAM_MODEL_H

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/grid/grid_model/grid_weight_property.h>
#include <GsTLAppli/grid/grid_model/grid_property_set.h>
#include <GsTLAppli/math/continuous_distribution.h>

#include <QModelIndex>
#include <QVariant>
#include <QStringList>
#include <QAbstractProxyModel>
#include <QWidget>
#include <QHash>

class GUI_DECL Histogram_item {
public :
  Histogram_item(int& id):id_(id){}
  virtual ~Histogram_item(){}

  virtual int children_count() const =0;
  virtual Histogram_item* children(int row) =0;  
  virtual Histogram_item* parent() =0;  

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

  virtual bool operator<(const Histogram_item& rhs ) const {

    if(this->grid_name_ < rhs.grid_name()) return true;
    else if(this->grid_name() > rhs.grid_name()) return false;
    else return this->item_name_ <  rhs.item_name();

  }

  virtual bool operator==(const Histogram_item& rhs ) const {
    return item_name_ == rhs.item_name() &&  rhs.grid_name() == grid_name_;
  }

  virtual bool operator!=(const Histogram_item& rhs ) const {
    return !this->operator==(rhs);
  }
  /*
  float r_color() const {return color_.redF();}
  float g_color() const {return color_.greenF();}
  float b_color() const {return color_.blueF();}
  */
  QColor color() const {return color_;}
  QString display_format() const {return display_format_;}
  bool is_visible() {return is_visible_;}

  virtual void color(const QColor &color) {color_ = color;}
  /*
  virtual void r_color(float r) {color_.setRedF(r);}
  virtual void g_color(float g) {color_.setGreenF(g);}
  virtual void b_color(float b) {color_.setBlueF(b);}
  */
  virtual void display_format(QString display_format) {display_format_ = display_format;}
  virtual void set_visibility( bool on ) {is_visible_ = on;}

protected :

  const int id_;

  QString grid_name_;
  QString item_name_;  // Either the name of a property of a group

  Geostat_grid* grid_;

  QColor color_;
  QString display_format_;
  bool is_visible_;

};


class GUI_DECL Histogram_property_item :public Histogram_item {
public :
  Histogram_property_item( GsTLGridProperty* prop, int& id, Histogram_item* parent=0);
  virtual ~Histogram_property_item(){}

  virtual QString type() const {return "Property";}

  
  GsTLGridProperty* prop() {return prop_;}
  virtual GsTL_object_item* object_item() {return prop_;}

  virtual GsTLGridWeightProperty* weights() {return weights_;}
  virtual GsTLGridRegion* region() {return region_;}
  virtual void weights(GsTLGridWeightProperty* weights) {weights_ = weights;}
  virtual void region(GsTLGridRegion* region) {region_ = region;}
  virtual void weights(QString weights_name) {weights_ = grid_->weight_property(weights_name.toStdString());}
  virtual void region(QString region_name) {region_ = grid_->region(region_name.toStdString());}

  virtual int children_count() const {return 0;}
  virtual Histogram_item* children(int row) {return 0;}
  virtual Histogram_item* parent() {return parent_;};  

  /*
  virtual bool operator==(const Histogram_property_item& rhs ) const {
    return this->prop_ == rhs.prop();
  }

  virtual bool operator!=(const Histogram_property_item& rhs ) const {
    return this->prop_ != rhs.prop();
  }
  */
private :
  
  Histogram_item* parent_;
  GsTLGridProperty* prop_;
  GsTLGridRegion* region_;
  GsTLGridWeightProperty* weights_;


};


class GUI_DECL Histogram_group_item :public Histogram_item {
public :
  Histogram_group_item( GsTLGridPropertyGroup* group, int& id);
  virtual ~Histogram_group_item();

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
  virtual Histogram_item* children(int row);
  virtual Histogram_item* parent() {return 0;}  
  int row_for_children(Histogram_item*) const;

  //Need to propagate the change to underlying properties
  virtual void color(const QColor &color);
  virtual void display_format(QString line_format);
  virtual void set_visibility( bool on );


  /*
  inline bool operator==(const Histogram_group_item& rhs ) const {
    return this->group_ == rhs.group();
  }

  inline bool operator!=(const Histogram_group_item& rhs ) const {
    return this->group_ != rhs.group();
  }
  */
private :

  GsTLGridPropertyGroup* group_;
  GsTLGridRegion* region_;
  GsTLGridWeightProperty* weights_;

  std::set< Histogram_property_item*> prop_items_;

};




class GUI_DECL Histogram_distribution_item :public Histogram_item {
public :
  Histogram_distribution_item( Continuous_distribution* dist, int& id );
  virtual ~Histogram_distribution_item(){}

  virtual QString type() const {return "Distribution";}

  
  GsTLGridProperty* prop() {return 0;}
  virtual GsTL_object_item* object_item() {return dist_;}

  virtual GsTLGridWeightProperty* weights() {return 0;}
  virtual GsTLGridRegion* region() {return 0;}
  virtual void weights(GsTLGridWeightProperty* weights) {}
  virtual void region(GsTLGridRegion* region) {}
  virtual void weights(QString weights_name) {}
  virtual void region(QString region_name) {}

  virtual int children_count() const {return 0;}
  virtual Histogram_item* children(int row) {return 0;}
  virtual Histogram_item* parent() {return 0;}  


private :
  
  Histogram_item* parent_;
  Continuous_distribution* dist_;



};


class GUI_DECL Histogram_proxy_model : public QAbstractProxyModel
{
  Q_OBJECT

public:
    Histogram_proxy_model(QObject *parent);
    Histogram_proxy_model(QList< GsTL_object_item*> items, QObject *parent);
    ~Histogram_proxy_model();

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

    bool insert_row(GsTLGridProperty* prop,QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop, GsTLGridWeightProperty* weights, QColor color = QColor());
    bool insert_row(GsTLGridProperty* prop, GsTLGridRegion* region, QColor color = QColor());

    bool insert_row(GsTLGridPropertyGroup* group, QColor color = QColor());
    bool insert_row(GsTLGridPropertyGroup* group, GsTLGridRegion* region, QColor color = QColor());
    bool insert_row(GsTLGridPropertyGroup* group, GsTLGridWeightProperty* weights, QColor color = QColor());

    bool insert_row(Continuous_distribution* dist,QColor color = QColor());

    bool insert_row(Histogram_item* item);
    bool remove_row(Histogram_item* item);
    
    bool insert_rows(std::vector< GsTL_object_item*> items);
//    bool remove_rows(std::vector< GsTL_object_item*> items);

//	virtual QStringList mimeTypes() const;
//	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

signals :
  void data_added(Histogram_item* item);
  void data_removed(int id); 

  void data_display_changed(Histogram_item* item);
  void data_color_changed(Histogram_item* item);
  void data_visibility_changed(Histogram_item* item);
  void data_filter_changed(Histogram_item* item);
  void display_format_changed(Histogram_item* item);
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
  std::set< Histogram_item*> items_;
  //std::map<const GsTL_object_item*,Histogram_item*> lookup_items_;  
 // std::map<GsTL_object_item*,Histogram_item*> lookup_items_; 

  void find_items_to_be_removed(GsTL_object_item*, std::set< Histogram_item*>& items_to_be_removed);
  
  bool is_item_exist(GsTLGridProperty* prop);
  bool is_item_exist(GsTLGridProperty* prop, GsTLGridRegion* region);
  bool is_item_exist(GsTLGridProperty* prop, GsTLGridWeightProperty* weights);
  bool is_item_exist(GsTLGridPropertyGroup* prop);
  bool is_item_exist(GsTLGridPropertyGroup* prop, GsTLGridRegion* region);
  bool is_item_exist(GsTLGridPropertyGroup* prop, GsTLGridWeightProperty* weights);


  int current_id_; //id for each element in the histogram
};

#endif // HISTOGRAM_MODEL_H
