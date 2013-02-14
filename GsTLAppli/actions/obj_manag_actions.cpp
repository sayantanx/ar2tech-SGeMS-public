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
** This file is part of the "actions" module of the Geostatistical Earth
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

/*
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else

#include <Python.h>
#endif
*/
#include <Python.h>

#include <GsTLAppli/actions/obj_manag_actions.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/filters/filter.h>
#include <GsTLAppli/filters/save_project_objects.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/grid_categorical_property.h>
#include <GsTLAppli/grid/grid_model/grid_downscaler.h>


#include <GsTLAppli/geostat/utilities.h>

#include <GsTL/math/math_functions.h>

// these 3 Qt files are needed by Load_project
#include <qdir.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <QByteArray>


#include <stdlib.h>
#include <stdio.h>
//#include <GsTLAppli/filters/eclipse/pprt.h>
#include <memory.h>
#include <list>

// TL modified
#include <GsTLAppli/actions/python_wrapper.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>
//#include <q3progressdialog.h>
#include <qapplication.h>
//#include <GsTLAppli/filters/gslib/gslib_filter.h>
//#include <GsTLAppli/gui/appli/oinv_project_view.h>

Named_interface* New_rgrid::create_new_interface( std::string& ) {
  return new New_rgrid();
}

Named_interface* New_cartesian_grid_action::create_new_interface( std::string& ) {
  return new New_cartesian_grid_action();
}

Named_interface* Load_object_from_file::create_new_interface( std::string& ) {
  return new Load_object_from_file();
}

Named_interface* Save_geostat_grid::create_new_interface( std::string& ) {
  return new Save_geostat_grid();
}

Named_interface* Load_project::create_new_interface( std::string& ) {
  return new Load_project();
}


// AB
Named_interface* Set_active_region::create_new_interface( std::string& ) {
  return new Set_active_region();
}

Named_interface* Set_hard_data::create_new_interface( std::string& ) {
  return new Set_hard_data();
}

// TL modified
/*
Named_interface* Load_sim::create_new_interface( std::string& ) {
  return new Load_sim();
}
*/
Named_interface* RunScript::create_new_interface( std::string& ) {
  return new RunScript();
}




// name of the python module for parsing Eclipse PRT file
//const std::string Load_sim::parser = "readprt";

//=============================================


bool
New_rgrid::init( std::string& parameters, GsTL_project* proj,
                Error_messages_handler* errors ) {
  proj_ = proj ;
  errors_ = errors;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 4 )
    return false;

  name_ = params[0];
  nx_ = String_Op::to_number<int>( params[1] );
  ny_ = String_Op::to_number<int>( params[2] );
  nz_ = String_Op::to_number<int>( params[3] );
  
  return true;
}


bool
New_rgrid::exec() {
  appli_warning( "New_rgrid::exec() NOT FULLY IMPLEMENTED" );

  std::string full_name( "/GridObject/Model/" + name_ );
  SmartPtr<Named_interface> ni = 
      Root::instance()->new_interface("RGrid://"+name_, full_name);
  
  if( ni.raw_ptr() == 0 ) {
    errors_->report( "object " + full_name + " already exists. Use a different name." );
//    appli_warning( "object " << full_name << "already exists" );
    return false;
  }
  
  proj_->new_object( name_ );
  return true;
}


//=============================================

New_cartesian_grid_action::New_cartesian_grid_action() {
  xsize_ = 1;
  ysize_ = 1; 
  zsize_ = 1;
  Ox_ = 0;
  Oy_ = 0;
  Oz_ = 0; 
  z_rotation_ = 0.;
}

bool
New_cartesian_grid_action::init( std::string& parameters, GsTL_project* proj,
                                 Error_messages_handler* errors ) {
  proj_ = proj ;
  errors_ = errors;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 4 )
    return false;

  name_ = params[0];
  nx_ = String_Op::to_number<int>( params[1] );
  ny_ = String_Op::to_number<int>( params[2] );
  nz_ = String_Op::to_number<int>( params[3] );
  
  if( params.size() >= 7 ) {
    xsize_ = String_Op::to_number<float>( params[4] );
    ysize_ = String_Op::to_number<float>( params[5] );
    zsize_ = String_Op::to_number<float>( params[6] );
  }

  if( params.size() >= 10 ) {
    Ox_ = String_Op::to_number<float>( params[7] );
    Oy_ = String_Op::to_number<float>( params[8] );
    Oz_ = String_Op::to_number<float>( params[9] );
  }

   if( params.size() == 11 ) {
     z_rotation_ = String_Op::to_number<float>( params[10] );

   }

  return true;
}


bool
New_cartesian_grid_action::exec() {

  std::string full_name( "/GridObject/Model/" + name_ );
  SmartPtr<Named_interface> ni = 
      Root::instance()->new_interface("cgrid://"+name_, full_name);
  
  if( ni.raw_ptr() == 0 ) {
    errors_->report( "Object " + full_name + " already exists. Use a different name." );
//    appli_warning( "object " << full_name << "already exists" );
    return false;
  }
  
  Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
  grid->set_dimensions( nx_, ny_, nz_,
			xsize_, ysize_, zsize_ );
  grid->origin( GsTLPoint( Ox_, Oy_, Oz_ ) );
  grid->set_rotation_z(z_rotation_);

  proj_->new_object( name_ );
  return true;
}




//=========================================
Load_object_from_file::~Load_object_from_file() {
}

bool Load_object_from_file::init( std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() != 2 )
    return false;


  file_name_ = params[0];

  if( params[1] == "All" ) {
    if( !find_filter( file_name_ ) ) {
      errors->report( "Cannot find appropriate filter to read " + file_name_ );
      return false;
    }
    return true;
  }

  SmartPtr<Named_interface> ni =
    Root::instance()->new_interface( params[1], topLevelInputFilters_manager + "/" );

  if( ni.raw_ptr() == 0 )
    return false;

  Input_filter* filter = dynamic_cast<Input_filter*>( ni.raw_ptr() );
  appli_assert( filter );

  filter_ = SmartPtr<Input_filter>(filter);

  return true;
}


bool Load_object_from_file::exec() {
  std::string filter_errors;

  Named_interface* ni = filter_->read( file_name_, &filter_errors );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>(ni);

  // Notify the project that a new object was added. To do so, we need to 
  // get the name of the new grid from the grid manager.
  if( grid )  {
	  SmartPtr<Named_interface> dir = Root::instance()->interface( gridModels_manager );
	  Manager* mng = dynamic_cast<Manager*>( dir.raw_ptr() );
	  appli_assert( mng );

	  std::string grid_name = mng->name( dynamic_cast<Named_interface*>(grid) ); 

	  proj_->new_object(grid_name);
  }
  
  if( filter_errors.empty() ) return true;

  errors_->report( filter_errors );
  return false;
}



bool Load_object_from_file::find_filter( const std::string& filename ) {
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( topLevelInputFilters_manager );
  Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() );
  appli_assert( dir );

  Manager::type_iterator begin = dir->begin();
  Manager::type_iterator end = dir->end();

  for( ; begin != end ; ++begin ) {
    SmartPtr<Named_interface> filter_ni = 
      Root::instance()->new_interface( *begin, topLevelInputFilters_manager + "/" );
    Input_filter* filter = dynamic_cast<Input_filter*>( filter_ni.raw_ptr() );  
    appli_assert( filter );
    if( filter->can_handle( filename ) ) {
      filter_ = SmartPtr<Input_filter>( filter );
      return true;
    }
  }

  return false;
}




