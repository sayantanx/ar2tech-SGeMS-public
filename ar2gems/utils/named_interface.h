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
** This file is part of the "utils" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_UTILS_NAMED_INTERFACE_H__ 
#define __GSTLAPPLI_UTILS_NAMED_INTERFACE_H__ 
 
#include <GsTLAppli/utils/common.h>
#include <GsTL/utils/smartptr.h> 

#include <GsTLAppli/utils/gstl_object_item.h>
 
#include <string> 

 
class Directory; 
 
typedef long RefCount; 
 
 
/** A Named_interface is an object that can be instantiated and
* managed by a Manager. Named_interfaces are reference counted,
* hence can be used with smart pointers (SmartPtr). Remember that
* when using smart pointers, memory must be allocated with \c new.
* Example: \code SmartPtr<Named_interface> ni = new Cartesian_grid; \endcode 
*/
class UTILS_DECL Named_interface : public GsTL_object_item {

  public: 
    Named_interface() : GsTL_object_item(), references_(0), parent_interface_(0), name_("") {}
 
    const Named_interface* new_ref() const { 
      Named_interface * me = (Named_interface *) this; 
      ++me->references_; 
      return this; 
    } 
 
    void delete_ref() const { 
      Named_interface * me = (Named_interface *) this; 
      if ( --me->references_ == 0 ) me->on_zero_references(); 
    } 

    void unref_no_delete() const {
      Named_interface * me = (Named_interface *) this;
      if( me->references_ > 0 )
        --me->references_;
    }
 
    RefCount references() const { return references_; } 

    Named_interface* parent_interface(){ return parent_interface_;}
    const Named_interface* parent_interface() const { return parent_interface_;}
    void  parent_interface(Named_interface* parent){
    	parent_interface_ = parent;
    	parent_ = parent;
    }

    std::string name()const{return name_;}
    void name(std::string& name) {
      item_name_ = name.c_str();
      name_ = name;
    }
 

    virtual QVariant item_data(int column) const{
    	if(column == 0) return QString(name_.c_str());
    	else if(column == 1) return this->item_type();
    	else return QVariant();
    }
    virtual GsTL_object_item *parent(){return parent_interface_;}
    virtual const GsTL_object_item *parent() const {return parent_interface_;}

  protected: 
    virtual ~Named_interface() {} 
    Named_interface* parent_interface_;
    std::string name_;



  private: 
    RefCount references_; 


 
    virtual void on_zero_references() { delete this; } 
}; 
 
 
 
#endif 
