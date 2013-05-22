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



#ifndef CATEGORICAL_TABLE_MODEL_H
#define CATEGORICAL_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QColor>
#include <QPair>

class Categorical_table_model : public QAbstractTableModel
{
  Q_OBJECT

public:
    Categorical_table_model(QObject *parent);
    ~Categorical_table_model();

    virtual int rowCount(const QModelIndex & parent = QModelIndex() ) const {return items_.size();}
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const{return 3;}
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual QVariant	headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    
    virtual Qt::ItemFlags flags(const QModelIndex & index ) const;

public slots:
  void set_number_of_categories(int ncat);


private:
  
  struct cat_code {
    int code;
    QString name;
    QColor color;
  };

  QList<cat_code> items_;
  
 // QList<QPair<QString, QColor> > items_;
};

#endif // CATEGORICAL_TABLE_MODEL_H
