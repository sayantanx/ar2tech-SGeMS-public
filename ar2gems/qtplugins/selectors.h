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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
** Modeling Software (GEMS)
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

#ifndef __GSTLAPPLI_QTPLUGINS_SELECTORS_H__
#define __GSTLAPPLI_QTPLUGINS_SELECTORS_H__


#include <qtplugins/common.h>
#include <qtplugins/region_proxy_model.h>
#include <qtplugins/property_proxy_model.h>
#include <qtplugins/project_proxy_model.h>
#include <appli/project.h>

#include <qcombobox.h>
#include <QListWidget>
#include <QGroupBox>
#include <qdialog.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QListView>
#include <vector>
#include <set>

class QString;
class QLabel;
class QKeyEvent;


class QTPLUGINS_DECL  GridSelectorBasic : public QComboBox {

  Q_OBJECT

 public:
  GridSelectorBasic( QWidget* parent = 0, const char* name = 0,
		GsTL_project* project = 0 );
  virtual ~GridSelectorBasic();
  virtual void setCurrentText( const QString& );

  Geostat_grid* selected_grid_object();


 public:
  static const QString no_selection;


};

/*
class QTPLUGINS_DECL  GridSelectorBasic : public QComboBox, public Project_view {

  Q_OBJECT

 public:
  GridSelectorBasic( QWidget* parent = 0, const char* name = 0,
		GsTL_project* project = 0 );
  virtual ~GridSelectorBasic();
  virtual void setCurrentText( const QString& );

  virtual void init( GsTL_project* project );
  virtual void update( std::string obj );
  virtual void new_object( std::string obj );
  virtual void deleted_object( std::string obj );

 public:
  static const QString no_selection;
 signals:
  // Tells when a new object is available, ie has been added to
  // the project.
  void new_object_available( const QString& obj );
};
*/

/** Enables to choose a single property from a grid
 */ 
class QTPLUGINS_DECL  SinglePropertySelector : public QComboBox {

  Q_OBJECT

 public:
  SinglePropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~SinglePropertySelector() {}

  QString grid_name() const;
  
  public slots:
    virtual void show_properties( const QString& grid_name );
    virtual void set_selected_property( const QString& property_name );

  protected:
    QString current_grid_;
    //Property_proxy_model* model_;
    Filter_root_proxy_model* model_;
};



/** A widget to choose multiple properties from a grid
 */ 
/*
class QTPLUGINS_DECL  MultiPropertySelector : public QListView {

  Q_OBJECT

 public:
  MultiPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultiPropertySelector() {}

  public slots:
    void show_properties( const QString& grid_name );

  protected slots:
    void selection_size();

 signals:
  void selected_count( int );

  protected:
    QString current_grid_;
    Property_proxy_model* model_;
};
*/
class QTPLUGINS_DECL  MultiPropertySelector : public QListWidget {

  Q_OBJECT

 public:
  MultiPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultiPropertySelector() {}
  
  public slots:
    virtual void show_properties( const QString& grid_name );

  protected slots:
    void selection_size();

 signals:
  void selected_count( int );
};





class QTPLUGINS_DECL  PropertySelectorNoRegion : public QWidget {

  Q_OBJECT

 public:
  PropertySelectorNoRegion( QWidget* parent = 0, const char* name = 0,
		    GsTL_project* project = 0 );
  virtual ~PropertySelectorNoRegion() {};

  virtual void init( GsTL_project* project ) { }//object_selector_->init( project ); }

  QString selectedGrid() const;
  void setSelectedGrid( const QString& name );
  QString selectedProperty() const;
  void setSelectedProperty( const QString& name );
  virtual QSize sizeHint() const { return QSize( 180, 40 ); }

// public slots:
//  void update_properties( const QString& obj );  

 protected slots:
   void forward_property_changed();
   
 signals:
  void property_changed( const QString& string );
  void object_changed( const QString& string );
  void new_object_available( const QString& obj );


 protected:
  GridSelectorBasic* object_selector_;
  SinglePropertySelector* property_selector_;

};





/*
class QTPLUGINS_DECL  Ordered_property_selector_dialog
    : public QDialog {

  Q_OBJECT

public:
  Ordered_property_selector_dialog( QWidget* parent = 0, const char* name = 0 );
  virtual ~Ordered_property_selector_dialog() {}
  
  QStringList selection() const;

public slots:
  void show_properties( const QString& grid_name );
  void set_current_selection( const QStringList& list );
 
protected slots:
  void move_selected_item_up();
  void move_selected_item_down();

  void left2right();
  void right2left();
  void remove_selected_item();

protected:
  MultiPropertySelector* property_selector_;
  QListBox* selected_properties_;
};
*/

