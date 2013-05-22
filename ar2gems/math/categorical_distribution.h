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



#ifndef __GSTLAPPLI_MATH_CATEGORICAL_DISTRIBUTION_H__
#define __GSTLAPPLI_MATH_CATEGORICAL_DISTRIBUTION_H__

#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/math/common.h>
#include <boost/math/distributions.hpp>
#include <boost/random/discrete_distribution.hpp>


class Categorical_distribution : public Named_interface
{
public:

  static Named_interface* create_new_interface( std::string& params){
    return new Categorical_distribution;
  }

  template<typename Iterator>
  Categorical_distribution(Iterator weight_begin, Iterator weight_end);
  Categorical_distribution(){}
  virtual ~Categorical_distribution(void);

  int number_of_category() const {return n_;}
  int sample(unsigned int n=1);

  std::vector<double> probabilities() const {return dist_.probabilities();}
  double probability(int category) const {return dist_.probabilities()[category];}
  void initialize(std::vector<double> weights);
  int simulate(double p) const;

  std::string type(){return "Discrete";}

private :
  boost::random::discrete_distribution<int, double> dist_;
  std::string def_name_;
  unsigned int n_;

  std::vector<double> cum_weights_;

};

#endif
