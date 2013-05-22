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

#include <GsTLAppli/grid/grid_model/grid_downscaler.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <GsTLAppli/appli/manager_repository.h>





CGrid_downscaler::CGrid_downscaler(void)
{
  name_ = "CGridDownscaler";
}


MGrid_downscaler::MGrid_downscaler(void)
{
  name_ = "MGridDownscaler";
}


Geostat_grid* CGrid_downscaler::downscale(const Geostat_grid* src_grid, std::string name, int factor_x, int factor_y, int factor_z){
   const Cartesian_grid* grid = dynamic_cast<const Cartesian_grid*>(src_grid);
   if(grid == 0) return static_cast<Geostat_grid*>(0);

   const RGrid_geometry* geom =  grid->geometry();

   
   GsTLCoordVector up_cell_dims = grid->cell_dimensions();

   GsTLCoordVector down_cell_dims(up_cell_dims.x()/factor_x, 
                                  up_cell_dims.y()/factor_y, 
                                  up_cell_dims.z()/factor_z);

   GsTLPoint down_origin( grid->origin().x() - up_cell_dims.x()/2.0 + down_cell_dims.x()/2.0,
                          grid->origin().y() - up_cell_dims.y()/2.0 + down_cell_dims.y()/2.0,
                          grid->origin().z() - up_cell_dims.z()/2.0 + down_cell_dims.z()/2.0 );


  // ask manager to get a new grid and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return static_cast<Geostat_grid*>(0);
  }

  ni = Root::instance()->new_interface( "cgrid://"+name,
                                        gridModels_manager + "/" + name );
  Cartesian_grid* down_grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  appli_assert( down_grid != 0 );

  down_grid->set_dimensions( geom->dim(0)*factor_x,
                             geom->dim(1)*factor_y,
                             geom->dim(2)*factor_z,
                             down_cell_dims.x(),
                             down_cell_dims.y(),
                             down_cell_dims.z() );

  down_grid->origin( down_origin );
  down_grid->set_rotation_z( grid->rotation_z() );

  return down_grid;

}


Geostat_grid* MGrid_downscaler::downscale(const Geostat_grid* src_grid, std::string name, int factor_x, int factor_y, int factor_z){
   const Reduced_grid* grid = dynamic_cast<const Reduced_grid*>(src_grid);
   if(grid == 0) return static_cast<Geostat_grid*>(0);

   const RGrid_geometry* geom =  grid->geometry();

   
   GsTLCoordVector up_cell_dims = grid->cell_dimensions();

   GsTLCoordVector down_cell_dims(up_cell_dims.x()/factor_x, 
                                  up_cell_dims.y()/factor_y, 
                                  up_cell_dims.z()/factor_z);

   GsTLPoint down_origin( grid->origin().x() - up_cell_dims.x()/2.0 + down_cell_dims.x()/2.0,
                          grid->origin().y() - up_cell_dims.y()/2.0 + down_cell_dims.y()/2.0,
                          grid->origin().z() - up_cell_dims.z()/2.0 + down_cell_dims.z()/2.0 );


   int down_nx = geom->dim(0)*factor_x;
   int down_ny = geom->dim(1)*factor_y;
   int down_nz = geom->dim(2)*factor_z;  


  // ask manager to get a new grid and initialize it
  SmartPtr<Named_interface> ni =
    Root::instance()->interface( gridModels_manager + "/" + name );

  if( ni.raw_ptr() != 0 ) {
    GsTLcerr << "object " << name << " already exists\n" << gstlIO::end;
    return static_cast<Geostat_grid*>(0);
  }

  int down_grid_size = grid->size()*factor_x*factor_y*factor_z;
  int down_full_grid_size = grid->rgrid_size()*factor_x*factor_y*factor_z;

  ni = Root::instance()->new_interface( "reduced_grid://"+name+"::"+QString("%1").arg(down_grid_size).toStdString(),
                                        gridModels_manager + "/" + name );
  Reduced_grid* down_grid = dynamic_cast<Reduced_grid*>( ni.raw_ptr() );
  appli_assert( down_grid != 0 );

  const SGrid_cursor* cursor = grid->cursor();
	std::vector<bool> mask;
	mask.reserve(down_full_grid_size);
  for(int k=0; k<down_nz; ++k) {
    int up_k = k/factor_z;
    for(int j=0; j<down_ny; ++j) {
      int up_j = j/factor_y;
      for(int i=0; i<down_nx; ++i) {
        int up_i = i/factor_x;
        int up_nodeid = cursor->node_id(up_i,up_j, up_k );
        if( up_nodeid >= 0 )
          mask.push_back(true);
        else
          mask.push_back(false);
      }
    }
  }

  down_grid->set_dimensions( down_nx, down_ny, down_nz,
                        down_cell_dims.x(), down_cell_dims.y(), down_cell_dims.z(), 
                        mask, grid->rotation_z());
	down_grid->origin( down_origin );

  return down_grid;
  
}
