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

#include <GsTLAppli/gui/utils/upscale_property_dialog.h>
#include <GsTLAppli/actions/obj_manag_actions.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/group_selectors.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/error_messages_handler.h>

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QCheckBox>


Upscale_property_dialog::Upscale_property_dialog(QWidget *parent)
  : QDialog(parent)
{
  this->setWindowTitle("Block upscaling of properties");

  QVBoxLayout* main_layout = new QVBoxLayout(this);
  this->setLayout(main_layout);

  QHBoxLayout* source_layout = new QHBoxLayout(this);
  source_layout->addWidget(new QLabel("Source Grid",this));
  source_grid_selector_ = new GridSelector(this);
  source_layout->addWidget(source_grid_selector_);
  main_layout->addLayout(source_layout);

  QHBoxLayout* target_layout = new QHBoxLayout(this);
  target_layout->addWidget(new QLabel("Target Grid",this));
  target_grid_selector_ = new GridSelector(this);
  target_layout->addWidget(target_grid_selector_);
  main_layout->addLayout(target_layout);


  QButtonGroup* button_group = new QButtonGroup(this);
  use_group_ = new QRadioButton("From group",this);
  use_properties_ = new QRadioButton("From selected properties",this);
  use_group_->setChecked(true);
  use_properties_->setChecked(false);
  button_group->addButton(use_group_);
  button_group->addButton(use_properties_);
  button_group->setExclusive(true);

  QHBoxLayout* group_button_layout = new QHBoxLayout( this);
  group_button_layout->addWidget(use_group_);
  group_button_layout->addWidget(use_properties_);
  main_layout->addLayout(group_button_layout);

  QGroupBox* group_box = new QGroupBox("Select a group to upscale",this);
  group_selector_ = new SinglePropertyGroupSelector(this);
  group_box->setLayout(new QVBoxLayout());
  group_box->layout()->addWidget(group_selector_);
  main_layout->addWidget(group_box);

  QGroupBox* properties_box = new QGroupBox("Select properties to upscale",this);
  prop_selector_ = new OrderedPropertySelector(this);
  properties_box->setLayout(new QVBoxLayout());
  properties_box->layout()->addWidget(prop_selector_);
  main_layout->addWidget(properties_box);
  properties_box->setVisible(false);

  compute_variance_ = new QCheckBox("Compute block variance for continuous attribute ",this);
  main_layout->addWidget(compute_variance_);

  main_layout->addStretch();

 
  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* close = new QPushButton( "Create and Close", this);
  QPushButton* ok = new QPushButton( "Create", this);
  QPushButton* cancel = new QPushButton( "Exit", this);
  bottom_layout->addStretch();
  bottom_layout->addWidget( ok ); 
//  bottom_layout->addStretch();
  bottom_layout->addWidget( close ); 
//  bottom_layout->addStretch();
  bottom_layout->addWidget( cancel ); 
//  bottom_layout->addStretch();

  main_layout->addLayout(bottom_layout);

  QObject::connect( use_group_, SIGNAL( toggled(bool) ),
                    group_box, SLOT( setVisible(bool) ) );
  QObject::connect( use_group_, SIGNAL( toggled(bool) ),
                    properties_box, SLOT( setHidden(bool) ) );

  QObject::connect( source_grid_selector_, SIGNAL( activated(QString) ),
                    group_selector_, SLOT( show_property_groups(QString) ) );
  QObject::connect( source_grid_selector_, SIGNAL( activated(QString) ),
                    prop_selector_, SLOT( show_properties(QString) ) );

  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( create() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( create_and_close() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );

}

Upscale_property_dialog::~Upscale_property_dialog()
{

}



void Upscale_property_dialog::create()
{

  QStringList params_list;
  
  if(target_grid_selector_->selectedGrid().isEmpty()) return;
  if(source_grid_selector_->selectedGrid().isEmpty()) return;

  params_list.append( target_grid_selector_->selectedGrid() );
  params_list.append( source_grid_selector_->selectedGrid() );
  params_list.append( compute_variance_->isChecked()?"1":"0" );

  
  Action* upscaler;
  if(use_group_->isChecked()) {
    params_list.append( group_selector_->currentText() );
    upscaler = new Upscale_group();
  }
  else {
    QStringList props = prop_selector_->selected_properties();
    params_list.append(props);
    upscaler = new Upscale_properties();
  }

  QString params = params_list.join("::");

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( projects_manager + "/" + "project" );
  GsTL_project* project = dynamic_cast<GsTL_project*>( ni.raw_ptr() );
  Error_messages_handler error_messages;

  std::string param_str = params.toStdString();
  bool ok = upscaler->init(param_str,project,  &error_messages);

  if( !ok ) {
    GsTLcerr << error_messages.errors();
    GsTLcerr << gstlIO::end;
  }
  else {
    ok = upscaler->exec();
    if( !ok ) {
      GsTLcerr << "The properties could not be upscaled";
      GsTLcerr << gstlIO::end;
    }
  }
  delete upscaler;

}

void Upscale_property_dialog::create_and_close() {
  this->create();
  this->reject();
}
