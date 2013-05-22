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



#ifndef __GSTLAPPLI_MATH_STATS_DISTRIBUTION_H__
#define __GSTLAPPLI_MATH_STATS_DISTRIBUTION_H__

#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/math/common.h>
#include <boost/math/distributions.hpp>
/*
class MATH_DECL Statistical_distribution : public Named_interface
{
public:
  Statistical_distribution(void);
  virtual ~Statistical_distribution(void);
};
*/

class MATH_DECL Continuous_distribution : public Named_interface
{
public:
  
  Continuous_distribution(void){item_type_ = "ContinuousDistribution";}
  virtual ~Continuous_distribution(void){}

  virtual bool copy(const Continuous_distribution* dist) = 0;
  virtual Continuous_distribution* clone() = 0;

  virtual double cdf(float z) const=0;
  virtual float kurtosis() const=0;
  virtual float mean() const=0;
  virtual float median() const=0;
  virtual float mode()const =0 ;
  virtual double pdf(double z) const =0;
  virtual const std::pair<float,float> range() const =0;
  virtual float quantile(double p) const =0;
  virtual float skewness() const =0;
  virtual float variance() const =0;

// For backward compatibility with GsTL
  virtual float inverse(double p) const {return this->quantile(p);}
  virtual double prob(float z) const {return this->cdf(z);}
  virtual std::string type() const = 0;

  virtual bool is_parametric() const = 0;

};


class MATH_DECL Parametric_continuous_distribution : public Continuous_distribution
{
public:
  Parametric_continuous_distribution():Continuous_distribution(){}

  virtual void initialize(const std::vector<float>& params)=0;
  virtual std::vector<std::string> parameter_names() const=0;
  virtual std::vector<float> parameter_values() const=0;

  virtual bool is_parametric() const {return true;}


};



class MATH_DECL Beta_distribution : public Parametric_continuous_distribution
{
public:

  static Named_interface* create_new_interface( std::string& params){
    return new Beta_distribution;
  }

  Beta_distribution():Parametric_continuous_distribution(),dist_(0){}
  Beta_distribution(double alpha, double beta){
    dist_ = new boost::math::beta_distribution<>(alpha,beta);
  }

  virtual ~Beta_distribution(void){delete dist_;}

