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

#ifndef _GSTLAPPLI_GUI_APPLI_SCATTERPLOT_CONTROL_PANEL_H__
#define _GSTLAPPLI_GUI_APPLI_SCATTERPLOT_CONTROL_PANEL_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/ui_scatterplot_control_panel_base.h>

class GsTL_project;
class GsTLGridProperty;
class Grid_region;
class GridSelector;
class SinglePropertySelector;
 
class GUI_DECL Scatterplot_control_panel : public QWidget, public Ui::Scatterplot_control_panel_base {

  Q_OBJECT

public:
  Scatterplot_control_panel( GsTL_project* proj,
                             QWidget* parent = 0, const char* name = 0 );
  ~Scatterplot_control_panel();
  void init( GsTL_project* project ); 
  virtual QSize sizeHint() const { return QSize( 190, 670 ); }

  void show_lsfit_coeffs( float slope, float intercp );
  bool ls_fit_shown() const;
  void enable_lsfit( bool on );

public slots:
  void set_var1_clipping_values( float low, float high );
  void set_var2_clipping_values( float low, float high );
  
protected slots:
  void forward_var1_changed( const QString& );
  void forward_var2_changed( const QString& );
  void forward_var1_low_clip_changed();
  void forward_var1_high_clip_changed();
  void forward_var2_low_clip_changed();
  void forward_var2_high_clip_changed();
  
signals:
//  void var1_changed( const GsTLGridProperty* );
//  void var2_changed( const GsTLGridProperty* );
  void var1_changed(  GsTLGridProperty*,  const Grid_region* );
  void var2_changed(  GsTLGridProperty*,  const Grid_region* );

  void var1_low_clip_changed( float );
  void var2_low_clip_changed( float );
  void var1_high_clip_changed( float );
  void var2_high_clip_changed( float );

  void reset_var1_clipping_values_clicked();
  void reset_var2_clipping_values_clicked();

  void show_ls_fit( bool );

protected:  
  GridSelector* object_selector_;
  SinglePropertySelector* prop_selector_var1_;
  SinglePropertySelector* prop_selector_var2_;

protected:
  GsTLGridProperty* get_property( const SinglePropertySelector* );
  Grid_region* get_region( const GridSelector* );

};

#endif

