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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#ifndef __GSTLAPPLI_CROSS_VARIOG_COMPUTER_H__
#define __GSTLAPPLI_CROSS_VARIOG_COMPUTER_H__

#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/math/gstlvector.h>

#include <vector>
#include <map>
#include <string>

class Discrete_function;
class Geostat_grid;



class GRID_DECL Cross_variog_computer
{
 public:
    Cross_variog_computer(){}
    Cross_variog_computer(Geostat_grid *pset,Geostat_grid *grid);

    
    

    std::vector<int> compute_variogram_values(Discrete_function &f,
					       GsTLVector<int> v,int num,
					       float var, std::string & model_type,
					      double mod_param);

    float compute_variance();
    
 protected:
    virtual float compute_single_value(
	const std::vector< std::pair<float,float> >& candidates,
	std::string &model_type,double mod_param) ;

   

    
 protected:

   const Geostat_grid *pset_;
   Geostat_grid *grid_;

 private:

   enum MeasureType{semivariogram,covariance,correlogram,
		    general_relative_semivariogram,logarithmic_semivariogram,
		    semimadogram,indicator};

   std::map<std::string,MeasureType> models_;

   std::string temp_prop_name;

   double prim_var_;
   double sec_var_;
};
	
#endif
