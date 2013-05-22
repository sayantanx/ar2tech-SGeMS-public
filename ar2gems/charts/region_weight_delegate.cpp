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




#include <charts/region_weight_delegate.h>
#include <charts/chart_model_item.h>

#include <QComboBox>


Region_weight_delegate::Region_weight_delegate( QObject *parent)
  : QStyledItemDelegate(parent)
{

}

Region_weight_delegate::~Region_weight_delegate()
{

}

 void Region_weight_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
 {
   QStyledItemDelegate::paint(painter, option, index);
}



 QWidget *Region_weight_delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const

 {
   
   Chart_item *item = static_cast<Chart_item*>(index.internalPointer());

   QComboBox* options = new QComboBox(parent);

   options->addItem("<-None->");

  std::list<std::string> regions = item->grid()->region_list();
  for(std::list<std::string>::const_iterator it = regions.begin(); it != regions.end();  ++it) 
    options->addItem(QIcon(":/icons/appli/Pixmaps/region.svg"), it->c_str());
      
  std::list<std::string> weights = item->grid()->weight_property_list();
  for(std::list<std::string>::const_iterator it = weights.begin(); it != weights.end();  ++it) 
    options->addItem(QIcon(":/icons/appli/Pixmaps/weight_property.svg"), it->c_str());

  return options;

 }

void	Region_weight_delegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const{
  QComboBox* edit = static_cast<QComboBox*>(editor);
  int i = edit->findText(index.data().toString());
  edit->setCurrentIndex(i);
}

void	Region_weight_delegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const{
  QComboBox* edit = static_cast<QComboBox*>(editor);
  int i = edit->currentIndex();

  if(i==0) model->setData(index, QVariant(), Qt::EditRole );

  QString option = edit->itemText(i);

  Chart_item *item = static_cast<Chart_item*>(index.internalPointer());
   
   if(  i <= item->grid()->region_list().size() ) {
     option.append("_R_");
   }
   else {
     option.append("_W_");
   }
  model->setData(index, option, Qt::EditRole );

}


QSize Region_weight_delegate::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
  /*
    if (qVariantCanConvert<QColor>(index.data())) {
        QColor color = qVariantValue<QColor>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    */
}

void Region_weight_delegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }