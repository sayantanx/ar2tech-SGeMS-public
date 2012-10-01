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



/**********************************************************************
 ** Author: Nicolas Remy
 ** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
 **   University
 ** All rights reserved.
 **
 ** This file is part of the "main" module of the Geostatistical Earth
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

#include <GsTLAppli/main/lib_initializer.h>

#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/progress_notifier.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/actions/python_wrapper.h>
#include <GsTLAppli/actions/library_actions_init.h>

#include <GsTLAppli/gui/appli/qt_sp_application.h>
#include <GsTLAppli/gui/viewer/qvtkGsTLViewer.h>

#include <QSplashScreen>
#include <qapplication.h>
#include <qstatusbar.h>
#include <QTextStream>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qsettings.h>

#include <iostream>
#include <fstream>
#include <cstdio>

/*
#ifdef _DEBUG
#include <GsTLAppli/main/modeltest.h>
#include <GsTLAppli/gui/models/project_proxy_model.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/utils/manager.h>
#endif
*/
using namespace String_Op;

// Std_scribe is a scribe that writes to standard output or standard error
class Std_scribe: public Scribe {
public:
	Std_scribe(std::ostream& os) :
		os_(os) {
	}
	virtual void write(const std::string& str, const Channel*) {
		os_ << str;
	}

private:
	std::ostream& os_;
};

class File_logger: public Scribe {
public:
	File_logger(const std::string& filename) :
		of_(0), append_newline_(false) {
		of_ = new std::ofstream(filename.c_str());
		if (of_->bad()) {
			std::cerr << "unable to open filename" << std::endl;
			of_->open("log.txt");
			appli_assert( of_ );
		}
	}

	bool append_newline() const {
		return append_newline_;
	}
	void append_newline(bool on) {
		append_newline_ = on;
	}

	virtual void write(const std::string& str, const Channel*) {
		*of_ << str;
		if (append_newline_)
			*of_ << std::endl;
		else
			*of_ << std::flush;
	}

private:
	std::ofstream* of_;
	bool append_newline_;
};

class QTscribe: public Scribe {
public:
	virtual void write(const std::string& str, const Channel*) {
		QMessageBox::critical(qApp->activeWindow(), "An Error Occurred...", QString(str.c_str()), QMessageBox::Ok, QMessageBox::NoButton);

	}
};

class Status_bar_scribe: public Scribe {
public:
	Status_bar_scribe(QStatusBar* bar) :
		bar_(bar) {
	}
	virtual void write(const std::string& str, const Channel*) {
		QString msg(str.c_str());
		//msg.prepend( "Error: " );
		bar_->showMessage(msg);
	}
private:
	QStatusBar* bar_;
};

//====================================================


void print_help() {
	std::cerr << "Usage: sgems [-s] file\n" << "If the -s option is specified, the file is a Python script\n"
			<< "file. Otherwise, the file contains a list of SGeMS commands\n" << "that will be executed sequentially." << std::endl;
}

