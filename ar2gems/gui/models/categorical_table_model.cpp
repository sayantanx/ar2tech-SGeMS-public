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



#include  <GsTLAppli/gui/models/categorical_table_model.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>

Categorical_table_model::Categorical_table_model(QObject *parent)
  : QAbstractTableModel(parent)
{

}

Categorical_table_model::~Categorical_table_model()
{

}


QVariant Categorical_table_model::headerData ( int section, Qt::Orientation orientation, int role) const{

  if(role != Qt::DisplayRole) return QVariant();
  if(orientation == Qt::Horizontal) {  // Column Number
    if(section == 0) return "Code";    
    else if (section == 1) return "Name of category";    
    else if(section == 2) return "Color";    
    else return QVariant();
  }/*
  else {  // Vertical
    return QString("Code %1").arg(section);
  }
  */
  return QVariant();
}

QVariant Categorical_table_model::data ( const QModelIndex & index, int role ) const{

  if(!index.isValid()) return QVariant();

  if(role == Qt::DisplayRole) {
    int col = index.column();
    if(col ==0) return QVariant(items_.at(index.row()).code);
    else if(col == 1) return items_.at(index.row()).name;
    else if(col == 2) return items_.at(index.row()).color;
    else return QVariant();
  }

  else if(role == Qt::EditRole) {
    int col = index.column();
    if(col ==0) return QVariant(items_.at(index.row()).code);
    else if(col == 1) return items_.at(index.row()).name;
    else if(col == 2) return items_.at(index.row()).color;
    else return QVariant();
  }

  return QVariant();

}

bool Categorical_table_model::setData( const QModelIndex & index, const QVariant & value, int role ){

  if(role !=  Qt::EditRole) return false;

  int col = index.column();
  if( col == 0 ) {
    items_[index.row()].code = value.toInt();
    return true;
  }
  else if( col == 1 ) {
    items_[index.row()].name = value.toString();
    return true;
  }
  else if ( col == 2 ) {
    QColor color = value.value<QColor>();
    if(!color.isValid()) return false;
    items_[index.row()].color = color;
    return true;
  }
  else return false;

}

Qt::ItemFlags Categorical_table_model::flags(const QModelIndex & index ) const { 
  
  return Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;

}

void Categorical_table_model::set_number_of_categories(int ncat){
  if( items_.size() > ncat ) {
    int n = items_.size() - ncat ;
    for (int i = 0 ; i< n; ++i )
      items_.removeLast();
    this->reset();    

  }
  else if(items_.size() < ncat) {
    int n = items_.size();
    for (int i = n ; i< ncat; ++i ) {
      cat_code cat;
      cat.code = i;
      cat.name = QString("Category %1").arg(i);
      cat.color = CategoricalPropertyDefinitionDefault::default_color(i);
      items_.append(cat);
    }
    this->reset();
  }
}
