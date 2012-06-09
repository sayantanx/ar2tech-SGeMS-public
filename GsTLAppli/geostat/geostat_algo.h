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
** This file is part of the "geostat" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GEOSTAT_ALGO_H__ 
#define __GSTLAPPLI_GEOSTAT_ALGO_H__ 
 
 
#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/utils/named_interface.h> 
#include <GsTLAppli/geostat/parameters_handler_impl.h>
 
class Error_messages_handler; 
//class Parameters_handler; 
class GsTL_project; 
 
/** 
 * This is the interface common to all geostat algorithms. 
 * It is a Named_interface, so it can be created by a Manager. 
 */ 
 
class GEOSTAT_DECL Geostat_algo : public Named_interface { 
  public: 
    virtual ~Geostat_algo() {}; 
 
    /** Initializes the parameters of the algorithm 
     * @return false if errors were encountered. If error_mesg 
     * is not 0, the encountered errors are placed into error_mesg. 
     * @param error_mesg is a string containing the name of 
     * the parameters that contained errors. 
     * If the string is empty, the initialization 
     * was successful. The string has the following format: 
     * field1://explanation1;;field2://explanation;; 
     */  
    virtual bool initialize( const Parameters_handler* parameters, 
			     Error_messages_handler* errors ) = 0; 
 
    /** Runs the algorithm.  
     * @return 0 if the run was successful 
     */ 
    virtual int execute( GsTL_project* proj=0 ) = 0; 
 
    /** Tells the name of the algorithm 
     */ 
    virtual std::string name() const { return ""; }

protected :

  std::string parameters_;

  void extract_parameters(const Parameters_handler* parameters){
    const Parameters_handler_xml *para_xml =
          static_cast<const Parameters_handler_xml*> (parameters);
    parameters_ = para_xml->getDoc().toString(-1).toStdString();
  }

  
     
}; 
 
 
 
 
#endif 
