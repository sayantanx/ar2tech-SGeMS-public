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



#ifndef COLOR_DELEGATE_H
#define COLOR_DELEGATE_H

#include <GsTLAppli/extra/qtplugins/common.h>

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QModelIndex>

class QTPLUGINS_DECL Color_delegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
    Color_delegate(QObject *parent=0);
    ~Color_delegate();

     void paint(QPainter *painter, const QStyleOptionViewItem &option,
                const QModelIndex &index) const;
     QSize sizeHint(const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;


private:
    
};

#endif // COLOR_DELEGATE_H
