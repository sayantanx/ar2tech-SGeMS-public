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



#ifndef __GSTLAPPLI_GEOSTAT_NUMODEL_H__
#define __GSTLAPPLI_GEOSTAT_NUMODEL_H__


#include <geostat/geostat_algo.h>
#include <GsTL/utils/smartptr.h>
#include <math/gstlpoint.h>

#include <geostat/parameters_handler.h>
#include <utils/error_messages_handler.h>

#include <vector>

class Geostat_grid;
class Grid_continuous_property;

class  NuTauModel : public Geostat_algo {
 
 public:

	virtual bool initialize( const Parameters_handler* parameters,
			   Error_messages_handler* errors );

  /* Runs the algorithm. 
    @return 0 if the run was successful*/
	virtual int execute(GsTL_project *);

	virtual std::string name() const { return "NuTauModel"; }

   public:
     static Named_interface* create_new_interface(std::string&);


  private:

	Geostat_grid* grid_;
	std::vector< Grid_continuous_property* > props_;
	Grid_continuous_property* redun_prop_;
  Grid_continuous_property* nu_values_prop_;
  std::vector< Grid_continuous_property* > tau_values_props_;
  bool is_redun_prop_;
  bool is_nu_;
	float nu0_;
  std::vector<float> tau_;
  float x0_;

  float NuModel_compute(std::vector<float>& probs);
  float TauModel_compute(std::vector<float>& probs);
};


#endif