//=========================================

bool Save_geostat_grid::init( std::string& parameters, 
			      GsTL_project* proj, Error_messages_handler* errors ) {
 proj_ = proj;
  errors_ = errors;

  //TL modified
  std::list<std::string> write_list;
  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

 
  // Get the grid from the manager

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  appli_assert( grid );
  grid_ = SmartPtr<Geostat_grid>( grid );

  // Get the filter from the manager
  SmartPtr<Named_interface> filter_ni =
	  Root::instance()->new_interface( params[2], 
	  outfilters_manager + "/" );

  if( filter_ni.raw_ptr() == 0 )
	  return false;

  Output_filter* filter = dynamic_cast<Output_filter*>( filter_ni.raw_ptr() );
  appli_assert( filter );

  //TL modified
  if (params.size() == 3)
    filter->setWriteMode(false);
  else {
    if (params[3] == "0")
		filter->setWriteMode(false);
	else 
		filter->setWriteMode(true);
	for (int i = 4; i < params.size(); ++i)
	  write_list.push_back(params[i]);
  }

  //if properties not specified, take entire grid
  if (write_list.empty())
	  write_list = grid->property_list();

  // Initialize the file name
  file_name_ = params[1];
  std::string filter_errors = "";

  filter->setWriteList(write_list);
  filter_ = SmartPtr<Output_filter>(filter);
  return filter_->write( file_name_, grid_.raw_ptr(), &filter_errors);
}




bool Save_geostat_grid::exec() {
	return true;
}


// TL modified
bool RunScript::init(std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) 
{
	_proj = proj;
	_errors = errors;
	_scriptfile = parameters;

	if (_scriptfile == "")
		return false;
	return true;
}

bool RunScript::exec()
{
  std::string buf = "";
  char ch;

  // redirect stdout and stderr to sgems output zones
  
  PyRun_SimpleString(""
   "import redirect\n"
   "class CoutLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cout(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "class CerrLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cerr(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "import sys\n"
   "sys.stdout = CoutLogger()\n"
   "sys.stderr = CerrLogger()\n"
   "");
  
  FILE* fp = fopen( _scriptfile.c_str(), "r" );
  if( !fp ) {
    GsTLcerr << "can't open file " << _scriptfile.c_str() << gstlIO::end;
    return false;
  }

  while ((ch = fgetc(fp)) != EOF)
	  buf += ch;
  buf += "\n";

  PyRun_SimpleString( (char*) buf.c_str() );

  fclose( fp );	
  if( Python_project_wrapper::is_project_modified() ) {
    Python_project_wrapper::project()->update();
  }

  return true;
}

// ===================================


// TL modified
/*
Load_sim::Load_sim() {

}
Load_sim::~Load_sim() {
}

bool Load_sim::init(std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  dirname_ =  QString( parameters.c_str() );

  // a very crude validity check
  if( !dirname_.endsWith( ".prt", false ) ) {
    errors->report("wrong extension");
    return false;
  }
  
  return true;
}

bool Load_sim::exec() {
	Prt * p = NULL;
	int i,j,index = 0, totalsize = 0;
	PyObject * mod, *func;

	Py_Initialize();
	PyRun_SimpleString(""
		"import redirect\n"
		"class CoutLogger:\n"
		"    def __init__(self):\n"
		"        self.buf = []\n"
		"    def write(self, data):\n"
		"        self.buf.append(data)\n"
		"        if data.endswith('\\n'):\n"
		"            redirect.sgems_cout(''.join(self.buf))\n"
		"            self.buf = []\n"
		"\n"
		"class CerrLogger:\n"
		"    def __init__(self):\n"
		"        self.buf = []\n"
		"    def write(self, data):\n"
		"        self.buf.append(data)\n"
		"        if data.endswith('\\n'):\n"
		"            redirect.sgems_cerr(''.join(self.buf))\n"
		"            self.buf = []\n"
		"\n"
		"import sys\n"
		"sys.stdout = CoutLogger()\n"
		"sys.stderr = CerrLogger()\n"
		"");

	// !!! Main program will wait for this to finish !!!

	PyObject * fromList = Py_BuildValue("[s]", "readprt");

	if (!(mod = PyImport_ImportModuleEx((char *)parser.c_str(), NULL, NULL, fromList))){
		PyRun_SimpleString("print 'unable to get module'");
		PyErr_Print();
		return false;
	}

	if (!(func = PyDict_GetItemString(PyModule_GetDict(mod), parser.c_str()))) {
		PyRun_SimpleString("print 'unable to get function'");
		PyErr_Print();
		return false;
	}

	if (!PyObject_CallObject(func, Py_BuildValue("(s)",dirname_.ascii()))) {
		appli_warning("Unable to run script " << parser);
		PyErr_Print();
		return false;
	}


	
	pprt PRT;
	if (!(p = PRT.fill()))
		return false;

	totalsize = (p->X) * (p->Y) * (p->Z);

    appli_message( "creating new grid '" << dirname_.latin1() << "'" 
	  	           << " of dimensions: " << p->X << "x" << p->Y << "x" << p->Z); 
	
	// create a new grid
	SmartPtr<Named_interface> ni = Root::instance()->new_interface( "cgrid", 
		gridModels_manager + "/" + "eclipse" );
	Cartesian_grid* grid = dynamic_cast<Cartesian_grid*>( ni.raw_ptr() );
	appli_assert( grid != 0 );
	grid->set_dimensions( p->X, p->Y, p->Z);

	
	for (i = 0; i < p->num_dates; ++i) {
		for (j = 0; j < p->num_properties; ++j, ++index) {
			string temp = p->dates[i];
			temp+="_";
			temp+=p->properties[j];
			GsTLGridProperty* prop = grid->add_property( temp );
			transform((p->data)+index*totalsize, p->X, p->Y, p->Z, prop);
		}
	}

	proj_->new_object("eclipse");

	return true;
}

// performs axis conversion between sgems and eclipse format
void Load_sim::transform(const Type * src, int x, int y,
			      int z, GsTLGridProperty * dest)
{
  int i,j,k, cur, index = 0;

 
  for (i = z-1; i >= 0; --i) {
	  for (j = y-1; j >= 0; --j) { 
		cur = j*x+i*x*y;
		for (k = 0; k < x; ++k,++index){
			dest->set_value(*(src+cur+k), index);
		}
	  }
  }
}
*/

//===========================================



//===========================================
Load_project::~Load_project() {
}

bool Load_project::init( std::string& parameters, 
				  GsTL_project* proj, Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;

  dirname_ =  QString( parameters.c_str() );
  if( !dirname_.endsWith( ".prj" ) && !dirname_.endsWith( ".prj/") ) return false;

  QFileInfo info( dirname_ );
  if( !info.isDir() ) return false;

  return true;
}


bool Load_project::exec() {

  QDir* dir = new QDir( dirname_ );
  if( !dir->cd( dirname_ ) ) {
    QByteArray tmp = dirname_.toAscii();
    GsTLcerr << "Could not load project " << tmp.constData() << gstlIO::end;
    return false;
  }

  // if there is already a project, close it
  if( !proj_->is_empty() )
    proj_->clear();
  
  proj_->name( std::string( dirname_.toAscii() ) );

  QFileInfoList files_info = 
    dir->entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot );
  QFileInfoList::iterator it= files_info.begin();

  // ensure that the grid are loaded first
  for(int i=0; i< files_info.size(); ++i) {
    if( files_info.at(i).fileName().endsWith(".grid") ) {
      files_info.move(i,0);
    }
  }


  while( it != files_info.end() ) {
    QByteArray tmp = (*it).absoluteFilePath().toAscii();
    std::string param( std::string( tmp.constData() ) + 
                       Actions::separator + "All" );
    bool ok = proj_->execute( "LoadObjectFromFile", param, errors_ );
    if( !ok ) {
      QByteArray tt = (*it).fileName().toAscii();
      errors_->report( "... this error occurred while loading \"" + 
                             std::string( tt.constData()) +"\"" );
    }

    ++it;
  }

  proj_->reset_change_monitor();
  return true;
}