int nogui_main(int argc, char** argv) {

	if (argc < 2) {
		print_help();
		return 1;
	}

	QString arg(argv[1]);
	if (argc == 2 && (arg.contains("-h") || arg.contains("--help"))) {
		print_help();
		return 0;
	}

	Std_scribe* error_scribe = new Std_scribe(std::cerr);
	error_scribe->subscribe(GsTLcerr);

	QApplication app(argc, argv);
	Lib_initializer::minimal_init();
	Lib_initializer::load_geostat_algos();
	Lib_initializer::load_filters_plugins();

	//-------------------------------
	// create a default project
	SmartPtr<Named_interface> ni = Root::instance()->new_interface("project", projects_manager + "/" + "project");
	GsTL_project* project = dynamic_cast<GsTL_project*> (ni.raw_ptr());
	appli_assert( project );
	Python_project_wrapper::set_project(project);

	//-------------------------------
	// Use a text-based progress notifier to report on progress of long tasks
	Root::instance()->factory("progress_notifier", Text_progress_notifier::create_new_interface);

	if (argc == 2) {
		// The file contains S-GeMS commands
		const char* script_filename = argv[1];
		QFile file(script_filename);
		if (!file.open(QIODevice::ReadOnly)) {
			GsTLcerr << "Can not open " << script_filename << ": no such file" << gstlIO::end;
			return 1;
		}
		std::cerr << "Executing commands...\n" << std::endl;

		QTextStream stream(&file);
		QString line;
		while (!stream.atEnd()) {
			line = stream.readLine();
			QByteArray tmp = line.toLatin1();
			line = tmp.trimmed();

			if (line.startsWith("#"))
				continue;

			Error_messages_handler error_messages;
			std::string command = qstring2string(line);
			String_Op::string_pair args = String_Op::split_string(command, " ", false);

			GsTLcerr << "Running: " << args.first << "\n" << gstlIO::end;

			bool ok = project->execute(args.first, args.second, &error_messages);
			if (!ok) {
				GsTLcerr << "Command " << args.first << " could not be performed: \n";
				if (!error_messages.empty())
					GsTLcerr << error_messages.errors() << "\n";
			}
		}

		file.close();
	} else {
		QString switch_arg(argv[1]);
		if (!switch_arg.contains("-s")) {
			print_help();
			return 1;
		}

		// The file contains a Python script

		const char* script_filename = argv[2];
		QFile file(script_filename);
		if (!file.open(QIODevice::ReadOnly)) {
			GsTLcerr << "Can not open " << script_filename << ": no such file" << gstlIO::end;
			return 1;
		}
		file.close();

		std::cerr << "Executing script...\n" << std::endl;

		FILE* fp = fopen(argv[2], "r");

		if (!fp) {

			std::cerr << "can't open file " << argv[2] << std::endl;

			return 1;

		}
		PyRun_SimpleFile(fp, argv[2]);

		fclose(fp);
	}

	GsTLcerr << gstlIO::end;
	Root::instance()->delete_interface(gridObject_manager);

	libGsTLAppli_actions_release();
	return 0;
}

QString path_to_splash_image() {
	QFileInfo finfo;
	QString image_file("new_splash2.png");

	char* env = getenv("GSTLAPPLIHOME");
	if (env) {
		QString envpath(env);
		QDir homedir(envpath);
		homedir.cd("bin");
		finfo.setFile(homedir, image_file);
	} else {
		QDir current_dir;
		finfo.setFile(current_dir, image_file);
	}
	return finfo.absoluteFilePath();
}

QString read_style_sheet() {
	QFileInfo finfo;
	QString qss_file("sgems.qss");

	char* env = getenv("GSTLAPPLIHOME");
	if (env) {
		QString envpath(env);
		QDir homedir(envpath);
//		homedir.cd("bin");
		finfo.setFile(homedir, qss_file);
	} else {
		QDir current_dir;
		finfo.setFile(current_dir, qss_file);
	}
    QFile file(finfo.absoluteFilePath());

    file.open(QFile::ReadOnly);
    QString style_sheet;
    if(file.isReadable())
    	style_sheet = QLatin1String(file.readAll());

    return style_sheet;

}

QString path_to_plugins() {
	QDir pluginsdir; //points to current directory

	char* env = getenv("GSTLAPPLIHOME");
	if (env) {
		QString envpath(env);
		pluginsdir.setPath(envpath);
	}

	pluginsdir.cd("plugins");
	return pluginsdir.absolutePath();
}