  Beta_distribution(const Beta_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Beta_distribution& operator = (const Beta_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Beta_distribution* dist = dynamic_cast<const Beta_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::beta_distribution<>(dist->boost_distribution()->alpha(),
                                                   dist->boost_distribution()->beta());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Beta_distribution* dist = new Beta_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double alpha, double beta){
    delete dist_;
    dist_ = new boost::math::beta_distribution<>(alpha,beta);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }

  virtual std::string type() const {return "Beta";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("alpha");
    pnames.push_back("beta");
    return pnames;
  }
 
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->alpha() );
    vals.push_back(dist_->beta() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::beta_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::beta_distribution<>* dist_;
};
class MATH_DECL Exponential_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Exponential_distribution;
  }

  Exponential_distribution():dist_(0){}
  Exponential_distribution(double lambda ){
    dist_ = new boost::math::exponential_distribution<>(lambda);
  }
  virtual ~Exponential_distribution(void){delete dist_;}

  Exponential_distribution(const Exponential_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Exponential_distribution& operator = (const Exponential_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Exponential_distribution* dist = dynamic_cast<const Exponential_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::exponential_distribution<>(dist->boost_distribution()->lambda());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Exponential_distribution* dist = new Exponential_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double lambda = 1.0){
    delete dist_;
    dist_ = new boost::math::exponential_distribution<>(lambda);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=1) return;
    this->initialize(params[0]);
  }

  virtual std::string type() const {return "Exponential";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("lambda");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->lambda() );
    return vals;
  }


  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::exponential_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::exponential_distribution<>* dist_;
};
class MATH_DECL Extreme_value_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Extreme_value_distribution;
  }

  Extreme_value_distribution():dist_(0){}
  Extreme_value_distribution(double location, double scale){
    dist_ = new boost::math::extreme_value_distribution<>(location,scale);
  }
  virtual ~Extreme_value_distribution(void){delete dist_;}

  Extreme_value_distribution(const Extreme_value_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Extreme_value_distribution& operator = (const Extreme_value_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Extreme_value_distribution* dist = dynamic_cast<const Extreme_value_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::extreme_value_distribution<>(dist->boost_distribution()->location(), 
                                                            dist->boost_distribution()->scale());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Extreme_value_distribution* dist = new Extreme_value_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double location, double scale){
    delete dist_;
    dist_ = new boost::math::extreme_value_distribution<>(location,scale);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }

  virtual std::string type() const {return "Extreme value";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("location");
    pnames.push_back("scale");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->location() );
    vals.push_back(dist_->scale() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::extreme_value_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::extreme_value_distribution<>* dist_;
};

class MATH_DECL Gamma_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Gamma_distribution;
  }

  Gamma_distribution():dist_(0){}
  Gamma_distribution(double shape, double scale = 1.0){
    dist_ = new boost::math::gamma_distribution<>(shape,scale);
  }
  virtual ~Gamma_distribution(void){delete dist_;}

  Gamma_distribution(const Gamma_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Gamma_distribution& operator = (const Gamma_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Gamma_distribution* dist = dynamic_cast<const Gamma_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::gamma_distribution<>(dist->boost_distribution()->shape(), 
                                                    dist->boost_distribution()->scale());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Gamma_distribution* dist = new Gamma_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double shape, double scale = 1.0){
    delete dist_;
    dist_ = new boost::math::gamma_distribution<>(shape,scale);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }

  virtual std::string type() const {return "Gamma";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("shape");
    pnames.push_back("scale");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->shape() );
    vals.push_back(dist_->scale() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::gamma_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::gamma_distribution<>* dist_;
};

/*
class MATH_DECL Inverse_gamma_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Inverse_gamma_distribution;
  }

  Inverse_gamma_distribution():dist_(0){}
  Inverse_gamma_distribution(double shape, double scale = 1.0){
    dist_ = new boost::math::inverse_gamma_distribution<>(shape,scale);
  }

  virtual ~Inverse_gamma_distribution(void);

  void initialize(double shape, double scale = 1.0){
    delete dist_;
    dist_ = new boost::math::inverse_gamma_distribution<>(shape,scale);
  }
  virtual std::string type() const {return "Inverse gamma";}

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

private :
  boost::math::inverse_gamma_distribution<>* dist_;
};

class MATH_DECL Inverse_Gaussian_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Inverse_Gaussian_distribution;
  }
  
  Inverse_Gaussian_distribution():dist_(0){}
  Inverse_Gaussian_distribution(double mean, double scale = 1.0){
    dist_ = new boost::math::inverse_gaussian_distribution<>(mean,scale);
  }
  virtual ~Inverse_Gaussian_distribution(void);

  void initialize(double mean, double scale = 1.0){
    delete dist_;
    dist_ = new boost::math::inverse_gaussian_distribution<>(mean,scale);
  }
  virtual std::string type() const {return "Inverse Gaussian";}

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

private :
  boost::math::inverse_gaussian_distribution<>* dist_;
};
*/

class MATH_DECL Logistic_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Logistic_distribution;
  }
  
  Logistic_distribution():dist_(0){}
  Logistic_distribution(double location , double scale){
    dist_ = new boost::math::logistic_distribution<>(location,scale);
  }
  virtual ~Logistic_distribution(void){delete dist_;}

  Logistic_distribution(const Logistic_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Logistic_distribution& operator = (const Logistic_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Logistic_distribution* dist = dynamic_cast<const Logistic_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::logistic_distribution<>(dist->boost_distribution()->location(), 
                                                       dist->boost_distribution()->scale());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Logistic_distribution* dist = new Logistic_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double location =0 , double scale=1.0){
    delete dist_;
    dist_ = new boost::math::logistic_distribution<>(location,scale);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }

  virtual std::string type() const {return "Logistic";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("location");
    pnames.push_back("scale");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->location() );
    vals.push_back(dist_->scale() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::logistic_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::logistic_distribution<>* dist_;
};

class MATH_DECL LogNormal_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new LogNormal_distribution;
  }
  
  LogNormal_distribution():dist_(0){}
  LogNormal_distribution(double location , double scale){

	  float gaussian_var = std::log( scale*scale/location/location + 1 );
	  float gaussian_mean = std::log(location) -  gaussian_var/2;

    dist_ = new boost::math::lognormal_distribution<>(gaussian_mean,std::sqrt(gaussian_var));
  }
  virtual ~LogNormal_distribution(void){delete dist_;}

  LogNormal_distribution(const LogNormal_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual LogNormal_distribution& operator = (const LogNormal_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const LogNormal_distribution* dist = dynamic_cast<const LogNormal_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::lognormal_distribution<>(dist->boost_distribution()->location(), 
                                                        dist->boost_distribution()->scale());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    LogNormal_distribution* dist = new LogNormal_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double location =0 , double scale=1.0){
    delete dist_;
    dist_ = new boost::math::lognormal_distribution<>(location,scale);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }

  virtual std::string type() const {return "LogNormal";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("location");
    pnames.push_back("scale");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->location() );
    vals.push_back(dist_->scale() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::lognormal_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::lognormal_distribution<>* dist_;

//  float mean_;
//  float variance_;
	

};
/*
class MATH_DECL Noncentral_beta_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Noncentral_beta_distribution;
  }
  
  Noncentral_beta_distribution():dist_(0){}
  Noncentral_beta_distribution(double alpha, double beta, double lambda){
    dist_ = new boost::math::non_central_beta_distribution<>(alpha,beta, lambda);
  }
  virtual ~Noncentral_beta_distribution(void);

  void initialize(double alpha, double beta, double lambda){
    delete dist_;
    dist_ = new boost::math::non_central_beta_distribution<>(alpha,beta, lambda);
  }
  virtual std::string type() const {return "Noncentral beta";}

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

private :
  boost::math::non_central_beta_distribution<>* dist_;
};
*/
class MATH_DECL Gaussian_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Gaussian_distribution;
  }
  
  Gaussian_distribution():Parametric_continuous_distribution(),dist_(0){}
  Gaussian_distribution(double mean , double std){
    dist_ = new boost::math::normal_distribution<>(mean,std);
  }
  virtual ~Gaussian_distribution(void){delete dist_;}

  Gaussian_distribution(const Gaussian_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Gaussian_distribution& operator = (const Gaussian_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Gaussian_distribution* dist = dynamic_cast<const Gaussian_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::normal_distribution<>(dist->boost_distribution()->mean(), 
                                                     dist->boost_distribution()->standard_deviation());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Gaussian_distribution* dist = new Gaussian_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double mean =0 , double std=1.0){
    delete dist_;
    dist_ = new boost::math::normal_distribution<>(mean,std);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }
  virtual std::string type() const {return "Gaussian";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("mean");
    pnames.push_back("std");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->mean() );
    vals.push_back(dist_->scale() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::normal_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::normal_distribution<>* dist_;
};

class MATH_DECL Pareto_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Pareto_distribution;
  }
  
  Pareto_distribution():dist_(0){}
  Pareto_distribution(double scale, double shape){
    dist_ = new boost::math::pareto_distribution<>(scale,shape);
  }
  virtual ~Pareto_distribution(void){delete dist_;}

  Pareto_distribution(const Pareto_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Pareto_distribution& operator = (const Pareto_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Pareto_distribution* dist = dynamic_cast<const Pareto_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::pareto_distribution<>(dist->boost_distribution()->scale(), 
                                                     dist->boost_distribution()->shape());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Pareto_distribution* dist = new Pareto_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double scale, double shape){
    delete dist_;
    dist_ = new boost::math::pareto_distribution<>(scale,shape);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }
  virtual std::string type() const {return "Pareto";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("location");
    pnames.push_back("shape");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->scale() );
    vals.push_back(dist_->shape() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::pareto_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::pareto_distribution<>* dist_;
};
class MATH_DECL Rayleigh_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Rayleigh_distribution;
  }
  
  Rayleigh_distribution():dist_(0){}
  Rayleigh_distribution(double sigma){
    dist_ = new boost::math::rayleigh_distribution<>(sigma);
  }
  virtual ~Rayleigh_distribution(void){delete dist_;}

  Rayleigh_distribution(const Rayleigh_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Rayleigh_distribution& operator = (const Rayleigh_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Rayleigh_distribution* dist = dynamic_cast<const Rayleigh_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::rayleigh_distribution<>(dist->boost_distribution()->sigma());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Rayleigh_distribution* dist = new Rayleigh_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double sigma=1){
    delete dist_;
    dist_ = new boost::math::rayleigh_distribution<>(sigma);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=1) return;
    this->initialize(params[0]);
  }
  virtual std::string type() const {return "Rayleigh";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("sigma");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->sigma() );
    return vals;
  }


  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::rayleigh_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::rayleigh_distribution<>* dist_;
};

