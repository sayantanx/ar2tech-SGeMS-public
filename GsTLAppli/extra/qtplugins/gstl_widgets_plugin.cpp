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
** This file is part of the "extra" module of the Geostatistical Earth
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
** modifed Alexandre Boucher
**********************************************************************/



#include <GsTLAppli/extra/qtplugins/gstl_widgets_plugin.h>



GsTL_Widgets_plugin::GsTL_Widgets_plugin(QObject * o) : QObject(o)

{
  
  widgets.append(new GridSelectorPlugin(this));
  widgets.append(new PropertySelectorPlugin(this));
  widgets.append(new SinglePropertySelectorPlugin(this));
  widgets.append(new MultiPropertySelectorPlugin(this));
  widgets.append(new OrderedPropertySelectorPlugin(this));
  widgets.append(new GsTLTablePlugin(this));  
  widgets.append(new FileChooserPlugin(this));
  widgets.append(new VariogramInputPlugin(this));
  widgets.append(new KrigingTypeSelectorPlugin(this));
  widgets.append(new GsTLButtonGroupPlugin(this));
  widgets.append(new GsTLGroupBoxPlugin(this));
  widgets.append(new TrendComponentsPlugin(this));
  widgets.append(new EllipsoidInputPlugin(this));
  widgets.append(new UpperTailCdfInputPlugin(this));
  widgets.append(new LowerTailCdfInputPlugin(this));
  widgets.append(new NonParamCdfInputPlugin(this));
  widgets.append(new CloneGroupBoxPlugin(this));
  widgets.append(new SingleRegionSelectorPlugin(this));
  widgets.append(new GridRegionSelectorPlugin(this));
  widgets.append(new MultiRegionSelectorPlugin(this));
  widgets.append(new OctanSearchPlugin(this));
  widgets.append(new AdvancedSearchPlugin(this));
  widgets.append(new PropertyRegionSelectorPlugin(this));
  widgets.append(new SingleCategoricalPropertySelectorPlugin(this));
  widgets.append(new MultiCategoricalPropertySelectorPlugin(this));
  widgets.append(new OrderedCategoricalPropertySelectorPlugin(this));
  widgets.append(new CategoricalDefinitionSelectorPlugin(this));
  widgets.append(new MultiCategoricalDefinitionSelectorPlugin(this));
  widgets.append(new HardDataKrigingPropertySelectorPlugin(this));
  widgets.append(new Drift_componentsPlugin(this));
  widgets.append(new CategoricalDefinitionViewerPlugin(this));
  widgets.append(new SingleWeightPropertySelectorPlugin(this));

  
}



QList<QDesignerCustomWidgetInterface*> GsTL_Widgets_plugin::customWidgets() const

{
  return widgets;
}

Q_EXPORT_PLUGIN2( GsTLwidgets, GsTL_Widgets_plugin )