#include <qtplugins/ui_order_properties_dialog.h>

class QTPLUGINS_DECL  Ordered_property_selector_dialog
    : public QDialog, public Ui::Ordered_property_selector_dialog_base {

  Q_OBJECT

public:
  Ordered_property_selector_dialog( QWidget* parent = 0, const char* name = 0 );
  virtual ~Ordered_property_selector_dialog() {}
  
  QStringList selection() const;

public slots:
  void show_properties( const QString& grid_name );
  void set_current_selection( const QStringList& list );
 
signals :
	void selectedProperties(const QStringList&);

protected slots:
  void move_selected_item_up();
  void move_selected_item_down();
  void left2right();
  void right2left();
  void remove_selected_item();

protected:
  MultiPropertySelector* property_selector_;
};


class QTPLUGINS_DECL  GsTLGroupBox : public QGroupBox {

  Q_OBJECT
  Q_PROPERTY( QString keyword READ keyword WRITE set_keyword DESIGNABLE true STORED true )
  Q_PROPERTY( bool displayed READ is_displayed WRITE set_displayed DESIGNABLE true STORED true )
  Q_PROPERTY( bool activated READ is_activated WRITE set_activated DESIGNABLE true STORED true )

 public:
  GsTLGroupBox( QWidget* parent = 0, const char* name = 0,
		const QString& keyword="Abracadabra",
    bool displayed = true, bool activated = true  );
  GsTLGroupBox(const QString &title, QWidget *parent);
/*  GsTLGroupBox( int strips, Qt::Orientation orientation,
		QWidget* parent = 0, const char* name = 0,
		const QString& keyword = "Abracadabra",
		bool displayed = true, bool activated = true  );
 */ 
  QString keyword() const;
  void set_keyword( const QString& keyword );

  bool is_displayed() const;
  void set_displayed( bool on );
  bool is_activated() const { return activated_; }
  void set_activated( bool on ) { activated_ = on; }

 public slots:
  virtual void setHidden( bool on );
  virtual void setHidden( const QString& keyword );
  virtual void setShown( bool on );
  virtual void setShown( const QString& keyword );

  virtual void setDeActivated( bool on );
  virtual void setDeActivated( const QString& keyword );
  virtual void setActivated( bool on );
  virtual void setActivated( const QString& keyword );

  virtual void setEnabled( bool on );
  virtual void setEnabled( const QString& keyword );
  virtual void setDisabled( bool on );
  virtual void setDisabled( const QString& keyword );

 protected:
  QString keyword_;
  std::set< QString > keywords_map_;
  bool displayed_;
  bool activated_;
};



class QTPLUGINS_DECL  GsTLButtonGroup : public QGroupBox {

  Q_OBJECT
  Q_PROPERTY( QString keyword READ keyword WRITE set_keyword DESIGNABLE true STORED true )
  Q_PROPERTY( bool displayed READ is_displayed WRITE set_displayed DESIGNABLE true STORED true )
  Q_PROPERTY( bool activated READ is_activated WRITE set_activated DESIGNABLE true STORED true )

 public:
  GsTLButtonGroup( QWidget* parent = 0, const char* name = 0,
		               const QString& keyword = "Abracadabra",
		               bool displayed = true, bool activated = true  );
  QString keyword() const;
  void set_keyword( const QString& keyword );

  bool is_displayed() const { return displayed_; }
  void set_displayed( bool on ) { displayed_ = on; }
  bool is_activated() const { return activated_; }
  void set_activated( bool on ) { activated_ = on; }

 public slots:
  virtual void setHidden( bool on );
  virtual void setHidden( const QString& keyword );
  virtual void setShown( bool on );
  virtual void setShown( const QString& keyword );
  
  virtual void setDeActivated( bool on );
  virtual void setDeActivated( const QString& keyword );
  virtual void setActivated( bool on );
  virtual void setActivated( const QString& keyword );

 protected:
  QString keyword_;
  bool displayed_;
  bool activated_;

};


class QTPLUGINS_DECL  CloneGroupBox : public GsTLGroupBox {

  Q_OBJECT

 public:
  CloneGroupBox( QWidget* parent = 0, const char* name = 0 );
  int count() const { return count_; }
  void init_cloning();

  static QWidget* clone_widget( QWidget* widget, QWidget* parent = 0,
                                const char* name = 0 );

 public slots:
  void set_count( int n );

