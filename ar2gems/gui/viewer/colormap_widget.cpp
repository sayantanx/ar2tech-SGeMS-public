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
 * colormap_widget.cpp
 *
 *  Created on: May 13, 2011
 *      Author: aboucher
 */

#include "colormap_widget.h"

#include <QPainter>

Qcolormap_bitmap::Qcolormap_bitmap( QWidget * parent, const char* name)
  : QWidget( parent), cmap_( 0 ) {
  if (name)
    setObjectName(name);
  if( parent )
    setGeometry( parent->geometry() );
  setAttribute(Qt::WA_NoBackground);
  this->setFixedHeight(20);
//  this->setFixedWidth(150);

}

void Qcolormap_bitmap::draw_color_scale(const Colormap * cmap)
{
  
  cmap_ = dynamic_cast<const Colormap_continuous*>(cmap);
  update();
}


/* This function paints the colorscale on the widget.
 * It paints a sequence of rectangles, each colored with a color from the
 * colormap. The final colormap should cover the whole length of the widget,
 * so we need to determine the width of each rectangle (as it depends on
 * the number of colors to be painted, and the size of the widget). The
 * "difficulty" comes from the fact that the widths of the rectangles
 * has to be an integer.
 */

void Qcolormap_bitmap::paintEvent(QPaintEvent *) {
  QPainter painter(this);

  QPainter widget_painter;

  widget_painter.begin( this );
  widget_painter.setPen( Qt::black );

  if( !cmap_) {
    // paint the whole widget in gray
    widget_painter.fillRect( 0,0,this->width(), this->height(),
			     Qt::gray );

    widget_painter.end();

    return;
  }

  // We don't want to paint more than 80 colors. Why 80?... why not?
  int nb_colors_to_paint = std::min( 80, cmap_->color_scale()->colors_count() );


  // Find the size of the color rectangles (called colorbox_width)
  int* colorbox_width = new int[nb_colors_to_paint];
  int base_width = this->width() / nb_colors_to_paint;
  int remaining_width = this->width() % nb_colors_to_paint;

  // distribute the remaining_width among the rectangles so that the sum
  // of all rectangle width is equal to the widget width
  for( int l=0; l < nb_colors_to_paint ; l++ )
    colorbox_width[l]=base_width;

  while(remaining_width > 0) {

    int skip = nb_colors_to_paint / remaining_width;

    for( int l = 0 ; l < nb_colors_to_paint; l += skip ) {

      colorbox_width[l]++;

      remaining_width--;

    }
  }

  // We can now draw the colormap
  float min = cmap_->lower_bound();
  float max = cmap_->upper_bound();
  float skip = cmap_->color_scale()->colors_count() / nb_colors_to_paint;
//  float skip = (max-min) / float(nb_colors_to_paint);
  const int y_cmap_start = 0;

  int widget_height = this->height();
  int already_painted_width = 0;
  int alpha = static_cast<int>(255 * cmap_->get_alpha());
  for( int k = 0; k < nb_colors_to_paint ; k++ ) {
    float r,g,b;
    cmap_->color_scale()->color(int(k*skip), r,g,b);
//    cmap->color(min+float(k)*skip, r,g,b);
    QBrush brush( QColor( int(255*r), int(255*g), int(255*b), alpha ) );
    widget_painter.fillRect( already_painted_width, y_cmap_start,
			     colorbox_width[k], widget_height,
			     brush );
    already_painted_width += colorbox_width[k];
  }

  // draw tickmarks:
  widget_painter.fillRect( 0,0,this->width(), y_cmap_start,
			   Qt::gray );
  const int main_ticks = 4;
  const int tick_length = 5;
  const int intermediate_ticks = 4;
  const int intermediate_tick_length = 2;
  const int tick_y_end = y_cmap_start;
  const int spacing = 4;
  const int text_offset = 1;

  QFont painter_font( widget_painter.font() );
  painter_font.setPointSize( 7 );
  widget_painter.setFont( painter_font );

  int intertick_distance =
    int( (this->width() -2 ) / ((intermediate_ticks+1)*(main_ticks+1) ) );
  if(intertick_distance == 0) return;
  int actual_nb_ticks = this->width() / intertick_distance;

  QString tickmark_value;

  widget_painter.drawLine( 0, tick_y_end,
			   width(), tick_y_end );

  const int max_text_length = 6;

  for(int i=0; i<= actual_nb_ticks; i++) {
    int tick_x_pos = 1 + i*intertick_distance;

    int tick_y_begin = y_cmap_start - intermediate_tick_length;
    if( (i % (intermediate_ticks+1)) == 0 )
      tick_y_begin = y_cmap_start - tick_length;

    widget_painter.drawLine( tick_x_pos, tick_y_begin,
			     tick_x_pos, tick_y_end );

    if( (i % (intermediate_ticks+1)) == 0 ) {
      double val = min + i*(max - min)/ static_cast<double>(actual_nb_ticks) ;
      tickmark_value.setNum(val, 'g', 2);

      int max_char = std::min( max_text_length, (int) tickmark_value.length() );

      int actual_text_offset = text_offset;
      if( i != 0 )
	actual_text_offset = text_offset + 6;
      widget_painter.drawText(tick_x_pos-actual_text_offset, tick_y_begin - spacing,
			      tickmark_value);
    }
  }

  widget_painter.drawLine( width() - 2, y_cmap_start - tick_length,
			   width() - 2, tick_y_end );

  widget_painter.end();
  delete [] colorbox_width;
}