//==========================================

bool Copy_property::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  proj_ = proj;
  errors_ = errors;
  bool overwrite = false;
  bool mark_as_hard = false;

  std::vector< std::string > params = 
      String_Op::decompose_string( parameters, Actions::separator,
				   Actions::unique );

  if( params.size() < 4 )
    return false;

  if( params.size() >= 5 ) {
    overwrite = String_Op::to_number<bool>( params[4] );
    if( params.size() >=6 )
      mark_as_hard = String_Op::to_number<bool>( params[5] );
  }


  // Get the 2 grids from the manager

  SmartPtr<Named_interface> serv_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  server_ = dynamic_cast<Geostat_grid*>( serv_grid_ni.raw_ptr() );

  SmartPtr<Named_interface> client_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[2] );
  client_ = dynamic_cast<Geostat_grid*>( client_grid_ni.raw_ptr() );

  if( !client_ || !server_ ) return false;

  server_prop_ = server_->property( params[1] );
  client_prop_ = client_->property( params[3] );
  if( !client_prop_ ) {
    GsTLGridCategoricalProperty* server_cprop = dynamic_cast<GsTLGridCategoricalProperty*>(server_prop_);
    if(server_cprop) {
      client_prop_ = client_->add_categorical_property( params[3], server_cprop->get_category_definition()->name());
    }
    else
      client_prop_ = client_->add_property( params[3] );
  }

  if( !server_prop_ || !client_prop_ ) return false;

  copier_ = Property_copier_factory::get_copier( server_, client_ );
  if( !copier_ ) return false;

  copier_->does_overwrite( overwrite );
  copier_->marks_as_hard( mark_as_hard );

  return true;
}


bool Copy_property::exec() {
  bool ok = copier_->copy( server_, server_prop_, client_, client_prop_ );
  if( !ok ) return false;

  proj_->update();
  return ok;
}


Named_interface* Copy_property::create_new_interface( std::string& ) {
  return new Copy_property;
}



//================================================

bool Swap_property_to_disk::init( std::string& parameters, GsTL_project* proj,
                                 Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return false;

  // get the grid from the grid manager
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );

  if( !grid ) {
    std::ostringstream message;
    message << "grid \"" << params[0] << "\" does not exist"; 
    errors->report_if( true, message.str() );
    return false;
  }

  for( unsigned int i=1; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property( params[i] );
    if( prop )
      properties_to_swap_.push_back( prop );
  }

  return true;
}

bool Swap_property_to_disk::exec() {
  for( unsigned int i=0; i < properties_to_swap_.size() ; i++ ) {
    properties_to_swap_[i]->swap_to_disk();
  }
  return true;
}


Named_interface* Swap_property_to_disk::create_new_interface( std::string& ) {
  return new Swap_property_to_disk; 
}



//================================================

bool Swap_property_to_ram::init( std::string& parameters, GsTL_project* proj,
                                 Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return false;

  // get the grid from the grid manager
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );

  if( !grid ) {
    std::ostringstream message;
    message << "grid \"" << params[0] << "\" does not exist"; 
    errors->report( false, message.str() );
    return false;
  }

  for( unsigned int i=1; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property( params[i] );
    if( prop )
      properties_to_swap_.push_back( prop );
  }

  return true;
}

bool Swap_property_to_ram::exec() {
  for( unsigned int i=0; i < properties_to_swap_.size() ; i++ ) {
    properties_to_swap_[i]->swap_to_memory();
  }
  return true;
}


Named_interface* Swap_property_to_ram::create_new_interface( std::string& ) {
  return new Swap_property_to_ram; 
}





//================================================

bool Delete_objects::init( std::string& parameters, GsTL_project* project,
                           Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.empty() ) return true;

  SmartPtr<Named_interface> ni_dir =
    Root::instance()->interface( gridModels_manager );
  Manager* grid_manager = dynamic_cast<Manager*>( ni_dir.raw_ptr() );
  appli_assert( grid_manager );
  if( !grid_manager ) return false;
  
  for( std::vector<std::string>::const_iterator it = params.begin() ;
        it != params.end(); ++it ) {
    bool ok = grid_manager->delete_interface( "/" + (*it) );
    if( ok )
      project->deleted_object( *it );
  }

  project->update();
  return true;
}


bool Delete_objects::exec() {
  return true;
}


Named_interface* Delete_objects::create_new_interface( std::string& ) {
  return new Delete_objects; 
}




//================================================
/* DeleteProperties grid_name::prop1::prop2...
*/
bool Delete_properties::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return true;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  for( unsigned int i = 1 ; i < params.size() ; i++ ) {
    grid->remove_property( params[i] );
  }

  proj->update( params[0] );
  return true;
}


bool Delete_properties::exec() {
  return true;
}


Named_interface* Delete_properties::create_new_interface( std::string& ) {
  return new Delete_properties; 
}




//================================================
/* Clear_property_value_if grid_name::prop::min::max
* will set to un-informed all value of property "prop" in grid "grid_name"
* that are in range [min,max]
*/
bool Clear_property_value_if::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 4 ) {
    errors->report( "some parameters are missing" );  
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  GsTLGridProperty* prop = grid->property( params[1] );
  if( !prop ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no property called \"" << params[1] << "\"";
    errors->report( message.str() ); 
    return false;
  }

  float min = String_Op::to_number<float>( params[2] );
  float max = String_Op::to_number<float>( params[3] );

  for( int i = 0 ; i < prop->size() ; i++ ) {
    if( prop->get_value( i ) >= min && prop->get_value(i) <= max )
      prop->set_not_informed( i );
  }

  proj->update( params[0] );
  return true;
}


bool Clear_property_value_if::exec() {
  return true;
}


Named_interface* 
Clear_property_value_if::create_new_interface( std::string& ) {
  return new Clear_property_value_if; 
}





//================================================
/* Set_active_region grid_name::region_name
* set region_name active
*/
bool Set_active_region::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.empty() ) {
    errors->report( "some parameters are missing" );  
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  if( params.size() == 1 ) grid->unselect_region();
  else {
    bool ok = grid->select_region(params[1]);

    if( !ok ) {
      std::ostringstream message;
      message << "Grid \"" << params[0] << "\" has no region called \"" << params[1] << "\"";
      errors->report( message.str() ); 
      return false;
    }
  }

  proj->update( params[0] );
  return true;
}


bool Set_active_region::exec() {
  return true;
}

//================================================
/* Set_hard_data grid_name::prop1...
* set region_name active
*/

bool Set_hard_data::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size()<2 ) {
    errors->report( "some parameters are missing" );  
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  for( unsigned int i = 1 ; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property( params[i] );
    if( !prop ) {
      std::ostringstream message;
      message << "Grid \"" << params[0] << "\" has no property called \"" << params[1] << "\"";
      errors->report( message.str() ); 
      return false;
    }
    for(unsigned int j=0; prop->size(); j++) 
      if(prop->is_informed(j)) prop->set_harddata(true,j);
  }
 

  proj->update( params[0] );
  return true;
}



