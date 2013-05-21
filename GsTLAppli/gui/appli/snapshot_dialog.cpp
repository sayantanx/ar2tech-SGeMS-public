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

#include <GsTLAppli/gui/appli/snapshot_dialog.h>
#include <GsTLAppli/qtplugins/filechooser.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <vtkSmartPointer.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkTIFFWriter.h>

#include <qcombobox.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qimage.h>
#include <qsound.h>
#include <qapplication.h>
#include <QHBoxLayout>

#include <fstream>

using namespace String_Op;

//====================================================

/* Save an image to ppm format
 * x,y are the dimensions of the image. The pixels are supposed to 
 * be stored row-wise, starting from the lower left corner.
 */
static void write_to_ppm_format( std::ofstream& stream,
				 unsigned char* image, int x, int y ) {
  stream << "P6" << std::endl
	 << x << " " << y << std::endl
	 << "255" << std::endl;

  
  for( int j=y-1; j>=0; j-- )
    for( int i=0; i<x; i++ )
      stream << image[3*(j*x+i)] << image[3*(j*x+i)+1] << image[3*(j*x+i)+2];

}

//======================================



Snapshot_dialog::Snapshot_dialog( vtkRenderWindow* render_area,
                                  QWidget* parent, const char* name )
  : QDialog( parent ),
    render_area_( render_area ) {

  setupUi(this);

  QHBoxLayout * h = new QHBoxLayout();
  file_chooser_box_->setLayout(h);
  file_chooser_ = new FileChooser( file_chooser_box_, "file_chooser" );
  file_chooser_->setMode( FileChooser::Any );
  h->addWidget(file_chooser_);

  filter_chooser_->addItem( "PNG (*.png)" );
  filter_chooser_->addItem( "PostScript (*.ps)" );
  filter_chooser_->addItem( "TIFF (*.tiff)" );
  filter_chooser_->addItem( "JPEG (*.jpeg)" );

  QObject::connect( snapshot_button_, SIGNAL( clicked() ),
                    this, SLOT( take_snapshot() ) );
  QObject::connect( close_button_, SIGNAL( clicked() ),
                    this, SLOT( accept() ) );

}


/*
void Snapshot_dialog::take_snapshot() {
  if( file_chooser_->fileName().isEmpty() ) return;

  // Create an offscreen renderer which copies the settings from the Viewer
  SoOffscreenRenderer *snapshot = 
    new SoOffscreenRenderer( render_area_->getViewportRegion() );
  //snapshot->setTransparencyType(SoGLRenderAction::BLEND);
  snapshot->setBackgroundColor( render_area_->getBackgroundColor() );
  snapshot->setComponents( SoOffscreenRenderer::RGB );

  // Create the buffer in snapshot
  if ( !snapshot->render( render_area_->getSceneManager()->getSceneGraph()) ) {
    appli_warning("Snapshot: Failed to render the scene.");
    return;
  }
  
  QString filename = file_chooser_->fileName();
  QString format = filter_chooser_->currentText();
  
  // Write the buffer to file
    
  if( format.contains( "postscript", false ) ) {
    FILE *fp = fopen( filename, "w");
    if( !fp ) {
      GsTLcerr << "Can't create file " << filename << "\n" << gstlIO::end; 
      return;
    }
    snapshot->writeToPostScript(fp);
    fclose(fp);
  }


  if( format.contains( "PPM", false ) || format.contains( "PNG", false ) ||
      format.contains( "BMP", false ) ) {
//    fclose(fp);
//    std::ofstream ofile( filename.latin1() );

    format = format.section( " (", 0, 0 );
    SbVec2s size = 
      render_area_->getGLRenderAction()->getViewportRegion().getViewportSizePixels();
    int x = size[0];
    int y = size[1];
 
//    write_to_ppm_format( ofile, snapshot->getBuffer(), x, y );
 
    QImage qimage( x, y, 32 );
    uchar* image = snapshot->getBuffer();
    
    //for( int j=y-1 ; j>=0 ; j-- ) {
    for( int i = 0 ; i < x ; i++ ) {
      for( int j = 0 ; j < y ; j++ ) {
        int r,g,b;
        r= image[3*(j*x+i)];
        g= image[3*(j*x+i)+1];
        b= image[3*(j*x+i)+2];
        uint *p = (uint *)qimage.scanLine(y-(j+1)) + i;
        *p = qRgb(r,g,b);   
      }
    }
    qimage.save( filename, format );

  }

  QSound::play( "shutter.wav" );

  delete snapshot;

}
*/


