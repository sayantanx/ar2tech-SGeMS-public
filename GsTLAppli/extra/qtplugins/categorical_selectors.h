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



/**********************************************************************
** Author: Alexandre Boucher
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Stanford Geostatistical Earth
** Modeling Software (SGEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#ifndef __GSTLAPPLI_QTPLUGINS_CATEGORICALSELECTORS_H__
#define __GSTLAPPLI_QTPLUGINS_CATEGORICALSELECTORS_H__


#include <GsTLAppli/extra/qtplugins/common.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/categorical_definition_proxy_model.h>
#include <GsTLAppli/appli/project.h>

#include <qcombobox.h>
#include <QListWidget>
#include <QGroupBox>
#include <qdialog.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QTableView>

#include <vector>
#include <set>

class QString;
class QLabel;
class QKeyEvent;

class GsTLGridProperty;
class CategoricalPropertyDefinition;

class QTPLUGINS_DECL  SingleCategoricalPropertySelector : public SinglePropertySelector {

  Q_OBJECT

 public:
  SingleCategoricalPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~SingleCategoricalPropertySelector() {}
  
  public slots:
    virtual void show_properties( const QString& grid_name );

};


/** A widget to choose multiple properties from a grid
 */ 
class QTPLUGINS_DECL  MultiCategoricalPropertySelector : public MultiPropertySelector {

  Q_OBJECT

 public:
  MultiCategoricalPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultiCategoricalPropertySelector() {}
  
  public slots:
    virtual void show_properties( const QString& grid_name );

};



class QTPLUGINS_DECL  Ordered_categorical_property_selector_dialog
    : public Ordered_property_selector_dialog {

  Q_OBJECT

public:
  Ordered_categorical_property_selector_dialog( QWidget* parent = 0, const char* name = 0 );
  virtual ~Ordered_categorical_property_selector_dialog() {}

public slots:
  void show_properties( const QString& grid_name );

protected:
  //MultiCategoricalPropertySelector* property_selector_;
};


class QTPLUGINS_DECL  OrderedCategoricalPropertySelector 
    : public GsTLGroupBox {

  Q_OBJECT

public:
  OrderedCategoricalPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~OrderedCategoricalPropertySelector() {}
  QStringList selected_properties() const;
  void set_selected_properties( QStringList list );

public slots:
  void show_properties( const QString& grid_name );
  virtual void show_selection_dialog();
  void forwardSelection(int);

signals:
  void selected_count( int );
  void forwardSelectedProperties(const QStringList&);

protected:
  MultiCategoricalPropertySelector* selected_properties_;
  QString grid_name_;
};



/** Enables to choose a categorical property definition
 */ 
class QTPLUGINS_DECL CategoricalDefinitionSelector : public QComboBox {

  Q_OBJECT

 public:
  CategoricalDefinitionSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~CategoricalDefinitionSelector() {}
  
  void initiliaze();

  public slots:
    virtual void show_definitons( );
};


/** A widget to choose multiple categorical property definition
 */ 
class QTPLUGINS_DECL  MultiCategoricalDefinitionSelector : public QListWidget {

  Q_OBJECT

 public:
  MultiCategoricalDefinitionSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultiCategoricalDefinitionSelector() {}
  
  public slots:
    void show_definitions( );

  protected slots:
    void selection_size();

 signals:
  void selected_count( int );
};


/** A widget to choose multiple categorical property definition
 */ 
class QTPLUGINS_DECL  CategoricalDefinitionViewer : public QGroupBox {

  Q_OBJECT

 public:
  CategoricalDefinitionViewer( QWidget* parent = 0, const char* name = 0 );
  virtual ~CategoricalDefinitionViewer() {}
  
  public slots:
    void show_definition( QString );

protected: 
  QVBoxLayout* baseLayout_;
  QString current_definition_;

};



/** A widget to choose multiple categorical property definition
 */ 
class QTPLUGINS_DECL  CategoricalDefinitionTable : public QTableView {

  Q_OBJECT

 public:
  CategoricalDefinitionTable( QWidget* parent = 0, const char* name = 0 );
  virtual ~CategoricalDefinitionTable() {}

  void initialize();
  QString get_definition_name() const {return current_definition_;}

 public slots:
  void show_definition( QString cdef_name);
  void show_definition( CategoricalPropertyDefinition* cdef );
  void show_definition( int ncat );
  void show_color_editor(const QModelIndex& index);
  void set_flags_name(Qt::ItemFlags flags) {model_->set_flags_name(flags);}
  void set_flags_color(Qt::ItemFlags flags) {model_->set_flags_color(flags);}

protected: 

  Category_proxy_model* model_;

  QString current_definition_;

};


/** A widget to choose multiple categories from a property
 */

/*
class QTPLUGINS_DECL  MultipleCategorySelector : public QTableView {

  Q_OBJECT

 public:
  MultipleCategorySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultipleCategorySelector() {}

  public slots:
    void show_categories( const QString& cat_def_name);
    void show_categories( const GsTLGridProperty* prop);
    void show_categories( const CategoricalPropertyDefinition* cdef);
    void show_default_categories( int number_of_category );

  protected slots:
    void selection_size();

    signals:
     void selected_count( int );

protected:


};
*/



class QTPLUGINS_DECL  MultipleCategorySelector : public QListWidget {

  Q_OBJECT

 public:
  MultipleCategorySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultipleCategorySelector() {}

  public slots:
    void show_categories( const QString& cat_def_name);
    void show_categories( const GsTLGridProperty* prop);
    void show_categories( const CategoricalPropertyDefinition* cdef);
    void show_default_categories( int number_of_category );

  protected slots:
    void selection_size();

    signals:
     void selected_count( int );

protected:


};


#endif
