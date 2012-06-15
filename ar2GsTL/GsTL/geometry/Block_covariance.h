#ifndef __GSTLAPPLI_GEOSTAT_ALGO_BLOCKCOVARIANCE_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_BLOCKCOVARIANCE_H__ 

#include <GsTLAppli/geostat/common.h>

#include <GsTL/geometry/covariance.h>
//#include <GsTLAppli/math/gstlvector.h>


template<class Location>
class Block_covariance : public Covariance<Location>{
 public:
  typedef Location first_argument_type;
  typedef Location second_argument_type;
  typedef typename Location::difference_type EuclideanVector;
  typedef typename Covariance_base<Location>::result_type result_type;

 public:

   Block_covariance():Covariance_base<Location>() {};
   virtual ~Block_covariance(){};

   Block_covariance(const Block_covariance<Location>& rhs) :
   Covariance<Location>(rhs){
    c0_ = rhs.c0_;
    block_vectors_ = rhs.block_vectors_;
    pt_covariance_ = rhs.pt_covariance_;
    block_covariance_ = rhs.block_covariance_;
   }

   Block_covariance(Covariance<Location> cov, float c0,
     std::vector< GsTLVector< float > > blk_vec )
     :Covariance<Location>(cov),c0_(c0),block_vectors_(blk_vec) {};

  Block_covariance<Location>* clone() const {
    return new Block_covariance<Location>(this);
  }

   Block_covariance(Covariance<Location>& cov, GsTLVector<int> block_pts, EuclideanVector cell_dim )
     :Covariance<Location>(cov) { 
       
      EuclideanVector delta(cell_dim[0]/block_pts[0],
        cell_dim[1]/block_pts[1],cell_dim[2]/block_pts[2]);
      EuclideanVector start((-cell_dim[0] + delta[0])/2,
        (-cell_dim[1] + delta[1])/2,(-cell_dim[2] + delta[2])/2);

      n_blkpoints_ = block_pts[0]*block_pts[1]*block_pts[2];

      for(int i =0; i< block_pts[0]; i++ )
        for(int j=0; j<block_pts[1]; j++ )
          for(int k=0; k<block_pts[2]; k++ ){
            EuclideanVector vec(start[0] + i*delta[0], start[1] + j*delta[1],
              start[2] + k*delta[2]);
            block_vectors_.push_back(vec);
          }

      EuclideanVector vec_c0(0.,0.,0.);
      c0_ = compute(vec_c0);

      pt_covariance_ = Covariance<Location>::compute(vec_c0);
      block_covariance_ = 0.0;
      for( int i=0; i < n_blkpoints_; ++i) {
        for(int j=0; j < n_blkpoints_; ++j) {
          block_covariance_ += Covariance<Location>::compute(block_vectors_[i]-block_vectors_[j]);
          if(i==j) block_covariance_-= Covariance<Location>::nugget();
        }
      }
      block_covariance_/=(n_blkpoints_*n_blkpoints_);
      /*
      typename std::vector< EuclideanVector >::const_iterator it = block_vectors_.begin();
      for(; it != block_vectors_.end() ; ++it) {
        EuclideanVector pt_in_block(it->x(),it->y(),it->z());
        block_covariance_ += Covariance<Location>::compute(pt_in_block);
      }
      block_covariance_/=n_blkpoints_; 
      */
//      block_covariance_-= Covariance<Location>::nugget();
   }

   virtual result_type c0() const { return c0_;}


  inline result_type operator()(const Location& u1, const Location& u2) const {
	  return compute( u2 - u1 );
  }

  inline result_type  compute(const Location head, const Location tail) const {
	  return compute(tail-head); 
  }

  inline  result_type compute(const EuclideanVector& vec) const  {
    result_type cov = 0;
    typename std::vector< EuclideanVector >::const_iterator it = block_vectors_.begin();
    for(; it != block_vectors_.end() ; ++it) {
      EuclideanVector eVec(vec[0] + it->x(),vec[1] + it->y(),vec[2] + it->z());
      cov += Covariance<Location>::compute(eVec);
 
     }
    return cov/n_blkpoints_;
  }

  inline double point_covariance() const {
    return pt_covariance_;
  }

  inline double block_covariance() const {
    return block_covariance_;
  }

protected:
  float c0_;
  std::vector< EuclideanVector > block_vectors_;
  float n_blkpoints_;
  double pt_covariance_;
  double block_covariance_;

};



#endif
