#include "categorical_distribution.h"


template<typename Iterator>
Categorical_distribution::Categorical_distribution(Iterator weight_begin, Iterator weight_end) : dist_(weight_begin,weight_end)
{
  n_ = std::distance(weight_begin, weight_end);
  cum_weights_.resize(n_);
  std::partial_sum(weight_begin, weight_end, cum_weights_.begin() );
  
}


Categorical_distribution::~Categorical_distribution(void)
{
}


void Categorical_distribution::initialize(std::vector<double> weights) { 

  dist_ = boost::random::discrete_distribution<int,double>();
  n_ = weights.size();
  cum_weights_.resize(n_);
  std::partial_sum(weights.begin(),weights.end(), cum_weights_.begin() );

}

int Categorical_distribution::simulate(double p) const{
  return std::distance(cum_weights_.begin() , std::find(cum_weights_.begin(), cum_weights_.end(), p ));
}