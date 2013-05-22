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

#ifndef __GSTLAPPLI_UTILS_MANAGER_H__ 
#define __GSTLAPPLI_UTILS_MANAGER_H__ 
 
 
#include <GsTLAppli/utils/common.h>
#include <GsTLAppli/utils/directory.h>
#include <GsTLAppli/utils/gstl_item_model.h>
 
 
#include <string> 
#include <map> 
#include <iostream> 
#include <iterator>
#include <vector>
#include <string>

class Channel;
class Root_manager;
 
typedef Named_interface* (*CallBackType)(std::string& ); 
 

UTILS_DECL Named_interface* create_directory(std::string&); 

 
/** Root is a manager singleton (see Design Patterns by Gamma et al. for a
 * precise definition of singleton). It is a globally accessible Manager
 * that manages all other managers in SGeMS.
 * Using Root, all managers are globally accessible. For example, to
 * access a Manager called TopLevel which is itself managed by Manager
 * InpuFilter, itself managed by Root, use:
 * \code
 * SmartPrt<Named_interface> ni =
 *   Root::instance()->interface( "/InputFilter/TopLevel" );
 * Manager* mng = dynamic_cast<Manager*>( ni.raw_ptr() );
 * \endcode
 * \c ni is a smart pointer to Manager TopLevel. Root returns a Named_interface
 * which is then casted into a required type, i.e. a Manager.
 * If TopLevel can create interfaces of type "gslib_filter", the following code
 * creates a new instance of type "gslib_filter" called "example":
 * \code
 * SmartPtr<Named_interface> ni =
 *   Root::instance()->new_interface( "gslib_filter",
 *                                    "/InputFilter/TopLevel/example" );
 * \endcode
 * \c ni can then be casted into a \c gslib_filter.
 */
class UTILS_DECL Root
{

public:

	static Root_manager* instance();

private:
	static Root_manager* instance_;

	Root();
	Root(const Root&);
	Root& operator=(const Root&);

};



class UTILS_DECL Manager : public Directory { 
 
  public: 
 
    /** Default constructor. The manager won't look for plug-ins
    * if it doesn't know how to create a type.
    */
    Manager() ; 

    /** This constructor allows to specify the name of the directory
    * to search for plug-ins if needed.
    */
    Manager( std::string& plugin_subdir ); 

    virtual ~Manager();

    /** 
     * Instantiates a new named object. If there already is an object named 
     * \c name, the defaut behavior is to return a null pointer (no object is
     * created). This default behavior can be overriden by passing the function
     * a non-null pointer \c final_name. In that case, the manager will modify
     * the last part of \c name (ie the path part of the name is not altered)
     * to make it unique (for example /grid/obj could be changed to /grid/obj_1).
     * The modified part of \c name is then written to \final_name
     * @param type_param is a string containing the name of the type to build 
     * and some optional parameters. The format of the string is: 
     *    type://optional_parameters 
     * The parameters are passed to the factory method (type CallBackType) to 
     * be processed. The format may vary. 
     * @param name is the name of the instance created. If no name is 
     * supplied, the object won't be managed (it won't appear in the  
     * list of managed objects).  
     * @param name is a pointer to a string that will contain the name under
     * the new interface has been registered. It can be different from \c name
     * if an interface called \c name was already there.
     * @return a Named_interface of the specified type. 
     */ 
    virtual SmartPtr<Named_interface> new_interface(  
    	const std::string& type_param, 
    	const std::string& name = "",
      std::string* final_name = 0
      ); 
 

    virtual Named_interface*
    new_interface_raw(const std::string& type_param,
                           const std::string& name  );

    /** Removes the interface called \a name from the list of managed 
    * interfaces. If the interface is not used anywhere else, it is 
    * automatically deleted (i.e. the memory is freed).
    * The function returns false if the it could not delete the interface
    * (usually because no interface called \a name is managed)
    */
    virtual bool delete_interface(const std::string& name, bool send_model_signal =true); 
     
    /** Use this function to retrieve the interface called \a name. If no such
    * interface is managed by the Manager, a null pointer is returned.
    */
    virtual SmartPtr<Named_interface> interface(const std::string& name);
    
