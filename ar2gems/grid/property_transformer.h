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


#ifndef __GSTLAPPLI_PROPERTY_TRANSFORMER_H__ 
#define __GSTLAPPLI_PROPERTY_TRANSFORMER_H__ 
 


#include <grid/common.h>
#include <utils/named_interface.h> 
#include <grid/grid_model/geostat_grid.h>
#include <grid/grid_model/grid_property.h> 
#include <grid/grid_model/grid_region.h> 
#include <Eigen/Core>
#include <QtXml>
#include <vector> 
#include <string>

 
/** Multi_property_transformer is the root of all algorithm that perform 
  multi-varaite transformed such as pca, maf and stepwise
 */ 

class GRID_DECL Multi_property_transformer : public Named_interface {
 
 public: 
 
  Multi_property_transformer():nvariate_(-1){}
  Multi_property_transformer(std::string name):nvariate_(-1){}
  virtual ~Multi_property_transformer() {} 

  virtual bool initialize(QDomElement elem)=0;
  virtual QDomElement serialize() const=0;

  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "Multi_property_transformer"; }

  virtual int number_of_variates() const {return nvariate_;}

  virtual std::string name() const { return name_; }
  virtual void name(std::string name){ name_ = name; }


 
  //------------------------------------ 
  // Properties management 
 
  /** Take the vector of property and return a vector of transformed property 
   */ 
  virtual std::vector<Grid_continuous_property*> forward_transform(Geostat_grid* grid,  std::vector<const Grid_continuous_property*>, const Grid_region* region = 0 ) = 0;
  
  /** Take a vector 
   */
  virtual std::vector<Grid_continuous_property*> back_transform(Geostat_grid* grid,  std::vector<const Grid_continuous_property*>, const Grid_region* region = 0 ) = 0;

protected:
  int nvariate_;

}; 
 

class GRID_DECL PCA_transformer : public Multi_property_transformer {
   
public:
  static Named_interface* create_new_interface( std::string& name);
 
 
//  PCA_transformer( std::vector<const Grid_continuous_property*> props, const Grid_region* region = 0, bool use_covariance=true);
  PCA_transformer() {
    variate_names_.clear();
  } 
  virtual ~PCA_transformer() {} 

  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "PCA_transformer"; }

  bool initialize( std::vector<const Grid_continuous_property*> props, 
                   const Grid_region* region = 0, bool use_covariance=true);
  virtual bool initialize(QDomElement elem);
  virtual QDomElement serialize() const; 

  //------------------------------------ 
  // Properties management 
 
  /** Take the vector of property and return a vector of transformed property 
   */ 
  virtual std::vector<Grid_continuous_property*> forward_transform(Geostat_grid* grid, 
              std::vector<const Grid_continuous_property*>, const Grid_region* region = 0 );
  
  /** Take a vector 
   */
  virtual std::vector<Grid_continuous_property*> back_transform(Geostat_grid* grid,     
              std::vector<const Grid_continuous_property*>, const Grid_region* region = 0 );

private:
  std::vector<std::string> variate_names_;

  Eigen::VectorXf means_;
  Eigen::VectorXf eigenvalues_;
  Eigen::MatrixXf eigenvectors_;

  

}; 
 
#endif 
 