bool Set_hard_data::exec() {
  return true;
}


//================================================
/* DeleteRegions grid_name::region1::region2...
*/
bool Delete_regions::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return true;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  for( unsigned int i = 1 ; i < params.size() ; i++ ) {
    grid->remove_region( params[i] );
  }

  proj->update( params[0] );
  return true;
}


bool Delete_regions::exec() {
  return true;
}


Named_interface* Delete_regions::create_new_interface( std::string& ) {
  return new Delete_regions; 
}


//================================================
/* Set_region_from_property grid_name::region::prop::min::max
* will set the "region" based on property "prop" in grid "grid_name"
* being in range [min,max].  Uninformed values are automatically set as
* outside the region
*/
bool Set_region_from_property::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() != 5 ) {
    errors->report( "some parameters are missing" );  
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  GsTLGridRegion* region = grid->add_region(params[1]);
  if( !region ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has already a region called \"" << params[1] << "\"";
    errors->report( message.str() );
    return false;
  }


  GsTLGridProperty* prop = grid->property( params[2] );
  if( !prop ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no property called \"" << params[2] << "\"";
    errors->report( message.str() ); 
    return false;
  }

  float min = String_Op::to_number<float>( params[3] );
  float max = String_Op::to_number<float>( params[4] );

  for( int i = 0 ; i < prop->size() ; i++ ) {
    if(prop->is_informed( i ) && prop->get_value( i ) >= min && prop->get_value(i) <= max )
      region->set_region_value(true,i);
    else 
      region->set_region_value(false,i);
  }

  proj->update( params[0] );
  return true;
}


bool Set_region_from_property::exec() {
  return true;
}


Named_interface* 
Set_region_from_property::create_new_interface( std::string& ) {
  return new Set_region_from_property; 
}


//================================================
/* Set_region_from_categorical_property grid_name::region::prop::name1[::name2::]
* will set the "region" based on property "prop" in grid "grid_name"
* being in of of the category.  Uninformed values are automatically set as
* outside the region
*/
bool Set_region_from_categorical_property::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() <4 ) {
    errors->report( "some parameters are missing" );
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() );
    return false;
  }


  GsTLGridCategoricalProperty* prop = grid->categorical_property(params[2]);
  if( !prop ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has no categorical property called \"" << params[2] << "\"";
    errors->report( message.str() );
    return false;
  }




  std::vector<int> in_region_code;
  const CategoricalPropertyDefinition *cat_def = prop->get_category_definition();
  for(int i=3; i< params.size(); i++) {
	  if( !cat_def->is_category_exist(params[i]) ) {
		    std::ostringstream message;
		    message << "Property \"" << params[2] << "\" has no category called \"" << params[i] << "\"";
		    errors->report( message.str() );
		    return false;

	  }
	  in_region_code.push_back( cat_def->category_id(params[i]) );
  }
  std::sort(in_region_code.begin(), in_region_code.end());

  GsTLGridRegion* region = grid->add_region(params[1]);
  if( !region ) {
    std::ostringstream message;
    message << "Grid \"" << params[0] << "\" has already a region called \"" << params[1] << "\"";
    errors->report( message.str() );
    return false;
  }

  for( int i = 0 ; i < prop->size() ; i++ ) {
    if( !prop->is_informed( i ) ) {
    	region->set_region_value(false,i);
    }
    else {
    	std::vector<int>::const_iterator it =
    			std::find(in_region_code.begin(), in_region_code.end(), prop->get_value(i));
    	if(it != in_region_code.end()) {
    		region->set_region_value(true,i);
    	}
    	else {
    		region->set_region_value(false,i);
    	}
    }
  }

  proj->update( params[0] );
  return true;
}


bool Set_region_from_categorical_property::exec() {
  return true;
}


Named_interface*
Set_region_from_categorical_property::create_new_interface( std::string& ) {
  return new Set_region_from_categorical_property;
}


//================================================
/* Convert_continuous_to_categorical_property grid_name::prop1::[prop2::]
* will copy convert the property from continuous to categorical
*/
bool Convert_continuous_to_categorical_property::
init( std::string& parameters, GsTL_project* proj,
      Error_messages_handler* errors ) {

  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() <2 ) {
    errors->report( "some parameters are missing, Needs at least two properties ()" );
    return false;
  }

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() );
    return false;
  }

  for(int i=1; i<params.size(); ++i) {
    GsTLGridCategoricalProperty* cprop = grid->categorical_property(params[i]);
    if(cprop) continue;
    GsTLGridProperty* prop = grid->property(params[i]);
    if(prop == 0) continue;
    cprop = grid->add_categorical_property(prop->name()+" - categorical");
    for(int nodeid=0; nodeid < prop->size(); ++nodeid) {
      if( prop->is_informed(nodeid)) {
        cprop->set_value(static_cast<int>(prop->get_value(nodeid)),nodeid);
      }
    }
  }

  proj->update( params[0] );
  return true;
}


bool Convert_continuous_to_categorical_property::exec() {
  return true;
}


Named_interface*
Convert_continuous_to_categorical_property::create_new_interface( std::string& ) {
  return new Convert_continuous_to_categorical_property;
}



//================================================
/* Base class for combining regions
*/
bool Merge_regions::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 4 ) return true;

  grid_name_ = params[0];

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name_ );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << grid_name_ << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  for( unsigned int i = 2 ; i < params.size() ; i++ ) {
    GsTLGridRegion* region = grid->region( params[i] );
    if(region == NULL ) {
      std::ostringstream message;
      message << "No region called \"" << params[i] << "\" was found";
      errors->report( message.str() ); 
      return false;
    }
    regions_.push_back( grid->region( params[i]) );
  }
  
  
  new_region_ = grid->add_region(params[1]);
  proj_ = proj;

  return true;
}

/*
bool Merge_regions::exec() {
  return true;
}


Named_interface* Merge_regions::create_new_interface( std::string& ) {
  return new Merge_regions; 
}
*/
//================================================
/* MergeRegionsUnion grid_name::newRegion::region1::region2...
* Initilize through the base class Merge_regions
*/


bool Merge_regions_union::exec() {
  for( unsigned int i = 0 ; i < new_region_->size() ; i++ ) {
    new_region_->set_region_value( false, i );
    for( unsigned int j = 0 ; j < regions_.size() ; j++ ) {
      if( regions_[j]->is_inside_region( i ) ) {
        new_region_->set_region_value( true, i );
        continue;
      }
    }
    
  }

  proj_->update( grid_name_ );
  return true;
}
Named_interface* Merge_regions_union::create_new_interface( std::string& ) {
  return new Merge_regions_union; 
}

//================================================
/* MergeRegionsUnion grid_name::newRegion::region1::region2...
* Initilize through the base class Merge_regions
*/


bool Merge_regions_intersection::exec() {
  for( unsigned int i = 0 ; i < new_region_->size() ; i++ ) {
    new_region_->set_region_value( true, i );
    for( unsigned int j = 0 ; j < regions_.size() ; j++ ) {
      if( !regions_[j]->is_inside_region( i ) ) {
        new_region_->set_region_value( false, i );
        continue;
      }
    }
    
  }

  proj_->update( grid_name_ );
  return true;
}
Named_interface* Merge_regions_intersection::create_new_interface( std::string& ) {
  return new Merge_regions_intersection; 
}


