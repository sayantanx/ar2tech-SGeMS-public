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



/*
 * colormap_widget.h
 *
 *  Created on: May 13, 2011
 *      Author: aboucher
 */

#ifndef COLORMAP_WIDGET_H_
#define COLORMAP_WIDGET_H_

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/colormap.h>

#include <QWidget>
#include <QPaintEvent>

/** Qcolormap_bitmap is a simple widget which shows the colors of a colormap.
 */
class GUI_DECL Qcolormap_bitmap : public QWidget {

  Q_OBJECT

 public:
  Qcolormap_bitmap( QWidget * parent = 0, const char * name = 0 );

 public slots:
   /** This function paints a colorscale (specified by colormap \c cmap) on the widget.
   * It paints a sequence of rectangles, each colored with a color from the
   * colormap.
   */
  //virtual void draw_color_scale( const Colormap_continuous* cmap );
  virtual void draw_color_scale( const Colormap* cmap );

 protected:
  virtual void paintEvent( QPaintEvent * );
  //virtual void resizeEvent( QResizeEvent * );

 protected:
  const Colormap_continuous* cmap_;
};


/** QTransparency_colormap_bitmap is a simple widget which shows the colors of
 * a colormap and transparent intervals.
 */
class GUI_DECL QTransparency_colormap_bitmap : public Qcolormap_bitmap {

  Q_OBJECT

 public:
  virtual void draw_color_scale( const Colormap_continuous* cmap );
  QTransparency_colormap_bitmap( QWidget * parent = 0, const char * name = 0 );

 public slots:
  void paint_transparent_intervals( const QString& str );

 protected:
   QString transparency_string_;
   virtual void paintEvent( QPaintEvent * );
};




#endif /* COLORMAP_WIDGET_H_ */
