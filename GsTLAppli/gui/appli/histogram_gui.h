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
** This file is part of the "gui" module of the Geostatistical Earth
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

#ifndef __GSTLAPPI_GUI_APPLI_HISTOGRAM_GUI_H__
#define __GSTLAPPI_GUI_APPLI_HISTOGRAM_GUI_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/appli/data_analysis_gui.h>
#include <GsTLAppli/gui/appli/histogram_control_panel.h>


#include <qstring.h>

class GsTL_project;
class PropertySelector;
class Histogram;
class Grid_continuous_property;
class QPainter;


class GUI_DECL Histogram_gui : public Data_analysis_gui {

    Q_OBJECT

public:
  Histogram_gui( GsTL_project* project,
                 QWidget* parent, const char* name = 0 );
  virtual ~Histogram_gui();

public slots:
  void update_low_clip( float );
  void update_high_clip( float );
  void reset_clipping_values();
  void update_bins( int count );
  void get_data_from(  Grid_continuous_property*, const Grid_region* = NULL );
  virtual void set_x_axis_logscale( bool on );
  void changeCurve (const QString &);

protected:
  void clean();
  void update_all();
  void refresh_stats();
  virtual void paint_stats( QPainter& );

  //TL modified
  // !!! Temporarily removed !!!
  void savePostScript(SimplePs &){}
  void build_stats();




protected:

  Histogram_control_panel* control_panel_;

  Histogram* histogram_; 

  // TL modified
  QwtPlotCurve * _cdfCurve;
};

#endif

