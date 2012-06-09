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



/*
 * grid_param_viewer.cpp
 *
 *  Created on: May 20, 2011
 *      Author: aboucher
 */

#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/viewer/grid_param_viewer.h>
#include <GsTLAppli/gui/viewer/grid_section_view.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QButtonGroup>
#include <QGroupBox>
#include <QComboBox>



void Grid_viz_parameters::display(){
	vtk_prop_->set_visibility(true);

}
void Grid_viz_parameters::undisplay(){
	vtk_prop_->set_visibility(false);
}


/*
 *   CGrid_viz_parameters
 */
Named_interface* CGrid_viz_parameters::create_new_interface(std::string&){
	return new CGrid_viz_parameters;

}

CGrid_viz_parameters::CGrid_viz_parameters() : Grid_viz_parameters(), viewer_(0){

}

CGrid_viz_parameters::~CGrid_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
	delete vtk_prop_;
}


void CGrid_viz_parameters::initialize(Geostat_grid* grid, vtkRenderer* renderer){

	grid_ = grid;
	Named_interface* vtk_prop_ni = Root::instance()->new_interface_raw(grid->classname(), vtkProps_manager + "/");

	vtk_prop_ = dynamic_cast<GsTL_vtkProp*> (vtk_prop_ni);
	appli_assert(vtk_prop_ != 0);

	vtk_prop_->init(grid, renderer);

	viewer_ = new CGrid_param_viewer(vtk_prop_);
	viewer_->setHidden(true);
}

QWidget* CGrid_viz_parameters::interface(){
	return viewer_;
}


/*
 *   MGrid_viz_parameters
 */
Named_interface* MGrid_viz_parameters::create_new_interface(std::string&){
	return new MGrid_viz_parameters;

}

MGrid_viz_parameters::MGrid_viz_parameters() : Grid_viz_parameters(), viewer_(0){

}

MGrid_viz_parameters::~MGrid_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
	delete vtk_prop_;
}


void MGrid_viz_parameters::initialize(Geostat_grid* grid, vtkRenderer* renderer){
	grid_ = grid;
	Named_interface* vtk_prop_ni = Root::instance()->new_interface_raw(grid->classname(), vtkProps_manager + "/");

	vtk_prop_ = dynamic_cast<GsTL_vtkProp*> (vtk_prop_ni);
	appli_assert(vtk_prop_ != 0);

	vtk_prop_->init(grid, renderer);

	viewer_ = new MGrid_param_viewer(vtk_prop_);
	viewer_->setHidden(true);
}
QWidget* MGrid_viz_parameters::interface(){
	return viewer_;
}


/*
 *   PSet_viz_parameters
 */
Named_interface* PSet_viz_parameters::create_new_interface(std::string&){
	return new PSet_viz_parameters;

}

PSet_viz_parameters::PSet_viz_parameters() : Grid_viz_parameters(), viewer_(0){

}

PSet_viz_parameters::~PSet_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
	delete vtk_prop_;
}


void PSet_viz_parameters::initialize(Geostat_grid* grid, vtkRenderer* renderer){

	grid_ = grid;
	Named_interface* vtk_prop_ni = Root::instance()->new_interface_raw(grid->classname(), vtkProps_manager + "/");

	vtk_prop_ = dynamic_cast<GsTL_vtkProp*> (vtk_prop_ni);
	appli_assert(vtk_prop_ != 0);

	vtk_prop_->init(grid, renderer);

	viewer_ = new PSet_param_viewer(vtk_prop_);
	viewer_->setHidden(true);
}
QWidget* PSet_viz_parameters::interface(){
	return viewer_;
}


/*
 *   Log_grid_viz_parameters
 */

Named_interface* Log_grid_viz_parameters::create_new_interface(std::string&){
	return new Log_grid_viz_parameters;

}

Log_grid_viz_parameters::Log_grid_viz_parameters() : Grid_viz_parameters(), viewer_(0){

}

Log_grid_viz_parameters::~Log_grid_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
	delete vtk_prop_;
}


