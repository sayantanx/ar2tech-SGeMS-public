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



#include "python_script.h"
#include <stdio.h>
#include <utils/gstl_messages.h>

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

Named_interface* Python_script::create_new_interface(std::string& _filename)
{
	return new Python_script(_filename);
}

Python_script::Python_script(const std::string& _filename) :
	filename_(_filename)
{
}

Python_script::~Python_script()
{
}

std::string Python_script::filename() const
{
	return filename_;
}

void Python_script::execute(const std::string grid_name, const std::vector<std::string> prop_names) const
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

	PyObject* function = PyDict_GetItemString(dictionary_copy, "sgems_execute_action");
	if (PyCallable_Check(function))
	{
		//		PyObject* properties = Py_BuildValue("[ss]", prop_names.at(0).c_str(), prop_names.at(0).c_str());
		//		if (NULL == properties)
		//		{
		//			std::cout << "building value failed\n";
		//		}
		//		PyObject* properties = PyList_New(prop_names.size());
		//		for (int i = 0; i < prop_names.size(); ++i)
		//		{
		//			PyList_SetItem(properties, i, PyString_FromString(prop_names.at(i).c_str()));
		//		}
		//		PyObject* result = PyObject_CallFunction(function, "s[o]", grid_name.c_str(), properties);

		PyObject* result = NULL;

		switch (prop_names.size())
		{
		case 1:
			result = PyObject_CallFunction(function, "s[s]", grid_name.c_str(), prop_names.at(0).c_str());
			break;
		case 2:
			result = PyObject_CallFunction(function, "s[ss]", grid_name.c_str(), prop_names.at(0).c_str(), prop_names.at(1).c_str());
			break;
		default:
			GsTLcerr << "Execution of python script failed.\n Cannot have more than 2 properties selected\n" << gstlIO::end;
			break;
		}

		//		if (NULL == result)
		//		{
		//			std::cout << "execution failed\n";
		//		}

		Py_XDECREF(result);
	}

	Py_XDECREF(dictionary_copy);
	fclose(fp);
}
