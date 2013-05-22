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

#ifndef __GSTLAPPLI_GRID_MODEL_GEOSTAT_GRID_H__ 
#define __GSTLAPPLI_GRID_MODEL_GEOSTAT_GRID_H__ 
 


#include <grid/common.h>
//#include <grid/grid_model/gval_iterator.h>
#include <utils/named_interface.h> 
#include <math/gstlpoint.h> 
#include <grid/coordinate_mapper.h>

#include <QModelIndex>
#include <QVariant>
#include <QString>
#include <QMimeData>
#include <QAbstractProxyModel>
#include <QStringList>


#include <typeinfo> 
#include <list> 
#include <string>

class Grid_continuous_property;
class Grid_categorical_property;
class Grid_weight_property;
class Grid_region;
class GsTLGridPropertyGroup;
class MultiRealization_property; 
class CategoricalPropertyDefinition;
template<class R> class Gval_iterator; 
template<class R> class Gval_const_iterator; 
class LinearMapIndex; 
class TabularMapIndex; 
class Neighborhood; 
template<class Location_> class Covariance; 


 
/** Geostat_grid is the root of the grid-models hierarchy. It is the 
 * base object on which any geostatistics algorithm can be applied. 
 *  
 * Every node of a geostat_grid is assigned a unique id (node_id). 
 * It is possible to re-order the nodes by re-defining function node_id(...). 
 */ 
 
class GRID_DECL Geostat_grid : public Named_interface {


 public: 
 
  typedef float property_type;  
  typedef GsTLPoint location_type; 
 
  typedef Gval_iterator<LinearMapIndex> iterator; 
  typedef Gval_const_iterator<LinearMapIndex> const_iterator; 
  typedef Gval_iterator<TabularMapIndex> random_path_iterator; 
 
  
 public: 
 
  Geostat_grid(): coord_mapper_(0){}
  Geostat_grid(std::string name);
  virtual ~Geostat_grid() {} 

  // Returns the most specific name of the current class 
  virtual std::string classname() const { return "Geostat_grid"; }

  virtual std::string name() const { return name_; }
  virtual void name(std::string name){ name_ = name; }


  /** Get the topology of the grid
  * Allows conversion between geological and real-world coordinates
  */
  virtual Coordinate_mapper* coordinate_mapper() {return coord_mapper_;}
  virtual const Coordinate_mapper* coordinate_mapper() const {return coord_mapper_;}
  
  virtual void set_coordinate_mapper(Coordinate_mapper* coord_mapper) {coord_mapper_ = coord_mapper;}

  /** get the support (length or volume) of a cell at location nodeid
  */
  virtual double get_support(int nodeid) const {return 1;}

 
  //------------------------------------ 
  // Properties management 
 
  /** Adds a new property called \a name. 
   * A pointer to the new property is returned. If \a add_property(...) failed, 
   * for example because the property already existed, a null pointer 
   * is returned. 
   */ 
  virtual Grid_continuous_property* add_property( const std::string& name ) = 0;
  
  /** Adds a new property called \a name where the values
   * are loaded from the file \a filename.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_continuous_property* add_property_from_disk( const std::string& name,const std::string& filename )=0;

  /** Adds a new weight property called \a name. 
   * A pointer to the new property is returned. If \a add_property(...) failed, 
   * for example because the property already existed, a null pointer 
   * is returned. 
   */ 
  virtual Grid_weight_property* add_weight_property( const std::string& name ) = 0;
  
  /** Adds a new weight property called \a name where the values
   * are loaded from the file \a filename.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_weight_property* add_weight_property_from_disk( const std::string& name,const std::string& filename )=0;

  /** Adds a new categorical property called \a name.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_categorical_property* add_categorical_property(
			  const std::string& name,
			  const std::string& definition_name="Default") = 0;

  /** Adds a new categorical property called \a name where the values
   * are loaded from the file @filename.
   * A pointer to the new property is returned. If \a add_categorical_property(...)
   * failed,for example because the property already existed, a null pointer
   * is returned.
   */
  virtual Grid_categorical_property* add_categorical_property_from_disk(
			  const std::string& name, const std::string& filename,
			  const std::string& definition_name="Default") = 0;

