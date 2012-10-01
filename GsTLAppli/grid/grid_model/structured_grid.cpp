#include <GsTLAppli/grid/grid_model/structured_grid.h>

#include <vtkPoints.h>
#include <vtkCell.h>
#include <vtkGenericCell.h>

Named_interface* Structured_grid::create_new_interface(std::string& name){
  return new Structured_grid(name, 1,1,1);
}


Structured_grid::Structured_grid()  : Cartesian_grid()
{
  sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
  cell_centers_filter_ = vtkSmartPointer<vtkCellCenters>::New();

  //cell_centers_filter_->SetInputConnection(sgrid_geom_->GetProducerPort());
  cell_centers_filter_->SetInputData(sgrid_geom_);

//  cellCentersFilter->SetInputData(imageData);

  
}  

Structured_grid::Structured_grid(std::string name) : Cartesian_grid(name)
{
  sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
  cell_centers_filter_ = vtkSmartPointer<vtkCellCenters>::New();
  cell_centers_filter_->SetInputData(sgrid_geom_);
}

Structured_grid::Structured_grid(std::string name, int nx, int ny, int nz) :
  Cartesian_grid(name, nx, ny, nz)
{
  sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
  cell_centers_filter_ = vtkSmartPointer<vtkCellCenters>::New();
  cell_centers_filter_->SetInputData(sgrid_geom_);
}


Structured_grid::~Structured_grid()
{
}


void Structured_grid::set_structured_points( std::vector<GsTLPoint>& corner_points){

  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
  pts->SetDataTypeToDouble ();
  pts->SetNumberOfPoints( (this->nx()+1) * (this->ny()+1) * (this->nz()+1) );

  std::vector<GsTLPoint>::const_iterator it = corner_points.begin();
  for(int i=0 ; it!= corner_points.end(); ++it,++i) {
    pts->SetPoint(i,it->x(),it->y(),it->z());
  }
  
  sgrid_geom_->SetDimensions(this->nx()+1, this->ny()+1, this->nz()+1);
  sgrid_geom_->SetPoints(pts);

  cell_centers_filter_->Update();

  GsTLCoordVector dims( 1.0/this->nx(), 1.0/this->ny(), 1.0/this->nz());
  geometry_->set_cell_dims( dims );

  // The uvw coordinates should start at (0,0,0) and finish at (1,1,1)
  //double origin_coord[3];
  //sgrid_geom_->GetPoint(0,origin_coord);
  //origin_ = GsTLPoint(origin_coord[0],origin_coord[1],origin_coord[2]); 
  origin_ = GsTLPoint(0,0,0); 
  coord_mapper_ = new Structured_grid_coord_mapper(this);
  
}


GsTLPoint Structured_grid::get_corner_point_locations(int id) const {
  double d_coord[3];
  sgrid_geom_->GetPoint(id,d_coord);
  return GsTLPoint(d_coord[0],d_coord[1],d_coord[2]); 

}


Geostat_grid::location_type Structured_grid::xyz_location( int node_id ) const 
{
  
  appli_assert( node_id >= 0 && node_id <(signed int)( sgrid_geom_->GetNumberOfCells()) ); 

  double loc[3];
  cell_centers_filter_->GetOutput()->GetPoint(node_id,loc);
  return Geostat_grid::location_type(loc[0],loc[1],loc[2]);

} 



/*
--------------------------
*/


Structured_grid_coord_mapper::Structured_grid_coord_mapper(Structured_grid* sgrid)
  :Coordinate_mapper(sgrid), sgrid_(sgrid)
{ 
  grid_cell_dimension_.x() = sgrid_->geometry()->dim(0);
  grid_cell_dimension_.y() = sgrid_->geometry()->dim(1);
  grid_cell_dimension_.z() = sgrid_->geometry()->dim(2);
}

GsTLPoint Structured_grid_coord_mapper::uvw_coords(GsTLPoint xyz)  {
  vtkSmartPointer<vtkStructuredGrid> geom = sgrid_->get_structured_geometry();


  // Need to get the ijk of the cell and add the parametric coordinate once normalized

  vtkSmartPointer<vtkGenericCell> gcell = vtkSmartPointer<vtkGenericCell>::New();
  
  double xyzcoord[3];
  xyzcoord[0] = xyz.x();
  xyzcoord[0] = xyz.y();
  xyzcoord[0] = xyz.z();

  double pcoord[3];
  double weights[8];
  int subid;

  int cellid = geom->FindCell(xyzcoord,0,gcell,-1,1e-4,subid,pcoord,weights);

  if(cellid <0 ) return GsTLPoint(-1,-1,-1);
  //geom->GetCell(cellid, gcell);  //may not be necessary if gcell is already stored in the FindCell
  int i,j,k;
  sgrid_->cursor()->coords(cellid,i,j,k);

  double u = (static_cast<double>(i) + pcoord[0])/grid_cell_dimension_.x();
  double v = (static_cast<double>(j) + pcoord[1])/grid_cell_dimension_.y();
  double w = (static_cast<double>(k) + pcoord[2])/grid_cell_dimension_.z();

  return GsTLPoint(u,v,w );

}


GsTLPoint Structured_grid_coord_mapper::xyz_coords(GsTLPoint uvw)  {
  vtkSmartPointer<vtkStructuredGrid> geom = sgrid_->get_structured_geometry();

  int i,j,k;
  int node_id = sgrid_->closest_node(uvw);
  sgrid_->cursor()->coords(node_id,i,j,k);

  //The coordinates wihtin the cell
  double pcoords[3];
  pcoords[0] = uvw.x() - static_cast<double>(i)/grid_cell_dimension_.x();
  pcoords[1] = uvw.y() - static_cast<double>(j)/grid_cell_dimension_.y();
  pcoords[2] = uvw.z() - static_cast<double>(k)/grid_cell_dimension_.z();

  double xyz[3];
  double* weights;
  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
  geom->GetCell(node_id,cell);
  int subid=0;
  cell->EvaluateLocation(subid,pcoords,xyz,weights);
  return GsTLPoint(xyz[0],xyz[1],xyz[2]);


}