//================================================
/* SetRegionComplement grid_name::region[::newRegion]
* If only two parameters are used, the region is changed in place
*/
bool Set_region_complement::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return true;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  GsTLGridRegion* region = grid->region( params[1] );
  if(region == NULL ) {
    std::ostringstream message;
    message << "No region called \"" << params[1] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  GsTLGridRegion* new_region;
  if(params.size() == 2) new_region = region;
  else  new_region = grid->add_region(params[2]);

  for( unsigned int i = 0 ; i < new_region->size() ; i++ )
    new_region->set_region_value( !region->is_inside_region(i), i );


  proj->update( params[0] );
  return true;
}


bool Set_region_complement::exec() {
  return true;
}


Named_interface* Set_region_complement::create_new_interface( std::string& ) {
  return new Set_region_complement; 
}




//================================================
/* Clear_property_value_from_property grid_name::prop1::prop2...
*/
bool Clear_property_value_from_property::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return true;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  GsTLGridProperty* mask = grid->property(params[1]);
  if(!mask){
    std::ostringstream message;
    message << "No property called \"" << params[1] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  for( unsigned int i = 2 ; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property(params[i]);
    if(!prop) continue;
    for( unsigned int i = 0; i <grid->size(); ++i ) 
      if(!mask->is_informed(i) ) prop->set_not_informed(i);
  }

  proj->update( params[0] );
  return true;
}


bool Clear_property_value_from_property::exec() {
  return true;
}


Named_interface* Clear_property_value_from_property::create_new_interface( std::string& ) {
  return new Clear_property_value_from_property(); 
}



//================================================
/* Clear_property_value_from_property grid_name::region::prop1[::prop2::]
*/
bool Clear_property_inside_region::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 3 ) return false;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  GsTLGridRegion* mask = grid->region(params[1]);
  if(!mask){
    std::ostringstream message;
    message << "No region called \"" << params[1] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  for( unsigned int i = 2 ; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property(params[i]);
    if(!prop) continue;
    for( unsigned int i = 0; i <grid->size(); ++i ) 
      if(mask->is_inside_region(i) ) prop->set_not_informed(i);
  }

  proj->update( params[0] );
  return true;
}


bool Clear_property_inside_region::exec() {
  return true;
}


Named_interface* Clear_property_inside_region::create_new_interface( std::string& ) {
  return new Clear_property_inside_region(); 
}


//================================================
/* Clear_property_outside_region grid_name::region::prop1[::prop2::]
*/
bool Clear_property_outside_region::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 3 ) return false;

  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + params[0] );
  Geostat_grid* grid = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid ) {
    std::ostringstream message;
    message << "No grid called \"" << params[0] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  GsTLGridRegion* mask = grid->region(params[1]);
  if(!mask){
    std::ostringstream message;
    message << "No region called \"" << params[1] << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  for( unsigned int i = 2 ; i < params.size() ; i++ ) {
    GsTLGridProperty* prop = grid->property(params[i]);
    if(!prop) continue;
    for( unsigned int i = 0; i <grid->size(); ++i ) 
      if(!mask->is_inside_region(i) ) prop->set_not_informed(i);
  }

  proj->update( params[0] );
  return true;
}


bool Clear_property_outside_region::exec() {
  return true;
}


Named_interface* Clear_property_outside_region::create_new_interface( std::string& ) {
  return new Clear_property_outside_region(); 
}




//================================================
/* Create_trend grid_name::Direction[::new_prop_name]
*  where Direction = {X, Y, Z, -X, -Y, -Z}
*  if no new_prop_name is provided then it is called trend_{X,Y,Z}
*  depending of the deirection   
*/

Create_trend::Create_trend(){
  directions_.insert("X");
  directions_.insert("Y");
  directions_.insert("Z");
  directions_.insert("-X");
  directions_.insert("-Y");
  directions_.insert("-Z");
}

bool Create_trend::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) return true;
  if( !is_direction_valid(params[1], errors ) ) return false;

  direction_id_ = params[1];
  std::string prop_name;
  if(params.size() == 3) prop_name = params[2];
  else prop_name = "trend_"+params[1];
  
  grid_name_ = params[0];
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name_ );
  grid_ = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid_ ) {
    std::ostringstream message;
    message << "No grid called \"" << grid_name_ << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  

  trend_ = grid_->add_property(prop_name);
  if(!trend_){
    std::ostringstream message;
    message << "Property already exist";
    errors->report( message.str() ); 
    return false;
  }  

  return true;
}


bool Create_trend::exec() {
  grid_->select_property(trend_->name());
  Geostat_grid::iterator it = grid_->begin();

// This is hard coded and should be replaced
// with a more flexible structure
  int id;
  float s = 1;
  if(direction_id_ == "X" || direction_id_ == "-X" )
    id = 0;
  if(direction_id_ == "Y" || direction_id_ == "-Y" )
    id = 1;
  if(direction_id_ == "Z" || direction_id_ == "-Z" )
    id = 2;

  if(direction_id_.size() == 2) s = -1;
  Geovalue::property_type min = 9e99;
  Geovalue::property_type max = -9e99;
  for( ; it!= grid_->end(); it++ ) {
    float t = s*(it->location()[id]);
    it->set_property_value(t);
    if(t < min) min = t;
    if(t > max) max = t;
  }
  for( it= grid_->begin(); it!= grid_->end(); it++ ) {
    Geovalue::property_type t = it->property_value();
    it->set_property_value((t-min)/(max-min));
  }

//  proj_->update( grid_name_ );
  return true;
}


bool Create_trend::is_direction_valid(std::string direction,
                                      Error_messages_handler* errors){
  std::set<std::string>::iterator it = directions_.find(direction);
  if(it == directions_.end()) {
    std::ostringstream message;
    message << "No direction \"" << direction << "\" is implemented \n The choices are:";
    for(it =directions_.begin(); it != directions_.end() ; it++ ) 
      message<<*it<<" ";
    errors->report( message.str() );
    return false;
  }
  return true;
}

std::vector<string> Create_trend::get_trend_functions(){
  std::vector<string> trends(directions_.begin(), directions_.end() );
  return trends;
}

Named_interface* Create_trend::create_new_interface( std::string& ) {
  return new Create_trend(); 
}


/** The parameters for this function are:
 * - the name of the grid
 * - Name of the property to be truncated
 * [ type of truncation: userdefined, decile, quantile, decile ]
 * if userdefined [tresholds]
 * Note that categorical property do not need threshold specifications
 */
Create_indicator_properties::Create_indicator_properties(){

}

Named_interface*
Create_indicator_properties::create_new_interface( std::string& ){
	return new Create_indicator_properties;
}

bool Create_indicator_properties::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){
	proj_ = proj;
  std::vector< std::string > params =
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() < 2 ) {
  	errors->report( "Need at least 2 parameters for categorical property and 3 for continuous property" );
  	return false;
  }

  std::string grid_name = params[0];
  SmartPtr<Named_interface> grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + grid_name );
  grid_ = dynamic_cast<Geostat_grid*>( grid_ni.raw_ptr() );
  if( !grid_ ) {
    std::ostringstream message;
    message << "No grid called \"" << grid_name << "\" was found";
    errors->report( message.str() );
    return false;
  }
  data_prop_ = grid_->property(params[1]);
  if(data_prop_ == 0) {
    errors->report( "No property named "+params[1]+" exists");
    return false;
  }
  GsTLGridCategoricalProperty* cprop = grid_->categorical_property(params[1]);
  if( cprop == 0 && params.size() == 2 ) {
    errors->report( "The thresholding options must be specified for continuous property");
    return false;
  }

  if(cprop == 0 ) {
  	if(params[2] == "Decile" || params[2] == "Quartile" || params[2] == "Quintile") {
  		get_thresholds_from_data(params[2]);
  	}
  	else if(params[2] == "UserDefined") {
  		for(int i=3; i< params.size(); i++) {
  			thresholds_.push_back(String_Op::to_number<float>(params[i]));
  		}
  	}
  	else {
      errors->report( "The thresholding option is not recognized");
      return false;
    }
  	group_name_ = data_prop_->name()+" "+params[2]+" indicator";
  }
  else {
  	group_name_ = data_prop_->name()+" categorical indicator";

  }
