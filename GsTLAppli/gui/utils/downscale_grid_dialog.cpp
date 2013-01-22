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

#include <GsTLAppli/gui/utils/downscale_grid_dialog.h>
#include <GsTLAppli/actions/obj_manag_actions.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
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

Downscale_grid_dialog::Downscale_grid_dialog(QWidget *parent)
  : QDialog(parent)
{
  this->setWindowTitle("Downscaling of a regular grid");

  QVBoxLayout* main_layout = new QVBoxLayout(this);
  this->setLayout(main_layout);
/*
  QGridLayout* grid_layout = new QGridLayout(this);
  grid_selector_ = new GridSelector(this);
  grid_layout->addWidget(new QLabel("Source grid",this),0,0);
  grid_layout->addWidget(grid_selector_,0,1);

  grid_name_ = new QLineEdit(this);
  grid_layout->addWidget(new QLabel("Name of target grid",this), 1,0);
  grid_layout->addWidget(grid_name_, 1,1);
  main_layout->addLayout(grid_layout);
  main_layout->addSpacing(1);
*/

  QHBoxLayout* grid_layout = new QHBoxLayout(this);
  grid_selector_ = new GridSelector(this);
  grid_layout->addWidget(new QLabel("Grid to be downscaled",this));
  grid_layout->addWidget(grid_selector_);
  main_layout->addLayout(grid_layout);
//  main_layout->addWidget(new QLabel("Source grid",this));
//  main_layout->addWidget(grid_selector_);
  main_layout->addSpacing(2);

  QHBoxLayout* name_layout = new QHBoxLayout(this);
  grid_name_ = new QLineEdit(this);
  name_layout->addWidget(new QLabel("Name of target grid",this));
  name_layout->addWidget(grid_name_);
  main_layout->addLayout(name_layout);
  main_layout->addSpacing(2);


  QGridLayout* factor_layout = new QGridLayout(this);
  nx_ = new QSpinBox(this);
  ny_ = new QSpinBox(this);
  nz_ = new QSpinBox(this);
  nx_->setMinimum(0);
  ny_->setMinimum(0);
  nz_->setMinimum(0);

  factor_layout->addWidget(new QLabel("X",this),0,0);
  factor_layout->addWidget(new QLabel("Y",this),0,1);
  factor_layout->addWidget(new QLabel("Z",this),0,2);
  factor_layout->addWidget(nx_,1,0);
  factor_layout->addWidget(ny_,1,1);
  factor_layout->addWidget(nz_,1,2);

  QGroupBox* factor_box = new QGroupBox(this);
  factor_box->setTitle("Factors to subdivide the cells");
  factor_box->setLayout(factor_layout);
  main_layout->addWidget(factor_box);

//  main_layout->addWidget(new QLabel("Factors to subdivide the cells",this));
//  main_layout->addLayout(factor_layout);

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

  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( create() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( create_and_close() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );

 

}

Downscale_grid_dialog::~Downscale_grid_dialog()
{

}

void Downscale_grid_dialog::create()
{

  QStringList params_list;
  
  if(grid_selector_->selectedGrid().isEmpty()) return;
  if(grid_name_->text().isEmpty()) return;
  if( nx_->value() == 1 && ny_->value() == 1 && nz_->value() == 1) return;

  params_list.append( grid_selector_->selectedGrid() );
  params_list.append( grid_name_->text() );
  params_list.append( nx_->text() );
  params_list.append( ny_->text() );
  params_list.append( nz_->text() );

  QString params = params_list.join("::");

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( projects_manager + "/" + "project" );
  GsTL_project* project = dynamic_cast<GsTL_project*>( ni.raw_ptr() );
  Error_messages_handler error_messages;

  Downscale_grid downscaler;
  std::string param_str = params.toStdString();
  bool ok = downscaler.init(param_str ,project,  &error_messages);

  if( !ok ) {
    GsTLcerr << error_messages.errors();
    GsTLcerr << gstlIO::end;
  }
  else {
    ok = downscaler.exec();
    if( !ok ) {
      GsTLcerr << "The grid could not be downscaled";
      GsTLcerr << gstlIO::end;
    }
  }

}

void Downscale_grid_dialog::create_and_close() {
  this->create();
  this->reject();
}
