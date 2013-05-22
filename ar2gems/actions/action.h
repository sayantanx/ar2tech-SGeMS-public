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



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "actions" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_ACTIONS_ACTION_H__ 
#define __GSTLAPPLI_ACTIONS_ACTION_H__ 
 
#include <actions/common.h>
#include <utils/named_interface.h> 
 
#include <string> 
 
 
class GsTL_project; 
class Error_messages_handler; 
 
/** The base class for all actions. 
 */ 
class ACTIONS_DECL Action : public Named_interface { 
 
 public: 
  virtual ~Action() {} 
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) = 0; 
  virtual bool exec() = 0; 

 
}; // end of class Action 
 
/** The base class for all actions on property that may not need parameters.
 * Parameters may be optional
 */
class ACTIONS_DECL PropertyNoParameterAction : public Action {

 public:
	PropertyNoParameterAction(){}
	PropertyNoParameterAction(std::string name):name_(name){}
  virtual ~PropertyNoParameterAction() {}
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) = 0;
  virtual bool exec() = 0;

	virtual std::string name() const {
		return name_;
	}

 protected :
	std::string name_;

};

/** The base class for all actions on property group that may not need parameters.
 * Parameters may be optional
 */
class ACTIONS_DECL GroupNoParameterAction : public Action {

 public:
	GroupNoParameterAction(){}
	GroupNoParameterAction(std::string name):name_(name){}
  virtual ~GroupNoParameterAction() {}
  virtual bool init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ) = 0;
  virtual bool exec() = 0;

  virtual std::string name() const {
		return name_;
	}

 protected :
	std::string name_;
};
#endif 