// Check if it already exists
  GsTLGridPropertyGroup* group =  grid_->get_group( group_name_ );
  if(group) {
    errors->report( "A group with the indicators already exists");
    return false;
  }


  return true;

}

void Create_indicator_properties::get_thresholds_from_data(std::string option){
	std::vector<float> data(data_prop_->begin(true),data_prop_->end());
	std::sort(data.begin(),data.end());
	int size = data.size();
	float quant;
	if(option == "Decile") {
		quant = 0.1;
	}
	else if(option == "Quartile"){
		quant = 0.25;
	}
	else if(option == "Quintile"){
		quant = 0.2;
	}
	for(float i=quant; i < 1; i+=quant ) {
		thresholds_.push_back( data[int(i*size)] );
	}
}

bool Create_indicator_properties::exec(){
	GsTLGridCategoricalProperty* cprop = dynamic_cast<GsTLGridCategoricalProperty*>(data_prop_);
	if(cprop == 0) {
		IndicatorContinuousPropertyGroup* group =
				dynamic_cast<IndicatorContinuousPropertyGroup*>(
						grid_->add_group(group_name_,"ContinuousIndicator")
						);
		group->set_thresholds(thresholds_);

		for(int t =0; t< thresholds_.size(); t++) {
			std::string name = data_prop_->name()+" less than "+String_Op::to_string(thresholds_[t]);
			GsTLGridProperty* prop =  grid_->add_property(name);
			if(!prop) continue;
			for(int i = 0; i < prop->size(); ++i) {
				if( data_prop_->is_informed(i) ) {
					int id = data_prop_->get_value(i) < thresholds_[t];
					prop->set_value(id, i);
				}
			}
			group->add_property(prop);
		}
	} else { // This is a categorical property
		IndicatorCategoricalPropertyGroup* group =
				dynamic_cast<IndicatorCategoricalPropertyGroup*>(
						grid_->add_group(group_name_,"CategoricalIndicator"));

		const CategoricalPropertyDefinition* def = cprop->get_category_definition();
		const CategoricalPropertyDefinitionName* defname =
				dynamic_cast<const CategoricalPropertyDefinitionName*>(def);

		int ncat;
		if(defname) ncat = defname->number_of_category();
		else {
			ncat = -1;
			GsTLGridProperty::const_iterator it = cprop->begin(true);
			for( ; it != cprop->end(); ++it) {
				if(*it > ncat ) ncat = *it;
			}
      ncat++;
		}
		
		for(int c=0 ; c < ncat ; c++ ) {
			std::string name = data_prop_->name()+" indicator "+def->get_category_name(c);
			GsTLGridProperty* prop =  grid_->add_property(name);
			if(!prop) continue;
			for(int i = 0; i < prop->size(); ++i) {
				if( data_prop_->is_informed(i) )
					prop->set_value(static_cast<float>(data_prop_->get_value(i) == c), i);
			}
			group->add_property(prop);
		}
	}
	return true;

}


/*
--------------------------------------
*/


Named_interface* Downscale_grid::create_new_interface( std::string& ) {
  return new Downscale_grid; 
}


bool Downscale_grid::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() !=5 )  {
    std::ostringstream message;
    message << "Need five paramters (UpscaledGrid::NewGridName::factorX::factorY::factorZ)";
    errors->report( message.str() ); 
    return false;
  }


  //Get grid
  std::string up_grid_name = params[0];
  SmartPtr<Named_interface> up_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + up_grid_name );

  Geostat_grid*  up_grid = dynamic_cast<Geostat_grid*>( up_grid_ni.raw_ptr() );
  if( !up_grid ) {
    std::ostringstream message;
    message << "No grid called \"" << up_grid_name << "\" was found";
    errors->report( message.str() ); 
    return false;
  }
  
  //Check if the grid already exist
  std::string down_grid_name = params[1];
  SmartPtr<Named_interface> down_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + down_grid_name );
  if(down_grid_ni.raw_ptr() != 0) {
    std::ostringstream message;
    message << "A grid called \"" << down_grid_name << "\" already exist";
    errors->report( message.str() ); 
    return false;
  }

  int fx = String_Op::to_number<int>(params[2]);
  int fy = String_Op::to_number<int>(params[3]);
  int fz = String_Op::to_number<int>(params[4]);

  if(fx <= 0 || fy <= 0 || fz <= 0) {
    errors->report( "All downscaling factor must be integer greater than zero" ); 
    return false;
  }

  SmartPtr<Named_interface> dscale_ni =
    Root::instance()->new_interface(up_grid->classname(), downscaler_manager+"/" );



  Grid_downscaler*  dscaler = dynamic_cast<Grid_downscaler*>( dscale_ni.raw_ptr() );
  if(dscaler == 0) {
    errors->report( "Could not find a factory for "+up_grid->classname() ); 
    return false;
  }

  Geostat_grid* down_grid = dscaler->downscale(up_grid, down_grid_name, fx,fy,fz );

  proj->new_object(down_grid_name);

  return down_grid != 0;
}



/*
--------------------------------------
*/


Named_interface* Upscale_properties::create_new_interface( std::string& ) {
  return new Upscale_properties; 
}


