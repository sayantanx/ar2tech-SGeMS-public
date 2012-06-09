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



#include <GsTLAppli/filters/distribution_filter.h>

#include <QDomDocument>
#include <QFile>

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>


#include <QStringList>
#include <QDomText>
#include <QTextStream>
#include <QIODevice>

Named_interface* Distribution_input_filter::create_new_interface( std::string& ){
  return new Distribution_input_filter;
}


Distribution_input_filter::Distribution_input_filter(void)
  : Input_filter()
{
}


Distribution_input_filter::~Distribution_input_filter(void)
{
}


bool Distribution_input_filter::can_handle( const std::string& filename ) { 
  QFile file(filename.c_str());

  bool ok = false;

  /* If we can't open it, let's show an error message. */
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ok = false;
  }
  else {
    QDomDocument doc("Distributions"); 
    if (doc.setContent(&file)) {
      QDomElement root_elem = doc.documentElement(); 
      QString type = root_elem.tagName();
      if( type.startsWith("Distribution") ) ok = true;

    }
  }
  file.close();
  return ok;

}
Named_interface* Distribution_input_filter::read( const std::string& filename, 
                              std::string* errors ){
  QFile file(filename.c_str());
  /* If we can't open it, let's show an error message. */
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return 0;
  }
 
  QDomDocument doc("Distributions"); 
  if( !doc.setContent(&file) ) {
    file.close();
    return 0;
  }

  QDomElement root_elem = doc.documentElement(); 
  QString type = root_elem.tagName();
  if(root_elem.tagName() == "Distribution") {

      if( !root_elem.hasAttribute("type") || !root_elem.hasAttribute("name") ) return 0;
      if (root_elem.attribute("type") == "Non-parametric") 
        return this->read_non_parametric_distribution(root_elem);
      else 
        return this->read_parametric_distribution(root_elem);
  }

  else if( root_elem.tagName() == "Distributions") {
    Named_interface* ni=0;
    QDomElement elem = root_elem.firstChildElement("Distribution");
    for( ; !elem.isNull();  elem = elem.nextSiblingElement("Distribution") ) {
      
      if( !elem.hasAttribute("type") || !elem.hasAttribute("name") ) return 0;
      if (elem.attribute("type") == "Non-parametric") 
        ni = this->read_non_parametric_distribution(elem);
      else 
        ni = this->read_parametric_distribution(elem);

    }

    return ni;
  }

  return 0;
}


Named_interface* Distribution_input_filter::read_parametric_distribution(const QDomElement& elem){
  QString name = elem.attribute("name");
  QString type = elem.attribute("type");

  if(name.isEmpty()  || type.isEmpty() ) return 0;

  // Not registered at first to ensure the correct parameter were provided
  SmartPtr<Named_interface> ni = Root::instance()->new_interface(type.toStdString(),continuous_distributions_manager+"/");    
  SmartPtr<Parametric_continuous_distribution> dist = SmartPtr<Parametric_continuous_distribution>(dynamic_cast<Parametric_continuous_distribution*>(ni.raw_ptr()));
  std::vector<std::string> pnames = dist->parameter_names();
  std::vector<float> params;
  bool ok = true;
  for(int i=0; i<pnames.size(); ++i) {
    //QString param_str = elem.attribute("name");
    float param = elem.attribute(pnames[i].c_str()).toFloat(&ok);
    if(ok) {
      params.push_back(param);
    }
  }
  if(params.size() != pnames.size() ) return 0;
  std::string final_name;
  ni = Root::instance()->new_interface(type.toStdString(), 
                                       continuous_distributions_manager+"/"+name.toStdString(), &final_name);

  Parametric_continuous_distribution* p_dist = dynamic_cast<Parametric_continuous_distribution*>(ni.raw_ptr());
  if(p_dist == 0) return 0;

  p_dist->initialize(params);

  return p_dist;
}

