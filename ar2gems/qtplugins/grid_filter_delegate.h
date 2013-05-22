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



#ifndef GRID_FILTER_DELEGATE_H
#define GRID_FILTER_DELEGATE_H

#include <GsTLAppli/qtplugins/common.h>
#include <GsTLAppli/qtplugins/selectors.h>
#include <GsTLAppli/qtplugins/categorical_selectors.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_filter.h>


#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QDialog>
#include <QFrame>
#include <QModelIndex>
#include <QDoubleSpinBox>

#include <fstream>

class QTPLUGINS_DECL Grid_filter_abstract_delegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  Grid_filter_abstract_delegate( QObject *parent =0 ) : QStyledItemDelegate(parent){}
  ~Grid_filter_abstract_delegate(){}

public slots:
  virtual void set_grid(const Geostat_grid* grid){} //=0;
  virtual void set_grid(const QString& grid_name){} //=0



};


class QTPLUGINS_DECL Grid_filter_region_delegate : public Grid_filter_abstract_delegate
{
  Q_OBJECT

public:
    Grid_filter_region_delegate( QObject *parent =0 );
    ~Grid_filter_region_delegate();

     virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
     virtual QSize sizeHint(const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     virtual void	setEditorData ( QWidget * editor, const QModelIndex & index ) const;
     virtual void	setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
     virtual void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const QModelIndex &index) const; 

public slots:
    virtual void set_grid(const Geostat_grid* grid);
    virtual void set_grid(const QString& grid_name);


private:
    const Geostat_grid* grid_;
    QString grid_name_;

};
/*
  delagate for coordinates
*/

class QTPLUGINS_DECL Grid_filter_coord_bounded_editor : public QFrame
{
  Q_OBJECT

public:
  Grid_filter_coord_bounded_editor(const Geostat_grid* grid, QWidget *parent =0);
  ~Grid_filter_coord_bounded_editor(){}

  double get_lower_bound() const { return lower_bound_spin_->value(); }
  double get_upper_bound() const { return upper_bound_spin_->value(); }
  void set_lower_bound(double lower_bound){ lower_bound_spin_->setValue(lower_bound); }
  void set_upper_bound(double upper_bound){ upper_bound_spin_->setValue(upper_bound); }

 signals:
     void editingFinished();

protected :
 // void focusOutEvent( QFocusEvent * );
 // void leaveEvent ( QEvent* );


private:
  QDoubleSpinBox* lower_bound_spin_;
  QDoubleSpinBox* upper_bound_spin_;
};

class QTPLUGINS_DECL Grid_filter_coord_bounded_delegate : public Grid_filter_abstract_delegate
{
  Q_OBJECT

public:
    Grid_filter_coord_bounded_delegate( QObject *parent =0 );
    ~Grid_filter_coord_bounded_delegate();

     virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
     virtual QSize sizeHint(const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     virtual void	setEditorData ( QWidget * editor, const QModelIndex & index ) const;
     virtual void	setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
     virtual void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const QModelIndex &index) const; 

public slots:
    virtual void set_grid(const Geostat_grid* grid);
    virtual void set_grid(const QString& grid_name);

protected slots :

 private slots:
     void commitAndCloseEditor();

private:
    const Geostat_grid* grid_;
    QString grid_name_;

};



/*
  delagate for coordinates
*/

class QTPLUGINS_DECL Grid_filter_threshold_property_editor : public QFrame
{
  Q_OBJECT

public:
  Grid_filter_threshold_property_editor(const Geostat_grid* grid, QWidget *parent =0);
  ~Grid_filter_threshold_property_editor(){}

  double get_bound() const { return bound_spin_->value(); }
  void set_bound(double lower_bound){ bound_spin_->setValue(lower_bound); }

  QString get_property() const { return prop_selector_->currentText(); }
  void set_property(QString prop_name){ prop_selector_->set_selected_property(prop_name); }


 signals:
     void editingFinished();

protected :
 // void focusOutEvent( QFocusEvent * );
 // void leaveEvent ( QEvent* );


private:
  SinglePropertySelector* prop_selector_;
  QDoubleSpinBox* bound_spin_;

};

class QTPLUGINS_DECL Grid_filter_threshold_property_delegate : public Grid_filter_abstract_delegate
{
  Q_OBJECT

public:
    Grid_filter_threshold_property_delegate( QObject *parent =0 );
    ~Grid_filter_threshold_property_delegate();

     virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
     virtual QSize sizeHint(const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     virtual void	setEditorData ( QWidget * editor, const QModelIndex & index ) const;
     virtual void	setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
     virtual void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const QModelIndex &index) const; 

public slots:
    virtual void set_grid(const Geostat_grid* grid);
    virtual void set_grid(const QString& grid_name);

protected slots :

 private slots:
     void commitAndCloseEditor();

private:

//  enum comparison_enum {GREATERTHAN, LESSTHAN};
//  comparison_enum comparison_type_; 
  const Geostat_grid* grid_;
  QString grid_name_;

};

//
//  --------------------------------------------------------
//

class QTPLUGINS_DECL Grid_filter_categorical_property_editor : public QFrame
{
  Q_OBJECT

public:
  Grid_filter_categorical_property_editor(const Geostat_grid* grid, QWidget *parent =0);
  ~Grid_filter_categorical_property_editor(){}

  QString get_property() const { return prop_selector_->currentText(); }
  void set_property(QString prop_name){ prop_selector_->set_selected_property(prop_name); }

  QString get_category() const { return category_selector_->currentText(); }
  void set_category(QString cat_name){ category_selector_->set_selected_category(cat_name); }


 signals:
     void editingFinished();

protected :
 // void focusOutEvent( QFocusEvent * );
 // void leaveEvent ( QEvent* );


private:
  SingleCategoricalPropertySelector* prop_selector_;
  SingleCategorySelector* category_selector_;

};


class QTPLUGINS_DECL Grid_filter_cateorical_property_delegate : public Grid_filter_abstract_delegate
{
  Q_OBJECT

public:
    Grid_filter_cateorical_property_delegate( QObject *parent =0 );
    ~Grid_filter_cateorical_property_delegate();

     virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
     virtual QSize sizeHint(const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
     virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     virtual void	setEditorData ( QWidget * editor, const QModelIndex & index ) const;
     virtual void	setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
     virtual void updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option, const QModelIndex &index) const; 

public slots:
    virtual void set_grid(const Geostat_grid* grid);
    virtual void set_grid(const QString& grid_name);


private:
    const Geostat_grid* grid_;
    QString grid_name_;

};

#endif // REGION_WEIGHT_DELEGATE_H
