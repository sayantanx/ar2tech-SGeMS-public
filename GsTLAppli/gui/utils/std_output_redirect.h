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



#ifndef __GSTLAPPLI_MAIN_STD_OUTPUT_REDIRECT_H__
#define __GSTLAPPLI_MAIN_STD_OUTPUT_REDIRECT_H__

#include <iostream>

class QTextEdit;


class GsTLbuf: public std::streambuf {
 private:
  QTextEdit* text_edit_;
  void put_buffer(void);
  void put_char(int);

 protected:
  int	overflow(int);
  int	sync();

 public:
  GsTLbuf( QTextEdit* text_edit, int bsize );
  ~GsTLbuf();
};

#endif