int gui_main(int argc, char **argv) {

	File_logger* initialization_log = new File_logger("sgems_status.log");
	initialization_log->subscribe(GsTLlog);

	QTscribe* qt_error_scribe = new QTscribe;
	qt_error_scribe->subscribe(GsTLcerr);

	GsTLlog << gstlIO::no_wait_end;
	//-----

	//---------------------
	// Initialize the libraries, manage the splash-screen, and set-up the
	// main application window

	QApplication app(argc, argv);
	app.setStyleSheet(read_style_sheet());

  // Set the icon theme
  /*
static const char * GENERIC_ICON_TO_CHECK = "document-open";
static const char * FALLBACK_ICON_THEME = "silk";
if (!QIcon::hasThemeIcon(GENERIC_ICON_TO_CHECK)) {
    //If there is no default working icon theme then we should
    //use an icon theme that we provide via a .qrc file
    //This case happens under Windows and Mac OS X
    //This does not happen under GNOME or KDE
    QIcon::setThemeName(FALLBACK_ICON_THEME);
}
*/

	QApplication::addLibraryPath(path_to_plugins());

	QPixmap pixmap(path_to_splash_image());

	QSplashScreen* splash = new QSplashScreen(pixmap);

	splash->setFont(QFont("Times", 8, QFont::Bold));
	splash->showMessage("Initializing...");
	app.processEvents();
	Lib_initializer::init();

	splash->showMessage("Loading colormaps...");
	app.processEvents();
	Lib_initializer::load_colormaps();

	splash->showMessage("Loading geostat plugins...");
	app.processEvents();
	Lib_initializer::load_geostat_algos();
	Lib_initializer::load_filters_plugins();

	splash->showMessage("Loading action plugins...");
	app.processEvents();
	Lib_initializer::load_action_plugins();
	Lib_initializer::load_python_scripts();
  std::cout<<"load group script"<<std::endl;
	Lib_initializer::load_python_group_scripts();

  QSP_application* appli = new QSP_application(0);


	Status_bar_scribe* statusbar_scribe = new Status_bar_scribe(appli->statusBar());
	statusbar_scribe->subscribe(GsTLcerr);
	statusbar_scribe->subscribe(GsTLcout);

	appli->init();
	appli->setWindowTitle("SGeMS");


	// restore preferences
	QSettings settings;
	settings.setPath(QSettings::NativeFormat, QSettings::UserScope, "scrf.stanford.edu");
	int height = settings.value("/geometry/height", 480).toInt();
	int width = settings.value("/geometry/width", 640).toInt();
	appli->resize(QSize(width, height));

	bool show_algo_panel = settings.value("/panels/algo", true).toBool();
	bool show_cli_panel = settings.value("/panels/cli", false).toBool();

	appli->setWindowIcon(QPixmap("new_icon6_32x32.bmp"));
  std::cout<<"appli->show();"<<std::endl;
	appli->show();
	appli->show_algo_panel(show_algo_panel);
	appli->show_commands_panel(show_cli_panel);

	//------------------
	SmartPtr<Named_interface> ni = Root::instance()->interface(projects_manager + "/" + "project");
	GsTL_project* project = dynamic_cast<GsTL_project*> (ni.raw_ptr());
	appli_assert( project );
	Python_project_wrapper::set_project(project);

	//------------------


	splash->finish(appli);
	delete splash;

	//------------------------
	// Everything is now ready. We set up one more logging device, and it's done

	GsTLlog << gstlIO::wait_end;

	initialization_log->unsubscribe(GsTLlog);

	File_logger* main_log = new File_logger("sgems_history.log");
	main_log->subscribe(GsTLlog);
	main_log->append_newline(true);

  /*
#ifdef _DEBUG

  // test the models:
  //project->execute("LoadObjectFromFile","C:/data/Shortcourse/Petroleum/Fluvial.prj/well-log data indicators::All");
  //project->execute("LoadObjectFromFile","C:/data/Shortcourse/Petroleum/Fluvial.prj/well-log data::All");
  project->execute("LoadObjectFromFile","C:/data/Shortcourse/Petroleum/Fluvial.prj/Simulation Grid::All");

  ModelTest t1(dynamic_cast<Root_model*>(Root::instance()->model()));
  Filter_root_proxy_model* mm =new  Filter_root_proxy_model(); 
  ModelTest t2(mm);
  delete mm;
#endif
*/
	app.exec();

	//delete appli;
	Lib_initializer::release();

	Root::instance()->list_all(std::cout);

	return 0;
}

int main(int argc, char **argv) {
  /*
#ifdef _WIN32
  char* vtk_dir;
  vtk_dir = getenv("VTKDIR");
  std::string vtk_env_path = "VTK_AUTOLOAD_PATH=.;";
  if(strlen(vtk_dir) > 0) {
#ifdef _DEBUG
    vtk_env_path += std::string(vtk_dir)+"\\BIN\\DEBUG";
#else
     vtk_env_path += std::string(vtk_dir)+"\\BIN\\RELEASE";
#endif
     
  }
  //std::string vtk_env_path = "VTK_AUTOLOAD_PATH=.;"+std::string(vtk_dir)+"\\BIN\\RELEASE";
  //std::string vtk_env_path = "VTK_AUTOLOAD_PATH=.";
  putenv(vtk_env_path.c_str());
  //putenv("VTK_AUTOLOAD_PATH=.;"+vtk_dir+"\BIN\DEBUG");
#endif
*/
	if (argc == 1) {
		return gui_main(argc, argv);
	} else {
		// use no gui version
		return nogui_main(argc, argv);
	}
}

