/* -----------------------------------------------------------------------------
** Copyright� 2012 Advanced Resources and Risk Technology, LLC
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



#ifndef __GSTLAPPLI_MATH_NONPARAM_DISTRIBUTION_H__
#define __GSTLAPPLI_MATH_NONPARAM_DISTRIBUTION_H__


#include <math/continuous_distribution.h>
#include <math/common.h>


#include <boost/math/distributions.hpp>

#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/cdf/interpolators.h>
#include <GsTL/univariate_stats/build_cdf.h>

#include <vector>
#include <algorithm>

#include <cstdlib>


class MATH_DECL  Non_parametric_distribution : public Continuous_distribution {
public:

  typedef std::vector<float>::iterator        z_iterator;
  typedef std::vector<double>::iterator            p_iterator;
  typedef std::vector<float>::const_iterator  const_z_iterator;
  typedef std::vector<double>::const_iterator      const_p_iterator;

public :
    static Named_interface* create_new_interface( std::string& params){
    return new Non_parametric_distribution;
  }

  //Non_parametric_distribution():dist_(0){}
  Non_parametric_distribution(){
    dist_ = new Non_param_cdf<>();
  }
  virtual bool copy(const Continuous_distribution* dist);

  virtual Continuous_distribution* clone(){
    Non_parametric_distribution* dist = new Non_parametric_distribution;
    dist->copy(this);
    return dist; 
  }

  template<class Z_iterator>
  void initialize(Z_iterator z_begin, Z_iterator z_end, 
    const Tail_interpolator low_interp = Tail_interpolator( new No_TI() ),
		const Linear_interpol mid_interp  = Linear_interpol(),
		const Tail_interpolator up_interp  = Tail_interpolator( new No_TI() )
		)  {
      delete dist_;
      dist_ = new  Non_param_cdf<>(z_begin,z_end,low_interp, mid_interp, up_interp);

  }

  template<class Z_iterator, class P_iterator>
  void initialize(Z_iterator z_begin, Z_iterator z_end,
		P_iterator p_begin,
    const Tail_interpolator low_interp = Tail_interpolator( new No_TI() ),
		const Linear_interpol mid_interp  = Linear_interpol(),
		const Tail_interpolator up_interp  = Tail_interpolator( new No_TI() )
		) 
  {
      delete dist_;
      dist_ = new  Non_param_cdf<>(z_begin, z_end,  p_begin, low_interp, mid_interp, up_interp);
  }

  void upper_tail_interpolator( Tail_interpolator interpolator ) {
    dist_->upper_tail_interpolator(interpolator);
  } 
  void lower_tail_interpolator( Tail_interpolator interpolator ) {
    dist_->lower_tail_interpolator(interpolator);
  } 
  void middle_interpolator( Linear_interpol interpolator ) {
    dist_->middle_interpolator(interpolator);
  } 

  Tail_interpolator upper_tail_interpolator(  ) const {
    return dist_->upper_tail_interpolator();
  } 
  Tail_interpolator lower_tail_interpolator(  ) const {
    return dist_->lower_tail_interpolator();
  } 
  Linear_interpol middle_interpolator(  ) const {
    return dist_->middle_interpolator();
  } 

  template<class ForwardIterator>
  inline void z_set(ForwardIterator z_begin, ForwardIterator z_end){
    dist_->z_set(z_begin, z_end);
  }
 
  template<class ForwardIterator>
  inline void p_set(ForwardIterator p_begin, ForwardIterator p_end){
    dist_->p_set(p_begin, p_end);
  }

  bool is_valid_distribution() const {
    return is_valid_cdf(this->p_begin(), this->p_end(),
  		       GsTL::continuous_variable_tag());
  }
  virtual int size() const { return dist_->size(); }
  virtual void resize(int new_size) { 
    return dist_->resize(new_size); 
  }

  inline z_iterator z_begin() {return dist_->z_begin();}
  inline z_iterator z_end() {return dist_->z_end();}
  
  inline p_iterator p_begin() {return dist_->p_begin();}
  inline p_iterator p_end() {return dist_->p_end();}
  
  inline const_z_iterator z_begin() const {return dist_->z_begin();}
  inline const_z_iterator z_end() const {return dist_->z_end();}
  
  inline const_p_iterator p_begin() const {return dist_->p_begin();}
  inline const_p_iterator p_end() const {return dist_->p_end();}

  virtual double cdf(float z) const {return dist_->prob(z);}
  virtual float kurtosis() const {return -1;}
  virtual float mean() const {return dist_->mean();}
  virtual float median() const {return dist_->inverse(0.5);}
  virtual float mode()const {return -1;}
  virtual double pdf(double z) const;
  virtual const std::pair<float,float> range() const {return std::make_pair(dist_->inverse(0.000000000001),dist_->inverse(0.999999999999));}
  virtual float quantile(double p) const {return dist_->inverse(p);}
  virtual float skewness() const {return dist_->skewness();}
  virtual float variance() const {return dist_->variance();}

  virtual std::string type() const {return "Non-parametric";}
  virtual bool is_parametric() const {return false;}



private :
  Non_param_cdf<>* dist_;

};


#endif
