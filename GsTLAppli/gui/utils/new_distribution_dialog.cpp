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




#include <GsTLAppli/gui/utils/new_distribution_dialog.h>
#include <GsTLAppli/appli/root_model.h>
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <QAbstractProxyModel>

New_distribution_dialog::New_distribution_dialog(QWidget *parent)
    : QDialog(parent), dist_(0)
{
  ui.setupUi(this);

  this->setWindowTitle("Create new distribution");
  this->setWindowIcon(QIcon(":/icons/appli/Pixmaps/distributions.svg"));

  dist_type_tree_ = new Continuous_distribution_type_table(this);
  dist_tree_ = new Continuous_distribution_table(this);
  ui.dist_type_box->setLayout(new QVBoxLayout(this));
  ui.dist_name_box->setLayout(new QVBoxLayout(this));
  ui.dist_type_box->layout()->addWidget(dist_type_tree_);
  ui.dist_name_box->layout()->addWidget(dist_tree_);

  
  params_ = new Continuous_distribution_parameter_widget(this);
  QVBoxLayout* param_box_layout = new QVBoxLayout(this);
  param_box_layout->addWidget(params_);
  param_box_layout->addStretch();
  ui.param_box->setLayout(param_box_layout);
  
  chart_ = new Chart_distribution(this);
  ui.chart_frame->setLayout(new QVBoxLayout(this));
  ui.chart_frame->layout()->addWidget(chart_);

  connect(dist_type_tree_, SIGNAL(doubleClicked ( const QModelIndex & )), 
          this, SLOT( show_params_new(const QModelIndex &)  ) );

  connect(dist_tree_, SIGNAL(doubleClicked ( const QModelIndex & )), 
          this, SLOT( show_params_existing(const QModelIndex &)  ) );

  connect(params_, SIGNAL(distribution_changed(Continuous_distribution*)), 
          chart_, SLOT(set_distribution( Continuous_distribution* )) );

  connect(ui.apply, SIGNAL(clicked()), this, SLOT(create_edit_distribution()));
  connect(ui.apply_close, SIGNAL(clicked()), this, SLOT(create_edit_distribution_close()));
  connect(ui.close, SIGNAL(clicked()), this, SLOT(close_dialog()));

  //Initialize with the non-parametric distribution
  dist_ = new Non_parametric_distribution();
  params_->initialize(dist_);

}

New_distribution_dialog::~New_distribution_dialog()
{
  delete dist_;
}

void New_distribution_dialog::show_params_new(const QModelIndex& index){

  if(!index.isValid()) return;
  // Two types of parameters
  // 1) does not exist yet

  QAbstractProxyModel* proxy_model = dynamic_cast<QAbstractProxyModel*>(dist_type_tree_->model());
  QModelIndex src_index = proxy_model->mapToSource(index);

  GsTL_object_item* item = static_cast<GsTL_object_item*>(src_index.internalPointer());

  if( item->item_type() == "Factory" ) {
    // Need to get the parent (manager) to create a new non-managed object
    Manager* dist_manager = dynamic_cast<Manager*>(item->parent());
    if(dist_manager ==0) return;

    std::string dist_type = item->item_data(0).toString().toStdString();
    Named_interface* ni = dist_manager->new_interface_raw(dist_type,"");

    delete dist_;
    dist_ = dynamic_cast<Continuous_distribution*>(ni);
    params_->initialize(dist_);

    ui.apply->setEnabled(true);
    ui.apply_close->setEnabled(true);
    params_->setEnabled(true);

  }

}

void New_distribution_dialog::show_params_existing(const QModelIndex& index){
  if(!index.isValid()) return;
  // Two types of parameters
  // 1) does not exist yet

  QAbstractProxyModel* proxy_model = dynamic_cast<QAbstractProxyModel*>(dist_tree_->model());
  QModelIndex src_index = proxy_model->mapToSource(index);

  GsTL_object_item* item = static_cast<GsTL_object_item*>(src_index.internalPointer());

  if( item->item_type() == "ContinuousDistribution" ) {
    Named_interface* ni = Root::instance()->interface(continuous_distributions_manager+"/"+item->item_name().toStdString()).raw_ptr();

  ui.apply->setDisabled(true);
  ui.apply_close->setDisabled(true);

  params_->initialize( dynamic_cast<Continuous_distribution*>(ni) );
  params_->setDisabled(true);

  }

}


void New_distribution_dialog::create_edit_distribution(){

  QString name = params_->name();
  if(name.isEmpty()) return;

  Named_interface* ni = Root::instance()->new_interface(dist_->type(),continuous_distributions_manager+"/"+name.toStdString()).raw_ptr();
  if(ni == 0) return;

  Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>(ni);

  dist->copy(dist_);


}
void New_distribution_dialog::create_edit_distribution_close(){

  QString name = params_->name();
  if(name.isEmpty()) return;

  Named_interface* ni = Root::instance()->new_interface(continuous_distributions_manager+"/"+dist_->type(),name.toStdString()).raw_ptr();
  if(ni == 0) return;
  Continuous_distribution* dist = dynamic_cast<Continuous_distribution*>(ni);

  dist->copy(dist_);

  return this->done(0);

}

void New_distribution_dialog::close_dialog(){
  return this->done(0);
}