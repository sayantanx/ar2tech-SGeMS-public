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



#ifndef PYTHON_SCRIPT_H_
#define PYTHON_SCRIPT_H_

#include <utils/named_interface.h>
#include <actions/common.h>

#include <vector>

class ACTIONS_DECL Python_script: public Named_interface
{
public:
	static Named_interface* create_new_interface(std::string&);

	Python_script(const std::string& filename);
	virtual ~Python_script();

	std::string filename() const;

	void execute(const std::string grid_name, const std::vector<std::string> prop_names) const;

protected:
	std::string filename_;
};

#endif /* PYTHON_SCRIPT_H_ */
