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
 * Vtk_project_view_gui.h
 *
 *  Created on: May 13, 2011
 *      Author: aboucher
 */

#ifndef VTK_PROJECT_VIEW_GUI_H_
#define VTK_PROJECT_VIEW_GUI_H_



#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/math/gstlvector.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/viewer/vtk_prop_map.h>
#include <GsTLAppli/gui/viewer/qvtkGsTLViewer.h>
#include <GsTLAppli/gui/viewer/visualization_parameters.h>
#include <GsTLAppli/gui/models/visualization_tree_view.h>
#include <GsTLAppli/gui/models/gstl_item.h>
#include <GsTLAppli/extra/qtplugins/project_proxy_model.h>

#include <QFrame>
#include <QTreeView>
#include <QDockWidget>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDragLeaveEvent>
#include <QTabWidget>
#include <QPoint>

#include <QPushButton>

#include <vtkEventQtSlotConnect.h>
#include <vtkRenderWindowInteractor.h>

class GUI_DECL Vtk_view: public QFrame, public Project_view
{

Q_OBJECT

public:
	Vtk_view(GsTL_project* project = 0, QWidget* parent = 0);
	virtual ~Vtk_view();

	void initialize(GsTL_project* project = 0, QWidget* parent = 0);

	void load_colorbar(QString & s){}

	const QvtkGsTLViewer* get_render_area() const;
	QvtkGsTLViewer* get_render_area();
	vtkRenderWindowInteractor* get_interactor();


	// Project View classes
	virtual void update(std::string obj);
	virtual void new_object(std::string obj);
	virtual void deleted_object(std::string obj);

public slots :

	void view_all();
	void set_home_view();
	void home_view();
	void face_view();
	void side_view();
	void top_view();
	void snapshot();
	void reRender();


	void display_viz_parameters( const QModelIndex & index );
	Visualization_parameters* get_viz_parameters( const QModelIndex & index );
	Visualization_parameters* get_viz_parameters( GsTL_object_item* item);

	GsTL_vtkProp* get_vtk_prop(  Geostat_grid* grid);


	void display_item ( const QModelIndex & index );
	void undisplay_item ( const QModelIndex & index );

	void item_single_clicked ( const QModelIndex & index );
	void item_double_clicked ( const QModelIndex & index );
  void vizualization_selection ( const QModelIndex & index );

	void set_white_background( bool on );

	void execute_action(QString _action_name, QString _params);

	void pick(vtkObject* caller);

  void remove_viz_parameters(const QModelIndex & source_parent, int start, int end);


signals :
  void rendering_window_modified();

protected :
	virtual void set_view_plane(const GsTLVector<float>& focal_point,
                              const GsTLVector<float>& position,
                              const GsTLVector<float>& view_up);


	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dragLeaveEvent(QDragLeaveEvent *);
	void dropEvent(QDropEvent *);



protected :
	QFrame* init_viewer_frame();
	QWidget* init_control_frame();

protected :
    QPushButton *view_all_button_;
    QPushButton *face_view_button_;
    QPushButton *side_view_button_;
    QPushButton *top_view_button_;
    QPushButton *home_view_button_;
    QPushButton *set_home_view_button_;
    QPushButton *snapshot_button_;
    QPushButton *background_button_;
    QPushButton *legend_scale_button_;
    QPushButton *orientation_button_;
    QPushButton *distance_button_;

    Visualization_tree_view *tree_;
    //Filter_root_proxy_model* model_;
    //Filter_root_proxy_checkable_model* model_;
    Visualization_proxy_model * model_;
    


protected :
    typedef std::map<void*, Visualization_parameters*> viz_param_mapT;
    std::map<void*, Visualization_parameters*> viz_params_;


	QvtkGsTLViewer* gstl_vtk_viewer_;

  struct Camera_view {
    bool is_initialized;
    double camPos[3];
    double viewUp[3];
    double fp[3];
    double viewAngle; 
    double parallelScale;
    int parallel;
    int useHorizontalViewAngle;
  };

  Camera_view camera_view_;

//	QDockWidget *params_viewer_;
	QFrame *params_viewer_;

	vtkEventQtSlotConnect* connections_;

};


Named_interface* Create_vtk_view(std::string&);

#endif /* VTK_PROJECT_VIEW_GUI_H_ */
