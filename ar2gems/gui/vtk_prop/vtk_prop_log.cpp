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
 * vtk_prop_log.cpp
 *
 *  Created on: Jun 22, 2011
 *      Author: aboucher
 */

#include <gui/vtk_prop/vtk_prop_log.h>

#include <gui/vtk_prop/vtk_prop_pointset.h>
#include <grid/grid_model/point_set.h>
#include <gui/utils/colormap.h>


#include <vtkSmartPointer.h>
#include <vtkMaskPoints.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkLine.h>

#include <vtkMath.h>

Named_interface* vtkProp_log::create_new_interface( std::string& ) {
  return new vtkProp_log;
}

vtkProp_log::vtkProp_log()
  : GsTL_vtkProp(),
    grid_( 0 ), append_log_(0),
    mapper_(0),color_table_(0) {
}


vtkProp_log::~vtkProp_log() {


	renderer_->RemoveActor(actor_);
  append_log_->Delete();
  data_pass_through_->Delete();
  region_append_log_->Delete();
  mapper_->Delete();
  actor_->Delete();

  renderer_->RemoveActor(actor_);
  std::vector<log_pipeline>::iterator it_pipeline = log_pipelines_.begin();
  for( ; it_pipeline != log_pipelines_.begin(); ++it_pipeline ) {
	  renderer_->RemoveActor(it_pipeline->log_actor);
	  it_pipeline->vtk_data_log->Delete();
	  it_pipeline->log_mapper->Delete();
	  it_pipeline->log_actor->Delete();
  }

  color_table_->Delete();

}

vtkProp* vtkProp_log::prop() {
  return actor_;
}

void vtkProp_log::set_visibility(bool on){
  std::vector<log_pipeline>::iterator it_pipeline = log_pipelines_.begin();

  for( ; it_pipeline != log_pipelines_.begin(); ++it_pipeline ) {
	  it_pipeline->log_actor->SetVisibility(on);
  }
  actor_->SetVisibility(on);

  this->update_colorbar_visibility( );
}

bool vtkProp_log::is_visibile(){
	return actor_->GetVisibility() == 1;
}


void vtkProp_log::init(  Geostat_grid* grid, vtkRenderer* renderer ) {
	renderer_ = renderer;
    geostat_grid_ = grid;
    grid_ = dynamic_cast< Log_data_grid*>( grid );


  color_table_ = vtkLookupTableWithEnabling::New();

  append_log_ = vtkAppendPolyData::New();

  data_pass_through_ = vtkPassThrough::New();
  data_pass_through_->SetInputConnection(append_log_->GetOutputPort());

  mapper_ = vtkPolyDataMapper::New();
  actor_ = vtkActor::New();
  mapper_->SetInputConnection(append_log_->GetOutputPort());
  actor_->SetProperty(vtk_property_);
  actor_->SetMapper(mapper_);
  renderer_->AddActor(actor_);

  region_append_log_ = vtkAppendPolyData::New();

  //Create one vtkPolydata per dh
  int n_logs = grid_->number_of_logs();
  for( int i=0; i< n_logs; ++i ) {

	  log_pipeline pipeline;
	  log_pipeline region_pipeline;


	  pipeline.log_data = &(grid_->get_log_data(i));
	  pipeline.vtk_data_log = vtkPolyData::New();
	  region_pipeline.log_data = &(grid_->get_log_data(i));
	  region_pipeline.vtk_data_log = vtkPolyData::New();
    std::map<int,Log_data::Segment_geometry>::const_iterator it = pipeline.log_data->segment_begin();
	  std::map<int,Log_data::Segment_geometry>::const_iterator it_end = pipeline.log_data->segment_end();
	  pipeline.vtk_data_log->Allocate( pipeline.log_data->number_of_segments() );

	  vtkPoints* linePoints = vtkPoints::New();
	  linePoints->SetNumberOfPoints(pipeline.log_data->number_of_segments()*2);

	  int index = 0;
	  for( ; it != it_end; ++it) {
		  vtkSmartPointer<vtkLine>  line  = vtkSmartPointer<vtkLine>::New();
		  linePoints->InsertPoint(index, it->second.start.x(),it->second.start.y(),it->second.start.z());
		  line->GetPointIds()->SetId(0, index);
		  index++;

		  linePoints->InsertPoint(++index, it->second.end.x(),it->second.end.y(),it->second.end.z());
		  line->GetPointIds()->SetId(1, index++);
		  index++;

		  pipeline.vtk_data_log->InsertNextCell(line->GetCellType(), line->GetPointIds());
	  }
	  pipeline.vtk_data_log->SetPoints(linePoints);
	  linePoints->Delete();

	  pipeline.log_mapper = vtkPolyDataMapper::New();
    pipeline.log_mapper->SetInputData(pipeline.vtk_data_log);

	  pipeline.log_actor = vtkActor::New();
	  pipeline.log_actor->SetMapper(pipeline.log_mapper);

	//  renderer_->AddActor(pipeline.log_actor);

	  log_pipelines_.push_back(pipeline);
	  log_pipelines_region_.push_back(region_pipeline);

    append_log_->AddInputData(pipeline.vtk_data_log);

  }
  mapper_->Modified();


}

