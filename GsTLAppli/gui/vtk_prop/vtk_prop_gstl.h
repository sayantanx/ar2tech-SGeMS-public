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




#ifndef __GSTLAPPLI_GUI_VTK_PROPS_H__
#define __GSTLAPPLI_GUI_VTK_PROPS_H__
 

#include <vtkProp.h>


#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/named_interface.h> 
#include <GsTLAppli/gui/vtk_prop/vtk_common.h>
 
#include <list> 
#include <vector>
#include <map> 


#include <vtkPropCollection.h>
#include <vtkActor.h>
#include <vtkThreshold.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkLegendBoxActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>

#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkPolyDataMapper.h>

#include <vtkAppendPolyData.h>

class Geostat_grid; 
class GsTLGridProperty;
class Grid_region;
class Colormap; 
class QString;

 
class GUI_DECL GsTL_vtkProp : public Named_interface {
 public: 

  GsTL_vtkProp();
  virtual ~GsTL_vtkProp();
  virtual void init( Geostat_grid* grid, vtkRenderer* renderer ) = 0;
  virtual const Geostat_grid* grid() const {return geostat_grid_; }
  virtual Geostat_grid* grid() {return geostat_grid_; }
 
  virtual vtkProp* prop()=0;// { return prop_; }
  vtkRenderer* get_renderer(){return renderer_;}
  virtual void set_visibility(bool on) = 0;
  virtual bool is_visibile() = 0;


  virtual void set_colormap( Colormap* cmap ){}
  virtual Colormap* get_colormap(){return cmap_;}
  virtual void refresh_colormap() = 0;

  virtual bool add_thresholding( int id, float min, float max, bool is_visible);
  virtual bool remove_thresholding(int id);
  virtual bool remove_all_thresholding();
  virtual bool update_thresholding(int id, float min, float max, bool is_visible);
  virtual bool enable_thresholding();
  virtual bool disable_thresholding();
  virtual bool update_thresholding_colortable();


  virtual int add_section(int id, QString orientation, bool is_visible){return 0;} //return total number of steps in that direction
  virtual bool remove_section(int id);
  virtual bool remove_all_sections();
  virtual bool update_section(int id, int steps, bool is_visible);
  virtual bool enable_section(int id);
  virtual bool disable_section(int id);

  virtual void enable_full_view()=0;
  virtual void disable_full_view()=0;

//  virtual void set_property(const GsTLGridProperty* prop )=0;

  /** Changes the current property and refreshes the display 
   */ 

  virtual void set_property( const std::string& property_name, Colormap* cmap );

  virtual std::string current_property_name() const { return current_property_name_; }
  virtual GsTLGridProperty* current_property() const { return current_property_; }
 
  virtual void set_region(const std::string& property_name )=0;
  virtual Grid_region* current_region() const { return current_region_; }

  void set_cell_representation(const QString& type );
  void show_edges(bool );

  void update_colorbar();
  void remove_colorbar();
	void set_colorbar_visible(bool visibility);
  void update_colorbar_visibility( );
	bool is_colorbar_visible();
  
  void is_cbar_requested(bool on);
  bool is_cbar_requested(){return is_cbar_requested_;}

  void set_colorbar_font_size(int size);
  void set_colorbar_title(const QString& font );
  void set_colorbar_height(double height);
  void set_colorbar_width( double width );
  void set_colorbar_x_position(double x);
  void set_colorbar_y_position(double y);
  void set_colorbar_position(double x, double y);
  void get_colorbar_position(double& x, double& y);


	void set_colorbar_width(float width);
	void set_colorbar_height(float height);
	void set_colorbars_text_color(bool is_background_white);
  void set_colorbars_text_color();
	void set_number_of_ticks(int n_ticks);



 protected: 
  virtual void refresh() = 0; 
  virtual void property_deleted( const std::string& prop_name ) = 0;

  virtual bool connect_threshold_to_data(vtkThreshold* thresholder)=0;
  virtual bool disable_threshold_pipeline(){return true;}
  virtual bool enable_threshold_pipeline(){return true;}
  virtual void set_colortable_to_mapper(vtkMapper* mapper)=0;

  virtual bool disable_section_pipeline(){return true;}
  virtual bool enable_section_pipeline(){return true;}

  bool compute_min_max( std::pair<float, float>& min_max,
			const GsTLGridProperty* property );


 protected: 

  vtkRenderer* renderer_;
  vtkActor* actor_;

  struct threshold_pipeline {
	  int id;
	  bool enabled;
	  vtkThreshold* thresholder;
	  vtkDataSetSurfaceFilter* surface;
  };
  vtkAppendPolyData* threshold_poly_data_;

  typedef std::map<int, threshold_pipeline> threshold_map;
  threshold_map threshold_pipelines_;

  bool is_thresold_active_;


  enum section_orientation {XSECTION, YSECTION, ZSECTION};

  struct section_pipeline {
    section_orientation orientation;
    int id;
    bool enabled;
    vtkPlane* plane;
    vtkCutter* cutter;
    vtkPolyDataMapper* mapper;
    vtkActor* actor;
  };
  vtkAppendPolyData* section_poly_data_;
  typedef std::map<int, section_pipeline> section_map;
  section_map section_pipelines_;

  bool is_section_active_;

  Geostat_grid* geostat_grid_;
  GsTLGridProperty* current_property_;
  Grid_region* current_region_;
  std::string current_property_name_;
  std::string current_region_name_;

 
 

  Colormap* cmap_; 
  typedef std::pair<float,float> Bounds_pair; 
  typedef std::map<std::string, std::pair<Bounds_pair, Colormap*> > CmapMap; 
  // cmap_map associates 2 elements to each property: 
  //   - the min and max values of the property 
  //   - a colormap to use to paint the property. The bounds of the colormap 
  //     need not be the min and max of the property 
  CmapMap cmap_map_; 


  vtkProperty* vtk_property_;
  vtkScalarBarActor* cbar_actor_;
  vtkScalarBarWidget* cbar_widget_;
  bool is_cbar_requested_; // synced with the user interface
 
}; 
 
#endif 
