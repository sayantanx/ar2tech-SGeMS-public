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

#ifndef __GSTLAPPLI_QTPLUGINS_KRIGING_TYPE_SELECTOR_H__
#define __GSTLAPPLI_QTPLUGINS_KRIGING_TYPE_SELECTOR_H__

#include <GsTLAppli/extra/qtplugins/common.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>

#include <QString>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>


#include <vector>
#include <utility>


class QTPLUGINS_DECL  Drift_components : public QWidget {

  Q_OBJECT

public:
  Drift_components( QWidget* parent = 0 );
  virtual ~Drift_components();

  void set_drift_components( const std::vector<std::string>& components );

  QStringList drift_components() const;
  QString grid_name() const;

  void set_button_text(const QString& text);

public slots:
	void set_grid_name(const QString& grid_name);
	void set_drift_components( const QStringList& components );

protected slots:
	void show_selection_dialog();


protected :
	void set_selection_tip();

protected:
  QString grid_name_;
  QStringList drifts_;
  QPushButton* drifts_button_;

};





class QTPLUGINS_DECL  TrendComponents : public QWidget {

  Q_OBJECT

public:
  TrendComponents( QWidget* parent = 0, const char* name = 0 );
  virtual ~TrendComponents();

  void setTrendComponents( const std::vector<bool>& components );
  std::vector<bool> trendComponents() const;
  QStringList drift_components() const;
  QStringList hd_drift_components() const;

  void set_drift_components(const QStringList& drifts) const;
  void set_hd_drift_components(const QStringList& drifts) const;

public slots:
	void set_grid_name(const QString& grid_name);
  void set_hd_grid_name(const QString& grid_name);

protected:
  QCheckBox* x_;
  QCheckBox* y_;
  QCheckBox* z_;
  QCheckBox* xz_;
  QCheckBox* xy_;
  QCheckBox* yz_;
  QCheckBox* z2_;
  QCheckBox* y2_;
  QCheckBox* x2_;

  Drift_components* drifts_;
  Drift_components* hd_drifts_;
};


class QTPLUGINS_DECL KrigingTypeSelector : public QWidget {

  Q_OBJECT

 public:
  KrigingTypeSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~KrigingTypeSelector() {}
//  virtual QSize sizeHint() const { return QSize( 245, 80 ); }

  void setKrigingType( const QString& type );
  QString krigingType() const;

  void setSkMean( float m );
  float skMean() const;

  void setTrendComponents( const std::vector<bool>& components );
  std::vector<bool> trendComponents() const;

  void setLocalMeanProperty( const std::pair<QString, QString>& prop );
  std::pair<QString, QString> LocalMeanProperty() const;

  QString local_mean_property() const;
  QString hard_data_local_mean_property() const;
  void set_local_mean_property(const QString& prop);
  void set_hard_data_local_mean_property(const QString& prop);


  QStringList drift_components() const;
  QStringList hd_drift_components() const;

  void set_drift_components(const QStringList& drifts) const;
  void set_hd_drift_components(const QStringList& drifts) const;

  QString get_grid() const;
  QString get_hd_grid() const;

 public slots:
  void set_grid( const QString& grid_name );
  void set_hd_grid( const QString& grid_name );
  void promptForParameters( int id );

 signals:
  void krigingTypeChanged( const QString& );
  void krigingParametersChanged();

protected :
  QString grid_name_;
  QString hd_grid_name_;
  TrendComponents* trends_;
  QComboBox* kriging_type_;
  QGroupBox* sk_mean_box_;
  QLineEdit* sk_mean_edit_;

  QFrame* lvm_box_;
  QFrame* lvm_grid_box_;
  QFrame* lvm_hd_box_;
  SinglePropertySelector* local_mean_property_;
  SinglePropertySelector* hd_local_mean_property_;

};

class QTPLUGINS_DECL HardDataKrigingPropertySelector : public PropertySelector {
	Q_OBJECT
public:
	HardDataKrigingPropertySelector(QWidget* parent);
	~HardDataKrigingPropertySelector();

	QString get_kriging_type();
	QString get_domain_grid();
	bool is_kriging_field_needed();

	Drift_components* get_drift_widget();
	SinglePropertySelector* get_lvm_widget();

public slots :
	void set_kriging_type(const QString& kriging_type);
	void set_domain_grid(const QString& grid_name);

protected :
	QString domain_grid_name_;  // This is the name of the grid where the estimation/simulation is done
	QString kriging_type_;
	QGroupBox* lvm_box_;
	Drift_components* drifts_;
    SinglePropertySelector* local_mean_property_;
};

#endif
