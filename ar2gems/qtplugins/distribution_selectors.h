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



#ifndef DISTRIBUTION_SELECTORS_H
#define DISTRIBUTION_SELECTORS_H

#include <qtplugins/common.h>

#include <QListView>
#include <QTreeView>
#include <QTableView>
#include <QComboBox>

class QTPLUGINS_DECL Distribution_tree : public QTreeView
{
  Q_OBJECT

public:
    Distribution_tree(QWidget *parent);
    ~Distribution_tree();

 //   void initialize();

private:
    
};

class QTPLUGINS_DECL Continuous_distribution_table : public QTableView
{
  Q_OBJECT

public:
    Continuous_distribution_table(QWidget *parent);
    ~Continuous_distribution_table();

//  void initialize();

private:
    
};


class QTPLUGINS_DECL Continuous_distribution_selector : public QComboBox
{
  Q_OBJECT

public:
    Continuous_distribution_selector(QWidget *parent);
    ~Continuous_distribution_selector();

//  void initialize();

private:
    
};

class QTPLUGINS_DECL Categorical_distribution_table : public QTableView
{
  Q_OBJECT

public:
    Categorical_distribution_table(QWidget *parent);
    ~Categorical_distribution_table();
//  void initialize();
private:
    
};


class QTPLUGINS_DECL Distribution_type_tree : public QTreeView
{
  Q_OBJECT

public:
    Distribution_type_tree(QWidget *parent);
    ~Distribution_type_tree();

private:
    
};

class QTPLUGINS_DECL Continuous_distribution_type_table : public QTableView
{
  Q_OBJECT

public:
    Continuous_distribution_type_table(QWidget *parent);
    ~Continuous_distribution_type_table();

private:
    
};

class QTPLUGINS_DECL Categorical_distribution_type_table : public QTableView
{
  Q_OBJECT

public:
    Categorical_distribution_type_table(QWidget *parent);
    ~Categorical_distribution_type_table();

private:
    
};


#endif // DISTRIBUTION_SELECTORS_H
