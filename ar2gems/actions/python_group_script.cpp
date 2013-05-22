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



#include "python_group_script.h"
#include <stdio.h>
#include <GsTLAppli/utils/gstl_messages.h>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

Named_interface* Python_group_script::create_new_interface(std::string& _filename)
{
	return new Python_group_script(_filename);
}

Python_group_script::Python_group_script(const std::string& _filename) :
	filename_(_filename)
{
}

Python_group_script::~Python_group_script()
{
}

std::string Python_group_script::filename() const
{
	return filename_;
}

void Python_group_script::execute(const std::string grid_name, const std::string group_name) const
{
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

	FILE* fp = fopen(filename_.c_str(), "r");
	if (!fp)
	{
		GsTLcerr << "can't open file " << filename_ << gstlIO::end;
		return;
	}

	PyObject* module = PyImport_AddModule("__main__");
	PyObject* dictionary = PyModule_GetDict(module);
	PyObject* dictionary_copy = PyDict_Copy(dictionary);

	PyRun_File(fp, filename_.c_str(), Py_file_input, dictionary_copy, dictionary_copy);

	PyObject* function = PyDict_GetItemString(dictionary_copy, "sgems_execute_group_action");
	if (PyCallable_Check(function))
	{
		PyObject* result = PyObject_CallFunction(function, "ss", grid_name.c_str(), group_name.c_str());

		//		if (NULL == result)
		//		{
		//			std::cout << "execution failed\n";
		//		}

		Py_XDECREF(result);
	}

	Py_XDECREF(dictionary_copy);
	fclose(fp);
}
