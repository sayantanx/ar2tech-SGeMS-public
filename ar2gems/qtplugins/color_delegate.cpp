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





#include <qtplugins/color_delegate.h>




Color_delegate::Color_delegate(QObject *parent)
  : QStyledItemDelegate(parent)
{

}

Color_delegate::~Color_delegate()
{

}

 void Color_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
 {
  if (qVariantCanConvert<QColor>(index.data())) {
    QColor color = qVariantValue<QColor>(index.data());
    painter->fillRect(option.rect, color);
     /*
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
        */
  } else {
      QStyledItemDelegate::paint(painter, option, index);
  }
}


 // Cannot use the normal channel to create an editor
 // For an editor to show up we will use the double click signal from the model
 QWidget *Color_delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const

 {
     if (qVariantCanConvert<QColor>(index.data())) {
       return 0;
     } else {
         return QStyledItemDelegate::createEditor(parent, option, index);
     }
 }


QSize Color_delegate::sizeHint(const QStyleOptionViewItem &option,
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