/*
void Qcolormap_bitmap::resizeEvent( QResizeEvent* event ) {
  //buffer_.resize( event->size() );
  //buffer_ = buffer_.scaled(event->size());
  //  buffer_.fill( Qt::gray );

  // no drawing needed; paintEvent() invoked immediately after
  //draw_color_scale( cmap_ );
}
*/






QTransparency_colormap_bitmap::
QTransparency_colormap_bitmap( QWidget * parent, const char * name )
: Qcolormap_bitmap( parent, name ) {

}


void QTransparency_colormap_bitmap::
draw_color_scale( const Colormap_continuous* cmap ) {
  cmap_ = cmap;
  if( !transparency_string_.isEmpty() )
    update();
  else
    Qcolormap_bitmap::update();
}

void QTransparency_colormap_bitmap::
paint_transparent_intervals( const QString& str ) {

  transparency_string_ = str;
  update();

}

void QTransparency_colormap_bitmap::paintEvent(QPaintEvent *)
{
  QPainter widget_painter;

  widget_painter.begin( this );
  widget_painter.setPen( Qt::black );

  if( !cmap_ ) {
    // paint the whole widget in gray
    widget_painter.fillRect( 0,0,this->width(), this->height(),
			     Qt::gray );

    widget_painter.end();
    return;
  }


  Qcolormap_bitmap::paintEvent(NULL);


  QBrush brush( Qt::lightGray );

  // We can now draw the colormap
  float min = cmap_->lower_bound();
  float max = cmap_->upper_bound();
  const int y_cmap_start = 20;

  int widget_height = this->height();
  int widget_width = this->width();

  QStringList intervals_str = transparency_string_.split(";");
  QStringList::Iterator it = intervals_str.begin();
  for ( ; it != intervals_str.end(); ++it ) {
    QStringList interval_str = (*it).split( ",");
    if( interval_str.size() != 2 ) continue;

    QByteArray tmp1 = interval_str[0].toLatin1();
    QByteArray tmp2 = interval_str[1].toLatin1();

    float low = std::max( tmp1.trimmed().toFloat(), min );
    float high= std::min( tmp2.trimmed().toFloat(), max );

    int a= (low  - min) / ( max - min ) * widget_width;
    int b= (high - min) / ( max - min ) * widget_width;

// Temporary commenting.  Crashes on windows

 //   widget_painter.fillRect( a, y_cmap_start,
	//	                  	     b-a, widget_height,
	//		                       brush );
  }
  widget_painter.end();

}

