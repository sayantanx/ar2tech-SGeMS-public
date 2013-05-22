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


#include <gui/utils/set_not_informed_dialog.h>
#include <qtplugins/selectors.h>
#include <utils/gstl_messages.h>
#include <utils/error_messages_handler.h>
#include <actions/common.h>
#include <actions/defines.h>
#include <grid/grid_model/geostat_grid.h>
#include <grid/grid_model/grid_categorical_property.h>
#include <appli/manager_repository.h>
#include <appli/project.h>

#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QButtonGroup>


Set_not_informed_dialog::
Set_not_informed_dialog( GsTL_project* proj,
                    QWidget* parent, const char* name ){
  if (name)
    setObjectName(name);
  project_ = proj;
  
  QVBoxLayout* main_layout = new QVBoxLayout( this);
  main_layout->setMargin(9);
  main_layout->setSpacing(-1);
  
  QGroupBox* grid_box = new QGroupBox( this);
  QGroupBox* prop_box = new QGroupBox( this);
  
  QVBoxLayout *vg = new QVBoxLayout(grid_box);
  QVBoxLayout *vp = new QVBoxLayout(prop_box);

  vg->addWidget(new QLabel("Select grid",grid_box) );
  gridSelector_ = new GridSelectorBasic(grid_box, "grid", proj );
  vg->addWidget(gridSelector_ );
  grid_box->setLayout( vg );

  vp->addWidget(new QLabel("Property",prop_box) );
  propSelector_ = new SinglePropertySelector(prop_box, "property" );
  QHBoxLayout *h_min_max = new QHBoxLayout( prop_box );
  

  min_max_box_ = new QGroupBox( prop_box );
  QVBoxLayout *v_min = new QVBoxLayout( prop_box );
  QVBoxLayout *v_max = new QVBoxLayout( prop_box );

  v_min->addWidget(new QLabel("Min",prop_box));
  v_max->addWidget(new QLabel("Max",prop_box));
  minFilter_  = new QLineEdit(prop_box);
  maxFilter_  = new QLineEdit(prop_box);
  v_min->addWidget( minFilter_ );
  v_max->addWidget( maxFilter_ );

  h_min_max->addLayout(v_min);
  h_min_max->addLayout(v_max);
  min_max_box_->setLayout( h_min_max);
  min_max_box_->setTitle("Values between Min and Max will be inside the region");

  vp->addWidget( propSelector_ );
  vp->addWidget( min_max_box_ );

  grid_box->setLayout(vg);
  prop_box->setLayout(vp);


  QHBoxLayout* bottom_layout = new QHBoxLayout( this);
  bottom_layout->setSpacing(9);
  QPushButton* ok = new QPushButton( "Execute", this);
  QPushButton* close = new QPushButton( "Execute and Close", this);
  QPushButton* cancel = new QPushButton( "Cancel", this);

  bottom_layout->addStretch();
  bottom_layout->addWidget( ok );
  bottom_layout->addWidget( close );
  bottom_layout->addWidget( cancel );

  main_layout->addWidget( grid_box );
  main_layout->addWidget( prop_box );
  main_layout->addStretch();
  main_layout->addLayout( bottom_layout );

  
  QObject::connect( gridSelector_, SIGNAL( activated( const QString& ) ),
                    propSelector_, SLOT( show_properties( const QString& ) ) );



  QObject::connect( ok, SIGNAL( clicked() ),
                    this, SLOT( execute() ) );
  QObject::connect( close, SIGNAL( clicked() ),
                    this, SLOT( execute_and_close() ) );
  QObject::connect( cancel, SIGNAL( clicked() ),
                    this, SLOT( reject() ) );

  setSizeGripEnabled( true );
}

QString 
Set_not_informed_dialog::selected_grid() const {
  return gridSelector_->currentText();
}

QString 
Set_not_informed_dialog::selected_property() const{
  return propSelector_->currentText();
}


QString 
Set_not_informed_dialog::get_min_filter_value() const{
  return minFilter_->text();
}
QString 
Set_not_informed_dialog::get_max_filter_value() const {
  return maxFilter_->text();
}



bool Set_not_informed_dialog::execute(){
	  QString grid_name = selected_grid();
	  QString prop_name = selected_property();

	  if( grid_name.isEmpty() || prop_name.isEmpty()) return false;


	  QApplication::setOverrideCursor( Qt::WaitCursor );

	  QString sep = Actions::separator.c_str();
	  QStringList list;
	  list.append( grid_name );
	  list.append( prop_name );

	  std::string command;

		list.append( this->get_min_filter_value() );
		list.append( this->get_max_filter_value() );
		command = "ClearPropertyValueIf";

	  std::string parameters = list.join( sep ).toStdString();
	  if( parameters.empty() ) {
	    GsTLcerr << "Errors with the parameters selected";
	    GsTLcerr << gstlIO::end;
		return false;
	  }

	  // call the DeleteObjectProperties action
	  Error_messages_handler error_messages;



	  bool ok = project_->execute( command, parameters, &error_messages );

	  if( !ok ) {
	    GsTLcerr << "Command " << command << " could not be performed. \n";
	    if( !error_messages.empty() ) {
	      GsTLcerr << error_messages.errors();
	    }
	    GsTLcerr << gstlIO::end;
		return false;
	  }

	  QApplication::restoreOverrideCursor();
	  project_->update();
	  return true;

}

void Set_not_informed_dialog::execute_and_close(){
	bool ok = execute();
	if(ok) accept();
}