    /** Retrieve the name of an interface from a pointer to the interface
    */
    virtual std::string name( const Named_interface* interface ); 
 
    /** Use this function to register a new factory method. \a type is the
    * name of the new type to register, and \a callback the function to use
    * to create an interface of type \a type. The function must have the 
    * following signature:   
    * \code 
    *   Named_interface* function( std::string& );
    * \endcode
    * The string passed to the callback function is an optional argument 
    * containing parameters that can be used when creating a new interface
    */
    bool factory(const std::string& type, const CallBackType& callback);   
     
    virtual SmartPtr<Directory> directory() const; 
    virtual void directory(Directory*); 
     
 
    // To iterate on types  
    class type_iterator; 
    /** Returns an iterator to the begining of the list of all the types
    * the Manager can create
    */
    inline type_iterator begin() const; 
    inline type_iterator end() const;  
 
    // To iterate on the interfaces the manager currently manages 
    class interface_iterator; 
    /** Returns an iterator to the begining of the list of all the 
    * interfaces currently managed
    */
    inline interface_iterator begin_interfaces() const; 
    inline interface_iterator end_interfaces() const; 
 
    /** Returns the number og currently managed interfaces
     */
    inline int interface_count() const;

    // To iterate on the interfaces the manager currently manages 
    class names_iterator; 
    /** Returns an iterator to the begining of the list of all the 
    * names of the interfaces currently managed
    */
    inline names_iterator begin_names() const; 
    inline names_iterator end_names() const; 

    /** outputs to stream \a os a list of all the interfaces currently managed.
    * If the manager manages other managers, their content is recursively 
    * displayed. \a level indicates the level of the current manager in the 
    * manager hierarchy. This function is only used for debugging.
    */
    void list_all(std::ostream& os, int level=0) const; 

    /** This overloaded function outputs to a Channel instead of an std::stream
    */
    void list_all(Channel& ch, int level=0) const; 
    void list_factory_methods( Channel& ch ) const; 
     

    static void set_main_plugin_path( const std::string& path );
    static std::string main_plugin_path(); 
    std::string plugin_path(); 

	// TL modified
	std::string finish(std::string &);
 
// GsTL_object_item redefinition
    virtual QString item_type()const;

    virtual GsTL_object_item *child(int row);
    virtual const GsTL_object_item *child(int row) const;
    virtual int childCount() const;
    virtual int columnCount() const;
    virtual QVariant item_data(int column) const;
    virtual int row() const;


  private: 

    // TL modified
	std::vector< std::string> _buf;


    typedef std::map<std::string, CallBackType> CallBackMap; 
    typedef std::map<std::string, GsTL_object_item> CallBackObjectItemMap;
    typedef std::map<std::string, SmartPtr<Named_interface> > InterfaceMap; 
     
    CallBackMap factory_;
    CallBackObjectItemMap factory_item_;
    InterfaceMap interfaces_; 

 
    std::string plugin_subdir_; 
 
    /** @returns a pointer to the subdirectory "name" if it exists,  
     * 0 otherwise. 
     */ 
    SmartPtr<Directory> get_subdir(const std::string& name); 
     
    /** tries to load the library "type" (the actual name of the library 
     * is libtype.so or libtype.dll (depending on the plateform). 
     * The library is currently searched into the current directory. That 
     * behavior should be changed to search in a directory indicated by an 
     * environment variable. 
     */ 
    bool load_library( const std::string& type ); 
     
    static std::string plugin_dir_; 
    static const std::string dll_extension_;  
    static std::string init_plugin_path(); 

protected:
    GsTL_ItemModel * model_;
 
}; // end of class Manager 
 
 
 
 
class UTILS_DECL Manager::type_iterator { 
 private: 
  typedef std::map<std::string, CallBackType> Map; 
  typedef Map::const_iterator iterator; 
   
 public:
  typedef std::forward_iterator_tag iterator_category;
  typedef std::string value_type;
  typedef iterator::difference_type difference_type;
  typedef std::string* pointer;
  typedef std::string& reference;

