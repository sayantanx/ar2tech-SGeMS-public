/**********************************************************************
** Author: Alexandre Boucher
** Copyright (C) 2010 Advanced Resources and Technology, LLC
** All rights reserved.
**
** This file is part of the "gui" module of the Stanford Geostatistical Earth
** Modeling Software (SGeMS)
**
**
** This file may be distributed and/or modified under the terms of the
** GNU Library General Public License  as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact A. Boucher at Advanced Resources and Risk Technology. LLC www.ar2tech.com
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#ifndef __GSTLAPPLI_GRID_DOWNSCALER_H__
#define __GSTLAPPLI_GRID_DOWNSCALER_H__

//#include <../utils/named_interface.h>
#include <GsTLAppli/utils/named_interface.h>


class Geostat_grid;

class Grid_downscaler : public Named_interface
{
public:
  Grid_downscaler(void){}
  virtual ~Grid_downscaler(void){}

  virtual Geostat_grid* downscale(const Geostat_grid* src_grid, std::string name, int nx, int ny, int nz) = 0;

};

class CGrid_downscaler : public Grid_downscaler
{
public:

  static Named_interface* new_interface(std::string& str) {return new CGrid_downscaler;}

  CGrid_downscaler(void);
  virtual ~CGrid_downscaler(void){}

  virtual Geostat_grid* downscale(const Geostat_grid* src_grid, std::string name, int nx, int ny, int nz);

};



class MGrid_downscaler : public Grid_downscaler
{
public:

  static Named_interface* new_interface(std::string& str) {return new MGrid_downscaler;}

  MGrid_downscaler(void);
  virtual ~MGrid_downscaler(void){}

  virtual Geostat_grid* downscale(const Geostat_grid* src_grid, std::string name, int nx, int ny, int nz);

};


#endif