bool Upscale_properties::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() <3 )  {
    std::ostringstream message;
    message << "Need at least 3 paramters (TargetGrid::SourceGrid::Prop1[::prop2...])";
    errors->report( message.str() ); 
    return false;
  }

  //Get grid
  std::string target_grid_name = params[0];
  SmartPtr<Named_interface> target_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + target_grid_name );

  target_grid_ = dynamic_cast<Cartesian_grid*>( target_grid_ni.raw_ptr() );
  if( !target_grid_ ) {
    std::ostringstream message;
    message << "No regular grid called \"" << target_grid_name << "\" was found";
    errors->report( message.str() ); 
    return false;
  }


  std::string source_grid_name = params[1];
  SmartPtr<Named_interface> source_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + source_grid_name );

  source_grid_ = dynamic_cast<const Cartesian_grid*>( source_grid_ni.raw_ptr() );
  if( !source_grid_ ) {
    std::ostringstream message;
    message << "No regular grid called \"" << source_grid_name << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  if( source_grid_ ==  target_grid_) {
    errors->report( "Target and source grid are the smame!" ); 
    return false;
  }

// Check if the grids are compatible
  const RGrid_geometry* src_geom = source_grid_->geometry();
  const RGrid_geometry* target_geom = target_grid_->geometry();

  //Check if the cells are embedded
  double nx_f = (target_geom->cell_dims().x())/ src_geom->cell_dims().x() ;
  double ny_f = (target_geom->cell_dims().y())/ src_geom->cell_dims().y()  ;
  double nz_f = (target_geom->cell_dims().z())/ src_geom->cell_dims().z() ;

  int nx = static_cast<int>( GsTL::round(nx_f));
  int ny = static_cast<int>( GsTL::round(ny_f) );
  int nz = static_cast<int>( GsTL::round(nz_f) );
  
  if( !GsTL::equals((double)nx, nx_f) ) {
    errors->report( "The x dimension of the source grid is not compatible with the target grid" ); 
  }
  if( !GsTL::equals((double)ny, ny_f) ) {
    errors->report( "The y dimension of the source grid is not compatible with the target grid" ); 
  }
  if( !GsTL::equals((double)nz, nz_f) ) {
    errors->report( "The z dimension of the source grid is not compatible with the target grid" ); 
  }
  if(!errors->empty()) return false;

  n_cells_ = nx*ny*nz;

  GsTLPoint src_corner(source_grid_->origin().x() - src_geom->cell_dims().x()/2,
                       source_grid_->origin().y() - src_geom->cell_dims().y()/2,
                       source_grid_->origin().z() - src_geom->cell_dims().z()/2);

  GsTLPoint target_corner(target_grid_->origin().x() - target_geom->cell_dims().x()/2,
                       target_grid_->origin().y() - target_geom->cell_dims().y()/2,
                       target_grid_->origin().z() - target_geom->cell_dims().z()/2);

//  The corner coordinate must fit
  double ox_f = (src_corner.x() - target_corner.x())/ src_geom->cell_dims().x() ;
  double oy_f = (src_corner.y() - target_corner.y() )/ src_geom->cell_dims().y()  ;
  double oz_f = (src_corner.z() - target_corner.z() )/ src_geom->cell_dims().z() ;

  int ox_i = static_cast<int>( GsTL::round(ox_f));
  int oy_i = static_cast<int>( GsTL::round(oy_f) );
  int oz_i = static_cast<int>( GsTL::round(oz_f) );

  if( !GsTL::equals_with_epsilon((double)ox_i, ox_f)   || 
    !GsTL::equals_with_epsilon((double)oy_i, oy_f) || 
    !GsTL::equals_with_epsilon((double)oz_i, oz_f) ) {
    errors->report( "The source and target grid are not embedded"); 
    return false;
  }

  compute_variance_ = params[2] == "1";

  for(int i=3; i<params.size(); ++i) {
    const GsTLGridProperty* prop = source_grid_->property(params[i]);
    if(!prop) {
      errors->report( "The property "+params[i]+" does not exist"); 
      return false;
    }
    props_.push_back(prop);
  }

  

  return true;
}

bool Upscale_properties::exec() {

//  std::vector<int> counts(0,target_grid_->size());
  std::vector<int> index(source_grid_->size(),-1);
  
  const SGrid_cursor* source_cursor = source_grid_->cursor();
  const SGrid_cursor* target_cursor = target_grid_->cursor();
  Geostat_grid::const_iterator it = source_grid_->begin();

  const RGrid_geometry* target_geom = target_grid_->geometry();

  GsTLPoint target_origin = target_grid_->origin();

  GsTLPoint target_origin_corner(target_grid_->origin().x() - target_geom->cell_dims().x()/2,
                                 target_grid_->origin().y() - target_geom->cell_dims().y()/2,
                                 target_grid_->origin().z() - target_geom->cell_dims().z()/2);

  
  GsTLPoint target_extent_corner(target_origin_corner.x() + target_geom->cell_dims().x()*target_geom->dim(0),
                                 target_origin_corner.y() + target_geom->cell_dims().y()*target_geom->dim(1),
                                 target_origin_corner.z() + target_geom->cell_dims().z()*target_geom->dim(2) );


  for( ; it != source_grid_->end(); ++it ) {
    int source_nodeid = it->node_id();
    Geostat_grid::location_type src_loc = source_grid_->location(source_nodeid);
    if(src_loc.x() <= target_origin_corner.x() || 
       src_loc.y() <= target_origin_corner.y() || 
       src_loc.z() <= target_origin_corner.z()) continue;

    if(src_loc.x() > target_extent_corner.x() || 
       src_loc.y() > target_extent_corner.y() || 
       src_loc.z() > target_extent_corner.z()) continue;

    GsTLCoordVector delta = src_loc - target_origin_corner;

    int ti = delta.x()/target_geom->cell_dims().x();
    int tj = delta.y()/target_geom->cell_dims().y();
    int tk = delta.z()/target_geom->cell_dims().z();

    int tnodeid = target_cursor->node_id(ti,tj,tk);

    if(tnodeid < 0) continue;

 //   counts[tnodeid]++;
    index[source_nodeid] = tnodeid;

  }
  

  for(int ip=0; ip<props_.size(); ++ip) {

    const GsTLGridCategoricalProperty* cprop = dynamic_cast<const GsTLGridCategoricalProperty*>(props_[ip]);

    if(cprop) {
      process_categorical_property( cprop, index );
    }
    else{
      process_continuous_property( props_[ip], index );
    }
 }
  return true;
}


void Upscale_properties::process_continuous_property(const GsTLGridProperty* prop, std::vector<int>& index){
    GsTLGridProperty* mean_prop = target_grid_->add_property(prop->name()+" mean" );
    if( mean_prop == 0 ) return;
    //GsTLGridProperty* mean_prop = geostat_utils::add_property_to_grid(target_grid_,props_[ip]->name()+" mean" );
    GsTLGridProperty* var_prop =0;
    if(compute_variance_) {
      //var_prop = geostat_utils::add_property_to_grid(target_grid_,props_[ip]->name()+" variance" );
      var_prop = target_grid_->add_property(prop->name()+" variance" );
      if( var_prop == 0 ) {
        target_grid_->remove_property(mean_prop->name());
        return;
      }
    }

    upscaled_props_.push_back(mean_prop);
    if(compute_variance_) upscaled_props_.push_back(var_prop);

    std::vector<int> counts(target_grid_->size(),0);
    std::vector<GsTLGridProperty::property_type> target_mean(target_grid_->size(),0);
    std::vector<GsTLGridProperty::property_type> target_var;
    if(compute_variance_) {
      target_var.insert(target_var.begin(),target_grid_->size(),0);
    }

    for(int i=0; i<index.size(); ++i) {
      int target_id = index[i];
      if(target_id < 0) continue;
      if( !prop->is_informed(i) ) continue;
      GsTLGridProperty::property_type val = prop->get_value(i);
      target_mean[target_id] += val;
      if(compute_variance_) {
        target_var[target_id] += val*val;
      }
      counts[target_id]++;
    }

    for(int target_id=0; target_id<target_mean.size(); ++target_id) {
      if(counts[target_id] != n_cells_) continue;

      float m = target_mean[target_id]/n_cells_ ;
      mean_prop->set_value( m, target_id );
      if(compute_variance_) {
        var_prop->set_value( target_var[target_id]/n_cells_ -m*m , target_id );
      }
    }

}

