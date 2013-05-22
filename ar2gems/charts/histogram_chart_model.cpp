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




#include <charts/histogram_chart_model.h>
#include <utils/manager.h>

#include <QStringList>

Continuous_histo_chart_model::Continuous_histo_chart_model(QObject *parent)
  : QSortFilterProxyModel(parent)
{
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
	this->setSourceModel(model_);
  this->setDynamicSortFilter(true);
}

Continuous_histo_chart_model::~Continuous_histo_chart_model()
{

}

 bool Continuous_histo_chart_model::filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const
 {
   QModelIndex index1 = this->sourceModel()->index(sourceRow, 1, sourceParent);

   QString type = this->sourceModel()->data(index1).toString();
   if( type == "Factory"  || type == "CategoricalProperty") return false;

   //Check is a group is made solely of categorical property
   if(type == "Group") {
     int n =  this->sourceModel()->rowCount(index1);
     int count = 0;
     for(int i=0;i<n;++i) {
       if (this->sourceModel()->data( index1.child(i,0)).toString() == "CategoricalProperty" ) count++;
     }
     if( count == n ) return false; //All the member properties are categorical, so do not accept that group
   }

   return true;
 }

 QModelIndex Continuous_histo_chart_model::root_index(){
   return this->mapFromSource(model_->project_root_index());

 }

 QMimeData *Continuous_histo_chart_model::mimeData(const QModelIndexList &indexes) const
{
    if (indexes.count() <= 0)
        return 0;
    QStringList types = mimeTypes();
    if (types.isEmpty())
        return 0;
    QMimeData *data = new QMimeData();
    QString format = types.at(0);
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);
    QModelIndexList::ConstIterator it = indexes.begin();
    for (; it != indexes.end(); ++it) {

      QModelIndex src_index = this->mapToSource(*it);
      GsTL_object_item* item = static_cast<GsTL_object_item*>(src_index.internalPointer());
      QString name = item->item_name();
      QString type = item->item_type();
      if(type == "Grid") continue;
      QMap<int, QVariant> data_encoded;
      data_encoded.insert(0, name);
      data_encoded.insert(1, type);
      //Need to send the grid_name
      if(type.startsWith("Group:") ) {
        data_encoded.insert(2, item->parent()->item_name());
      }
      else {  // all other data are two steps removed from the grid (except the grid themselves)
        data_encoded.insert(2, item->parent()->parent()->item_name());
      }
      stream << (*it).row() << (*it).column() << data_encoded;
    }

    data->setData(format, encoded);
    return data;
}
 /*
 void QAbstractItemModel::encodeData(const QModelIndexList &indexes, QDataStream &stream) const
{
    QModelIndexList::ConstIterator it = indexes.begin();
    for (; it != indexes.end(); ++it)
        stream << (*it).row() << (*it).column() << itemData(*it);
}

 QMap<int, QVariant> QAbstractItemModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> roles;
    for (int i = 0; i < Qt::UserRole; ++i) {
        QVariant variantData = data(index, i);
        if (variantData.isValid())
            roles.insert(i, variantData);
    }
    return roles;
}
*/

 /*
  -------------------------
 */

 Categorical_histo_chart_model::Categorical_histo_chart_model(QObject *parent)
  : QSortFilterProxyModel(parent)
{
  model_ = dynamic_cast<Root_model*>(Root::instance()->model());
	this->setSourceModel(model_);
  this->setDynamicSortFilter(true);
}

Categorical_histo_chart_model::~Categorical_histo_chart_model()
{

}

 bool Categorical_histo_chart_model::filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const
 {
   QModelIndex index1 = this->sourceModel()->index(sourceRow, 1, sourceParent);

   QString type = this->sourceModel()->data(index1).toString();
   if( type == "Factory"  || type == "ContinuousProperty") return false;

   //Check is a group is made solely of continuous property
   if(type == "GsTLGridGroup") {
     int n =  this->sourceModel()->rowCount(index1);
     int count = 0;
     for(int i=0;i<n;++i) {
       if (this->sourceModel()->data( index1.child(i,0)).toString() == "ContinuousProperty" ) count++;
     }
     if( count == n ) return false; //All the member properties are continuous, so do not accept that group
   }

   return true;
 }

QModelIndex Categorical_histo_chart_model::root_index(){
  return this->mapFromSource(model_->project_root_index());

}