void vtkProp_log::set_property( const std::string& property_name, Colormap* cmap ){

	this->remove_all_thresholding();
	cmap_ = cmap;
	if(current_region_ && !property_name.empty()) {
		this->set_property_with_region(property_name,cmap);
		return;
	}

    if(property_name.empty()) {
    	  std::vector<log_pipeline>::iterator it_pipeline = log_pipelines_.begin();
    	  std::vector<log_pipeline>::iterator it_pipeline_region = log_pipelines_region_.begin();

    	  for( ; it_pipeline != log_pipelines_.end(); ++it_pipeline, ++it_pipeline_region ) {

    		  it_pipeline->vtk_data_log->GetCellData()->SetScalars(0);
    		  it_pipeline_region->vtk_data_log->GetCellData()->SetScalars(0);
    		  //it_pipeline->actor_->GetProperty()->SetRepresentationToWireframe();
    		  it_pipeline->vtk_data_log->Modified();
    		  if(current_region_) it_pipeline->vtk_data_log->Modified();

    	  }
    	  current_property_name_.clear();
    	  current_property_ = 0;
    	  return;
    }

    if( !geostat_grid_->property( property_name ) ) {
		appli_warning( "grid has no property called " << property_name );
		return;
    }


//    if( current_property_name_ == property_name ) return;
    current_property_ = geostat_grid_->property( property_name );
    current_property_->swap_to_memory();
    current_property_name_ = property_name;

    std::vector<log_pipeline>::iterator it_pipeline = log_pipelines_.begin();

    for( ; it_pipeline != log_pipelines_.end(); ++it_pipeline ) {

      std::map<int,Log_data::Segment_geometry>::const_iterator it = it_pipeline->log_data->segment_begin();
	    std::map<int,Log_data::Segment_geometry>::const_iterator it_end = it_pipeline->log_data->segment_end();
    //  Log_data::nodeid_to_log_coords::const_iterator it = it_pipeline->log_data->position_begin();
    //  Log_data::nodeid_to_log_coords::const_iterator it_end = it_pipeline->log_data->position_end();

      vtkSmartPointer<vtkFloatArray> values = vtkSmartPointer<vtkFloatArray>::New();
      values->SetNumberOfTuples(1);
      values->SetNumberOfValues(it_pipeline->log_data->number_of_segments());
      for(int i=0 ; it!= it_end; ++it, ++i) {
        int nodeid = it->first;
        float val = current_property_->get_value(nodeid);
          values->SetValue(i,current_property_->get_value(it->first));
      }
      it_pipeline->vtk_data_log->GetCellData()->SetScalars(values);
      it_pipeline->vtk_data_log->Modified();

    }

    cmap_->refresh();
    this->update_colorbar();
    mapper_->SetScalarRange(cmap->lower_bound(),cmap->upper_bound());
    mapper_->SetLookupTable(cmap_->color_table());
    mapper_->Modified();




}

