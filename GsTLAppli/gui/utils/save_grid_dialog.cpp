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



#include <GsTLAppli/gui/utils/save_grid_dialog.h>

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/filters/filter.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

//==========================================================

Save_grid_dialog::Save_grid_dialog( const GsTL_project* project,
				    QWidget * parent, const char * name) 
  : QFileDialog( parent ) {

  if (name)
    setObjectName(name);
  
  setModal(true);
  setFileMode( QFileDialog::AnyFile );
  setLabelText(QFileDialog::Accept, "Save");

  QGridLayout * lo = dynamic_cast<QGridLayout*>(this->layout());
  
  grid_selector_ = new QComboBox( this);
  QLabel* label = new QLabel( "Grid to save", this );
  lo->addWidget(label, 4,0,Qt::AlignLeft);
  lo->addWidget(grid_selector_,4,1,Qt::AlignLeft);



  //TL modified
  propList_ = new QListWidget( this);
  propList_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  QLabel* plabel = new QLabel( "Properties to save", this );
  lo->addWidget(plabel, 5,0,Qt::AlignLeft);
  lo->addWidget(propList_,5,1,Qt::AlignLeft);


  //TL + AB modified
  masked_as_regular_frame_ = new QFrame(this);
  saveRegular_ = new QCheckBox(masked_as_regular_frame_);
  //saveRegular_->setDisabled(true);
  QLabel* slabel = new QLabel( "Save masked grid as Cartesian", masked_as_regular_frame_ );
  QHBoxLayout* mgrid_to_cgrid_layout = new QHBoxLayout(masked_as_regular_frame_);
  mgrid_to_cgrid_layout->addWidget(slabel);
  mgrid_to_cgrid_layout->addWidget(saveRegular_);
  masked_as_regular_frame_->setLayout(mgrid_to_cgrid_layout);
  lo->addWidget(masked_as_regular_frame_,6,1,Qt::AlignLeft);
  masked_as_regular_frame_->setVisible(false);

 // lo->addWidget(slabel, 6,0,Qt::AlignLeft);
 // lo->addWidget(saveRegular_,6,1,Qt::AlignLeft);
  
  // search for available output filters

  QStringList filters;
  SmartPtr<Named_interface> ni_filter = 
    Root::instance()->interface( outfilters_manager );
  Manager* dir = dynamic_cast<Manager*>( ni_filter.raw_ptr() );
  appli_assert( dir );

  Manager::type_iterator begin = dir->begin();
  Manager::type_iterator end = dir->end();
  for( ; begin != end ; ++begin ) {
    SmartPtr<Output_filter> outf(dynamic_cast<Output_filter*>(dir->new_interface(*begin).raw_ptr() ));
    if( outf->type_data() != "Grid") continue;
    QString filt( begin->c_str() );
    filt += " (*.*)";
    filters.push_back( filt ) ;
  }

  setFilters( filters );

  
  // search for available grids

  const GsTL_project::String_list& grids = project->objects_list();
  typedef GsTL_project::String_list::const_iterator const_iterator;
  for( const_iterator it = grids.begin(); it != grids.end(); ++it ) {
    grid_selector_->addItem( it->c_str() );
  }

  QObject::connect( grid_selector_, SIGNAL( activated(const QString &) ),
	  this, SLOT( gridChanged(const QString &) ) );

  if (!grid_selector_->currentText().isEmpty())
	gridChanged(grid_selector_->currentText());
}


void Save_grid_dialog::gridChanged(const QString & s)
{
	SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + s.toStdString() );
	Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
	appli_assert( grid );

  std::list<std::string> pn = grid->property_list();
	propList_->clear();
	for (std::list<std::string>::iterator it = pn.begin(); it != pn.end(); ++it){
	        QListWidgetItem * nitem = new QListWidgetItem(it->c_str(), propList_);
		propList_->addItem(nitem);
		nitem->setSelected(true);
	}

	if (dynamic_cast<Reduced_grid*>(grid)) {
		masked_as_regular_frame_->setVisible(true);
  }
	else{
		masked_as_regular_frame_->setVisible(false);
  }
}

QString Save_grid_dialog::selected_grid() const {
  return grid_selector_->currentText();
}


void Save_grid_dialog::accept() { 

  selected_.clear();
  for (int i = 0; i < propList_->count(); ++i)
    if (propList_->item(i)->isSelected())
      selected_.push_back(propList_->item(i)->text());

  if( grid_selector_->currentText().isEmpty() ) {
    QMessageBox::warning( this, "No Grid Selected",
			  " You must specify a grid to save" );
	done(Rejected);
  }
  if (selected_.empty())
    QMessageBox::warning( this, "No Properties Selected",
			  " WARNING! Saving empty grid" );
  
  done(Accepted);
}