  /** Removes a given property from the property list 
   * @return false if the function failed, eg because the property 
   * did not exist 
   */ 
  virtual bool remove_property( const std::string& name ) = 0; 
 
  /** Selects one of the properties of the grid. That property becomes 
   * the property the grid operates on by default: iterators returned 
   * by begin(), end() will iterate on the default property.  
   */ 
  virtual Grid_continuous_property* select_property( const std::string& name ) = 0;
  
  /** Returns a pointer to the currently selected property. To select a 
  * property, use function \a select_property( name ).
  * By default, the first property that was added to the grid is the
  * selected property.
  */
  virtual const Grid_continuous_property* selected_property() const = 0; 
  virtual Grid_continuous_property* selected_property() = 0;
  
  /** Provides direct access to property \a name. The function returns
  * a pointer to the property array. If property \a name does not exist,
  * a null pointer is returned.
  */
  virtual const Grid_continuous_property* property( const std::string& name ) const = 0; 
  virtual Grid_continuous_property* property( const std::string& name ) = 0; 

  /** Provides direct access to categorical property \a name. The function returns
  * a pointer to the categorical property array. If categorical property \a name does not exist,
  * a null pointer is returned.
  */
  virtual const Grid_categorical_property* categorical_property( const std::string& name ) const = 0;
  virtual Grid_categorical_property* categorical_property( const std::string& name ) = 0;


  /** Provides direct access to weight property \a name. The function returns
  * a pointer to the categorical property array. If weight property \a name does not exist,
  * a null pointer is returned.
  */
  virtual const Grid_weight_property* weight_property( const std::string& name ) const = 0;
  virtual Grid_weight_property* weight_property( const std::string& name ) = 0;

  /** Gives the list of all the names of the properties currently in the grid.
  */
  virtual std::list<std::string> property_list() const = 0; 

  /** Gives the list of all the names of the categorical properties currently in the grid.
  */
   virtual std::list<std::string> categorical_property_list() const = 0; 

  /** Gives the list of all the names of the weight properties currently in the grid.
  */
   virtual std::list<std::string> weight_property_list() const = 0; 

  /** Adds a multi-realization property to the grid.
  */
  virtual MultiRealization_property*  
    add_multi_realization_property( const std::string& name ) = 0; 
 

  //--------------------------- 
  // PropertyGroup management 
  //--------------------------- 
  virtual GsTLGridPropertyGroup* add_group( const std::string& name, const std::string& type  )=0;   
  virtual std::list<std::string> get_group_names(const std::string& type = "") const=0;   
  virtual unsigned int group_size() const=0;   
  virtual GsTLGridPropertyGroup* get_group( const std::string& name )=0;   
  virtual const GsTLGridPropertyGroup* get_group( const std::string& name ) const =0;  
  virtual void remove_group( const std::string& name )=0;

  //--------------------------- 
  // Region management 
 

  virtual std::list<std::string> region_list() const = 0; 

  virtual const Grid_region* region(const std::string& region_name) const = 0;
  virtual Grid_region* region(const std::string& region_name) = 0;

  virtual Grid_region* add_region( const std::string& region_name ) = 0; 
  virtual bool remove_region( const std::string& region_name ) = 0; 
 
  /** Selects a region. After calling this function, any operation  
   * that can be restricted to a region will only operate on the 
   * selected region. For example, an iterator returned by begin() 
   * will only iterate on the selected region.  
   */ 
  virtual bool select_region( const std::string& region_name ) = 0; 
//  virtual bool select_region(const std::vector<std::string>& region_names) = 0;
 
  virtual const Grid_region* selected_region() const = 0; 
  virtual Grid_region* selected_region() = 0;

  virtual void unselect_region() =0;

  virtual bool is_inside_selected_region(int node_id) const = 0;

  //-------------------------------- 
  // neighborhood definition 

  /** Creates a new ellipsoid neighborhood. The ellipsoid is defined by the 
  * 3 affinity factors \a x, \a y, \a z, and the 3 angles (in degrees) \a ang1,
  * \a ang2, \a ang3.
  * If covariance \a cov is not 0, it will be used by the neighborhood to sort
  * the neighbors it finds: the first neighbor will be the closest to the 
  * central point u, ie the point v such that cov(u,v) is highest.
  * Set \a only_harddata to true to force the neighborhood to only consider
  * hard-data.
  * \warning It is the user's responsability to \c delete the neighborhood 
  * when it is not needed anymore.}
  */
  virtual Neighborhood* neighborhood( double x, double y, double z, 
				      double ang1, double ang2, double ang3, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0)=0 ; 