void vtkProp_log::set_property_with_region( const std::string& property_name, Colormap* cmap ){

// The region pipeline would have been set up previously by set_region

	cmap_ = cmap;
    if( !geostat_grid_->property( property_name ) ) {
		appli_warning( "grid has no property called " << property_name );
		return;
    }

    current_property_ = geostat_grid_->property( property_name );
    current_property_->swap_to_memory();
    current_property_name_ = property_name;
    std::vector<log_pipeline>::iterator it_pipeline_region = log_pipelines_region_.begin();

    for( ; it_pipeline_region != log_pipelines_region_.end(); ++it_pipeline_region ) {
      std::map<int,Log_data::Segment_geometry>::const_iterator it = it_pipeline_region->log_data->segment_begin();
	    std::map<int,Log_data::Segment_geometry>::const_iterator it_end = it_pipeline_region->log_data->segment_end();

		  vtkSmartPointer<vtkFloatArray> values = vtkSmartPointer<vtkFloatArray>::New();
		  values->SetNumberOfTuples(1);
		  // Count number of segment
		  std::string log_name = it_pipeline_region->log_data->name();
		  int size_region_in_segment = grid_->number_of_segment_inside_region(log_name, current_region_ );
		  values->SetNumberOfValues(size_region_in_segment);

		  int i=0;
		  for(; it!= it_end; ++it) {
			  int nodeid = it->first;
			  if( !current_region_->is_inside_region(nodeid) ) continue;
			  float val = current_property_->get_value(nodeid);
			  values->SetValue(i++,current_property_->get_value(it->first));
		  }
		  it_pipeline_region->vtk_data_log->GetCellData()->SetScalars(values);
		  it_pipeline_region->vtk_data_log->Modified();
    }

    if(cmap_) {
    	cmap_->refresh();
    	mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
    	mapper_->SetLookupTable(cmap_->color_table());
    }

    region_append_log_->Modified();
    mapper_->Modified();

    this->update_colorbar();

}



void vtkProp_log::enable_full_view(){
	if(!renderer_->HasViewProp(actor_) ) {
		renderer_->AddActor(actor_);
	}
	actor_->VisibilityOn();

}



void vtkProp_log::disable_full_view(){
	actor_->VisibilityOff();
	renderer_->RemoveActor(actor_);
  }


void vtkProp_log::property_deleted( const std::string& prop_name ) {
  // If a property has been deleted, we have to:
  //   - remove its corresponding colormap entry if it exists,
  //   - if the deleted property was being displayed, paint the object
  //     in the default color
  CmapMap::iterator found = cmap_map_.find( prop_name );
  if( found == cmap_map_.end() ) return;

  delete found->second.second;
  cmap_map_.erase( found );

  if( current_property_name_ == prop_name ) {
    // the deleted property was being displayed
    current_property_name_ = "";
    current_property_ = 0;
    cmap_ = 0;
  }
}



void vtkProp_log::line_width( int size ) {

	  actor_->GetProperty()->SetLineWidth(size);
}


void vtkProp_log::refresh() {
  refresh_colormap();
  if(current_property_ == 0 ) return;


  std::vector<log_pipeline>::iterator it_pipeline = log_pipelines_.begin();

  for( ; it_pipeline != log_pipelines_.end(); ++it_pipeline ) {
    std::map<int,Log_data::Segment_geometry>::const_iterator it = it_pipeline->log_data->segment_begin();
	  std::map<int,Log_data::Segment_geometry>::const_iterator it_end = it_pipeline->log_data->segment_end();

	  vtkSmartPointer<vtkFloatArray> values = vtkSmartPointer<vtkFloatArray>::New();
	  values->SetNumberOfTuples(1);
	  values->SetNumberOfValues(it_pipeline->log_data->number_of_segments());

	  for(int i=0 ; it!= it_end; ++it, ++i) {
		  values->SetValue(i,current_property_->get_value(it->first));
	  }

	  it_pipeline->vtk_data_log->GetCellData()->SetScalars(values);

  }

  mapper_->Modified();
  this->update_colorbar();

}