class MATH_DECL Triangular_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Triangular_distribution;
  }
  
  Triangular_distribution():dist_(0){}
  Triangular_distribution(double lower , double mode, double upper ){
    dist_ = new boost::math::triangular_distribution<>(lower, mode, upper);
  }
  virtual ~Triangular_distribution(void){delete dist_;}

  Triangular_distribution(const Triangular_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Triangular_distribution& operator = (const Triangular_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Triangular_distribution* dist = dynamic_cast<const Triangular_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::triangular_distribution<>(dist->boost_distribution()->lower(), 
                                                         dist->boost_distribution()->mode(),
                                                         dist->boost_distribution()->upper());
      return true;
    }
    else
      return false;
  }


  virtual Continuous_distribution* clone(){
    Triangular_distribution* dist = new Triangular_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double lower = -1, double mode = 0, double upper = 1){
    delete dist_;
    dist_ = new boost::math::triangular_distribution<>(lower, mode, upper);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=3) return;
    this->initialize(params[0],params[1],params[2]);
  }
  virtual std::string type() const {return "Triangular";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("lower");
    pnames.push_back("mode");
    pnames.push_back("upper");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->lower() );
    vals.push_back(dist_->mode() );
    vals.push_back(dist_->upper() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::triangular_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::triangular_distribution<>* dist_;
};

