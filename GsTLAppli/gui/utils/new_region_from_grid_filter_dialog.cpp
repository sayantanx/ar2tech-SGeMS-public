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


#include <GsTLAppli/gui/utils/new_region_from_grid_filter_dialog.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/grid_filter_widgets.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/actions/common.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_filter.h>
#include <GsTLAppli/grid/utilities.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QButtonGroup>


New_region_from_grid_filter_dialog::
New_region_from_grid_filter_dialog( QWidget* parent ){
  
  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);
  
  QGroupBox* grid_box = new QGroupBox("Select grid", this);
   QVBoxLayout *grid_layout = new QVBoxLayout(grid_box);
  gridSelector_ = new GridSelectorBasic(grid_box, "grid" );
  grid_layout->addWidget(gridSelector_);
  grid_box->setLayout(grid_layout);


  QGroupBox* new_region_box = new QGroupBox("Name for New Region", this);
  QVBoxLayout *new_region_layout = new QVBoxLayout(new_region_box);
  new_region_ = new QLineEdit(new_region_box);
  new_region_layout->addWidget(new_region_ );
  new_region_box->setLayout( new_region_layout );

  grid_filter_view_ = new Grid_filter_selector(this);

  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* ok = new QPushButton( "Create", this);
  QPushButton* close = new QPushButton( "Create and Close", this);
  QPushButton* cancel = new QPushButton( "Cancel", this);

  bottom_layout->addStretch();
  bottom_layout->addWidget( ok );
  bottom_layout->addWidget( close );
  bottom_layout->addWidget( cancel );

  main_layout->addWidget( grid_box );
  main_layout->addWidget( new_region_box );
  main_layout->addWidget( grid_filter_view_ );
  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );

  
  QObject::connect( gridSelector_, SIGNAL( activated( const QString& ) ),
                    grid_filter_view_, SLOT( set_grid( const QString& ) ) );


  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( create_region() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( create_region_and_close() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );

  setSizeGripEnabled( true );
}

QString 
New_region_from_grid_filter_dialog::selected_grid() const {
  return gridSelector_->currentText();
}


QString 
New_region_from_grid_filter_dialog::new_region_name() const {
  return new_region_->text();
}



bool New_region_from_grid_filter_dialog::create_region(){
	  QString grid_name = selected_grid();
	  QString region_name = new_region_name();

	  if( grid_name.isEmpty() ) return false;

	  if( region_name.isEmpty() ) {
	    GsTLcerr << "Must select a grid.";
	    GsTLcerr << gstlIO::end;
		return false;
	  }

    Grid_filter* filter = grid_filter_view_->create_filter();
    if(filter == 0) {
      GsTLcerr << "No filer selected"<<gstlIO::end;
      return false;
    }

    Geostat_grid* grid = get_grid_from_manager(grid_name.toStdString());
    if(grid==0) return 0;

    GsTLGridRegion* region = grid->add_region(region_name.toStdString());
    if(region==0) return 0;

    int region_size = region->size();
    for(int i=0; i< region_size; ++i) {
      if(filter->is_valid_nodeid(i)) region->set_region_value(true,i);
    }

    delete filter;

	  QApplication::restoreOverrideCursor();
	  return true;

}

void New_region_from_grid_filter_dialog::create_region_and_close(){
	bool ok = create_region();
	if(ok) accept();
}
