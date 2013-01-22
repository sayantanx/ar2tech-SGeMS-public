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

#ifndef __GSTLAPPLI_VTK_DESCRIPTION_POINTSET_H__
#define __GSTLAPPLI_VTK_DESCRIPTION_POINTSET_H__
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

class vtkPolyData;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkProp;
class vtkMaskPoints;
class vtkLookupTableWithEnabling;
class vtkGeometryFilter;

#include <vtkPassThrough.h>

 
class GUI_DECL vtkProp_pointset : public GsTL_vtkProp{
 public: 
  static Named_interface* create_new_interface( std::string& ); 
 
 public: 
  vtkProp_pointset();
  ~vtkProp_pointset();
 
//  virtual void init( const Geostat_grid* grid );
  virtual void init( Geostat_grid* grid, vtkRenderer* renderer );

  virtual vtkProp* prop();
  virtual void set_visibility(bool on);
  virtual bool is_visibile();

  virtual void enable_full_view();
  virtual void disable_full_view();

  virtual void property_display_mode( VTK::Property_display_mode mode );
  virtual void point_size( int size ); 
  virtual void property_deleted( const std::string& prop_name );

//  virtual void set_property( const std::string& property_name );
//  virtual void set_property(const GsTLGridProperty* prop );
  virtual void set_property( const std::string& property_name, Colormap* cmap );
  virtual void set_region(const std::string& region_name );
  virtual void set_colormap( Colormap* cmap );
  virtual void refresh_colormap();

 protected: 
  virtual void refresh(); 
  virtual bool connect_threshold_to_data(vtkThreshold* thresholder);
  virtual bool disable_threshold_pipeline();
  virtual bool enable_threshold_pipeline();
  virtual void set_colortable_to_mapper(vtkMapper* mapper);
 
 protected: 
  const Point_set* grid_; 

  vtkPolyData* pset_polydata_;
  vtkPolyDataMapper* mapper_;
  vtkPassThrough* data_pass_through_;


  vtkThreshold* region_threshold_;
  vtkGeometryFilter* region_filter_;
  vtkDataSetSurfaceFilter *vtk_surface_region_extractor_;

  vtkLookupTableWithEnabling* color_table_;


}; 
 
#endif 
