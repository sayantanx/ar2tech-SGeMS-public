#include <GsTLAppli/grid/grid_model/structured_grid.h>

#include <vtkPoints.h>

Named_interface* Structured_grid::create_new_interface(std::string& name){
  return new Structured_grid(name, 1,1,1);
}


Structured_grid::Structured_grid()  : Cartesian_grid()
{
    sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
}  

Structured_grid::Structured_grid(std::string name) : Cartesian_grid(name)
{
    sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
}

Structured_grid::Structured_grid(std::string name, int nx, int ny, int nz) :
  Cartesian_grid(name, nx, ny, nz)
{
  sgrid_geom_ = vtkSmartPointer<vtkStructuredGrid>::New();
}


Structured_grid::~Structured_grid()
{
}


void Structured_grid::set_structured_points( std::vector<GsTLCoordVector>& corner_points){

  vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
  pts->SetDataTypeToDouble ();
  pts->SetNumberOfPoints( (this->nx()+1) * (this->ny()+1) * (this->nz()+1) );

  std::vector<GsTLCoordVector>::const_iterator it = corner_points.begin();
  for(int i=0 ; it!= corner_points.end(); ++it,++i) {
    pts->SetPoint(i,it->x(),it->y(),it->z());
  }
  
  sgrid_geom_->SetDimensions(this->nx()+1, this->ny()+1, this->nz()+1);
  sgrid_geom_->SetPoints(pts);
  double origin_coord[3];
  sgrid_geom_->GetPoint(0,origin_coord);
  origin_ = GsTLPoint(origin_coord[0],origin_coord[1],origin_coord[2]); 
}