  /** Creates a new ellipsoid neighborhood. Triplet \a dim defines the affinity 
  * ratios of the ellispoid, and triplet angles the rotation angles.
  * If covariance \a cov is not 0, it will be used by the neighborhood to sort
  * the neighbors it finds: the first neighbor will be the closest to the 
  * central point u, ie the point v such that cov(u,v) is highest.
  * Set \a only_harddata to true to force the neighborhood to only consider
  * hard-data.
  * \warning It is the user's responsability to \c delete the neighborhood 
  * when it is not needed anymore.
  */
  virtual Neighborhood* neighborhood( const GsTLTripletTmpl<double>& dim, 
				      const GsTLTripletTmpl<double>& angles, 
				      const Covariance<location_type>* cov=0, 
				      bool only_harddata = false, const Grid_region* region = 0,
              Coordinate_mapper* coord_mapper=0)=0; 
  virtual Neighborhood* colocated_neighborhood( const std::string& prop ); 

  /** It is the used reponsability to set a selected property
  * to the colocated neighborhood
  */
  virtual Neighborhood* colocated_neighborhood(  ); 
  //---------------------------   
  // iterators 
 
  /** returns an iterator to the begining of the current region, 
   * iterating on property \a prop. If no property is specified, the iterator
   * iterated on the current selected property. 
   */ 
  virtual iterator begin( Grid_continuous_property* prop = 0 ) = 0; 
  virtual iterator end( Grid_continuous_property* prop = 0 ) = 0; 
  virtual const_iterator begin( const Grid_continuous_property* prop = 0 ) const = 0; 
  virtual const_iterator end( const Grid_continuous_property* prop = 0 ) const = 0; 

  /** Call this function before any call to \a random_path_begin(), or 
  * \a random_path_end(), to initialize the random path. 
  * The new random path is obtained by shuffling the elements of the current
  * random path. 
  * If \a from_scratch is true, the new random path is obtained by shuffling 
  * the default path (ie the path that visits node 1 first, then node 2, ...).
  */
  virtual void init_random_path( bool from_scratch = true ) = 0; 

  /** Get the beginning of the current random path. To change the random path
  * use \a init_random_path().
  */
  virtual random_path_iterator random_path_begin( Grid_continuous_property* prop = 0 ) = 0; 
  virtual random_path_iterator random_path_end( Grid_continuous_property* prop = 0 ) = 0; 
 
 
  //---------------------------- 
  // Misc. 
   
  /** Computes the location of a node, given its node_id. 
  // This is the "geological" coordinates.  Use xyz_location to get
  // the real work coordinate
   */ 
  virtual location_type location( int node_id ) const = 0; 

  /** Computes the location of a node, given its node_id. 
  // This is the "real world" coordinates.  Use location to get
  // the geological coordinate
   */ 
  virtual location_type xyz_location( int node_id ) const {return this->location(node_id);} 

  /** Find the node of the grid which is closest to the input point 
   * The input point (geovalue) may not be a grid node.  
   * @return the node-id of the node closest to "P" 
   */ 
  virtual GsTLInt closest_node( const location_type& P ) const = 0; 
 
  /** This function returns the node-id of the index-th node. 
   * This is useful if there is a re-mapping of the node-id, ie 
   * if we want that the first node has node-id 45, instead of the  
   * default id 1. (this is especially useful for multiple grids). 
   */ 
  virtual GsTLInt node_id( GsTLInt index ) const { return index; } 
 
  /** Returns the total number of nodes in the grid.
  */
  virtual GsTLInt size() const  = 0;

  //TL modified
  virtual bool reNameProperty(std::string old_name, std::string new_name) = 0;


  //GsTL_object_item
  virtual int row() const;
  virtual GsTL_object_item* properties_item(){return this->child(0);}
  virtual GsTL_object_item* regions_item()=0;


public :



 protected :
   Coordinate_mapper* coord_mapper_;


 protected :
  std::string name_;

}; 
 
 
#endif 
 