 protected:
  int count_;
  std::vector< QWidget* > cloned_widgets_;
  std::vector< QWidget* > labels_;
  QWidget* main_widget_;
  QLabel* main_label_;
  int adjust_;
  QLayout * layout_;
};



class QTPLUGINS_DECL  OrderedPropertySelector 
    : public GsTLGroupBox {

  Q_OBJECT

public:
  OrderedPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~OrderedPropertySelector() {}
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
  MultiPropertySelector* selected_properties_;
  QString grid_name_;  
};


/** Enables to choose a single region from a grid
 */ 
class QTPLUGINS_DECL  SingleRegionSelector : public QComboBox {

  Q_OBJECT

 public:
  SingleRegionSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~SingleRegionSelector() {}
  
  public slots:
    void show_regions( const QString& grid_name );

  protected:
    QString current_grid_;
};



/** A widget to choose multiple properties from a grid
 */ 
class QTPLUGINS_DECL  MultiRegionSelector : public QListWidget {

  Q_OBJECT

 public:
  MultiRegionSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultiRegionSelector() {}
  
  public slots:
    void show_regions( const QString& grid_name );

  protected slots:
    void selection_size();

 signals:
  void selected_count( int );
};


class QTPLUGINS_DECL  GridSelector : public QWidget {

  Q_OBJECT

 public:
  GridSelector( QWidget* parent = 0, const char* name = 0,
		    GsTL_project* project = 0 );
  virtual ~GridSelector() {};

  virtual void init( GsTL_project* project ) { }//object_selector_->init( project ); }

  QString selectedGrid() const;
  Geostat_grid* selected_grid_object() {return object_selector_->selected_grid_object();}
  void setSelectedGrid( const QString& name );
  QString selectedRegion() const;
  void setSelectedRegion( const QString& name );
  virtual QSize sizeHint() const { return QSize( 180, 40 ); }

// public slots:
//  void update_properties( const QString& obj );  

 protected slots:
   void forward_region_changed();
   void select_regions();
   void set_selection_tip();
   void update_region_model();
 //  void clear_region_selection();
   
 signals:
  void region_changed( const QString& string );
  void object_changed( const QString& string );
  void new_object_available( const QString& obj );
  // not defined in QTabWidget
  void activated( const QString& obj );


 protected:

  //Region_proxy_model_no_selection* region_model_;
  Filter_root_proxy_model* region_model_;
  QDialog* region_dialog_;
  QListView* region_selector_;
  GridSelectorBasic* object_selector_;
  QPushButton* region_button_;

};

/*
class QTPLUGINS_DECL  GridSelector : public QTabWidget {

  Q_OBJECT

 public:
  GridSelector( QWidget* parent = 0, const char* name = 0,
		    GsTL_project* project = 0 );
  virtual ~GridSelector() {};

  virtual void init( GsTL_project* project ) { }//object_selector_->init( project ); }

  QString selectedGrid() const;
  void setSelectedGrid( const QString& name );
  QString selectedRegion() const;
  void setSelectedRegion( const QString& name );
  virtual QSize sizeHint() const { return QSize( 180, 40 ); }

// public slots:
//  void update_properties( const QString& obj );

 protected slots:
   void forward_region_changed();

 signals:
  void region_changed( const QString& string );
  void object_changed( const QString& string );
  void new_object_available( const QString& obj );
  // not defined in QTabWidget
  void activated( const QString& obj );


 protected:
 // QTabWidget* tab_;
  GridSelectorBasic* object_selector_;
  SingleRegionSelector* region_selector_;

};
*/


class QTPLUGINS_DECL  PropertySelector : public QWidget {

  Q_OBJECT

 public:
  PropertySelector( QWidget* parent = 0, const char* name = 0,
		    GsTL_project* project = 0 );
  virtual ~PropertySelector() {};

  virtual void init( GsTL_project* project ) { object_selector_->init( project ); }

  QString selectedGrid() const;
  void setSelectedGrid( const QString& name );
  QString selectedRegion() const;
  void setSelectedRegion( const QString& name );
  QString selectedProperty() const;
  void setSelectedProperty( const QString& name );
  virtual QSize sizeHint() const { return QSize( 180, 40 ); }

// public slots:
//  void update_properties( const QString& obj );  

 protected slots:
   void forward_property_changed();
   void forward_region_changed();
   
 signals:
  void property_changed( const QString& string );
  void region_changed( const QString& string );
  void object_changed( const QString& string );
  void new_object_available( const QString& obj );


 protected:

  GridSelector* object_selector_;
  SinglePropertySelector* property_selector_;

};



#endif
