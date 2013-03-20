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
 * vtk_prop_log.h
 *
 *  Created on: Jun 22, 2011
 *      Author: aboucher
 */

#ifndef VTK_PROP_LOG_H_
#define VTK_PROP_LOG_H_

#include <GsTLAppli/gui/vtk_prop/vtk_prop_gstl.h>
#include <GsTLAppli/grid/grid_model/log_data_grid.h>


#include <vtkProp.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkLookupTableWithEnabling.h>
#include <vtkThreshold.h>
#include <vtkMapper.h>
#include <vtkAppendPolyData.h>
#include <vtkPassThrough.h>

class vtkProp_log : public GsTL_vtkProp {
public:
 static Named_interface* create_new_interface( std::string& );

public:
 vtkProp_log();
 ~vtkProp_log();

//  virtual void init( const Geostat_grid* grid );
 virtual void init( Geostat_grid* grid, vtkRenderer* renderer );

 virtual vtkProp* prop();
 virtual void set_visibility(bool on);
 virtual bool is_visibile();

 virtual void enable_full_view();
 virtual void disable_full_view();

 virtual void line_width( int size );
 virtual void property_deleted( const std::string& prop_name );

//  virtual void set_property( const std::string& property_name );
//  virtual void set_property(const GsTLGridProperty* prop );
 virtual void set_property( const std::string& property_name, Colormap* cmap );
 virtual void set_property_with_region( const std::string& property_name, Colormap* cmap );
 virtual void set_region(const std::string& region_name );

 void set_colormap( Colormap* cmap );
 virtual void refresh_colormap();

protected:
 virtual void refresh();
 virtual bool connect_threshold_to_data(vtkThreshold* thresholder);
 virtual bool disable_threshold_pipeline();
 virtual bool enable_threshold_pipeline();
 virtual void set_colortable_to_mapper(vtkMapper* mapper);

protected:

 struct log_pipeline {
	 const Log_data* log_data;
	 vtkPolyData* vtk_data_log;
	 vtkPolyDataMapper* log_mapper;
	 vtkActor* log_actor;
 };

 const Log_data_grid* grid_;

 std::vector<log_pipeline> log_pipelines_;
 std::vector<log_pipeline> log_pipelines_region_;

 vtkAppendPolyData* append_log_;
 vtkPolyDataMapper* mapper_;
 vtkPassThrough* data_pass_through_;

 vtkLookupTableWithEnabling* color_table_;

 vtkAppendPolyData* region_append_log_;
// vtkPolyDataMapper* region_mapper_;
// vtkActor* region_actor_;

 Colormap* current_cmap_;

};

#endif /* VTK_PROP_LOG_H_ */
