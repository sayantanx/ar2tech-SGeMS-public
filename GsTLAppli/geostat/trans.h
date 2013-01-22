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



#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_TRANS_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_TRANS_H__

#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/math/continuous_distribution.h>

class Geostat_grid;
class GsTLGridRegion;


class GEOSTAT_DECL trans : public Geostat_algo {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /** Runs the algorithm. 
   * @return 0 if the run was successful
   */
  
	virtual int execute( GsTL_project* proj=0 );
  /** Tells the name of the algorithm
   */
	virtual std::string name() const { return "trans"; }
   
 private:

	Geostat_grid* grid_;
		
	std::vector< float > reference_;

	bool is_local_cond_;
	
  GsTLGridRegion* region_;
	std::vector<GsTLGridProperty*> props_;
	GsTLGridProperty* cond_prop_;
	std::vector<float> weights_;
	typedef std::vector<float>::iterator wgth_iterator;

  SmartPtr<Continuous_distribution> cdf_source_;
  SmartPtr<Continuous_distribution> cdf_target_;

//	Cdf<float>* cdf_source_;
//	Cdf<float>* cdf_target_;

	void cdf_transform(GsTLGridProperty* prop);
	void  cdf_transform_weighted( GsTLGridProperty* prop  );

	SmartPtr<Continuous_distribution> get_cdf( const Parameters_handler* parameters,
		Error_messages_handler* errors, std::string suffix );


};

#endif

