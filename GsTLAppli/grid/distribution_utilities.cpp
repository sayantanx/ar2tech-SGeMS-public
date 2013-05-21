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



#include <GsTLAppli/grid/distribution_utilities.h>
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>

Non_parametric_distribution* 
  build_non_param_distribution( std::string name, Grid_continuous_property* prop, 
    const Tail_interpolator low_interp,
		const Linear_interpol mid_interp,
		const Tail_interpolator up_interp	)  
{
  Non_parametric_distribution* dist;
  if( name.empty() ) {
    dist = new Non_parametric_distribution();
  }
  else {
    Named_interface* ni = Root::instance()->new_interface("Non-parametric",continuous_distributions_manager+"/"+name).raw_ptr();
    dist = dynamic_cast<Non_parametric_distribution*>(ni);
    if(dist == 0) return 0;
  }

  std::vector<float> values(prop->begin(), prop->end());
  std::sort(values.begin(), values.end());

  int n = values.size();
  std::vector<float> cdf;
  cdf.reserve(n);
  n += 1;
  for(int i=1; i<=values.size(); ++i) {
    cdf.push_back( float(i)/n );
  }

  dist->initialize(values.begin(),values.end(),cdf.begin(), low_interp, mid_interp, up_interp);

  return dist;

};


Non_parametric_distribution* 
  build_non_param_distribution( std::string name,
  const Grid_continuous_property* prop, const Grid_region* region, 
  const Tail_interpolator low_interp,
	const Linear_interpol mid_interp,
	const Tail_interpolator up_interp	)  
{

  Non_parametric_distribution* dist;
  if( name.empty() ) {
    dist = new Non_parametric_distribution();
  }
  else {
    Named_interface* ni = Root::instance()->new_interface("Non-parametric",continuous_distributions_manager+"/"+name).raw_ptr();
    dist = dynamic_cast<Non_parametric_distribution*>(ni);
    if(dist == 0) return 0;
  }

  std::vector<float> values;
 
  values.reserve(region->active_size());
  double p = 0.0; 
  for(int i=0; i<prop->size(); ++i) {
    if( prop->is_informed(i) &&  region->is_inside_region(i)) {
      values.push_back( prop->get_value(i) );
    }
  }
  std::sort(values.begin(), values.end());
  int n = values.size();
  std::vector<float> cdf;
  cdf.reserve(n);
  n += 1;
  for(int i=1; i<=values.size(); ++i) {
    cdf.push_back( float(i)/n );
  }

  dist->initialize(values.begin(),values.end(),cdf.begin(), low_interp, mid_interp, up_interp);

  return dist;
};


Non_parametric_distribution* 
  build_non_param_distribution( std::string name,const Grid_continuous_property* prop, const GsTLGridWeightProperty* weight, 
  const Tail_interpolator low_interp,
	const Linear_interpol mid_interp,
	const Tail_interpolator up_interp	)  
{

  Non_parametric_distribution* dist;
  if( name.empty() ) {
    dist = new Non_parametric_distribution();
  }
  else {
    Named_interface* ni = Root::instance()->new_interface("Non-parametric",continuous_distributions_manager+"/"+name).raw_ptr();
    dist = dynamic_cast<Non_parametric_distribution*>(ni);
    if(dist == 0) return 0;
  }

  typedef std::pair<float, float> z_p_pair_type;
  std::vector<z_p_pair_type> values;

  for(int i=0; i<prop->size(); ++i) {
    if( !prop->is_informed(i) || !weight->is_informed(i) ) continue;

    float w = weight->get_value(i);
    if(w == 0) continue;
    values.push_back(  std::make_pair(prop->get_value(i), w) );
  }

  std::sort(values.begin(), values.end()); // Sorting on pairs works on the first element

  std::vector<float> z;
  std::vector<float> p;
  z.reserve(values.size());
  p.reserve(values.size());
  std::vector<z_p_pair_type>::const_iterator it = values.begin();
  float sum_w = 0.0;
  for( ; it != values.end() ; ++it) {
    z.push_back(it->first);
    sum_w +=it->second;
    p.push_back(sum_w);
  }
  if(sum_w > 1.0)  //may happen due to rounding errors
    p.back() = 1.0;
    
  dist->initialize(z.begin(),z.end(),p.begin(), low_interp, mid_interp, up_interp);

  bool ok = dist->is_valid_distribution();

  

  return dist;

};