void Upscale_properties::process_categorical_property(const GsTLGridCategoricalProperty* cprop, std::vector<int>& index){
  int ncat = cprop->get_number_of_category();

  const CategoricalPropertyDefinition* cdef = cprop->get_category_definition();

  std::vector<GsTLGridProperty*> props;
  std::vector<std::vector<float> > means;
  for( int c=0; c<ncat; ++c ) {
    GsTLGridProperty* prop = target_grid_->add_property(cprop->name()+" "+cdef->get_category_name(c)+" upscaled");
    means.push_back( std::vector<float>(target_grid_->size(), 0.0) );
    if(prop == 0) {
      for(int i=0; i<props.size(); ++i) {
        target_grid_->remove_property(props[i]->name());
      }
      props.clear();
      return;
    }
    props.push_back(prop);
  }
  upscaled_props_.insert(upscaled_props_.end(), props.begin(), props.end());

  std::vector<int> counts(target_grid_->size(),0);

  for(int i=0; i<index.size(); ++i) {
    int target_id = index[i];
    if(target_id < 0) continue;
    if( !cprop->is_informed(i) ) continue;
    int val = cprop->get_value(i);
    means[val][target_id] += 1.0;
    counts[target_id]++;
  }

  for(int target_id=0; target_id< target_grid_->size(); ++target_id) {
    if(counts[target_id] != n_cells_) continue;

    for( int c=0; c<ncat; ++c ) {
      float m = means[c][target_id]/n_cells_ ;
      props[c]->set_value( m, target_id );
    }

  }

  // Build a group to store the properies
  GsTLGridPropertyGroup* group = target_grid_->add_group(cprop->name()+" upscaled", "General");
  if( group ) {
    for( int c=0; c<ncat; ++c ) {
      group->add_property(props[c]);
    }
  }

}

std::vector<GsTLGridProperty*> Upscale_properties::get_upscaled_properties(){
  return upscaled_props_;
}

/*
 -----------------------------------------
*/


Named_interface* Upscale_group::create_new_interface( std::string& ) {
  return new Upscale_group; 
}


bool Upscale_group::init( std::string& parameters, GsTL_project* proj,
                              Error_messages_handler* errors ) {
  std::vector< std::string > params = 
    String_Op::decompose_string( parameters, Actions::separator,
                      				   Actions::unique );

  if( params.size() !=4 )  {
    std::ostringstream message;
    message << "Need 4 paramters (TargetGrid::SourceGrid::VarianceOption01::Group)";
    errors->report( message.str() ); 
    return false;
  }

  std::string target_grid_name = params[0];
  SmartPtr<Named_interface> target_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + target_grid_name );

  target_grid_ = dynamic_cast<Cartesian_grid*>( target_grid_ni.raw_ptr() );
  if( !target_grid_ ) {
    std::ostringstream message;
    message << "No regular grid called \"" << target_grid_name << "\" was found";
    errors->report( message.str() ); 
    return false;
  }


  std::string source_grid_name = params[1];
  SmartPtr<Named_interface> source_grid_ni =
    Root::instance()->interface( gridModels_manager + "/" + source_grid_name );

  source_grid_ = dynamic_cast<const Cartesian_grid*>( source_grid_ni.raw_ptr() );
  if( !source_grid_ ) {
    std::ostringstream message;
    message << "No regular grid called \"" << source_grid_name << "\" was found";
    errors->report( message.str() ); 
    return false;
  }

  group_name_ = params[3];
  const GsTLGridPropertyGroup* group = source_grid_->get_group(group_name_ );
  if(group == 0) {
    errors->report( "No group called "+group_name_+" in grid "+ params[1] );
  }

  QStringList param_list;
  param_list.append(params[0].c_str());
  param_list.append(params[1].c_str());
  param_list.append(params[2].c_str());

  std::vector<std::string> pnames = group->property_names();
  for(int i=0; i<pnames.size(); ++i) {
    param_list.append(pnames[i].c_str());
  }

  QString params_properties = param_list.join("::");

  std::string param_str = params_properties.toStdString();
  return upscaler_.init(param_str, proj, errors );


}



bool Upscale_group::exec(){
  bool ok = upscaler_.exec();
  if(ok) {
    GsTLGridPropertyGroup* group = target_grid_->add_group(group_name_+" upscaled", "General");
    if(group) {
      std::vector<GsTLGridProperty*> props = upscaler_.get_upscaled_properties();
      for( int i=0; i<props .size(); ++i )
        group->add_property(props[i]);
    }
  }
  return ok;
}


/*
 ------------------------
 */

Named_interface* Save_project::create_new_interface( std::string& ){
  return new Save_project;
}

bool Save_project::init( std::string& parameters, GsTL_project* proj,
                     Error_messages_handler* errors ){

  QString dirname = QString::fromStdString(parameters);
  if( !dirname.endsWith( ".prj" ) && !dirname.endsWith( ".prj/" ) ) 
    dirname.append( ".prj" );

//  preferences_.project_name = dirname;
  proj->name( std::string( dirname.toStdString() ) ); 
  //QDir* dir = new QDir( dirname );
  QDir dir( dirname );

  // If the directory already exists, erase all the files. 
  // Possible improvements: currently all the files are erased sequentially, and
  // if erasing fails, the functions is aborted. But that results in a partially
  // deleted project, and no new project saved. A better solution would be to 
  // check beforehand that we will be able to erase all the files (not difficult
  // with QFileInfo).
  if( dir.exists( dirname ) ) {
    bool ok = dir.cd( dirname );
    QStringList files = dir.entryList( QDir::Files );
    for( QStringList::iterator it = files.begin(); it != files.end(); ++it ) {
      QString tmp = *it;
      if (dir.isRelativePath(*it))
	        tmp = dir.filePath(*it);

      if( !dir.remove( tmp ) )
        errors->report("Project "+ dirname.toStdString() +" already exists and can not be "+
                        "overwritten");
    }
  }
  else {
    bool created = dir.mkdir( dirname );
    if( !created ) {
      errors->report("Could not create project "+dirname.toStdString());
    }
  }
  if(!errors->empty()) return false;
  

  if( !dir.cd( dirname ) ) {
    errors->report("Could not create project "+dirname.toStdString());
    return false;
  }

//  dir->cdUp();
//  preferences_.last_save_directory = dir->absolutePath();
//  dir->cd( dirname );

  //Get the folder to save
  SmartPtr<Named_interface> project_filter_ni = Root::instance()->interface(project_filters_manager+"/project_filters");
  Save_project_objects* project_filter = dynamic_cast<Save_project_objects*>(project_filter_ni.raw_ptr());
  if(project_filter==0) {
    errors->report("Could not get the filters manager for the project");
    return false;
  }

  typedef const std::map<std::string, std::pair<std::string, std::string> > dir_to_paramsT;
  dir_to_paramsT::const_iterator it_dir  = project_filter->directory_to_save().begin();

  // Loop over all the directory to save
  for( ; it_dir!= project_filter->directory_to_save().end(); ++it_dir) {
    SmartPtr<Named_interface> dir_object_ni = Root::instance()->interface(it_dir->first);  
    Manager* dir_object = dynamic_cast<Manager*>( dir_object_ni.raw_ptr() );
    if(dir_object==0) continue;

    //Get all the object in the directory
    Manager::interface_iterator it_object = dir_object->begin_interfaces();
    for( ; it_object != dir_object->end_interfaces(); ++it_object ) {

      SmartPtr<Named_interface> out_filter_ni = Root::instance()->new_interface( it_dir->second.first , it_dir->second.second);

      Output_filter* out_filter = dynamic_cast<Output_filter*>(out_filter_ni.raw_ptr());
      if(out_filter==0) continue;
      std::string error_filter;
      bool ok = out_filter->write(dirname.toStdString()+"/"+it_object->raw_ptr()->name(), it_object->raw_ptr(),  &error_filter );

      if(!ok) {
        errors->report("Error writing the project.  Operation aborted.  May result in loss of data.  Try saving in another directory\n"+error_filter);
        return false;
      }

    }
  }

  return true;


}