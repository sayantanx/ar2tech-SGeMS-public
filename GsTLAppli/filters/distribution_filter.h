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



#ifndef DISTRIBUTION_FILTER_H_
#define DISTRIBUTION_FILTER_H_

#include <GsTLAppli/filters/common.h>
#include <GsTLAppli/filters/filter.h>

#include <GsTLAppli/math/continuous_distribution.h>
#include <GsTLAppli/math/non_parametric_distribution.h>

#include <QDomElement>

class FILTERS_DECL Distribution_input_filter : public Input_filter
{
public:
  static Named_interface* create_new_interface( std::string& ); 


public:
  Distribution_input_filter(void);
  virtual ~Distribution_input_filter(void);

  virtual std::string filter_name() const {return"Distribution";}  
  virtual std::string file_extensions() const { return "*.dist.xml"; }

  virtual bool can_handle( const std::string& filename );
  virtual Named_interface* read( const std::string& filename, 
                              std::string* errors = 0 );

private:
  Named_interface* read_parametric_distribution(const QDomElement& dist_element );

  Named_interface* read_non_parametric_distribution(const QDomElement& dist_element );

};


class FILTERS_DECL Distribution_output_filter : public Output_filter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
  static QDomElement serialize_distribution(const Continuous_distribution* dist, QDomDocument& doc);
  
   
 public: 
  Distribution_output_filter(); 
  virtual ~Distribution_output_filter(); 
   
  virtual std::string filter_name() const { return "Distribution" ; }  
  virtual bool write( std::string outfile, const Named_interface* distribution_ni, 
                      std::string* errors = 0 ); 

  bool write( std::string outfile, std::vector<const Named_interface*> distributions_ni, 
                      std::string* errors = 0 ); 

  //Write all distributions
  bool write( std::string outfile, std::string* errors = 0 ); 
   
}; 


#endif