void Log_grid_viz_parameters::initialize(Geostat_grid* grid, vtkRenderer* renderer){
	grid_ = grid;
	Named_interface* vtk_prop_ni = Root::instance()->new_interface_raw(grid->classname(), vtkProps_manager + "/");

	vtk_prop_ = dynamic_cast<GsTL_vtkProp*> (vtk_prop_ni);
	appli_assert(vtk_prop_ != 0);

	vtk_prop_->init(grid, renderer);
	viewer_ = new Log_grid_param_viewer(vtk_prop_);
	viewer_->setHidden(true);
}
QWidget* Log_grid_viz_parameters::interface(){
	return viewer_;
}


/*
 *   CGrid_viz_parameters
 */
Named_interface* Structured_grid_viz_parameters::create_new_interface(std::string&){
	return new Structured_grid_viz_parameters;

}

Structured_grid_viz_parameters::Structured_grid_viz_parameters() : Grid_viz_parameters(), viewer_(0){

}

Structured_grid_viz_parameters::~Structured_grid_viz_parameters(){
	viewer_->setParent(0);
	delete viewer_;
	delete vtk_prop_;
}


void Structured_grid_viz_parameters::initialize(Geostat_grid* grid, vtkRenderer* renderer){

	grid_ = grid;
	Named_interface* vtk_prop_ni = Root::instance()->new_interface_raw(grid->classname(), vtkProps_manager + "/");

	vtk_prop_ = dynamic_cast<GsTL_vtkProp*> (vtk_prop_ni);
	appli_assert(vtk_prop_ != 0);

	vtk_prop_->init(grid, renderer);

  viewer_ = new Structured_grid_param_viewer(vtk_prop_);
	viewer_->setHidden(true);
}

QWidget* Structured_grid_viz_parameters::interface(){
	return viewer_;
}



/*
Named_interface* create_grid_param_viewer_interface(std::string&){
	return new CGrid_param_viewer;
}
*/
Grid_param_viewer::Grid_param_viewer() : QTabWidget()
{
//	QVBoxLayout* layout = new QVBoxLayout(this);
//	tbox_ = new QToolBox(this);

//	tbox_->addItem(new QFrame(this),"Information");
//	layout->addWidget(tbox_);
//	layout->addStretch();
//	this->setLayout(layout);
}

Grid_param_viewer::~Grid_param_viewer()
{

}



const Geostat_grid* Grid_param_viewer::grid() {
	return grid_;
}
QString Grid_param_viewer::grid_name() const{
	return QString::fromStdString(grid_->name());
}


void Grid_param_viewer::update(std::string obj){
	// If this object had been modified, check to see if one property had been deleted
	if( obj != grid_->name() ) return;

// I believe that there is nothing to be done. since it should be deleted by an external manager.
// cannot self-delete (unfortunately!)
}

void Grid_param_viewer::new_object(std::string obj){
	//Nothing to be done.
}
void Grid_param_viewer::deleted_object(std::string obj){
	//Nothing to be done.
}