class MATH_DECL Uniform_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Uniform_distribution;
  }
  
  Uniform_distribution():dist_(0){}
  Uniform_distribution(double lower, double upper ){
    dist_ = new boost::math::uniform_distribution<>(lower,upper);
  }
  virtual ~Uniform_distribution(void){delete dist_;}

  Uniform_distribution(const Uniform_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Uniform_distribution& operator = (const Uniform_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Uniform_distribution* dist = dynamic_cast<const Uniform_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::uniform_distribution<>(dist->boost_distribution()->lower(), 
                                                      dist->boost_distribution()->upper());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Uniform_distribution* dist = new Uniform_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double lower=0, double upper =1){
    delete dist_;
    dist_ = new boost::math::uniform_distribution<>(lower,upper);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }
  virtual std::string type() const {return "Uniform";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("lower");
    pnames.push_back("upper");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->lower() );
    vals.push_back(dist_->upper() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::uniform_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::uniform_distribution<>* dist_;
};
class MATH_DECL Weibull_distribution : public Parametric_continuous_distribution
{
public:
  static Named_interface* create_new_interface( std::string& params){
    return new Weibull_distribution;
  }
  
  Weibull_distribution():dist_(0){}
  Weibull_distribution(double shape, double scale){
    dist_ = new boost::math::weibull_distribution<>(shape,scale);
  }
  virtual ~Weibull_distribution(void){delete dist_;}

  Weibull_distribution(const Weibull_distribution &rhs){
    *dist_ = *rhs.dist_;
  }

  virtual Weibull_distribution& operator = (const Weibull_distribution &rhs){
    *dist_ = *rhs.dist_;
    return *this;
  }

  virtual bool copy(const Continuous_distribution* cdist){
    const Weibull_distribution* dist = dynamic_cast<const Weibull_distribution*>(cdist);
    if(dist) {
      dist_ = new boost::math::weibull_distribution<>(dist->boost_distribution()->shape(), 
                                                      dist->boost_distribution()->scale());
      return true;
    }
    else
      return false;
  }

  virtual Continuous_distribution* clone(){
    Weibull_distribution* dist = new Weibull_distribution;
    dist->copy(this);
    return dist; 
  }

  void initialize(double shape, double scale=1){
    delete dist_;
    dist_ = new boost::math::weibull_distribution<>(shape,scale);
  }
  virtual void initialize(const std::vector<float>& params){
    if(params.size()!=2) return;
    this->initialize(params[0],params[1]);
  }
  virtual std::string type() const {return "Weibull";}
  virtual std::vector<std::string> parameter_names() const{
    std::vector<std::string> pnames;
    pnames.push_back("shape");
    pnames.push_back("scale");
    return pnames;
  }
  virtual std::vector<float> parameter_values() const{
    std::vector<float> vals;
    if(dist_ == 0) return vals;
    vals.push_back(dist_->shape() );
    vals.push_back(dist_->scale() );
    return vals;
  }

  virtual double cdf(float z) const {return boost::math::cdf(*dist_,z);}
  virtual float kurtosis() const {return boost::math::kurtosis(*dist_);}
  virtual float mean() const {return boost::math::mean(*dist_);}
  virtual float median() const {return boost::math::median(*dist_);}
  virtual float mode()const {return boost::math::mode(*dist_);}
  virtual double pdf(double z) const {return boost::math::pdf(*dist_,z);}
  virtual const std::pair<float,float> range() const {return boost::math::range(*dist_);}
  virtual float quantile(double p) const {return boost::math::quantile(*dist_,p);}
  virtual float skewness() const {return boost::math::skewness(*dist_);}
  virtual float variance() const {return boost::math::variance(*dist_);}

  const boost::math::weibull_distribution<>* boost_distribution() const {return dist_;}

private :
  boost::math::weibull_distribution<>* dist_;
};


/*
class MATH_DECL Discrete_distribution : public Named_interface
{
public:
  Discrete_distribution(void){}
  virtual ~Discrete_distribution(void);
};
*/
#endif