  public: 
    type_iterator(iterator& curr) : curr_(curr) {}; 
    type_iterator& operator++(int) { ++curr_; return *this; } 
    type_iterator operator++() { curr_++; return *this; } 
    type_iterator& operator--(int) { --curr_; return *this; } 
    type_iterator operator--() { curr_--; return *this; } 
    bool operator!=(const type_iterator& rhs) { return rhs.curr_ != curr_; } 
    bool operator==(const type_iterator& rhs) { return rhs.curr_ == curr_; } 
    const std::string& operator*() { return curr_->first; } 
    const std::string* operator->() { return &curr_->first; } 
 
  private: 
      iterator curr_; 
 
}; // end of class type_iterator 
 
 
class UTILS_DECL Manager::interface_iterator { 
 private: 
    typedef std::map<std::string, SmartPtr<Named_interface> > Map; 
    typedef Map::const_iterator iterator; 
 
  public: 
    interface_iterator(iterator& curr) : curr_(curr) {}; 
    interface_iterator& operator++(int) { ++curr_; return *this; } 
    interface_iterator operator++() { curr_++; return *this; } 
    interface_iterator& operator--(int) { --curr_; return *this; } 
    interface_iterator operator--() { curr_--; return *this; } 
    bool operator!=(const interface_iterator& rhs) { return rhs.curr_ != curr_; } 
    bool operator==(const interface_iterator& rhs) { return rhs.curr_ == curr_; } 
    const SmartPtr<Named_interface>& operator*() { return curr_->second; } 
    const SmartPtr<Named_interface>* operator->() { return &curr_->second; } 
 
  private: 
    iterator curr_; 
 
}; // end of class interface_iterator 
 
 
class UTILS_DECL Manager::names_iterator { 
 private: 
    typedef std::map<std::string, SmartPtr<Named_interface> > Map; 
    typedef Map::const_iterator iterator; 
 
  public: 
    names_iterator(iterator& curr) : curr_(curr) {}; 
    names_iterator& operator++(int) { ++curr_; return *this; } 
    names_iterator operator++() { curr_++; return *this; } 
    names_iterator& operator--(int) { --curr_; return *this; } 
    names_iterator operator--() { curr_--; return *this; } 
    bool operator!=(const names_iterator& rhs) { return rhs.curr_ != curr_; } 
    bool operator==(const names_iterator& rhs) { return rhs.curr_ == curr_; } 
    const std::string& operator*() { return curr_->first; } 
    const std::string* operator->() { return &curr_->first; } 
 
  private: 
    iterator curr_; 
 
}; // end of class names_iterator 
  
 
class UTILS_DECL Root_manager : public Manager {
public :
//	Root_manager() : Manager(), model_(0){}
	Root_manager() : Manager(){ name_ = ("Root");}
	virtual ~Root_manager(){ delete model_;}

	GsTL_ItemModel* model(){return model_;}
	void model(GsTL_ItemModel* model){model_ = model;}

//private :
//	GsTL_ItemModel* model_;
};

//================================== 
//    inline functions 
 
inline Manager::type_iterator 
Manager::begin() const { 
  CallBackMap::const_iterator it = factory_.begin(); 
  return type_iterator( it );  
} 
 
inline Manager::type_iterator 
Manager::end() const { 
  CallBackMap::const_iterator it = factory_.end() ; 
  return type_iterator( it );  
} 
 
 
inline Manager::interface_iterator 
Manager::begin_interfaces() const { 
  InterfaceMap::const_iterator it = interfaces_.begin(); 
  return interface_iterator( it );  
} 
  
inline Manager::interface_iterator 
Manager::end_interfaces() const { 
  InterfaceMap::const_iterator it = interfaces_.end(); 
  return interface_iterator( it ); 
} 
 
inline int Manager::interface_count() const{
	return interfaces_.size();

}

inline Manager::names_iterator 
Manager::begin_names() const { 
  InterfaceMap::const_iterator it = interfaces_.begin(); 
  return names_iterator( it );  
} 
  
inline Manager::names_iterator 
Manager::end_names() const { 
  InterfaceMap::const_iterator it = interfaces_.end(); 
  return names_iterator( it ); 
} 
 
 
inline std::string Manager::main_plugin_path() { 
  return plugin_dir_; 
} 
 
inline std::string Manager::plugin_path() { 
  return plugin_dir_ + plugin_subdir_ ; 
} 
 
#endif 
