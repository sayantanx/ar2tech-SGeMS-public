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



#include <GsTLAppli/geostat/clustering_from_centroids.h>


#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>


#include <vtkKMeansStatistics.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkTable.h>
#include <vtkSmartPointer.h>
#include <vtkMultiBlockDataSet.h>

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>

Named_interface* Clustering_from_centroids::create_new_interface(std::string& ) {
  return new Clustering_from_centroids;
}


Clustering_from_centroids::Clustering_from_centroids(void)
  :region_(0), grid_(0)
{
}


Clustering_from_centroids::~Clustering_from_centroids(void)
{
}


bool Clustering_from_centroids::initialize( const Parameters_handler* parameters,
			  Error_messages_handler* errors ) {
  
  std::string grid_name = parameters->value( "grid_name.value" );
  errors->report( grid_name.empty(), 
		  "grid_name", "No grid selected" );

  std::string property_names_str = parameters->value( "property_names.value" );
  errors->report( property_names_str.empty(), 
		  "property_names", "No property name specified" );

  if(!errors->empty()) return false;

  // Get the simulation grid from the grid manager
  if( !grid_name.empty() ) {
    bool ok = geostat_utils::create( grid_, grid_name,
				 "grid_name", errors );
    if( !ok ) return false;
  }
  else 
    return false;


  std::vector<std::string> prop_names;
  String_Op::decompose_string( prop_names, property_names_str, ";");

  for(int i=0; i<prop_names.size(); ++i) {
    Grid_continuous_property* prop = grid_->property(prop_names[i]);
    if(prop == 0) {
       errors->report( "property_names", "No property called "+prop_names[i] );
       return false;
    }
    props_.push_back(prop);
  }

  std::string region_name = parameters->value( "grid_name.region" );

  if(!region_name.empty()) {
    region_ = grid_->region(region_name);
    if(region_ == 0) {
      errors->report( "grid_name", "The specified region ("+region_name+") does not exist" );
      return false;
    }
  }

  clusters_prop_name_ = parameters->value( "clusters_property_name.value" );
  errors->report(clusters_prop_name_.empty(),"clusters_property_name", "No output name specified");
  infile_name_ =  parameters->value( "centroid_file_name.value" );
  errors->report(infile_name_.empty(),"centroid_file_name", "No centroid file specified");
 

  QDomDocument doc("kmeansCentroids");
  QFile file(infile_name_.c_str());      
  if (!file.open(QIODevice::ReadOnly))  {
     errors->report( "centroid_file", "Cannot input the file :"+infile_name_ );
     return false; 
  }
     
  if (!doc.setContent(&file)) 
  {          
    file.close(); 
    errors->report( "centroid_file", "Error reading the xml content in the file :"+infile_name_ );
    return false;
  }
  file.close();
  
  QDomElement root = doc.firstChildElement("CentroidClusters");
  // Get the number of centroids
  QDomElement desc_el = root.firstChildElement("Description");
  int n_clusters = desc_el.attribute("NumberOfClusters","-1").toInt();
  int n_coords = desc_el.attribute("NumberOfCoordinates","-1").toInt();

  if(n_clusters <= 0 || n_coords  <= 0 ) {
    errors->report( "centroid_file", "Error reading the xml file; the number of centroids and/or coordinates are not set properly.");
    return false;
  }

  for(int k=0; k < n_clusters; ++k) {
    QDomElement centroid_el =  root.firstChildElement(QString("Centroid%1").arg(k));
    std::vector<float> coords;
   
    for(int c=0; c<n_coords; ++c) {
      float v = centroid_el.attribute(QString("Coord%1").arg(c)).toFloat();
      coords.push_back(v);
    }
    centroids_.push_back(coords);
  }

  if( !errors->empty() )
    return false;
 
  this->extract_parameters(parameters);
  return true;
}


int Clustering_from_centroids::execute( GsTL_project* proj ){

  Grid_categorical_property* kprop = grid_->add_categorical_property(clusters_prop_name_);
  while( kprop ==0 ) {
    clusters_prop_name_ = clusters_prop_name_+"_0";
    kprop = grid_->add_categorical_property(clusters_prop_name_);
  }
  kprop->set_parameters(parameters_);

  //Get the nodeid of the acceptable values
  std::vector<int> ids;
  for(int i=0; i<grid_->size(); ++i) {
    if(region_!=0 && !region_->is_inside_region(i)) continue;
    bool informed_ok = true;
    std::vector<float> data;
    data.reserve(props_.size());
    for(int p=0; p<props_.size(); ++p) {
      if( !props_[p]->is_informed(i) ) {
        informed_ok = false;
        break;
      }
      data.push_back(props_[p]->get_value(i));
    }
    if(!informed_ok) continue;

    //Compute the distance between the vector and each of the centroid
    std::vector<std::vector<float> >::const_iterator it =  centroids_.begin();
    std::vector<float> distance;
    int centroid_closest;
    double min_distance = 9e60;
    for(int centroid_id = 0 ;  it !=  centroids_.end(); ++it, ++centroid_id ) {
      double d = 0.0;
      for(int c = 0; c < it->size() ; ++c ) 
        d += std::pow((*it)[c] - data[c],2);
      if( d < min_distance ) {
        min_distance = d;
        centroid_closest = centroid_id;
      }
    }
    kprop->set_value(centroid_closest,i);

  }

  return 0;

}