Named_interface* Distribution_input_filter::read_non_parametric_distribution(const QDomElement& elem ){

  QString name = elem.attribute("name");
  QString type = elem.attribute("type");

  if(name.isEmpty()  || type.isEmpty() || type != "Non-parametric") return 0;

  QDomElement p_elem = elem.firstChildElement("Cdf");
  QDomElement q_elem = elem.firstChildElement("Quantiles");
  if(p_elem.isNull()  || q_elem.isNull()) return 0;

  //QDomText p_text = p_elem.firstChild().toText();
//  QString p_str = p_elem.firstChild().toText().data();
//  QString q_str = q_elem.firstChild().toText().data();
 // QString p_str1 = p_text.
//  QString p_str = p_elem.nodeValue();
  QStringList p_list = p_elem.firstChild().toText().data().split(" ");
 // QString q_str = q_elem.nodeValue();
  QStringList q_list = q_elem.firstChild().toText().data().split(" ");
  if(q_list.size() != p_list.size() ) return 0;

  std::vector<double> p;
  std::vector<double> q;
  p.reserve(q_list.size());
  q.reserve(q_list.size());
  for(int i=0; i< q_list.size() ; ++i) {
    p.push_back(p_list.at(i).toDouble());
    q.push_back(q_list.at(i).toDouble());
  }


  std::string final_name;
  SmartPtr<Named_interface> ni = Root::instance()->new_interface("Non-parametric",
                                    continuous_distributions_manager+"/"+name.toStdString(), &final_name);

  Non_parametric_distribution* np_dist = dynamic_cast<Non_parametric_distribution*>(ni.raw_ptr());
  if(np_dist == 0) return 0;
  np_dist->initialize(q.begin(), q.end(), p.begin());


  QDomElement lti_elem = elem.firstChildElement("LowerTail");
  QString lti_type = lti_elem.attribute("type");
  if(lti_type == "No Extrapolation") {
    np_dist->lower_tail_interpolator( Tail_interpolator( new No_TI()) );
  }
  else if(lti_type == "Power-Lower") {
    float min = lti_elem.attribute("min").toFloat();
    float omega = lti_elem.attribute("omega").toFloat();
    np_dist->lower_tail_interpolator( Tail_interpolator( new Power_LTI(min, omega)) );
  }
  else  if(lti_type == "Exponential-Lower") {
    np_dist->lower_tail_interpolator( Tail_interpolator( new Exponential_LTI()) );
  }


  QDomElement uti_elem = elem.firstChildElement("UpperTail");
  QString uti_type = uti_elem.attribute("type");
  if(uti_type == "No Extrapolation") {
    np_dist->upper_tail_interpolator( Tail_interpolator( new No_TI()) );
  }
  else if(uti_type == "Power-Upper") {
    float max = uti_elem.attribute("max").toFloat();
    float omega = uti_elem.attribute("omega").toFloat();
    np_dist->upper_tail_interpolator( Tail_interpolator( new Power_UTI(max, omega)) );
  }
  else  if(uti_type == "Hyperbolic-Upper") {
    float omega = uti_elem.attribute("omega").toFloat();
    np_dist->upper_tail_interpolator( Tail_interpolator( new Hyperbolic_UTI(omega)) );
  }  

  return np_dist;

}


/*
 ------------------------
*/

Named_interface* Distribution_output_filter::create_new_interface( std::string& ){
  return new Distribution_output_filter;
}
   