void Grid_param_viewer::init_visualization_page() {
//	QString str();
	QFrame* frame = new QFrame(this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	frame->setLayout(layout);
	this->addTab(frame,"Preferences");

	QComboBox* cbar_orientation_selector = new QComboBox(frame);
	QStringList items;
	items<<"None"<<"Left"<<"Bottom"<<"Right"<<"Top";
	cbar_orientation_selector->addItems(items);
	cbar_orientation_selector->setCurrentIndex(0);

	/*
	QButtonGroup* group_button = new QButtonGroup(frame);
	QRadioButton* radio_none = new QRadioButton("&None", frame);
	QRadioButton* radio_horizontal = new QRadioButton("&Bottom", frame);
	QRadioButton* radio_vertical = new QRadioButton("&Left", frame);
	group_button->addButton(radio_none,0);
	group_button->addButton(radio_horizontal,1);
	group_button->addButton(radio_vertical,2);
	radio_none->setChecked(true);
	QVBoxLayout* button_layout = new QVBoxLayout();
	button_layout->addWidget(radio_none);
	button_layout->addWidget(radio_horizontal);
	button_layout->addWidget(radio_vertical);

	*/

//	QGroupBox* cbar_box = new QGroupBox("Colorbar",frame);
	QHBoxLayout* cbar_layout = new QHBoxLayout();
	cbar_layout->addWidget(new QLabel("Colorbar",this));
	cbar_layout->addWidget(cbar_orientation_selector);

	layout->addLayout(cbar_layout);
//	layout->addStretch();

	QObject::connect(cbar_orientation_selector,SIGNAL(currentIndexChanged(const QString&)),
					 this, SLOT(colorbar_display_option(const QString&)));


}

void Grid_param_viewer::colorbar_display_option(const QString& location){

	if(location == "None") {
		vtk_prop_->remove_colorbar();
	}
	else if (location == "Bottom") {
		vtk_prop_->set_colorbar_to_bottom();
	}
	else if (location == "Left") {
		vtk_prop_->set_colorbar_to_left();
	}
	else if (location == "Top") {
		vtk_prop_->set_colorbar_to_top();
	}
	else if (location == "Right") {
		vtk_prop_->set_colorbar_to_right();
	}
  emit this->rendering_modified();
}




SGrid_param_viewer::SGrid_param_viewer()
	:Grid_param_viewer() {

}



void SGrid_param_viewer::init_visualization_page(){
	Grid_param_viewer::init_visualization_page();
	QComboBox* block_representation_selector = new QComboBox(this->widget(0));
	QStringList items;
	items<<"Surface"<<"Wireframe"<<"Points";
	block_representation_selector->addItems(items);


	//QWidget* block_representation = new QWidget(this);
	QHBoxLayout* block_layout = new QHBoxLayout();
	block_layout->addWidget(new QLabel("Block View",this->widget(0)));
	block_layout->addWidget(block_representation_selector);
	//block_representation->setLayout(block_layout);

	radio_edge_button_ = new QRadioButton("Show edges", this->widget(0));


	this->widget(0)->layout()->addItem(block_layout);
	this->widget(0)->layout()->addWidget(radio_edge_button_);

  QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(this->widget(0)->layout());
  layout->addStretch();

//	dynamic_cast<QVBoxLayout*>(this->widget(0)->layout())->addStretch();

	QObject::connect(block_representation_selector,SIGNAL(currentIndexChanged(const QString&)),
					 this, SLOT(cell_representation(const QString&)));
	QObject::connect(block_representation_selector,SIGNAL(currentIndexChanged(const QString&)),
					 this, SLOT(show_edge_button(const QString&)));
	QObject::connect(radio_edge_button_,SIGNAL(toggled(bool)),
					 this, SLOT(show_edges(bool)));

	block_representation_selector->setCurrentIndex(0);
	radio_edge_button_->setChecked(false);

}

void SGrid_param_viewer::cell_representation( const QString & type){
	vtk_prop_->set_cell_representation(type);
  emit this->rendering_modified();

}

void SGrid_param_viewer::show_edge_button(const QString& type){

	radio_edge_button_->setVisible(type == "Surface");

}

void SGrid_param_viewer::show_edges( bool on ){
	vtk_prop_->show_edges(on);
  emit this->rendering_modified();
}

void SGrid_param_viewer::show_grid_outline( bool on ){
	  appli_assert( vtk_prop_strati_ );
	  vtk_prop_strati_->show_bounding_box( on );
    emit this->rendering_modified();
}


void SGrid_param_viewer::init_section_page(){
	Grid_section_view *section = new Grid_section_view(vtk_prop_,this);
	this->addTab(section,"Section");
	QObject::connect(section, SIGNAL(rendering_modified()),
					 this, SIGNAL(rendering_modified()));
}


/*
 *   CGrid_param_viewer
 */

//Named_interface* CGrid_param_viewer::create_new_interface(std::string&){
//	return new CGrid_param_viewer;
//}


CGrid_param_viewer::CGrid_param_viewer()
	:SGrid_param_viewer() {
}

CGrid_param_viewer::CGrid_param_viewer(GsTL_vtkProp* vtk_prop)
	:SGrid_param_viewer() {
	this->initialize(vtk_prop);
}

void CGrid_param_viewer::initialize(GsTL_vtkProp* vtk_prop)
{
	vtk_prop_ = vtk_prop;
//	vtk_prop_strati_ = dynamic_cast<vtkProp_strati_grid*>(vtk_prop);
	vtk_prop_cgrid_ = dynamic_cast<vtkProp_cgrid*>(vtk_prop);
//	appli_assert( vtk_prop_strati_ );
	appli_assert( vtk_prop_cgrid_ );

	grid_ = vtk_prop->grid();
	cgrid_ = dynamic_cast<const Cartesian_grid*>(grid_);
	appli_assert( cgrid_  );

	this->init_visualization_page();
  this->init_section_page();
	this->init_stats_page();
  

}



void CGrid_param_viewer::init_stats_page(){

	  GsTLCoordVector cell_dims = cgrid_->cell_dimensions();
	  GsTLPoint origin = cgrid_->origin();

	  QVBoxLayout * layout = new QVBoxLayout(this);
	  QFrame * desc_widget = new QFrame(this);

//	  layout->setSpacing( 4 );

	  QString count;
	  QFont font;
	  font.setBold(true);

	  // number of cells
	  QLabel* dimslabel;
	  dimslabel = new QLabel( "Number of cells (X,Y,Z)", desc_widget );
	  layout->addWidget(dimslabel);
	  dimslabel->setFont( font );
	  QString nCellString = QString("( %1, %2, %3 )").arg(cgrid_->nx()).arg(cgrid_->ny()).arg(cgrid_->nz());
	  layout->addWidget(new QLabel(nCellString, desc_widget ));

	  // cell dimensions
	  layout->addSpacing( 8 );
	  QLabel* sizelabel = new QLabel( "Cells dimensions", desc_widget );
	  layout->addWidget(sizelabel);
	  sizelabel->setFont( font );
	  QString sizeString = QString("( %1, %2, %3 )").arg(cell_dims.x()).arg(cell_dims.y()).arg(cell_dims.z());
	  layout->addWidget(new QLabel(sizeString, desc_widget ));

	  // Origin
	  layout->addSpacing( 8 );
	  QLabel* originlabel = new QLabel( "Origin (center of origin cell)", desc_widget );
	  layout->addWidget(originlabel);
	  originlabel->setFont( font );
	  QString originString = QString("( %1, %2, %3 )").arg(origin.x()).arg(origin.y()).arg(origin.z());
	  layout->addWidget(new QLabel(originString, desc_widget ));
    layout->addStretch();


//	  // Active Region
//	  layout->addSpacing( 8 );
//	  const GsTLGridRegion* region = cgrid_->selected_region();
//	  QString region_name;
//	  if(region) region_name = region->name().c_str();
//	  else region_name = "No Region Selected";
//	  QLabel* regionlabel = new QLabel( "Active Region", desc_widget);
//	  layout->addWidget(regionlabel);
//	  regionlabel->setFont( font );
//	  layout->addWidget( new QLabel(region_name, desc_widget));

//	  layout->addStretch();
	  desc_widget->setLayout(layout);

//	  this->addItem(desc_widget,"Information");
	  this->addTab(desc_widget,"Information");



}

/*
 *  MGrid_param_viewer
 */

//Named_interface* MGrid_param_viewer::create_new_interface(std::string&){
//	return new MGrid_param_viewer;
//}


MGrid_param_viewer::MGrid_param_viewer()
	:SGrid_param_viewer() {

}

MGrid_param_viewer::MGrid_param_viewer(GsTL_vtkProp* vtk_prop)
	:SGrid_param_viewer() {
	this->initialize(vtk_prop);
}

void MGrid_param_viewer::initialize(GsTL_vtkProp* vtk_prop)
{
	vtk_prop_ = vtk_prop;
	vtk_prop_mgrid_ = dynamic_cast<vtkProp_mgrid*>(vtk_prop);
//	vtk_prop_strati_ = dynamic_cast<vtkProp_strati_grid*>(vtk_prop);
	appli_assert( vtk_prop_mgrid_ );
//	appli_assert( vtk_prop_strati_ );
	grid_ = vtk_prop->grid();
	mgrid_ = dynamic_cast<const Reduced_grid*>(grid_);
	appli_assert( mgrid_  );

	this->init_visualization_page();
  this->init_section_page();
	this->init_stats_page();

}


void MGrid_param_viewer::init_stats_page(){
	  GsTLCoordVector cell_dims = mgrid_->cell_dimensions();
	  GsTLPoint origin = mgrid_->origin();

	  QVBoxLayout * layout = new QVBoxLayout();
	  QWidget* desc_widget  = new QFrame();

	  layout->setSpacing( 4 );

	  QString count;
	  QFont font;
	  font.setBold(true);

	  // number of cells
	  QLabel* dimslabel;
	  dimslabel = new QLabel( "Number of cells (X,Y,Z)", desc_widget );
	  layout->addWidget(dimslabel);
	  dimslabel->setFont( font );
	  QString nCellString = QString("( %1, %2, %3 )").arg(mgrid_->nx()).arg(mgrid_->ny()).arg(mgrid_->nz());
	  layout->addWidget(new QLabel(nCellString, desc_widget ));

	  // cell dimensions
	  layout->addSpacing( 8 );
	  QLabel* sizelabel = new QLabel( "Cells dimensions", desc_widget );
	  layout->addWidget(sizelabel);
	  sizelabel->setFont( font );
	  QString sizeString = QString("( %1, %2, %3 )").arg(cell_dims.x()).arg(cell_dims.y()).arg(cell_dims.z());
	  layout->addWidget(new QLabel(sizeString, desc_widget ));

	  // Origin
	  layout->addSpacing( 8 );
	  QLabel* originlabel = new QLabel( "Origin (center of origin cell)", desc_widget );
	  layout->addWidget(originlabel);
	  originlabel->setFont( font );
	  QString originString = QString("( %1, %2, %3 )").arg(origin.x()).arg(origin.y()).arg(origin.z());
	  layout->addWidget(new QLabel(originString, desc_widget ));

	  QLabel *num = new QLabel(QString("# of active cells: ")+count.setNum(mgrid_->size()), desc_widget);
	  num->setFont(font);
	  layout->addWidget(num);

	  // Active Region
	  layout->addSpacing( 8 );
	  const GsTLGridRegion* region = grid_->selected_region();
	  QString region_name;
	  if(region) region_name = region->name().c_str();
	  else region_name = "No Region Selected";
	  QLabel* regionlabel = new QLabel( "Active Region", desc_widget);
	  layout->addWidget(regionlabel);
	  regionlabel->setFont( font );
	  layout->addWidget( new QLabel(region_name, desc_widget));

	  layout->addStretch();
	  desc_widget->setLayout(layout);
	  this->addTab(desc_widget,"Information");
//	  this->addItem(desc_widget,"Preferences");
}


/*
 *   PSet_param_viewer
 */


//Named_interface* PSet_param_viewer::create_new_interface(std::string&){
//	return new PSet_param_viewer;
//}


PSet_param_viewer::PSet_param_viewer()
	:Grid_param_viewer() {

}

PSet_param_viewer::PSet_param_viewer(GsTL_vtkProp* vtk_prop)
	:Grid_param_viewer() {
	this->initialize(vtk_prop);
}


void PSet_param_viewer::initialize(GsTL_vtkProp* vtk_prop)
{
	vtk_prop_ = vtk_prop;
	vtk_prop_pset_ = dynamic_cast<vtkProp_pointset*>(vtk_prop);
	appli_assert( vtk_prop_pset_ );

	grid_ = vtk_prop->grid();
	pset_ = dynamic_cast<const Point_set*>(grid_);
	appli_assert( pset_  );

	this->init_visualization_page();
	this->init_stats_page();

}

void PSet_param_viewer::init_visualization_page() {
	Grid_param_viewer::init_visualization_page();


	QHBoxLayout* size_layout = new QHBoxLayout(this->widget(0));
	size_layout->addWidget(new QLabel("Size of Points",this->widget(0)));
	QSpinBox* size = new QSpinBox(this->widget(0));
	size->setValue(3);
	size->setMinimum(1);
	size_layout->addWidget(size);

	this->widget(0)->layout()->addItem(size_layout);

	dynamic_cast<QVBoxLayout*>(this->widget(0)->layout())->addStretch();

	connect(size, SIGNAL(valueChanged(int)), this, SLOT(point_size(int)));
}


void PSet_param_viewer::init_stats_page()
{
  int points_count = pset_->size();
  QString count;
  count.setNum( points_count );

  QVBoxLayout * layout = new QVBoxLayout();
  QWidget* desc_widget  = new QFrame();


  layout->setSpacing( 4 );

  layout->addWidget(new QLabel( QString("# of points: ")+count, desc_widget ));


  GsTLPoint min, max;
  this->bounding_box( min, max );

  QFont font;
  font.setBold(true);

  layout->addSpacing( 8 );
  QLabel* bboxlabel = new QLabel( "Bounding Box", desc_widget );
  layout->addWidget(bboxlabel);
  bboxlabel->setFont( font );
  std::ostringstream minlabel;

  minlabel << "Min: ( " << min.x() << " , " << min.y() << " , " << min.z() << " )";
  layout->addWidget(new QLabel( QString( minlabel.str().c_str() ), desc_widget ));

  std::ostringstream maxlabel;
  maxlabel << "Max: ( " << max.x() << " , " << max.y() << " , " << max.z() << " )";
  layout->addWidget(new QLabel( QString( maxlabel.str().c_str() ), desc_widget ));

  /*
  // Active Region
  layout->addSpacing( 8 );
  const GsTLGridRegion* region = grid_->selected_region();
  QString region_name;
  if(region) region_name = region->name().c_str();
  else region_name = "No Region Selected";
  QLabel* regionlabel = new QLabel( "Active Region", desc_widget);
  layout->addWidget(regionlabel);
  regionlabel->setFont( font );
  layout->addWidget( new QLabel(region_name, desc_widget));
*/

  layout->addStretch(1);

  desc_widget->setLayout(layout);
//  this->addItem(desc_widget,"Preferences");
  this->addTab(desc_widget,"Statistics");
}

void PSet_param_viewer::point_size(int size) {
	vtk_prop_pset_->point_size(size);
  emit this->rendering_modified();
}


void PSet_param_viewer::bounding_box( GsTLPoint& min, GsTLPoint& max ) {
 const std::vector<GsTLPoint>& point_coords = pset_->point_locations();
 if( point_coords.empty() ) return;

 min = point_coords[0];
 max = point_coords[0];

 std::vector<GsTLPoint>::const_iterator it = point_coords.begin();
 for( ; it != point_coords.end() ; ++it ) {
   for( int dim = 0; dim < 3; dim++ ) {
     min[dim] = std::min( (*it)[dim], min[dim] );
     max[dim] = std::max( (*it)[dim], max[dim] );
   }
 }
}



/*
 *   Log_grid_param_viewer
 */


//Named_interface* Log_grid_param_viewer::create_new_interface(std::string&){
//	return new Log_grid_param_viewer;
//}


Log_grid_param_viewer::Log_grid_param_viewer()
	:Grid_param_viewer() {

}

Log_grid_param_viewer::Log_grid_param_viewer(GsTL_vtkProp* vtk_prop)
	:Grid_param_viewer() {
	this->initialize(vtk_prop);
}

void Log_grid_param_viewer::initialize(GsTL_vtkProp* vtk_prop)
{
	vtk_prop_ = vtk_prop;
	vtk_prop_log_ = dynamic_cast<vtkProp_log*>(vtk_prop);
	appli_assert( vtk_prop_log_ );

	grid_ = vtk_prop->grid();
	log_grid_ = dynamic_cast<const Log_data_grid*>(grid_);
	appli_assert( log_grid_  );

	this->init_visualization_page();
	this->init_stats_page();

}

void Log_grid_param_viewer::init_visualization_page() {

	Grid_param_viewer::init_visualization_page();

	QHBoxLayout* width_layout = new QHBoxLayout(this->widget(0));
	width_layout->addWidget(new QLabel("Width of segment",this->widget(0)));
	QSpinBox* width = new QSpinBox(this->widget(0));
	width->setValue(3);
	width->setMinimum(1);
	width_layout->addWidget(width);

	this->widget(0)->layout()->addItem(width_layout);

	dynamic_cast<QVBoxLayout*>(this->widget(0)->layout())->addStretch();
	connect(width, SIGNAL(valueChanged(int)), this, SLOT(line_width(int)));
}

void Log_grid_param_viewer::init_stats_page()
{
  int points_count = log_grid_->size();
  QString count;
  count.setNum( points_count );

  QVBoxLayout * layout = new QVBoxLayout();
  QWidget* desc_widget  = new QFrame();


  layout->setSpacing( 4 );

  layout->addWidget(new QLabel( QString("# of drillholes: %1").arg(log_grid_->number_of_logs()), desc_widget ));

  layout->addWidget(new QLabel( QString("# of segments: %1").arg(log_grid_->size()), desc_widget ));

  // Compute average, min and max length of the segments
  int nlogs = log_grid_->number_of_logs();
  float min_length = 9e10;
  float max_length = -9e10;
  float mean = 0.0;

  for(int i=0; i<nlogs; ++i) {
	  Log_data logdata = log_grid_->get_log_data(i);
	  float min_length_log = logdata.min_segment_length();
	  if(min_length_log < min_length) min_length = min_length_log;
	  float max_length_log = logdata.max_segment_length();
	  if(max_length_log > max_length) max_length = max_length_log;

	  mean += logdata.average_segment_length() * static_cast<float>( logdata.number_of_segments() );
  }
  mean /= static_cast<float>(log_grid_->size());


  layout->addWidget(new QLabel( QString("Average Length: %1").arg(mean), desc_widget ));
  layout->addWidget(new QLabel( QString("Minimum Length: %1").arg(min_length), desc_widget ));
  layout->addWidget(new QLabel( QString("Maximum Length: %1").arg(max_length), desc_widget ));


  GsTLPoint min, max;
  this->bounding_box( min, max );

  QFont font;
  font.setBold(true);

  layout->addSpacing( 8 );
  QLabel* bboxlabel = new QLabel( "Bounding Box", desc_widget );
  layout->addWidget(bboxlabel);
  bboxlabel->setFont( font );

  QString min_values = QString("Min: (%1,%2,%3)").arg(min.x()).arg(min.y()).arg(min.z());
  QString max_values = QString("Max: (%1,%2,%3)").arg(max.x()).arg(max.y()).arg(max.z());

  layout->addWidget(new QLabel( min_values, desc_widget ));
  layout->addWidget(new QLabel( max_values, desc_widget ));

  /*
  // Active Region
  layout->addSpacing( 8 );
  const GsTLGridRegion* region = grid_->selected_region();
  QString region_name;
  if(region) region_name = region->name().c_str();
  else region_name = "No Region Selected";
  QLabel* regionlabel = new QLabel( "Active Region", desc_widget);
  layout->addWidget(regionlabel);
  regionlabel->setFont( font );
  layout->addWidget( new QLabel(region_name, desc_widget));
*/

  layout->addStretch(1);

  desc_widget->setLayout(layout);
//  this->addItem(desc_widget,"Preferences");
  this->addTab(desc_widget,"Preferences");
}

void Log_grid_param_viewer::line_width(int width) {
	vtk_prop_log_->line_width(width);
  emit this->rendering_modified();
}


void Log_grid_param_viewer::bounding_box( GsTLPoint& min, GsTLPoint& max ) {
 const std::vector<GsTLPoint>& point_coords = log_grid_->point_locations();
 if( point_coords.empty() ) return;

 min = point_coords[0];
 max = point_coords[0];

 std::vector<GsTLPoint>::const_iterator it = point_coords.begin();
 for( ; it != point_coords.end() ; ++it ) {
   if( min.x() > it->x()) min.x() = it->x();
   if( max.x() < it->x()) max.x() = it->x();
   if( min.y() > it->y()) min.y() = it->y();
   if( max.y() < it->y()) max.y() = it->y();
   if( min.z() > it->z()) min.z() = it->z();
   if( max.z() < it->z()) max.z() = it->z();
 }
}

/*
  ----- Structured_grid_param_viewer
*/

Structured_grid_param_viewer::Structured_grid_param_viewer()
	:SGrid_param_viewer() {
}

Structured_grid_param_viewer::Structured_grid_param_viewer(GsTL_vtkProp* vtk_prop)
	:SGrid_param_viewer() {
	this->initialize(vtk_prop);
}

void Structured_grid_param_viewer::initialize(GsTL_vtkProp* vtk_prop)
{
	vtk_prop_ = vtk_prop;
//	vtk_prop_strati_ = dynamic_cast<vtkProp_strati_grid*>(vtk_prop);
  vtk_prop_struct_grid_ = dynamic_cast<vtkProp_structured_grid*>(vtk_prop);
//	appli_assert( vtk_prop_strati_ );
	appli_assert( vtk_prop_struct_grid_ );

	grid_ = vtk_prop->grid();
  struct_grid_ = dynamic_cast<Structured_grid*>(vtk_prop->grid());
	appli_assert( struct_grid_  );

	this->init_visualization_page();
  this->init_section_page();
	this->init_stats_page();
  

}



void Structured_grid_param_viewer::init_stats_page(){

  vtkSmartPointer<vtkStructuredGrid> sgrid_geom = struct_grid_->get_structured_geometry();
  int xo,yo,zo,xn,yn,zn;
  sgrid_geom->GetExtent(xo,yo,zo,xn,yn,zn);

	  GsTLCoordVector cell_dims = struct_grid_->cell_dimensions();
	  GsTLPoint origin = struct_grid_->origin();
	  QVBoxLayout * layout = new QVBoxLayout(this);
	  QFrame * desc_widget = new QFrame(this);


	  QString count;
	  QFont font;
	  font.setBold(true);

	  // number of cells
	  QLabel* dimslabel;
	  dimslabel = new QLabel( "Number of cells (X,Y,Z)", desc_widget );
	  layout->addWidget(dimslabel);
	  dimslabel->setFont( font );
	  QString nCellString = QString("( %1, %2, %3 )").arg(struct_grid_->nx()).arg(struct_grid_->ny()).arg(struct_grid_->nz());
	  layout->addWidget(new QLabel(nCellString, desc_widget ));
/*
	  // cell dimensions
	  layout->addSpacing( 8 );
	  QLabel* sizelabel = new QLabel( "Cells dimensions", desc_widget );
	  layout->addWidget(sizelabel);
	  sizelabel->setFont( font );
	  QString sizeString = QString("( %1, %2, %3 )").arg(cell_dims.x()).arg(cell_dims.y()).arg(cell_dims.z());
	  layout->addWidget(new QLabel(sizeString, desc_widget ));
*/
	  // Origin
	  layout->addSpacing( 8 );
	  QLabel* originlabel = new QLabel( "Origin (center of origin cell)", desc_widget );
	  layout->addWidget(originlabel);
	  originlabel->setFont( font );
	  QString originString = QString("( %1, %2, %3 )").arg(origin.x()).arg(origin.y()).arg(origin.z());
	  layout->addWidget(new QLabel(originString, desc_widget ));
    layout->addStretch();


	  desc_widget->setLayout(layout);

	  this->addTab(desc_widget,"Information");



}