void vtkProp_log::refresh_colormap() {

  if(cmap_ == 0) return;
  /*
  color_table_->SetNumberOfTableValues(cmap_->color_count());
  int nColors = color_table_->GetNumberOfTableValues();

  for( int i=0; i < nColors ; i++ ) {
	float r,g,b;
	cmap_->color(i, r,g,b);
	color_table_->SetTableValue(i,r,g,b);
  }
  color_table_->Build();

  mapper_->SetLookupTable(color_table_);
  mapper_->Update();
  */
  mapper_->SetScalarRange(cmap_->lower_bound(),cmap_->upper_bound());
  mapper_->SetLookupTable(cmap_->color_table());
  mapper_->Update();

  this->update_colorbar();

}


void vtkProp_log::set_region(const std::string& region_name ) {


	if(region_name == "" ) {
		current_region_ = 0;

    data_pass_through_->SetInputConnection(append_log_->GetOutputPort());

	}
	else {
		current_region_ = geostat_grid_->region(region_name);
		if( current_region_ == 0 ) return;

		std::vector<log_pipeline>::iterator it_pipeline_region = log_pipelines_region_.begin();
		for( ; it_pipeline_region != log_pipelines_region_.end();  ++it_pipeline_region  ) {
			region_append_log_->RemoveInputData(it_pipeline_region->vtk_data_log);
			it_pipeline_region->vtk_data_log->Delete();
			//it_pipeline_region->vtk_ma->Delete();

			it_pipeline_region->vtk_data_log = vtkPolyData::New();
			it_pipeline_region->vtk_data_log->Allocate( it_pipeline_region->log_data->number_of_segments() );
      std::map<int,Log_data::Segment_geometry>::const_iterator it = it_pipeline_region->log_data->segment_begin();
	    std::map<int,Log_data::Segment_geometry>::const_iterator it_end = it_pipeline_region->log_data->segment_end();

			vtkPoints* linePoints = vtkPoints::New();
			//	  linePoints->SetNumberOfPoints(Log_data.number_of_segments()*2);

			int index = 0;
			for( ; it != it_end; ++it) {
			  if( !current_region_->is_inside_region(it->first) ) continue;

			  vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
			  linePoints->InsertPoint(index, it->second.start.x(),it->second.start.y(),it->second.start.z());
			  line->GetPointIds()->SetId(0, index);
			  index++;

			  linePoints->InsertPoint(++index, it->second.end.x(),it->second.end.y(),it->second.end.z());
			  line->GetPointIds()->SetId(1, index++);
			  index++;

			  it_pipeline_region->vtk_data_log->InsertNextCell(line->GetCellType(), line->GetPointIds());
			}

			it_pipeline_region->vtk_data_log->SetPoints(linePoints);
			linePoints->Delete();

      region_append_log_->AddInputData(it_pipeline_region->vtk_data_log);

		}
    data_pass_through_->SetInputConnection(region_append_log_->GetOutputPort());

		this->set_property_with_region(current_property_name_,cmap_);

		region_append_log_->Modified();
		data_pass_through_->Modified();


	}
}



void vtkProp_log::set_colormap( Colormap* cmap ){
	cmap_ = cmap;

	mapper_->SetScalarRange(cmap_->lower_bound(), cmap_->upper_bound());
	mapper_->SetLookupTable(cmap_->color_table());
	mapper_->Modified();

	this->update_colorbar();



}

bool vtkProp_log::connect_threshold_to_data(vtkThreshold* thresholder){
  thresholder->SetInputConnection(data_pass_through_->GetOutputPort());
  return true;
}

bool vtkProp_log::enable_threshold_pipeline(){
	mapper_->SetInputConnection(threshold_poly_data_->GetOutputPort());
	mapper_->Modified();
  return true;
}

bool vtkProp_log::disable_threshold_pipeline(){


	mapper_->SetInputConnection(data_pass_through_->GetOutputPort());

	mapper_->Modified();
  return true;

}


void vtkProp_log::set_colortable_to_mapper(vtkMapper* mapper){
	double *scalar_range = mapper_->GetScalarRange();
	mapper->SetScalarRange(scalar_range[0],scalar_range[1]);
	mapper->SetLookupTable(mapper_->GetLookupTable());
}