void Snapshot_dialog::take_snapshot() {
  if( file_chooser_->fileName().isEmpty() ) return;
  
  QString filename = file_chooser_->fileName();
  QString format = filter_chooser_->currentText();

   //TL modified
  if (format.contains("PNG") && (!filename.endsWith(".png",Qt::CaseInsensitive)))
	  filename += ".png";
  if (format.contains("JPEG") &&
      ( (!filename.endsWith(".jpeg",Qt::CaseInsensitive)) || (!filename.endsWith(".jpg",Qt::CaseInsensitive)) ) )
	  filename += ".jpeg";
  if (format.contains("PostScript") && (!filename.endsWith(".ps",Qt::CaseInsensitive)))
	  filename += ".ps";
  if (format.contains("TIFF") && (!filename.endsWith(".tiff",Qt::CaseInsensitive)))
	  filename += ".tiff";

  std::ofstream of("a.txt");
  of << qstring2string(format) << "," << qstring2string(filename) << std::endl;
  of.close();
  take_snapshot( filename, format );

  QSound::play( "shutter.wav" );
}




void Snapshot_dialog::take_snapshot( const QString& filename, const QString& format ) {
  if( filename.isEmpty() ) return;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  vtkSmartPointer<vtkWindowToImageFilter> w2i = vtkSmartPointer<vtkWindowToImageFilter>::New();
  w2i->SetInput(render_area_);
  w2i->Modified();
  
  QByteArray ba_filename = filename.toLatin1();
  // Write the buffer to file
    
  if( format.contains( "postscript", Qt::CaseInsensitive ) || format.contains( "PS", Qt::CaseInsensitive ) ) {
 /*   FILE *fp = fopen( qstring2string(filename).c_str(), "w");
    if( !fp ) {
      GsTLcerr << "Can't create file " << qstring2string(filename) << "\n" << gstlIO::end; 
      return;
    }*/
    vtkSmartPointer<vtkPostScriptWriter> ps = vtkSmartPointer<vtkPostScriptWriter>::New();
    ps->SetInputConnection(w2i->GetOutputPort());
    ps->SetFileName(ba_filename.data());
    ps->Write();

  }


  if(  format.contains( "PNG", Qt::CaseInsensitive ) ) {
    
    vtkSmartPointer<vtkPNGWriter> png = vtkSmartPointer<vtkPNGWriter>::New();
    png->SetInputConnection(w2i->GetOutputPort());
    png->SetFileName(ba_filename.data());
    png->Write();
  }

  if( format.contains( "JPEG", Qt::CaseInsensitive ) || format.contains( "JPG", Qt::CaseInsensitive ) ) {

    vtkSmartPointer<vtkJPEGWriter> jpeg = vtkSmartPointer<vtkJPEGWriter>::New();
    jpeg->SetInputConnection(w2i->GetOutputPort());
    jpeg->SetFileName(ba_filename.data());
    jpeg->Write();
  }

  if( format.contains( "TIFF", Qt::CaseInsensitive ) ){

    vtkSmartPointer<vtkTIFFWriter> tiff = vtkSmartPointer<vtkTIFFWriter>::New();
    tiff->SetInputConnection(w2i->GetOutputPort());
    tiff->SetFileName(ba_filename.data());
    tiff->Write();
  }

  QApplication::restoreOverrideCursor();
}
