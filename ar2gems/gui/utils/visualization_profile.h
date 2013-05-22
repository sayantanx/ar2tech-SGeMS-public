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



/*
 * visualization_profile.h
 *
 *  Created on: May 11, 2011
 *      Author: aboucher
 */

#ifndef VISUALIZATION_PROFILE_H_
#define VISUALIZATION_PROFILE_H_

#include <GsTLAppli/utils/named_interface.h>
#include <GsTLAppli/gui/utils/colormap.h>

#include <QDomDocument>
#include <map>

/*
class Visualization_profile_grid: public Named_interface {
public :
	static Named_interface* create_new_interface(std::string& gridname);

public:
	Visualization_profile_grid(std::string grid_name);
	virtual ~Visualization_profile_grid();

	bool set_active_region(QString region_name);

	bool register_colormap( QString prop_name, QString Colormap );
	bool register_colormap( QString prop_name, QString Colorscale, float min, float max );
	bool register_threshold( QString prop_name, int id, float min, float max, bool is_enabled );
	bool unregister_threshold( QString prop_name, int id);
	bool update_threshold( QString prop_name, int id, float min, float max, bool is_enabled );

	bool remove_property(QString prop_name);
	bool rename_property(QString prop_old_name, QString prop_new_name);

	QDomElement property_profile(QString prop_name);
	QDomElement property_colormap(QString prop_name);
	QDomElement property_thresholds(QString prop_name);


	bool initialize_from_dom(QDomDocument);
	QDomDocument get_dom_properties();

	int clean_profile();

protected:
	QDomDocument dom_;
	QString grid_name_;

};
*/


class VisualizationProfile: public Named_interface {
public :
	static Named_interface* create_new_interface(std::string&);

public:
	VisualizationProfile();
	virtual ~VisualizationProfile();

	bool register_colormap( std::string grid_name, std::string prop_name,
						  Colormap* table );

	bool unregister_colormap( std::string grid_name, std::string prop_name);

	Colormap* get_colormap(std::string grid_name, std::string prop_name);

	bool initialize_from_dom(QDomDocument);
	QDomDocument get_dom_properties();

protected:
	std::map<std::string, Colormap*> cmaps_;

};
/*
class Visualization_profile_property: public Named_interface {
public :
	static Named_interface* create_new_interface(std::string&);

public:
	Visualization_profile_property();
	virtual ~Visualization_profile_property();

	bool initialize_from_dom(QDomDocument dom)=0;
	QDomDocument& get_dom()=0;

protected:
	QDomDocument dom_;
};
*/
/*
class Visualization_profile_continuous_property: public Visualization_profile_property {
public :
	static Named_interface* create_new_interface(std::string&);

public:
	Visualization_profile_continuous_property();
	virtual ~Visualization_profile_continuous_property();

	bool initialize_from_dom(QDomDocument dom)=0;
	QDomDocument& get_dom()=0;

protected:
	QDomDocument dom_;
};
*/
#endif /* VISUALIZATION_PROFILE_H_ */
