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



#ifndef CATEGORICAL_DEFINITION_PROXY_MODEL_H
#define CATEGORICAL_DEFINITION_PROXY_MODEL_H

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>

#include <GsTLAppli/extra/qtplugins/common.h>

#include <QSortFilterProxyModel>
#include <QAbstractProxyModel>
#include <QColor>

class QTPLUGINS_DECL Categorical_item {
public :
  Categorical_item(){}
  virtual ~Categorical_item(){}

  QString category_name() const {return category_name_;}

  bool operator<(const Categorical_item& rhs ) const {
    return this->category_name_ < rhs.category_name();
  }

  bool operator==(const Categorical_item& rhs ) const {
    return this->category_name_ == rhs.category_name();
  }

  bool operator!=(const Categorical_item& rhs ) const {
    return !this->operator==(rhs);
  }

  QColor color() const {return color_;}
  void color(const QColor &color) {color_ = color;}

  void set_visibility( bool on ) {is_visible_ = on;}
  bool is_visible() {return is_visible_;}

protected :
  QString category_name_;
  QColor color_;
  bool is_visible_;

};


class QTPLUGINS_DECL Categorical_definition_proxy_model : public QAbstractProxyModel{

	Q_OBJECT
public:
	Categorical_definition_proxy_model(QObject *parent = 0);
	virtual ~Categorical_definition_proxy_model();


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

//	virtual QStringList mimeTypes() const;
//	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

protected slots :
	virtual void begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	virtual void end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);

protected :

	Manager* cat_def_manager_;
  Root_model* model_;

protected :

  QStringList cdef_names_;
  QList<CategoricalPropertyDefinitionName*> cdefs_;

};





class Category_proxy_model : public QAbstractProxyModel
{
  Q_OBJECT

public:
    Category_proxy_model(QObject *parent);
    Category_proxy_model(CategoricalPropertyDefinitionName* cdef,  QObject *parent);
    Category_proxy_model(CategoricalPropertyDefinition* cdef, int ncat,  QObject *parent);
    ~Category_proxy_model();

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

    const CategoricalPropertyDefinition* get_categorical_definition() const {return cdef_;}

signals :
    void checked_category_changed(int , bool );

public slots :
  void initialize(CategoricalPropertyDefinitionName* cdef);
  void initialize(CategoricalPropertyDefinition* cdef, int ncat);
  void use_check_state(bool on);
  void set_flags_name(Qt::ItemFlags flags) {flags_name_ = flags;}
  void set_flags_color(Qt::ItemFlags flags) {flags_color_ = flags;}
  void set_read_only();
  void clear_checked();
  void check_all();
    /*  no insertion or deletion of rows:  only the exsiting data can change
    bool insert_row(CategoricalPropertyDefinitionName* cat_def);
    bool remove_rows(std::vector< GsTL_object_item*> items);

private slots :

	void begin_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void begin_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void end_insert_proxy_rows(const QModelIndex & source_parent, int start, int end);
	void end_remove_proxy_rows(const QModelIndex & source_parent, int start, int end);
  */
private:
  
  void set_connections();

private:

  Manager* root_;
  Root_model* model_;
//  std::set<Categorical_item*> items_;
  CategoricalPropertyDefinition* cdef_;
  std::vector<bool> visibility_;
  Qt::ItemFlags flags_name_;
  Qt::ItemFlags flags_color_;
  int nrow_;
    
};




#endif // CATEGORICAL_DEFINITION_PROXY_MODEL_H
