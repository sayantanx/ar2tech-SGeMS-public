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



#include <GsTLAppli/geostat/kmeans_clustering.h>


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

Named_interface* Kmeans_clustering::create_new_interface(std::string& ) {
  return new Kmeans_clustering;
}


Kmeans_clustering::Kmeans_clustering(void)
  :region_(0), grid_(0)
{
}


Kmeans_clustering::~Kmeans_clustering(void)
{
}



bool Kmeans_clustering::initialize( const Parameters_handler* parameters,
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
    GsTLGridProperty* prop = grid_->property(prop_names[i]);
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

  k_centroids_ = String_Op::to_number<int>(parameters->value( "k_centroids.value" ));

  kmeans_prop_name_ = parameters->value( "k_mean_property_name.value" );
 
  centroid_infile_ = parameters->value( "centroid_filename.value" );
  /*
  if(centroid_infile_.empty() ){
     errors->report( "centroid_filename", "A filename must be specified for the centroids" );
  }
  */
  if( !errors->empty() )
    return false;
 
  this->extract_parameters(parameters);
  return true;
}


int Kmeans_clustering::execute( GsTL_project* proj ){

  GsTLGridCategoricalProperty* kprop = grid_->add_categorical_property(kmeans_prop_name_);
  while( kprop ==0 ) {
    kmeans_prop_name_ = kmeans_prop_name_+"_0";
    kprop = grid_->add_categorical_property(kmeans_prop_name_);
  }
  kprop->set_parameters(parameters_);

  //Get the nodeid of the acceptable values
  std::vector<int> ids;
  for(int i=0; i<grid_->size(); ++i) {
    if(region_!=0 && !region_->is_inside_region(i)) continue;
    bool informed_ok = true;
    for(int p=0; p<props_.size(); ++p) {
      if( !props_[p]->is_informed(i) ) {
        informed_ok = false;
        break;
      }
    }
    if(!informed_ok) continue;

    ids.push_back(i);

  }

  //Create the vtkTable
  vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();

  for(int p=0; p<props_.size(); ++p) {
    const GsTLGridProperty* prop = props_[p];
    vtkSmartPointer<vtkFloatArray> data = vtkSmartPointer<vtkFloatArray>::New();
 
    data->SetName(prop->name().c_str());
    data->SetNumberOfValues(ids.size());
    for(int i=0; i<ids.size(); ++i) {
        data->SetValue(i, prop->get_value(ids[i])  );
    }
    table->AddColumn(data);

  }

  vtkSmartPointer<vtkKMeansStatistics> kmeans_stat = vtkSmartPointer<vtkKMeansStatistics>::New();

  //kmeans_stat->SetInput( vtkStatisticsAlgorithm::INPUT_DATA, table );
  kmeans_stat->SetInputData( vtkStatisticsAlgorithm::INPUT_DATA, table );
  for(int p=0; p<props_.size(); ++p) {
    kmeans_stat->SetColumnStatus( table->GetColumnName( p ) , 1 );
  }
  kmeans_stat->RequestSelectedColumns();
  kmeans_stat->SetAssessOption( true );
  kmeans_stat->SetDefaultNumberOfClusters( k_centroids_ );
  kmeans_stat->Update() ;

  //Output the clusters
  int cluster_col = kmeans_stat->GetOutput()->GetNumberOfColumns() - 1;
  for(unsigned int r = 0; r < kmeans_stat->GetOutput()->GetNumberOfRows(); r++)
  {
    vtkVariant v = kmeans_stat->GetOutput()->GetValue(r,cluster_col);
    kprop->set_value(v.ToInt(), ids[r]);
  }

  // Get the centroid
  vtkMultiBlockDataSet* outputMetaDS 
      = vtkMultiBlockDataSet::SafeDownCast( kmeans_stat->GetOutputDataObject( vtkStatisticsAlgorithm::OUTPUT_MODEL ) );
  vtkSmartPointer<vtkTable> outputMeta = vtkTable::SafeDownCast( outputMetaDS->GetBlock( 0 ) );
  //vtkSmartPointer<vtkTable> outputMeta = vtkTable::SafeDownCast( outputMetaDS->GetBlock( 1 ) );

  //Get the centroid
  std::vector<std::vector<float> > centroids;
  int k_centroids = outputMeta->GetColumnByName("K")->GetVariantValue(0).ToInt();
  for(int i=0; i<k_centroids; ++i) {
    centroids.push_back(std::vector<float>(props_.size(), 0.));
  }

  for(int p=0; p<props_.size(); ++p) {
    vtkDoubleArray* coord = vtkDoubleArray::SafeDownCast(outputMeta->GetColumnByName(props_[p]->name().c_str()));
    for(int k=0; k<k_centroids; ++k) {
      centroids[k][p] = coord->GetValue(k);
    }
  }

  if(!centroid_infile_.empty()) {
    QDomDocument doc("kmeansCentroids");
    QDomElement root = doc.createElement("CentroidClusters");

    QDomElement info = doc.createElement("Description");
    info.setAttribute("NumberOfClusters",k_centroids);
    info.setAttribute("Algorithm","kmeans");
    info.setAttribute("InputGrid",grid_->name().c_str());
    info.setAttribute("NumberOfCoordinates",(int)props_.size());
    for(int p=0; p<props_.size(); ++p) {
        info.setAttribute(QString("InputProperty%1").arg(p),props_[p]->name().c_str());
    }
    root.appendChild(info);

    for(int k=0; k<k_centroids; ++k) {
      QDomElement centroid = doc.createElement(QString("Centroid%1").arg(k));
   
      for(int p=0; p<props_.size(); ++p) {
        centroid.setAttribute(QString("Coord%1").arg(p),centroids[k][p]);
      }
       root.appendChild(centroid);
    }

    doc.appendChild(root);
    QFile infile(centroid_infile_.c_str());
    if( infile.open(QIODevice::WriteOnly | QIODevice::Text) )  {
      QTextStream out(&infile);
      out << doc.toString();
      infile.close();
    }
  }


  return 0;

}


