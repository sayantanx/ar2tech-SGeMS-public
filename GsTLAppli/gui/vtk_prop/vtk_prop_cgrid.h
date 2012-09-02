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



#ifndef __GSTLAPPLI_GUI_VTK_CGRID_H__
#define __GSTLAPPLI_GUI_VTK_CGRID_H__
  
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_strati_grid.h>
//#include <GsTLAppli/gui/vtk_prop/oinv_slice.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h> 

#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkThreshold.h>
#include <vtkUniformGrid.h>
#include <vtkUnsignedCharArray.h>
#include <vtkLookupTableWithEnabling.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPassThrough.h>

#include <vtkPlane.h>
#include <vtkCutter.h>
#include <vtkPolyDataMapper.h>

#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
//#include <vtkVolumeRayCastMapper.h>
#include <vtkImageShiftScale.h>
//#include <vtkVolumeTextureMapper2D.h>
//#include <vtkVolumeTextureMapper3D.h>

 
#include <map> 

class GUI_DECL vtkProp_cgrid : public GsTL_vtkProp {
 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
  static bool requires_manual_override( float x, float y, float z );

 public: 
  vtkProp_cgrid();
  virtual ~vtkProp_cgrid();
 
//  virtual void init( const Geostat_grid* grid );
  virtual void init( Geostat_grid* grid , vtkRenderer* renderer);

  virtual vtkProp* prop();
  virtual void set_visibility(bool on);
  virtual bool is_visibile();
 // virtual std::vector<vtkProp*> props();
 
//  virtual void update( const std::string& property_name, 
//		                   Oinv::Update_reason reason ); 
 
  virtual void set_property( const std::string& property_name );
  virtual void set_property( const std::string& property_name, Colormap* cmap );
//  virtual void set_property(const GsTLGridProperty* prop );
  virtual void set_region(const std::string& property_name );

  virtual void set_colormap( Colormap* cmap );
  virtual void refresh_colormap();

  virtual void enable_full_view();
  virtual void disable_full_view();

  virtual int add_slice( VTK::Axis axis, int position, bool visible );
  virtual void remove_slice( int slice_id );

  virtual int add_section(int id, QString orientation, bool is_visible);
  virtual bool update_section(int id, int steps, bool is_visible);

  virtual void display_mode( Display_mode mode ); 
  virtual void property_display_mode( VTK::Property_display_mode mode );

  virtual void show_bounding_box( bool on );
  virtual void hide_volume( bool on ) ;

  int vtk_3d_slices(vtkRenderWindowInteractor* iren);
  
 protected: 
  virtual void refresh();
  virtual void property_deleted( const std::string& prop_name );
  virtual bool connect_threshold_to_data(vtkThreshold* thresholder);
  virtual bool disable_threshold_pipeline();
  virtual bool enable_threshold_pipeline();
  virtual bool enable_section_pipeline();
  virtual bool disable_section_pipeline();

  virtual void set_colortable_to_mapper(vtkMapper* mapper);


 
 protected: 
//  typedef std::list< Oinv_slice* >::iterator SliceList_iterator;
   
  const Cartesian_grid* grid_; 

  vtkDataSetMapper* mapper_;

  vtkUniformGrid* image_data_;
  vtkUnsignedCharArray* vtk_blanking_;
  vtkPassThrough* data_pass_through_filter_;

  std::string cached_region_name_;
  vtkThreshold* region_threshold_;
  vtkDataSetSurfaceFilter *surface_extractor_;

  bool initialized_;
  
  // a transparency vector for each property
  std::map< std::string, QString > transparency_map_; 

  vtkPlane* implicit_plane_;
  vtkCutter* cutter_;
  vtkPolyDataMapper* cut_mapper_;
  vtkActor* cut_actor_;
  //  vtkActor* actor_;
  //  vtkProperty* vtk_property_;

  //  vtkVolume* volume_;
  //  vtkVolumeRayCastMapper* volume_mapper_;
  //  vtkVolumeTextureMapper2D* volume_mapper_;
    //vtkVolumeTextureMapper3D* volume_mapper_;
  //  vtkVolumeProperty* vtk_volume_property_;
}; 
 
 
 
 
#endif 
