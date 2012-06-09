
#ifndef __GSTL_COVARIANCE_TABLE_H__
#define __GSTL_COVARIANCE_TABLE_H__

#include <GsTL/utils/gstl_error_messages.h>
#include <GsTL/geometry/geometry.h>
#include <GsTLAppli/math/gstlvector.h>
#include <GsTLAppli/math/gstlpoint.h>

//=======================================
// Covariance table

class Covariance_table {
 public:

  template<class Covariance>
  Covariance_table(int nx, int ny, int nz, 
		   GsTLCoordVector cell_dim, const Covariance& covar) 
    : nx_(nx), ny_(ny), nz_(nz), 
    xsize_(nx*2+1), ysize_(ny*2+1), zsize_(nz*2+1), xysize_(xsize_*ysize_),
    size_(xsize_*ysize_*zsize_)  {
    
    covar_table_ = new float[size_];
    
    GsTLPoint center(0,0,0);
    
    int i=0;

    for(int z = -nz ; z <= nz ; z++)
    for(int y = -ny ; y <= ny ; y++)
      for(int x = -nx ; x <= nx ; x++) {
	GsTLPoint loc(x*cell_dim.x(),y*cell_dim.y(),z*cell_dim.z());
	covar_table_[i++] = static_cast<float>( covar(center, loc) );
      }
	}


  template<class Iterator>
  Covariance_table(int nx, int ny, int nz,Iterator begin, Iterator end)
   :nx_(nx), ny_(ny), nz_(nz),xsize_(nx_*2+1), ysize_(ny_*2+1), zsize_(nz_*2+1), 
   xysize_(xsize_*ysize_),size_(xsize_*ysize_*zsize_)
  {
    if(  size_ != std::distance(begin,end) ) {
      covar_table_= NULL;
      return;
    }
    covar_table_ = new float[size_];
    
    GsTLPoint center(0,0,0);
    
    int i=0;
    for( ; begin != end; ++begin ) 
      covar_table_[i++] = static_cast<float>( *begin );
	}

/*
** Build a regularized covariance map 
** The only restriction is that the center of block must correspond
** to a point on the grid, i.e. block must be of odd size.
** This could be done much faster in the Fourrier space.
** Furthermore the size nx, ny, nz should be large enough such that
** the margin of size blk_nx, blk_ny, blk_nz around the extremity of the map are not used.
** Add by A. Boucher August 2005.
*/

  template<class Covariance>
  Covariance_table(int nx, int ny, int nz,
			int blk_nx, int blk_ny, int blk_nz,
		   GsTLCoordVector cell_dim, const Covariance& covar) 
    : nx_(nx-blk_nx), ny_(ny-blk_ny), nz_(nz-blk_nz), 
    xsize_(nx_*2+1), ysize_(ny_*2+1), zsize_(nz_*2+1), xysize_(xsize_*ysize_),
    size_(xsize_*ysize_*zsize_)  {

    int blk_xsize = 2*blk_nx +1, blk_ysize = 2*blk_ny +1, blk_zsize = 2*blk_nz +1;
	int blk_size = blk_xsize * blk_ysize * blk_zsize;

    covar_table_ = new float[size_];
    
    GsTLPoint center(0,0,0);
    
    int i=0;

    for(int z = -nz_ ; z <= nz_ ; z++)
    for(int y = -ny_ ; y <= ny_ ; y++)
      for(int x = -nx_ ; x <= nx_ ; x++) {
		covar_table_[i]=0.0;
		location_3d<int> loc(x,y,z);
		for(int iz = -blk_nz ; iz <=blk_nz; iz++)
		for(int iy = -blk_ny ; iy <=blk_ny; iy++)
			for(int ix = -blk_nx ; ix <=blk_nx; ix++) {
				GsTLPoint loc( x+ix,y+iy,z+iz );
                covar_table_[i] = covar_table_[i] + static_cast<float>( covar(center, loc) )/blk_size;
			}
		i++;
      }
	}

  /** Copy constructor. 
   * Since a covariance table is probably a very large object, it is certainly 
   * not a good idea to use the copy constructor, at least not with the current
   * implementation. A smarter implementation would use smart pointers and only
   * copy the pointer to the table instead of the deep copy currently done.
   */
  Covariance_table(const Covariance_table& rhs) {
    covar_table_ = new float[rhs.size_];
    for(int i=0; i<rhs.size_; i++)
      covar_table_[i] = rhs.covar_table_[i];

    nx_ = rhs.nx_;
    ny_ = rhs.ny_;
    nz_ = rhs.nz_;
    xsize_ = rhs.xsize_;
    ysize_ = rhs.ysize_;
    zsize_ = rhs.zsize_;
    xysize_ = rhs.xysize_;
    size_ = rhs.size_;
  }


  /** Assignment. 
   * Since a covariance table is probably a very large object, it is certainly 
   * not a good idea to use the assignment operator, at least not with the current
   * implementation. A smarter implementation would use smart pointers and only
   * copy the pointer to the table instead of the deep copy currently done.
   */
  Covariance_table& operator=(const Covariance_table& rhs) {
    if(covar_table_ != 0)
      delete [] covar_table_;

    covar_table_ = new float[rhs.size_];
    for(int i=0; i<rhs.size_; i++)
      covar_table_[i] = rhs.covar_table_[i];

    nx_ = rhs.nx_;
    ny_ = rhs.ny_;
    nz_ = rhs.nz_;
    xsize_ = rhs.xsize_;
    ysize_ = rhs.ysize_;
    zsize_ = rhs.zsize_;
    xysize_ = rhs.xysize_;
    size_ = rhs.size_;
    
    return *this;
  }


  ~Covariance_table() {
    delete [] covar_table_ ;
  }
  
  template <class Location>
  inline double operator()(const Location& u1, const Location& u2) const;

  template <class Euclidean_vector>
  inline double operator()(const Euclidean_vector& vec) const;

 private:
  
  float* covar_table_;
  

  int nx_, ny_, nz_;
  int xsize_, ysize_, zsize_;
  int xysize_;
  int size_;

};  // end of class Covariance_table



template <class Location>
inline double 
Covariance_table::operator()(const Location& u1, const Location& u2) const {
  typedef typename Location::difference_type Euclidean_vector;
  Euclidean_vector vec = u2-u1;
  int index = (static_cast<int>(vec[2])+nz_)*xysize_  + 
              (static_cast<int>(vec[1])+ny_)*xsize_  + 
              static_cast<int>(vec[0])+nx_;

  gstl_assert(index >=0);
  if(index > size_)
    return 0;
  else
    return covar_table_[index];
};

template <class Euclidean_vector>
inline double 
Covariance_table::operator()(const Euclidean_vector&  vec ) const {

  int index = (static_cast<int>(vec[2])+nz_)*xysize_  + 
              (static_cast<int>(vec[1])+ny_)*xsize_  + 
              static_cast<int>(vec[0])+nx_;

  //gstl_assert(index >=0);
  if(index > size_ || index < 0 )
    return 0;
  else
    return covar_table_[index];
};

#endif
