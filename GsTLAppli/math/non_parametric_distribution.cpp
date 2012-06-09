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



#include <GsTLAppli/math/non_parametric_distribution.h>


double Non_parametric_distribution::pdf(double z) const{

  return dist_->pdf(z);

  /*
  double delta = 0.0001;
  double low = z-delta;
  double high = z+delta;

  double delta2 = 2*delta;
  double cdf_low = this->cdf(low);
  double cdf_high = this->cdf(high);
  double d_cdf = cdf_high - cdf_low;
  double pdf = d_cdf/delta2;
  return pdf;
  */
  /*
  return (this->cdf(high) - this->cdf(low))/2/delta;
  */

}

bool Non_parametric_distribution::copy(const Continuous_distribution* dist){

  const Non_parametric_distribution* np_dist = dynamic_cast<const Non_parametric_distribution*>(dist);
  if(np_dist == 0) return false;

  delete dist_;

  dist_ = new  Non_param_cdf<>(np_dist->z_begin(),np_dist->z_end(), np_dist->p_begin(), 
							   np_dist->lower_tail_interpolator(),
							   np_dist->middle_interpolator(),
							   np_dist->upper_tail_interpolator());

  return true;
}


