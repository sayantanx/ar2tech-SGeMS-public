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
 * Vtk_project_view_gui.cpp
 *
 *  Created on: May 13, 2011
 *      Author: aboucher
 */

#include <GsTL/utils/smartptr.h>

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/actions/action.h>
#include <GsTLAppli/actions/defines.h>

#include <GsTLAppli/gui/vtk_prop/vtk_prop_cgrid.h>
#include <GsTLAppli/gui/appli/snapshot_dialog.h>
#include <GsTLAppli/gui/models/project_selection_model.h>
#include <GsTLAppli/gui/viewer/vtk_project_view_gui.h>
#include <GsTLAppli/gui/viewer/grid_param_viewer.h>
#include <GsTLAppli/gui/viewer/property_param_viewer.h>
#include <GsTLAppli/gui/viewer/region_param_viewer.h>
#include <GsTLAppli/gui/viewer/group_param_viewer.h>

#include <vtkCamera.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>
#include <vtkDataSet.h>
#include <vtkCell.h>
#include <vtkCellData.h>

#include <QFrame>
#include <QVBoxLayout>
#include <QSplitter>
#include <QUrl>
#include <QFileInfo>
#include <QModelIndex>
#include <QMenu>
#include <QProgressDialog>

#include <strstream>
//#include <MouseButtons>

//#include <GsTLAppli/gui/models/modeltest.h>

Named_interface* Create_vtk_view(std::string&)
{
	return new Vtk_view();
}


Vtk_view::Vtk_view(GsTL_project* project, QWidget* parent) :
	QFrame(parent), Project_view(project), connections_(0), model_(0) {

	this->setAcceptDrops(true);
  camera_view_.is_initialized = false;
//	this->setContextMenuPolicy(Qt::CustomContextMenu);


//	this->initialize(project, parent);

}

void Vtk_view::initialize(GsTL_project* project, QWidget* parent){


	Project_view::init(project);
	this->setParent(parent);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(0, 0, 0, 0);

	QSplitter* splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    splitter->setHandleWidth(3);
    mainLayout->addWidget(splitter);

    QWidget* controlFrame = this->init_control_frame();
    QFrame* viewerMainFrame = this->init_viewer_frame();


    splitter->addWidget(controlFrame);
    splitter->addWidget(viewerMainFrame);

//    displayed_objects_.set_renderer(gstl_vtk_viewer_->get_renderer());

	QObject::connect(tree_, SIGNAL(	clicked ( const QModelIndex &)),
					this, SLOT(item_single_clicked( const QModelIndex &)) );
//	QObject::connect(tree_, SIGNAL(	doubleClicked ( const QModelIndex &)),
//					this, SLOT(item_double_clicked( const QModelIndex &)) );

	QObject::connect(model_, SIGNAL(	item_checked ( const QModelIndex &)),
					this, SLOT(vizualization_selection( const QModelIndex &)) );

	tree_->setContextMenuPolicy(Qt::CustomContextMenu);

	QObject::connect(this, SIGNAL(	rendering_window_modified ( )),
					gstl_vtk_viewer_, SLOT(update()) );

	QObject::connect(dynamic_cast<Root_model*>(Root::instance()->model()), SIGNAL(	rowsAboutToBeRemoved(const QModelIndex& , int , int )),
					this, SLOT(remove_viz_parameters(const QModelIndex&, int, int)) );

  

//	connect(tree_, SIGNAL(customContextMenuRequested(const QPoint&)),
//	    this, SLOT(contextMenu(const QPoint&)));
}

Vtk_view::~Vtk_view() {
	if(connections_) connections_->Delete();

}

