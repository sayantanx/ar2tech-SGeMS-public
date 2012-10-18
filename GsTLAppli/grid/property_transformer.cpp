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


#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/grid/property_transformer.h>
#include <Eigen/Eigen>

Named_interface* PCA_transformer::create_new_interface( std::string& name){
  return new PCA_transformer;
}
/*
PCA_transformer( std::vector<const GsTLGridProperty*> props, const GsTLGridRegion* region, bool use_covariance)
{
  this->initialize(props,region,use_covariance);
}
*/


bool PCA_transformer::initialize( std::vector<const GsTLGridProperty*> props, 
                                const GsTLGridRegion* region, bool use_covariance)
{
  int prop_size = props[0]->size();
  int n_prop = props.size();
  int max_size;
  if(region == 0) max_size = prop_size;
  else max_size = region->active_size();
  Eigen::MatrixXf X(max_size,props.size());

  variate_names_.clear();
  for(int j=0;j<n_prop; ++j) {
    variate_names_.push_back(props[j]->name());
  }

  int index = 0;
  for(int i=0; i<prop_size; ++i) {
    if(region!=0 && !region->is_inside_region(i) ) continue;
    bool are_informed = true;
    Eigen::VectorXf v(n_prop);
    for(int j=0;j<n_prop; ++j) {
      if( !props[j]->is_informed(i) ) {
        are_informed = false;
        break;
      }
      v(j) = props[j]->get_value(i);
    }
    if(!are_informed) continue;
    X.row(index) = v;
    index++;
  }
  int max_index = index-1;
  X.conservativeResize(index,n_prop);
  means_ = X.colwise().mean();
//  X.array().colwise() -= means_.array();
  for(int c=0; c<X.cols(); ++c ) {
    X.col(c).array() -= means_(c);
  }

  // Compute the covariance matrix
  Eigen::MatrixXf cov_matrix = (1 / (float) max_index) * X.transpose() * X;
   
  // compute the eigenvalue on the Cov Matrix
  Eigen::EigenSolver<Eigen::MatrixXf> eig_solver(cov_matrix);
  eigenvalues_ = eig_solver.eigenvalues().real();
  
  eigenvectors_ = eig_solver.eigenvectors().real();	

  // sort and get the permutation indices
//  Eigen::PermutationIndices pi;
//  for (int i = 0 ; i < m; i++)
//	  pi.push_back(std::make_pair(eigenvalues(i), i));
  return true;
}



 
  /** Take the vector of property and return a vector of transformed property 
   */ 
std::vector<GsTLGridProperty*> 
  PCA_transformer::forward_transform(Geostat_grid* grid, 
   std::vector<const GsTLGridProperty*> props, const GsTLGridRegion* region)
{
  // Create new property
  std::vector<GsTLGridProperty*> pc_props;
  for(int i=0;i<props.size(); ++ i) {
    std::ostringstream str_stream;
    str_stream<<this->name()<<" PC "<<i+1;

    GsTLGridProperty* prop = grid->add_property(str_stream.str());
    while(prop==0) {
      str_stream<<"_0";
      prop = grid->add_property(str_stream.str());
    }
    pc_props.push_back(prop);

  }
  int prop_size = props[0]->size();
  for(int i=0; i<prop_size; ++i) {
    if(region!=0 && !region->is_inside_region(i) ) continue;
    bool are_informed = true;
    Eigen::VectorXf v(props.size());
    for(int j=0;j<props.size(); ++j) {
      if( !props[j]->is_informed(i) ) {
        are_informed = false;
        break;
      }
      v(j) = props[j]->get_value(i);
    }
    if(!are_informed) continue;
    v -= means_;
    Eigen::VectorXf pc = v.transpose()*eigenvectors_;
    for(int j=0;j<props.size(); ++j) {
      pc_props[j]->set_value(pc(j),i);
    }

  }

  return pc_props;

}
  
  /** Take a vector 
   */
std::vector<GsTLGridProperty*> 
  PCA_transformer::back_transform(Geostat_grid* grid, 
   std::vector<const GsTLGridProperty*> pc_props, const GsTLGridRegion* region )
{

  // Create new property
  std::vector<GsTLGridProperty*> props;
  for(int i=0;i<pc_props.size(); ++ i) {
   
    std::ostringstream str_stream;
    str_stream<<this->name()<<" "<<variate_names_[i];

    //Check if the property name finish with "__real"
    QString name = QString::fromStdString(pc_props[i]->name());
    if( name.contains("__real" ) ) {
      int index = name.indexOf("__real" );
      str_stream<<name.right( name.size() - index ).toStdString();
    }

    GsTLGridProperty* prop = grid->add_property(str_stream.str());
    while(prop==0) {
      str_stream<<"_0";
      prop = grid->add_property(str_stream.str());
    }
    props.push_back(prop);
  }

  //Back transform
  int prop_size = pc_props[0]->size();
  for(int i=0; i<prop_size; ++i) {
    if(region!=0 && !region->is_inside_region(i) ) continue;
    bool are_informed = true;
    Eigen::VectorXf pc(props.size());
    for(int j=0;j<pc_props.size(); ++j) {
      if( !pc_props[j]->is_informed(i) ) {
        are_informed = false;
        break;
      }
      pc(j) = pc_props[j]->get_value(i);
    }
    if(!are_informed) continue;
    
    Eigen::VectorXf v = pc.transpose()*eigenvectors_.transpose();
    v += means_;
    for(int j=0;j<props.size(); ++j) {
      props[j]->set_value(v(j),i);
    }

  }

  return props;

}


bool PCA_transformer::initialize(QDomElement elem){

  return true;
}

QDomElement PCA_transformer::serialize() const{
  QDomDocument doc;
  QDomElement elem = doc.createElement("Transformer");
  elem.setAttribute("type",this->classname().c_str());
  elem.setAttribute("name",this->name().c_str());

  QStringList qvariate_names;
  for(int i=0; i< variate_names_.size(); ++i) {
    qvariate_names.append(variate_names_[i].c_str());
  }
  elem.setAttribute("variableNames",qvariate_names.join(";"));

  QStringList qstr_mean;
  for(int i=0; i< means_.rows(); ++i) {
    qstr_mean.append(  QString("%1").arg(means_(i)) );
  }
  elem.setAttribute("means",qstr_mean.join(";"));

  QStringList qstr_eigvalues;
  for(int i=0; i< eigenvalues_.rows(); ++i) {
    qstr_eigvalues.append(  QString("%1").arg(eigenvalues_(i)) );
  }
  elem.setAttribute("EigenValues",qstr_eigvalues.join(";"));

  QStringList qstr_eigvectors;
  for(int i=0; i< eigenvectors_.rows(); ++i) {
    for(int j=0; j< eigenvectors_.cols(); ++j) {
      qstr_eigvectors.append(  QString("%1").arg(eigenvectors_(i,j)) );
    }
  }
  elem.setAttribute("EigenVectors",qstr_eigvectors.join(";"));

  return elem;

}