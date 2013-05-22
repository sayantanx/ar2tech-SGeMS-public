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



#ifndef WEIGHT_PROPERTY_SELECTOR_H
#define WEIGHT_PROPERTY_SELECTOR_H

#include <QComboBox>
#include <GsTLAppli/qtplugins/common.h>
#include <GsTLAppli/qtplugins/weight_property_proxy_model.h>

class QTPLUGINS_DECL SingleWeightPropertySelector : public QComboBox
{
  Q_OBJECT

public:
    SingleWeightPropertySelector(QWidget *parent=0);
    ~SingleWeightPropertySelector();

  QString grid_name() const;
  
  public slots:
    virtual void show_properties( const QString& grid_name );
    virtual void set_selected_property( const QString& property_name );

private:
    QString current_grid_;
    //Property_proxy_model* model_;
    Weight_property_proxy_model_no_selection* model_;


    
};

#endif // WEIGHT_PROPERTY_SELECTOR_H