// The left-hand side of the splitter
QFrame* Vtk_view::init_viewer_frame(){

	QFrame* viewerMainFrame = new QFrame(this);
	viewerMainFrame->setAcceptDrops(true);
	QVBoxLayout *lhsMainLayout = new QVBoxLayout(viewerMainFrame);


	gstl_vtk_viewer_ = new QvtkGsTLViewer(this, "camera");
	gstl_vtk_viewer_->setAcceptDrops(true);
 //   QFrame* viewerFrame = new QFrame(viewerMainFrame);
	QSizePolicy sizePolicyViewFrame(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicyViewFrame.setHorizontalStretch(70);
	sizePolicyViewFrame.setVerticalStretch(0);
	sizePolicyViewFrame.setHeightForWidth(viewerMainFrame->sizePolicy().hasHeightForWidth());
	gstl_vtk_viewer_->setSizePolicy(sizePolicyViewFrame);
	gstl_vtk_viewer_->setMinimumSize(QSize(100, 100));
//	gstl_vtk_viewer_->setFrameShape(QFrame::Box);
//	gstl_vtk_viewer_->setFrameShadow(QFrame::Plain);
//	gstl_vtk_viewer_->setMidLineWidth(2);

// Add the control buttons
	QHBoxLayout *hboxControlButton = new QHBoxLayout();
	hboxControlButton->setSpacing(6);
	hboxControlButton->setContentsMargins(0, 0, 0, 0);
	QSpacerItem *spacerItemFront = new QSpacerItem(16, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

	hboxControlButton->addItem(spacerItemFront);

	QSizePolicy sizePolicyButton(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicyButton.setHorizontalStretch(0);
	sizePolicyButton.setVerticalStretch(0);
	sizePolicyButton.setHeightForWidth(true);

	view_all_button_ = new QPushButton(viewerMainFrame);
	view_all_button_->setObjectName(QString::fromUtf8("view_all_button_"));
	view_all_button_->setSizePolicy(sizePolicyButton);
	view_all_button_->setMaximumSize(QSize(31, 31));
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/global.png"), QSize(), QIcon::Normal, QIcon::Off);
	view_all_button_->setIcon(icon);
	view_all_button_->setIconSize(QSize(25, 25));
	view_all_button_->setFlat(true);
	view_all_button_->setToolTip("View All");

	hboxControlButton->addWidget(view_all_button_);

	face_view_button_ = new QPushButton(viewerMainFrame);
	face_view_button_->setObjectName(QString::fromUtf8("face_view_button_"));
	sizePolicyButton.setHeightForWidth(face_view_button_->sizePolicy().hasHeightForWidth());
	face_view_button_->setSizePolicy(sizePolicyButton);
	face_view_button_->setMaximumSize(QSize(31, 31));
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/xy.png"), QSize(), QIcon::Normal, QIcon::Off);
	face_view_button_->setIcon(icon1);
	face_view_button_->setIconSize(QSize(25, 25));
	face_view_button_->setFlat(true);
	face_view_button_->setToolTip("Face View");

	hboxControlButton->addWidget(face_view_button_);

	side_view_button_ = new QPushButton(viewerMainFrame);
	side_view_button_->setObjectName(QString::fromUtf8("side_view_button_"));
	sizePolicyButton.setHeightForWidth(side_view_button_->sizePolicy().hasHeightForWidth());
	side_view_button_->setSizePolicy(sizePolicyButton);
	side_view_button_->setMaximumSize(QSize(31, 31));
	QIcon icon2;
	icon2.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/yz.png"), QSize(), QIcon::Normal, QIcon::Off);
	side_view_button_->setIcon(icon2);
	side_view_button_->setIconSize(QSize(25, 25));
	side_view_button_->setFlat(true);
	side_view_button_->setToolTip("Side View");

	hboxControlButton->addWidget(side_view_button_);

	top_view_button_ = new QPushButton(viewerMainFrame);
	top_view_button_->setObjectName(QString::fromUtf8("top_view_button_"));
	sizePolicyButton.setHeightForWidth(top_view_button_->sizePolicy().hasHeightForWidth());
	top_view_button_->setSizePolicy(sizePolicyButton);
	top_view_button_->setMaximumSize(QSize(31, 31));
	QIcon icon3;
	icon3.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/xz.png"), QSize(), QIcon::Normal, QIcon::Off);
	top_view_button_->setIcon(icon3);
	top_view_button_->setIconSize(QSize(25, 25));
	top_view_button_->setFlat(true);
	top_view_button_->setToolTip("Plan View");

	hboxControlButton->addWidget(top_view_button_);

	home_view_button_ = new QPushButton(viewerMainFrame);
	home_view_button_->setObjectName(QString::fromUtf8("home_view_button_"));
	sizePolicyButton.setHeightForWidth(home_view_button_->sizePolicy().hasHeightForWidth());
	home_view_button_->setSizePolicy(sizePolicyButton);
	home_view_button_->setMaximumSize(QSize(31, 31));
	QIcon icon4;
	icon4.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/home.png"), QSize(), QIcon::Normal, QIcon::Off);
	home_view_button_->setIcon(icon4);
	home_view_button_->setIconSize(QSize(25, 25));
	home_view_button_->setFlat(true);
	home_view_button_->setToolTip("Home View");

	hboxControlButton->addWidget(home_view_button_);

	set_home_view_button_ = new QPushButton(viewerMainFrame);
	set_home_view_button_->setObjectName(QString::fromUtf8("set_home_view_button_"));
	sizePolicyButton.setHeightForWidth(set_home_view_button_->sizePolicy().hasHeightForWidth());
	set_home_view_button_->setSizePolicy(sizePolicyButton);
	set_home_view_button_->setMaximumSize(QSize(31, 31));
	QIcon icon5;
	icon5.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/sethome.png"), QSize(), QIcon::Normal, QIcon::Off);
	set_home_view_button_->setIcon(icon5);
	set_home_view_button_->setIconSize(QSize(25, 25));
	set_home_view_button_->setFlat(true);
	set_home_view_button_->setToolTip("Set Home View");
	hboxControlButton->addWidget(set_home_view_button_);


	background_button_ = new QPushButton(viewerMainFrame);
	background_button_->setObjectName(QString::fromUtf8("background_button_"));
	sizePolicyButton.setHeightForWidth(background_button_->sizePolicy().hasHeightForWidth());
	background_button_->setSizePolicy(sizePolicyButton);
	background_button_->setMaximumSize(QSize(31, 31));
	QIcon icon_background;
	icon_background.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/background_color.svg"), QSize(), QIcon::Normal, QIcon::Off);
	background_button_->setIcon(icon_background);
	background_button_->setIconSize(QSize(25, 25));
	background_button_->setCheckable(true);
	background_button_->setChecked(false);
	background_button_->setToolTip("Switch background color between black and white");
	hboxControlButton->addWidget(background_button_);


	legend_scale_button_ = new QPushButton(viewerMainFrame);
	legend_scale_button_->setObjectName(QString::fromUtf8("legend_scale_button_"));
	sizePolicyButton.setHeightForWidth(legend_scale_button_->sizePolicy().hasHeightForWidth());
	legend_scale_button_->setSizePolicy(sizePolicyButton);
	legend_scale_button_->setMaximumSize(QSize(31, 31));
	QIcon icon_scale;
	icon_scale.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/scale_legend.svg"), QSize(), QIcon::Normal, QIcon::Off);
	legend_scale_button_->setIcon(icon_scale);
	legend_scale_button_->setIconSize(QSize(25, 25));
	legend_scale_button_->setCheckable(true);
//	legend_scale_button_->setChecked(true);
	legend_scale_button_->setToolTip("View Scale");
	hboxControlButton->addWidget(legend_scale_button_);

	snapshot_button_ = new QPushButton(viewerMainFrame);
	snapshot_button_->setObjectName(QString::fromUtf8("snapshot_button_"));
	sizePolicyButton.setHeightForWidth(snapshot_button_->sizePolicy().hasHeightForWidth());
	snapshot_button_->setSizePolicy(sizePolicyButton);
	snapshot_button_->setMaximumSize(QSize(31, 31));
	QIcon icon6;
	icon6.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/camera.png"), QSize(), QIcon::Normal, QIcon::Off);
	snapshot_button_->setIcon(icon6);
	snapshot_button_->setIconSize(QSize(25, 25));
	snapshot_button_->setFlat(true);
	snapshot_button_->setToolTip("Take Snapshot");
	hboxControlButton->addWidget(snapshot_button_);

	orientation_button_ = new QPushButton(viewerMainFrame);
	orientation_button_->setObjectName(QString::fromUtf8("orientation_button_"));
	sizePolicyButton.setHeightForWidth(orientation_button_->sizePolicy().hasHeightForWidth());
	orientation_button_->setSizePolicy(sizePolicyButton);
	orientation_button_->setMaximumSize(QSize(31, 31));
	QIcon icon_orientation;
	icon_orientation.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/orientation_widget.svg"), QSize(), QIcon::Normal, QIcon::Off);
	orientation_button_->setIcon(icon_orientation);
	orientation_button_->setIconSize(QSize(25, 25));
	orientation_button_->setCheckable(true);
	orientation_button_->setChecked(true);
	orientation_button_->setToolTip("Turn on/off the orientation marker");
	hboxControlButton->addWidget(orientation_button_);


	distance_button_ = new QPushButton(viewerMainFrame);
	distance_button_->setObjectName(QString::fromUtf8("distance_button_"));
	sizePolicyButton.setHeightForWidth(distance_button_->sizePolicy().hasHeightForWidth());
	distance_button_->setSizePolicy(sizePolicyButton);
	distance_button_->setMaximumSize(QSize(31, 31));
	QIcon icon_distance;
	icon_distance.addFile(QString::fromUtf8(":/icons/appli/Pixmaps/distance_widget.svg"), QSize(), QIcon::Normal, QIcon::Off);
	distance_button_->setIcon(icon_distance);
	distance_button_->setIconSize(QSize(25, 25));
	distance_button_->setCheckable(true);
	distance_button_->setChecked(false);
	distance_button_->setToolTip("Turn on/off the distance rulers");
	hboxControlButton->addWidget(distance_button_);

	QSpacerItem *spacerItemback = new QSpacerItem(190, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	hboxControlButton->addItem(spacerItemback);


	lhsMainLayout->addWidget(gstl_vtk_viewer_);
	lhsMainLayout->addLayout(hboxControlButton);
	viewerMainFrame->setLayout(lhsMainLayout);

	QObject::connect(view_all_button_, SIGNAL(clicked()), this, SLOT(view_all()));
	QObject::connect(set_home_view_button_, SIGNAL(clicked()), this, SLOT(set_home_view()));
	QObject::connect(home_view_button_, SIGNAL(clicked()), this, SLOT(home_view()));
	QObject::connect(face_view_button_, SIGNAL(clicked()), this, SLOT(face_view()));
	QObject::connect(side_view_button_, SIGNAL(clicked()), this, SLOT(side_view()));
	QObject::connect(top_view_button_, SIGNAL(clicked()), this, SLOT(top_view()));
	QObject::connect(snapshot_button_, SIGNAL(clicked()), this, SLOT(snapshot()));
	bool ok = QObject::connect(background_button_, SIGNAL(clicked(bool)),
					this, SLOT(set_white_background(bool)));

	ok = QObject::connect(legend_scale_button_, SIGNAL(clicked(bool)),
						  gstl_vtk_viewer_, SLOT(set_scale_legend_visible(bool)) );
	ok = QObject::connect(orientation_button_, SIGNAL(toggled(bool)),
						  gstl_vtk_viewer_, SLOT(set_orientiation_axis_visible(bool)) );
	ok = QObject::connect(distance_button_, SIGNAL(toggled(bool)),
						  gstl_vtk_viewer_, SLOT(enable_distance_ruler(bool)) );
	// Set the signal for picking
	  connections_ = vtkEventQtSlotConnect::New();
	  connections_->Connect(gstl_vtk_viewer_->GetRenderWindow()->GetInteractor(),
	  vtkCommand::EndPickEvent,this,SLOT(pick(vtkObject*)));

	return viewerMainFrame;

}

// The right-hand side of the splitter
QWidget* Vtk_view::init_control_frame(){


	tree_ = new Visualization_tree_view(this);

  /*
	SmartPtr<Named_interface> model_ni =
			Root::instance()->interface(qitem_model_manager + "/project_model");
	Project_ItemModel* model = dynamic_cast<Project_ItemModel*>(model_ni.raw_ptr());
  */
//  Filter_root_proxy_model* model = new Filter_root_proxy_model(this);
  //model_ = new Filter_root_proxy_model(this);
  //model_ = new Filter_root_proxy_checkable_model(this);
  model_ = new Visualization_proxy_model(this);
  tree_->setModel(model_);
//  Root_model* mm = dynamic_cast<Root_model*>(Root::instance()->model());
//  tree_->setModel(mm);
//  tree_->setRootIndex(model_->project_root_index());
  //tree_->setSelectionMode(QAbstractItemView::NoSelection);
  tree_->setSelectionMode(QAbstractItemView::SingleSelection);
//  tree_->setRootIndex(mm->project_root_index());
 // tree_->setSelectionModel(new Project_selection_model(model_,tree_));

	
  /*
	Project_proxy_model *model = new Project_proxy_model(this);
	model->set_viz_parameters(&viz_params_);
	tree_->setModel(model);
	tree_->setSelectionModel(new Project_selection_model(model,tree_));
  tree_->setRootIndex(model->index(0,0,QModelIndex()));
 */
	params_viewer_ = new QFrame (this);
	params_viewer_->setLayout(new QVBoxLayout(this));

	QSplitter *controlSplitter = new QSplitter();
	controlSplitter->setOrientation(Qt::Vertical);
	controlSplitter->setHandleWidth(1);
	controlSplitter->addWidget(tree_);
//	controlSplitter->addWidget(params_frame);
	controlSplitter->addWidget(params_viewer_);
	controlSplitter->setAcceptDrops(true);
	return controlSplitter;

}


const QvtkGsTLViewer* Vtk_view::get_render_area() const
{
	return gstl_vtk_viewer_;
}
QvtkGsTLViewer* Vtk_view::get_render_area()
{
	return gstl_vtk_viewer_;
}

vtkRenderWindowInteractor* Vtk_view::get_interactor()
{
	return gstl_vtk_viewer_->GetInteractor();
}


// Project View classes
void Vtk_view::update(std::string obj) {

//	Vtk_prop_map::iterator it = displayed_objects_.find(obj);
//
//	if( it == displayed_objects_.end()  ) return;
//
//	it->second->update(VTK::PROPERTY_CHANGED);
/*
	// Update the display preference panels
	for (Pref_Panel_Map::iterator it2 = display_pref_panels_.begin(); it2 != display_pref_panels_.end(); ++it2)
	{
		it2->second->update();
	}
*/
}
void Vtk_view::new_object(std::string obj){

}

void Vtk_view::deleted_object(std::string obj){

//	displayed_objects_.delete_vtk_prop(obj);
//	display_pref_panels_.erase(obj);

}



void Vtk_view::view_all()
{
	gstl_vtk_viewer_->get_renderer()->ResetCamera();
  emit this->rendering_window_modified( );
}

void Vtk_view::set_home_view()
{
  vtkCamera* camera = gstl_vtk_viewer_->get_renderer()->GetActiveCamera();

   camera->GetPosition(camera_view_.camPos);
   camera->GetViewUp(camera_view_.viewUp);
   camera->GetFocalPoint(camera_view_.fp);
   camera_view_.parallel = camera->GetParallelProjection();
//   camera_view_.useHorizontalViewAngle = camera->GetUseHorizontalViewAngle();
//   camera_view_.viewAngle = camera->GetViewAngle();
   camera_view_.parallelScale= camera->GetParallelScale();
   camera_view_.is_initialized = true;

  emit this->rendering_window_modified( );
}

void Vtk_view::home_view()
{

 if(!camera_view_.is_initialized) {
   this->set_home_view();
   return;
 }

 vtkCamera* camera = gstl_vtk_viewer_->get_renderer()->GetActiveCamera();
 camera->SetPosition(camera_view_.camPos);
 camera->SetViewUp(camera_view_.viewUp);
 camera->SetFocalPoint(camera_view_.fp);
 camera->SetParallelProjection(camera_view_.parallel);
// camera->SetUseHorizontalViewAngle(camera_view_.useHorizontalViewAngle );
// camera->SetViewAngle(camera_view_.viewAngle );
 camera->SetParallelScale(camera_view_.parallelScale);


   emit this->rendering_window_modified( );

}

void Vtk_view::face_view()   //XZ
{
  GsTLVector<float> new_focal_point(0, 0, 0);
  GsTLVector<float> new_position(0, -1, 0);
  GsTLVector<float> new_view_up(0, 0, 1);

  set_view_plane(new_focal_point, new_position,new_view_up);
}

void Vtk_view::side_view()  //YZ
{
  GsTLVector<float> new_focal_point(0, 0, 0);
  GsTLVector<float> new_position(1, 0, 0);
  GsTLVector<float> new_view_up(0, 0, 1);

  set_view_plane(new_focal_point, new_position,new_view_up);
}

void Vtk_view::top_view()  //XY
{
  GsTLVector<float> new_focal_point(0, 0, 0);
  GsTLVector<float> new_position(0, 0, 1);
  GsTLVector<float> new_view_up(0, 1, 0);

  set_view_plane(new_focal_point, new_position,new_view_up);
}

void Vtk_view::snapshot()
{
  Snapshot_dialog* dialog = new Snapshot_dialog(gstl_vtk_viewer_->GetRenderWindow(), this);
  dialog->show();
}


void Vtk_view::reRender()
{
  gstl_vtk_viewer_->render();
  emit this->rendering_window_modified( );
}

void Vtk_view::set_view_plane(const GsTLVector<float>& focal_point,
                              const GsTLVector<float>& position,
                              const GsTLVector<float>& view_up)
{
  vtkCamera* camera = gstl_vtk_viewer_->get_renderer()->GetActiveCamera();

  camera->SetFocalPoint(focal_point[0],focal_point[1],focal_point[2]);
  camera->SetPosition(position[0],position[1],position[2]);
  camera->SetViewUp(view_up[0],view_up[1],view_up[2]);

  gstl_vtk_viewer_->get_renderer()->ResetCamera();
  gstl_vtk_viewer_->get_renderer()->ResetCameraClippingRange();

   emit this->rendering_window_modified( );

}


void Vtk_view::item_single_clicked ( const QModelIndex & index ){
//	show_preferences_view(index);
  this->display_viz_parameters(model_->mapToSource(index));
//  this->display_viz_parameters(index);
/*
	if(tree_->selectionModel()->isSelected(index)) {
		this->display_item(index);
	}
	else {
		this->undisplay_item(index);
	}\*/
}

void Vtk_view::item_double_clicked ( const QModelIndex & index ){
 QModelIndex index_src = model_->mapToSource(index);
// QModelIndex index_src = index;
	tree_->selectionModel()->select(index, QItemSelectionModel::Select); // The selection command is ignored
	Visualization_parameters* viz_param = get_viz_parameters( index_src );

	if(viz_param && tree_->selectionModel()->isSelected(index)) {
		viz_param->display();
//		this->display_item(index);
	}
	else {
		if(viz_param) viz_param->undisplay();
	}

  emit this->rendering_window_modified();

}


void Vtk_view::vizualization_selection ( const QModelIndex & index ){
 QModelIndex index_src = model_->mapToSource(index);
// QModelIndex index_src = index;
	//tree_->selectionModel()->select(index, QItemSelectionModel::Select); // The selection command is ignored
	Visualization_parameters* viz_param = get_viz_parameters( index_src );

//  QProgressDialog progress("Rendering in progress", QString(), 0, 0);
//  progress.setWindowModality(Qt::WindowModal);
//  progress.show();
  if(viz_param && model_->data(index,Qt::CheckStateRole).toBool() ) {
		viz_param->display();
//		this->display_item(index);
	}
	else {
		if(viz_param) viz_param->undisplay();
	}
//  progress.accept();

  emit this->rendering_window_modified();

}


GsTL_vtkProp* Vtk_view::get_vtk_prop( Geostat_grid* grid) {
	std::map<void*, Visualization_parameters*>::iterator it = viz_params_.find(static_cast<void*>(grid));

	if(it == viz_params_.end()) {
		Named_interface* ni = Root::instance()->new_interface_raw(grid->classname(),grid_viewer_manager+"/");
		Grid_viz_parameters* grid_viz_param = dynamic_cast<Grid_viz_parameters*>(ni);
		grid_viz_param->initialize(grid,gstl_vtk_viewer_->get_renderer());
		it = viz_params_.insert(std::make_pair(static_cast<void*>(grid),grid_viz_param)).first;
	}
	return it->second->vtk_prop();

}


Visualization_parameters* Vtk_view::get_viz_parameters( const QModelIndex & index ) {
	GsTL_object_item* item = static_cast<GsTL_object_item*>(index.internalPointer());
  QString t = item->item_name();
	return this->get_viz_parameters(item);
}

Visualization_parameters* Vtk_view::get_viz_parameters( GsTL_object_item* item)
{


	void* data_ptr = item->data_pointer();
	std::map<void*, Visualization_parameters*>::iterator it = viz_params_.find(data_ptr);

	if(it != viz_params_.end()) return it->second;

	Geostat_grid* grid = dynamic_cast<Geostat_grid*>(item);
	if( grid ) {
		Named_interface* ni = Root::instance()->new_interface_raw(grid->classname(),grid_viewer_manager+"/");
		Grid_viz_parameters* viz_param = dynamic_cast<Grid_viz_parameters*>(ni);
		viz_param->initialize(grid,gstl_vtk_viewer_->get_renderer());
		viz_params_.insert(std::make_pair(static_cast<void*>(grid),viz_param));
		viz_param->interface()->setParent(params_viewer_);

    Grid_param_viewer* viz_param_viewer = dynamic_cast<Grid_param_viewer*>(viz_param->interface());
    bool ok = QObject::connect(viz_param_viewer, SIGNAL(rendering_modified()), gstl_vtk_viewer_, SLOT(update()) );
    ok = QObject::connect(this, SIGNAL(background_color_changed()), viz_param_viewer, SLOT(update_from_background_color_changed()) );

		return viz_param;
	}


	GsTLGridProperty* prop = dynamic_cast<GsTLGridProperty*>(item);
	if(prop == 0 ){
		GsTL_object_property_item* prop_item = dynamic_cast<GsTL_object_property_item*>(item);
		if(prop_item) prop = prop_item->property();
	}
	if(prop) {
		grid = dynamic_cast< Geostat_grid*>(prop->parent()->parent());
		GsTL_vtkProp* vtk_prop = this->get_vtk_prop( grid );
		Named_interface* ni = Root::instance()->new_interface_raw("PropertyParamViewer",prop_viewer_manager+"/");
		Property_viz_parameters* viz_param = dynamic_cast<Property_viz_parameters*>(ni);
		viz_param->initialize(prop,vtk_prop);
		viz_param->interface()->setParent(params_viewer_);
		viz_params_.insert(std::make_pair(static_cast<void*>(prop),viz_param));

    Property_param_viewer* viz_param_viewer = dynamic_cast<Property_param_viewer*>(viz_param->interface());
    QObject::connect(viz_param_viewer, SIGNAL(rendering_modified()), gstl_vtk_viewer_, SLOT(update()) );

		return viz_param;
	}

	GsTLGridPropertyGroup* group = dynamic_cast<GsTLGridPropertyGroup*>(item);
	if(group) {
		grid = dynamic_cast< Geostat_grid*>(group->parent());
		GsTL_vtkProp* vtk_prop = this->get_vtk_prop( grid );

		Named_interface* ni = Root::instance()->new_interface_raw("GroupParamViewer",group_viewer_manager+"/");
		Group_viz_parameters* viz_param = dynamic_cast<Group_viz_parameters*>(ni);
		viz_param->initialize(group, vtk_prop);
		viz_param->interface()->setParent(params_viewer_);
		viz_params_.insert(std::make_pair(static_cast<void*>(group),viz_param));
		return viz_param;
	}
	Grid_region* region = dynamic_cast<Grid_region*>(item);
	if(region) {
		grid = dynamic_cast< Geostat_grid*>(region->parent()->parent());
		GsTL_vtkProp* vtk_prop = this->get_vtk_prop( grid );
		Named_interface* ni = Root::instance()->new_interface_raw("RegionParamViewer",region_viewer_manager+"/");
		Region_viz_parameters* viz_param = dynamic_cast<Region_viz_parameters*>(ni);
		viz_param->initialize(region, vtk_prop);
		viz_param->interface()->setParent(params_viewer_);
		viz_params_.insert(std::make_pair(static_cast<void*>(region),viz_param));
		return viz_param;
	}

  
	return 0;


}


void Vtk_view::display_viz_parameters( const QModelIndex & index ){

	Visualization_parameters* viz_param = get_viz_parameters( index);

	QLayoutItem* layout_item = params_viewer_->layout()->itemAt(0);
	QWidget* displayed_param;
	if(layout_item == 0)
		displayed_param = 0;
	else
		displayed_param = layout_item->widget();

	if( viz_param == 0 ) {  // Nothing to show
		if( displayed_param != 0) {
			params_viewer_->layout()->removeWidget(displayed_param);
			displayed_param->setHidden(true);
		}
	}

	else if( displayed_param != viz_param->interface() ) {
		if(displayed_param != 0) {
			params_viewer_->layout()->removeWidget(displayed_param);
			displayed_param->setHidden(true);
		}
		displayed_param = viz_param->interface();
		displayed_param->setVisible(true);
		params_viewer_->layout()->addWidget(displayed_param);
	}

	/*
	if( viz_param == 0 ) {  // Nothing to show
		if( displayed_viewer_ != 0) displayed_viewer_->setHidden(true);
	}

	else if( displayed_viewer_ != viz_param->interface() ) {
		if(displayed_viewer_ != 0) {
			params_viewer_->layout()->removeWidget(displayed_viewer_);
			displayed_viewer_->setHidden(true);
		}
		displayed_viewer_ = viz_param->interface();
		displayed_viewer_->setVisible(true);
		params_viewer_->layout()->addWidget(displayed_viewer_);
	}
*/
}


void Vtk_view::display_item ( const QModelIndex & index ){
	Visualization_parameters* viz_param = get_viz_parameters( index);
	viz_param->display();

}

void Vtk_view::undisplay_item ( const QModelIndex & index ){
	Visualization_parameters* viz_param = get_viz_parameters( index);
	viz_param->undisplay();
}


bool Vtk_view::eventFilter(QObject *obj, QEvent *ev){
	
	if (ev->type() == QEvent::Drop) {
        return true;
	} else if (ev->type() == QEvent::DragEnter){
        return true;
	} else if (ev->type() == QEvent::DragMove){
        return true;
    }
	else {
		return false;
    }

}

void Vtk_view::dragMoveEvent(QDragMoveEvent* e)
{
//	if (e->mimeData()->hasUrls())
//		e->acceptProposedAction();
}

void Vtk_view::dragEnterEvent(QDragEnterEvent * e)
{
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
	else e->ignore();

}

void Vtk_view::dragLeaveEvent(QDragLeaveEvent *)
{

}

void Vtk_view::dropEvent(QDropEvent * e)
{
	Error_messages_handler error_messages;
	bool ok = true;

	if (!e->mimeData()->hasUrls())
		return;
	QList<QUrl> urls = e->mimeData()->urls();

	for (int i = 0; i < urls.size(); ++i)
	{
		QString s = urls[i].toLocalFile();

		if (s.isEmpty())
			continue;

		appli_message("Loading file " << s.toStdString());
		QFileInfo f_info(s);
		if (f_info.isDir() && (s.endsWith(".prj", Qt::CaseInsensitive) || s.endsWith(".prj/", Qt::CaseInsensitive)))
		{
			ok *= project_->execute("LoadProject", s.toStdString(), &error_messages);
		} else
		{
			if (s.endsWith(".prt", Qt::CaseInsensitive))
			{
				ok *= project_->execute("LoadSimFromFile", s.toStdString(), &error_messages);
			} else
			{
				std::string param(s.toStdString() + Actions::separator + "All");
				ok *= project_->execute("LoadObjectFromFile", param, &error_messages);
			}
		}
	}
	if (!ok)
	{
		GsTLcerr << "All the selected files could not be loaded. \n";
		if (!error_messages.empty())
			GsTLcerr << error_messages.errors() << "\n";
		GsTLcerr << gstlIO::end;
	}

}

void Vtk_view::execute_action(QString action_name, QString params)
{
	Error_messages_handler error_messages;

	bool ok = project_->execute(action_name.toStdString(), params.toStdString(), &error_messages);

	if (!ok)
	{
		GsTLcerr << "Command " << action_name.toStdString() << " could not be performed: \n";
		if (!error_messages.empty())
			GsTLcerr << error_messages.errors() << "\n";
		GsTLcerr << gstlIO::end;
		return;
	}

//	this->update("");
}


void Vtk_view::set_white_background( bool on ) {

    if( on )
    	gstl_vtk_viewer_->set_background_color(1.0,1.0,1.0);
    else
    	gstl_vtk_viewer_->set_background_color( 0., 0., 0. );

    emit this->background_color_changed();

//  Loop over all the vtkprop and set the color label of the colorbar 

}

void Vtk_view::pick(vtkObject* caller){
  vtkRenderWindowInteractor* iren =
vtkRenderWindowInteractor::SafeDownCast(caller);

 vtkCellPicker *cellPicker = vtkCellPicker::SafeDownCast(iren->GetPicker());
 cellPicker->PrintSelf(cerr,vtkIndent());
 std::ostrstream picking;
 long nodeid = cellPicker->GetCellId();
 vtkDataSet* dset = cellPicker->GetDataSet();
 if(dset == 0) {
	 GsTLcout.clear();
	 return;
 }
 vtkCell* cell = dset->GetCell(nodeid);
 double loc[3];
 double pcoord[3];
 double *weight = new double[8];
 cellPicker->GetPCoords(pcoord);
 int subId = cellPicker->GetSubId();
 cell->EvaluateLocation( subId,pcoord,loc, weight);
 double value;
 
 vtkDataArray* cellData = dset->GetCellData()->GetScalars();
 if( cellData!= 0 )
  value = dset->GetCellData()->GetScalars()->GetComponent(0,nodeid);
 else
   value = GsTLGridProperty::no_data_value;

 GsTLcout <<"Value : "<<value<<"  (x,y,z) :("<<loc[0]<<","<<loc[1]<<","<<loc[2]<<")"<< gstlIO::end;
// GsTLcout << "Node id: "<<nodeid<<" Value : "<<value<<" (x,y,z) :("<<loc[0]<<","<<loc[1]<<","<<loc[2]<<")"<< gstlIO::end;
}

void Vtk_view::remove_viz_parameters(const QModelIndex & parent, int start, int end){

  		for(int i=start;i<=end;++i) {
			  QModelIndex index = parent.child(i,0);
			  if(!index.isValid()) continue;
			  void* ptr = index.internalPointer();
			  std::map<void*, Visualization_parameters*>::iterator it = viz_params_.find(ptr);
			  if(it == viz_params_.end()) continue;

        //if we are removing a grid.  Must manually removed all the properties (children), regions belonging to that grid.
        GsTL_object_item* item = static_cast<GsTL_object_item*>(ptr);
        if(dynamic_cast<Geostat_grid*>(item) != 0) {
          std::vector<void*> to_be_removed;
          std::map<void*, Visualization_parameters*>::iterator iit = viz_params_.begin();
          for( ;  iit != viz_params_.end(); ++iit ) {
            if( iit->first != item && iit->second->grid() == item ) {
              to_be_removed.push_back(iit->first);
            }
          }
          for(int i=0; i<to_be_removed.size(); ++i) {
            iit = viz_params_.find(to_be_removed[i]);
            Visualization_parameters* param = iit->second;
            viz_params_.erase(iit);
            delete param;
          }
        }
        //Delete the actual items
//        it = viz_params_.find(ptr);
        Visualization_parameters* param = it->second;
			  viz_params_.erase(it);
        delete param;

			  
		}
}