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
 ** This file is part of the "appli" module of the Geostatistical Earth
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

/* Edited A. Boucher @ ar2tech 2011
 * */


#ifndef __GSTLAPPLI_TREE_FACTORY_H__ 
#define __GSTLAPPLI_TREE_FACTORY_H__ 

#include <GsTLAppli/appli/common.h>
#include <GsTLAppli/utils/manager.h> 
#include <GsTLAppli/utils/singleton_holder.h> 

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
/*
class APPLI_DECL Root
{

public:

	static Manager* instance();

private:
	static Manager* instance_;

	Root();
	Root(const Root&);
	Root& operator=(const Root&);

};
*/
/* Aliases for manager names. Having all the names centralized  
 * makes maintanance easier. 
 */

// namespace MngRep { 

static const std::string actions_manager = "/Actions";
static const std::string python_script_manager = "/PythonScripts";
static const std::string python_group_script_manager = "/PythonGroupScripts";

static const std::string infilters_manager = "/InputFilters";
static const std::string outfilters_manager = "/OutputFilters";
static const std::string topLevelInputFilters_manager = "/InputFilters/TopLevel";
static const std::string gslibInputFilters_manager = "/InputFilters/Gslib";
static const std::string csvInputFilters_manager = "/InputFilters/Csv";

static const std::string grid_geom_xml_io_manager = "/GridGeometryXML";

static const std::string project_filters_manager = "/ProjectFilters";

static const std::string gridObject_manager = "/GridObject";
static const std::string gridModels_manager = "/GridObject/Model";
static const std::string vtkProps_manager = "/GridObject/vtkProps";
static const std::string QtSummaryDescription_manager = "/GridObject/QtSummaryDescription";
static const std::string downscaler_manager = "/GridObject/Downscalers";
static const std::string transformer_manager = "/GridObject/Transformer";

static const std::string mdsSpace_manager = "/mds";
static const std::string responseData_manager = "/metricData";

static const std::string geostatAlgo_manager = "/GeostatAlgo";
static const std::string qtWrapper_manager = "/QTWrapper";

static const std::string projectViews_manager = "/ProjectView";
static const std::string projects_manager = "/Project";

static const std::string geostatParamUtils_manager = "/GeostatParamUtils";
static const std::string xmlGeostatParamUtils_manager = "/GeostatParamUtils/XML";

static const std::string colorscale_manager = "/Colorscales";

static const std::string categoricalDefinition_manager = "/CategoricalDefinitions";
static const std::string colormap_manager = "/Colormaps";
static const std::string colormap_continuous_manager = "/Colormaps/Continuous";
static const std::string colormap_categorical_manager = "/Colormaps/Categorical";

static const std::string qitem_model_manager = "/QItemModels";
static const std::string qitem_grid_filter_model_manager = "/QItemModels/GridFilters";
static const std::string qitem_root_model_manager = "/QItemModels/RootModel";
static const std::string qitem_project_model_manager = "/QItemModels/ProjectModel";

static const std::string object_viewer_manager= "/ObjectParamViewers";
static const std::string grid_viewer_manager = "/ObjectParamViewers/GridParamViewers";
static const std::string prop_viewer_manager = "/ObjectParamViewers/PropParamViewers";
static const std::string group_viewer_manager = "/ObjectParamViewers/GroupParamViewers";
static const std::string region_viewer_manager = "/ObjectParamViewers/RegionParamViewers";

static const std::string distributions_manager = "/Distributions";
static const std::string continuous_distributions_manager = "/Distributions/Continuous";
static const std::string categorical_distributions_manager = "/Distributions/Categorical";

static const std::string contextMenus_manager = "/ContextMenus";
static const std::string eda_charts_manager = "/EdaCharts";

static const std::string gridFilter_manager = "/GridFilters";


//static const std::string metricProperty_manager = "MetricProperties";

// } 

#endif 
