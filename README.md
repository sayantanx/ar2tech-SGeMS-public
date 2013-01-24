ar2gems
=======

AR2Tech geostatistical libraries and software


------------------------------------
This project is managed and owned by Advanced Resources and Risk Technology, LLC (ar2tech).  
For any questions, please contact Alex Boucher (aboucher@ar2tech.com).

AR2GEMS is a branch of the open-source SGeMS software under its x-free license.  
This version is not an open-source and cannot be integrated into existing software or 
distributed without the explicit authorization of ar2tech.

If you would like to contribute code to this project you can either:
a)	License the new code with a BSD license
b)	Transfer copyright to ar2tech
If you are interested into another option please contact ar2tech.

Academic research:  There are no restrictions for academic purposes.  Note that only the 
plugins can be distributed by third-party, the software itself can only be distributed 
by ar2tech.  This is done to reduce fragmentation of the software.  

Commercial plugins: You are free to build proprietary plugins for commercial purposes 
within an organization (no distribution) or to be freely distributed (no requirement 
to release the source code).  Please contact ar2tech for licensing agreement if you 
intend to sell or distribute the plugins with a fee.

In case of doubt please contact Alex Boucher (aboucher@ar2tech.com).

----------Compiling SGeMS with Visual Studio 2010 on 64 bits--------------------
Note that Visual Studio SP1 must be installed.
Required external libraries:
Qt, VTK, Boost and Python

----------Compiling Qt 64 bits ----------------
1- Download qt source code (a zip file) from : http://qt-project.org/downloads
2- Unzip it (e.g. c:\Qt\4.8.3-x64)
3- Open the prompt shell for visual studio 2010 (
   go to the MS start->All Programs->Microsoft Visual studio 2010
   ->Visual Studio Tools->Visual Studio x64 Win64 Command Prompt (2010))
4- Go to the qt directory
5- Type:  configure -debug-and-release -no-webkit -platform win32-msvc2010 -no-script -no-scripttools -opensource 
6- Type : nmake

---------- Compiling VTK ---------------------------
1- Download and install cmake: http://www.cmake.org/
2- Get the vtk 6.0 source code either from the git or the website: http://vtk.org/VTK/resources/software.html
   SGeMS is currently built using the master branch of the github repository
3- Build the project files with with cmake or cmake-gui.  Be sure to select the Qt options.
4- Open the vtk project files into visual studio and built the release and debug version.

---------- Compiling Python 64 bits -----------------
Installing python 2.x from the installer only provide the release dll.  To get the 
debug version, download the source code, open the project and build the debug version.  
You can ignore all the errors.  Copy the debug .dll and .lib to the main python 
directory along the release version.

---------- Building ar2gems ---------------------------------------------
1- Get SGeMS from the github repository
2- Set the following environmental variable
QTDIR and QTDIRx64: path to qt installation direction
VTKDIR : path to vtk install directory
BOOSTDIR : path to the boost directory
PYTHONDIRx64: path to the 64 bits python install directory
AR2TECH_SGEMS_DIR : path to the source code
AR2TECH_GSTL_DIR : The GsTL library is now included in the main source code repository.  
                   The path is then AR2TECH_SGEMS_DIR\ar2GsTL
3- Open the visual studio solution and build the release and debug binaries
-------------------------------------------------------------


