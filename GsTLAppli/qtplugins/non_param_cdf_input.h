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



#ifndef NONPARAMCDFINPUT_H
#define NONPARAMCDFINPUT_H

#include <GsTLAppli/qtplugins/common.h>

#include <QWidget>
#include <QLabel>
#include <vector>



class QComboBox;
class PropertySelector;
class Continuous_distribution_selector;
class FileChooser;
class QLabel;
class QLineEdit;
class QRadioButton;


class QTPLUGINS_DECL  TailCdfInput : public QWidget {

  Q_OBJECT

 public:

  TailCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~TailCdfInput() {}

  QString getFunctionType() const;
  float getOmega() const;
  float getExtremeValue() const;
  QString getOmegaText() const;
  QString getExtremeValueText() const;
  void setOmega( QString minStr );
  void setExtremeValue( QString omegaStr );
  void setFunctionType(QString functionType);

 public slots:

  void setExtremeBoxEnabbled(int index);
  void setOmegaBoxEnabbled(int index);

protected :

	QComboBox* function_type;
  QLabel* min_max_label_;
  QLineEdit* extremeValue;
  QLabel* omega_label;
  QLineEdit* omega;

  std::vector<QString> function_type_index;

};





class QTPLUGINS_DECL LowerTailCdfInput : public TailCdfInput {

  Q_OBJECT

 public:

  LowerTailCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~LowerTailCdfInput() {}

};





class QTPLUGINS_DECL UpperTailCdfInput : public TailCdfInput {

  Q_OBJECT

 public:
  UpperTailCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~UpperTailCdfInput() {}
};







//======================================



class QTPLUGINS_DECL NonParamCdfInput : public QWidget {

  Q_OBJECT

 public:

  NonParamCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~NonParamCdfInput() {}

  QString getRefDistributionName() const;
  QString getRefGridName() const;
  QString getRefPropName() const;
  QString getRefRegionName() const;
  QString getRefFileName() const;
  QString getRefWeightName() const;

  bool isRefInDistribution() const;
  bool isRefOnFile() const;
  bool isRefOnGrid() const;
  bool isTieBreaking() const;

  void setRefDistributionName(QString distributionName);
  void setRefGridName(QString gridName);
  void setRefPropName(QString propName);
  void setRefRegionName(QString regionName);
  void setRefFileName(QString fileName);
  void setRefWeightName(QString weightName);
  
  void setRefInDistribution(bool on);
  void setRefOnFile(bool on);
  void setRefOnGrid(bool on);
  void setTieBreaking(bool on );

	LowerTailCdfInput* LTI;
	UpperTailCdfInput* UTI;

  public slots:

  void setReferenceGrid( const QString& grid_name);
  void setReferenceProperty(const QString& prop_name);

protected :
  Continuous_distribution_selector* distributionSelector;
	PropertySelector* refPropSelector;
	FileChooser*  refFileChooser;
  QRadioButton* from_file; 
  QRadioButton* from_grid;
  QRadioButton* from_distribution;
  QRadioButton* break_ties;

};





#endif



