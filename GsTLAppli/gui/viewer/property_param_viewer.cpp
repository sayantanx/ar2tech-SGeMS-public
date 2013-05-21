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



/*
 * property_param_viewer.cpp
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */


#include <GsTLAppli/gui/viewer/property_param_viewer.h>

#include <QLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>

//Named_interface* Property_param_viewer::create_new_interface(std::string&){
//	return new Property_param_viewer;
//}

Property_param_viewer::Property_param_viewer()
: is_displayed_(false)
{

}

Property_param_viewer::Property_param_viewer(Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop)
: is_displayed_(false)
{
	this->initialize(prop,vtk_prop);

}

Property_param_viewer::~Property_param_viewer()
{
	if( vtk_prop_->current_property() == prop_ ) {
		vtk_prop_->set_property("",0);
	}
}


const Geostat_grid* Property_param_viewer::grid() {
	return grid_;
}
QString Property_param_viewer::grid_name() const{
	return QString::fromStdString(grid_->name());
}


void Property_param_viewer::update(std::string obj){
	// If this object had been modified, check to see if one property had been deleted
	if( obj != grid_->name() ) return;

// I believe that there is nothing to be done. since it should be deleted by an external manager.
// cannot self-delete (unfortunately!)
}

void Property_param_viewer::new_object(std::string obj){
	//Nothing to be done.
}
void Property_param_viewer::deleted_object(std::string obj){
	//Nothing to be done.
}



void Property_param_viewer::initialize(Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop){
	vtk_prop_ = vtk_prop;
	grid_ = vtk_prop->grid();
	prop_ = prop;
	cprop_ = dynamic_cast<const Grid_categorical_property*>(prop_);
//	this->setTitle(QString("%1 on grid %2").arg(prop_->name().c_str()).arg(grid_->name().c_str()));

	QVBoxLayout * layout = new QVBoxLayout(this);


	QHBoxLayout *title_layout = new QHBoxLayout(this);

	QString tile_str = QString("<b>%1</b> (%2)").arg(prop_->name().c_str()).arg(grid_->name().c_str());
	QLabel *title = new QLabel(tile_str,this);

	QPixmap pixmap;
	if(cprop_)
		pixmap.load(QString::fromUtf8(":/icons/appli/Pixmaps/cat_property.svg"));
	else
		pixmap.load(QString::fromUtf8(":/icons/appli/Pixmaps/cont_property.svg"));

	QLabel *title_pixmap = new QLabel(this);
	title_pixmap->setPixmap (pixmap.scaledToHeight(20) );

	title_layout->addWidget(title_pixmap);
	title_layout->addWidget(title);
	title_layout->addStretch();


	layout->addLayout(title_layout);

	tab_ = new QTabWidget(this);
	this->init_color_page();
	this->init_filtering_page();
  //this->init_section_page();
	this->init_stats_page();
	layout->addWidget(tab_);
//	layout->addStretch(1);

	this->setLayout(layout);

	QObject::connect(color_, SIGNAL(display_modified()), this, SLOT(display()));
	QObject::connect(color_, SIGNAL(colormap_changed( const Colormap*)),
					 this, SIGNAL(colormap_changed( const Colormap*)));
	QObject::connect(color_, SIGNAL(colormap_changed( const Colormap*)),
					 this, SLOT(update_colormap_display()));


}


void Property_param_viewer::update_colormap_display(){
  vtk_prop_->refresh_colormap();

  emit this->rendering_modified();
}

void Property_param_viewer::display(){

  vtk_prop_->set_visibility(true);


	if(threshold_->is_thresholding_enabled()  ) {
		threshold_->display();
	}
	else {
		vtk_prop_->remove_all_thresholding();
	}


	vtk_prop_->set_property(prop_->name(), color_->colormap());

  vtk_prop_->update_colorbar_visibility();
//	if(!vtk_prop_->is_colorbar_visible()) {
//		vtk_prop_->set_colorbar_visible(true);
//	}


  emit this->rendering_modified();
}


void Property_param_viewer::undisplay(){

	vtk_prop_->set_property("", 0);
	is_displayed_ = false;
  vtk_prop_->update_colorbar_visibility();
  /*
	if(vtk_prop_->is_colorbar_visible()) {
		vtk_prop_->set_colorbar_visible(false);
	}
  */
  emit this->rendering_modified();


}


void Property_param_viewer::init_color_page(){


	if(cprop_ == 0) {
		color_ = new Colormap_continuous_controller(prop_,vtk_prop_,this);
	}
	else {  // Need to be replaced with a categorical colormap
		//color_ = new Colormap_continuous_controller(prop_,vtk_prop_,this);
		color_ = new Colormap_categorical_controller(cprop_,vtk_prop_,this);
	}
	tab_->addTab(color_,"Colormap");


}

void Property_param_viewer::init_filtering_page(){
	if(prop_->classname() == "Grid_categorical_property") {
		const Grid_categorical_property* cprop  = dynamic_cast<const Grid_categorical_property*>(prop_);
		threshold_ = new Property_thresholds_categorical_control(cprop,vtk_prop_,this);
	}
	else {
		threshold_ = new Property_thresholds_continuous_control(prop_,vtk_prop_,this);
	}
	tab_->addTab(threshold_,"Filtering");

	QObject::connect(threshold_, SIGNAL(rendering_modified()),
					 this, SIGNAL(rendering_modified()));

}

void Property_param_viewer::init_stats_page(){
	Property_stats_view *stats = get_stats_view(prop_,vtk_prop_,this);
	tab_->addTab(stats,"Stats");
}
/*
void Property_param_viewer::init_section_page(){
  if(vtk_prop_->grid()->classname() == "Point_set") return;
	Property_section_view *section = new Property_section_view(prop_,vtk_prop_,this);
	tab_->addTab(section,"Section");
	QObject::connect(section, SIGNAL(rendering_modified()),
					 this, SIGNAL(rendering_modified()));
}
*/
/*
 *
 */

/*
 *   Property_viz_parameters
 */
Named_interface* Property_viz_parameters::create_new_interface(std::string&){
	return new Property_viz_parameters;

}

Property_viz_parameters::Property_viz_parameters() : Visualization_parameters(), viewer_(0){

}

Property_viz_parameters::~Property_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
//	if(vtk_prop_->current_property() == prop_) {  //Done in the destructor of viewer_
//		vtk_prop_->set_property("",0);
//	}
}


void Property_viz_parameters::initialize( Grid_continuous_property* prop, GsTL_vtkProp* vtk_prop){
	prop_ = prop;
	vtk_prop_ = vtk_prop;
	viewer_ = new Property_param_viewer(prop, vtk_prop);

}

QWidget* Property_viz_parameters::interface(){
	return viewer_;
}

Grid_continuous_property* Property_viz_parameters::property(){
	return prop_;
}

void Property_viz_parameters::display(){
	viewer_->display();
}
void Property_viz_parameters::undisplay(){
	viewer_->undisplay();

}
