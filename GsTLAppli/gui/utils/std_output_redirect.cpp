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



#include <GsTLAppli/gui/utils/std_output_redirect.h>

#include <qtextedit.h>
#include <QChar>


GsTLbuf::GsTLbuf( QTextEdit* text_edit, int bsize)
  : std::streambuf(), text_edit_( text_edit ) {
  if (bsize) {
    char *ptr = new char[bsize];
    setp(ptr, ptr + bsize);
  }
  else
    setp(0, 0);
  
  setg(0, 0, 0);
}


GsTLbuf::~GsTLbuf() {
  sync();
  delete[] pbase();
}



int GsTLbuf::overflow(int c) {
  put_buffer();
  
  if (c != 0)
    if (pbase() == epptr())
      put_char(c);
    else
      sputc(c);
        
  return 0;
}



int GsTLbuf::sync() {
  put_buffer();
  return 0;
}

void GsTLbuf::put_char(int chr) {
  char c = chr;
  QChar qc( c );
  std::cerr << "put_char(" << c <<")" << std::endl;
  text_edit_->append( QString( qc ) );
}


void GsTLbuf::put_buffer() {
  if( pbase() != pptr() ) {
    int len = (pptr() - pbase());
    char* buffer = new char[len + 1];
    
    strncpy(buffer, pbase(), len);
    buffer[len] = 0;
    std::cerr << "put_buffer: " << buffer << std::endl;
    text_edit_->append( buffer );
        
    setp(pbase(), epptr());
    delete [] buffer;
    
  }
}