QDomElement Distribution_output_filter::serialize_distribution(const Continuous_distribution* dist, QDomDocument& doc){
  
  QDomElement elem = doc.createElement("Distribution");
  elem.setAttribute("type",dist->type().c_str());
  elem.setAttribute("name",dist->name().c_str());

  if(!dist->is_parametric()) {
    const Non_parametric_distribution* np_dist = dynamic_cast<const Non_parametric_distribution*>(dist);

    Non_parametric_distribution::const_z_iterator it_z = np_dist->z_begin();
    Non_parametric_distribution::const_p_iterator it_p = np_dist->p_begin();

    int size = std::distance( it_z, np_dist->z_end());
        QStringList p_str;
    QStringList q_str;
    p_str.reserve(size);
    q_str.reserve(size);
    for( ; it_z != np_dist->z_end(); ++it_z, ++it_p) {
      p_str.append( QString("%1").arg(*it_p)  );
      q_str.append( QString("%1").arg(*it_z)  );
    }

    
    QDomElement p_dom = doc.createElement("Cdf");
    QDomText p_dom_text = doc.createTextNode(p_str.join(" "));
    p_dom.appendChild(p_dom_text);
    elem.appendChild(p_dom);

    QDomElement q_dom = doc.createElement("Quantiles");
    QDomText q_dom_text = doc.createTextNode(q_str.join(" "));
    q_dom.appendChild(q_dom_text);
    elem.appendChild(q_dom);


    
    

//    p_dom.setNodeValue( p_str.join(" ") );
//    q_dom.setNodeValue( q_str.join(" ") );

    QDomElement lti_dom = doc.createElement("LowerTail");
    Tail_interpolator lti = np_dist->lower_tail_interpolator();
    std::vector<std::string> param_names = lti.parameter_names();
    std::vector<float> param_values = lti.parameter_values();
    lti_dom.setAttribute("type",lti.type().c_str());
    for(int i=0; i<param_names.size(); ++i) {
      lti_dom.setAttribute(param_names[i].c_str(),param_values[i]);
    }

    QDomElement uti_dom = doc.createElement("UpperTail");
    Tail_interpolator uti = np_dist->upper_tail_interpolator();
    param_names = uti.parameter_names();
    param_values = uti.parameter_values();
    uti_dom.setAttribute("type",uti.type().c_str());
    for(int i=0; i<param_names.size(); ++i) {
      uti_dom.setAttribute(param_names[i].c_str(),param_values[i]);
    }

    elem.appendChild(lti_dom);
    elem.appendChild(uti_dom);



  }
  else {
    const Parametric_continuous_distribution* p_dist = dynamic_cast<const Parametric_continuous_distribution*>(dist);
    std::vector<std::string> param_names = p_dist->parameter_names();
    std::vector<float> param_values = p_dist->parameter_values();
    for(int i=0; i<param_names.size(); ++i) {
      elem.setAttribute(param_names[i].c_str(),param_values[i]);
    }
  }

  return elem;

}

Distribution_output_filter::Distribution_output_filter(){
}
Distribution_output_filter::~Distribution_output_filter(){
}
  

bool Distribution_output_filter::write( std::string outfile, const Named_interface* ni, 
                      std::string* errors )
{
  const Continuous_distribution* c_dist = dynamic_cast<const Continuous_distribution*>(ni);

  QDomDocument doc("Distribution");
  QDomElement d_elem;
  d_elem = this->serialize_distribution(c_dist, doc);
  if(d_elem.isNull()) return false;
  doc.appendChild(d_elem);
  

  QFile file(outfile.c_str());
  if ( file.open( QIODevice::WriteOnly ) ) 
  {
    QTextStream text_stream(&file);
    text_stream << doc.toString() ;
    file.close();
    return true;
  }
  return false;


}


bool Distribution_output_filter::write( std::string outfile, std::vector<const Named_interface*> distributions_ni, 
                    std::string* errors )
{
  QDomDocument doc("Distribution");
  QDomElement root = doc.createElement("Distributions");
  doc.appendChild(root);

  for(int i = 0; i<distributions_ni.size(); ++i) {
    const Continuous_distribution* c_dist = dynamic_cast<const Continuous_distribution*>(distributions_ni[i]);
    if(c_dist==0) continue;
    QDomElement elem = this->serialize_distribution(c_dist,doc);
    if(elem.isNull()) continue;
    root.appendChild(elem);
  }


  QFile file(outfile.c_str());
  if ( file.open( QIODevice::WriteOnly ) ) 
  {
    QTextStream text_stream(&file);
    text_stream << doc.toString() ;
    file.close();
    return true;
  }
  return false;

}

//Write all distributions
bool Distribution_output_filter::write( std::string outfile, std::string* errors ) 
{

  QString outfile_str(outfile.c_str());
  if(!outfile_str.endsWith(".xml")) {
    outfile_str.append( ".dist.xml" );
  }
  outfile = outfile_str.toStdString();


  std::vector<const Named_interface*> dists_ni;
  Manager* dist_dir = dynamic_cast<Manager*>(Root::instance()->interface( continuous_distributions_manager).raw_ptr());
  Manager::interface_iterator it = dist_dir->begin_interfaces();
  for( ; it != dist_dir->end_interfaces(); ++it ) {
    dists_ni.push_back(it->raw_ptr());
  }

  return this->write(outfile, dists_ni, errors);

}
