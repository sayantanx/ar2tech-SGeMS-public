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
#include <vtkLegendBoxActor.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>

#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkPolyDataMapper.h>

#include <vtkAppendPolyData.h>

class Geostat_grid; 
class GsTLGridProperty;
class GsTLGridRegion;
class Colormap; 
class QString;
 
 
/** Oinv_description is the base class for all Open Inventor descriptions 
 * of grid models. Its aim is to describe a grid model (eg a cartesian grid  
 * or a triangulated surface) in Open Inventor terms, so that the grid can 
 * be displayed by Open Inventor.   
 */ 
 
class GUI_DECL GsTL_vtkProp : public Named_interface {
 public: 
	enum Display_mode { RENDERING=0, FULL=1, SLICES=2, BBOX=3 };
  GsTL_vtkProp();
  virtual ~GsTL_vtkProp();
//  virtual void init( const Geostat_grid* grid ) = 0;
  virtual void init( Geostat_grid* grid, vtkRenderer* renderer ) = 0;
  virtual const Geostat_grid* grid() const {return geostat_grid_; }
  virtual Geostat_grid* grid() {return geostat_grid_; }
 
  virtual vtkProp* prop()=0;// { return prop_; }
  vtkRenderer* get_renderer(){return renderer_;}
  virtual void set_visibility(bool on) = 0;
  virtual bool is_visibile() = 0;
  //virtual std::vector<vtkProp*> props()=0;// { return prop_; }
 
  /** Update the description of the geostat grid.  
   * @param property_name is a hint to which grid property has changed.  
   * If "property_name" is empty, the description is updated, even if it 
   * was not necessary. 
   */ 
//  virtual void update( const std::string& property_name, 
//		                   Oinv::Update_reason reason ) = 0; 
  virtual void update( VTK::Update_reason reason ,
    const std::vector<std::string>* properties = 0 );     
    //const std::string& property_name = "" ); 
 
 

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
  virtual GsTLGridRegion* current_region() const { return current_region_; }

  /** Turn on/off the painting of the property. 
   */ 
  virtual void property_display_mode( VTK::Property_display_mode mode )= 0;
  virtual VTK::Property_display_mode property_display_mode() const {
    return property_display_mode_; 
  } 
 

  void set_cell_representation(const QString& type );
  void show_edges(bool );

  virtual bool update_desc(QString &, QString &);

  void update_colorbar();
  void remove_colorbar();
	void set_colorbar_visible(bool visibility);
	bool is_colorbar_visible();
//	void set_colorbar_orientation(bool is_vertical);
	void set_colorbar_to_left();
	void set_colorbar_to_bottom();
	void set_colorbar_to_right();
	void set_colorbar_to_top();
	void set_colorbar_width(float width);
	void set_colorbar_height(float height);
	void set_colorbars_text_color(bool is_background_white);
	void set_number_of_ticks(int n_ticks);



 protected: 
  virtual void refresh() = 0; 
  virtual void property_deleted( const std::string& prop_name ) = 0;

  void set_colorbar_or_legend();

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
  GsTLGridRegion* current_region_;
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
 
  VTK::Property_display_mode property_display_mode_;

  vtkProperty* vtk_property_;
  vtkScalarBarActor* cbar_actor_;
  vtkLegendBoxActor* legend_actor_;
 

 
}; 
 
#endif 
