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




#include <qtplugins/grid_filter_delegate.h>
#include <appli/manager_repository.h>

#include<QEvent>



Grid_filter_region_delegate::Grid_filter_region_delegate( QObject *parent)
  : Grid_filter_abstract_delegate(parent), grid_(0)
{

}

Grid_filter_region_delegate::~Grid_filter_region_delegate()
{

}

 void Grid_filter_region_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
 {
   QStyledItemDelegate::paint(painter, option, index);
}



 QWidget *Grid_filter_region_delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const

 {
   //if( grid_ == 0 ) return  0;
   if(grid_name_.isEmpty()) return 0;
   SingleRegionSelector* region_selector = new SingleRegionSelector( parent );
   region_selector->show_regions( grid_name_ );
   return region_selector;
   //region_selector->show_regions( QString::fromStdString(grid_->name()) );

 }

void	Grid_filter_region_delegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const{
  SingleRegionSelector* edit = static_cast<SingleRegionSelector*>(editor);
  int i = edit->findText(index.data().toString());
  edit->setCurrentIndex(i);
}

void	Grid_filter_region_delegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const{
  SingleRegionSelector* edit = static_cast<SingleRegionSelector*>(editor);
  int i = edit->currentIndex();

  if(i==0) model->setData(index, QVariant(), Qt::EditRole );

  QString option = edit->itemText(i);
   
  model->setData(index, option, Qt::EditRole );

}


QSize Grid_filter_region_delegate::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
  /*
    if (qVariantCanConvert<QColor>(index.data())) {
        QColor color = qVariantValue<QColor>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    */
}

void Grid_filter_region_delegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }

void Grid_filter_region_delegate::set_grid(const Geostat_grid* grid){
  grid_ = grid;
  if(grid_) grid_name_ = QString::fromStdString(grid_->name());

}


void Grid_filter_region_delegate::set_grid(const QString& grid_name){
  grid_name_ = grid_name;
  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + grid_name.toStdString() );
  grid_ = dynamic_cast<Geostat_grid*>(ni.raw_ptr());
}

// 
//       ------------------------------
//


Grid_filter_coord_bounded_editor::Grid_filter_coord_bounded_editor(const Geostat_grid* grid, QWidget *parent)
:QFrame(parent)
{
  lower_bound_spin_ = new QDoubleSpinBox(this);
  upper_bound_spin_ = new QDoubleSpinBox(this);
  lower_bound_spin_->setMinimum(-9e10);
  lower_bound_spin_->setMaximum(9e10);
  lower_bound_spin_->setValue(0);
  upper_bound_spin_->setMinimum(-9e10);
  upper_bound_spin_->setMaximum(9e10);
  upper_bound_spin_->setValue(0);
  


  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->addWidget(new QLabel("Lower", this));
  layout->addWidget(lower_bound_spin_);
  layout->addStretch();
  layout->addWidget(new QLabel("Upper", this));
  layout->addWidget(upper_bound_spin_);
  layout->addStretch();

  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  this->setAutoFillBackground(true);

  //this->setFocusPolicy( Qt::StrongFocus );

}
/*
void Grid_filter_coord_bounded_editor::leaveEvent ( QEvent * event ) 
{
  //emit editingFinished();
}

void Grid_filter_coord_bounded_editor::focusOutEvent( QFocusEvent *   event )
{
 //   emit editingFinished();
}
*/
Grid_filter_coord_bounded_delegate::Grid_filter_coord_bounded_delegate( QObject *parent)
  : Grid_filter_abstract_delegate(parent), grid_(0)
{

}

Grid_filter_coord_bounded_delegate::~Grid_filter_coord_bounded_delegate()
{
}

 void Grid_filter_coord_bounded_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
 {
   QStyledItemDelegate::paint(painter, option, index);
}



 QWidget *Grid_filter_coord_bounded_delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const

 {
   Grid_filter_coord_bounded_editor* editor = new Grid_filter_coord_bounded_editor(grid_, parent);
         connect(editor, SIGNAL(editingFinished()),
                 this, SLOT(commitAndCloseEditor()));

   return editor;

 }

void	Grid_filter_coord_bounded_delegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const{
  Grid_filter_coord_bounded_editor* edit = static_cast<Grid_filter_coord_bounded_editor*>(editor);
  QStringList data = index.data().toString().split("<");  //format: low < X < up
  edit->set_lower_bound(data[0].toDouble());
  edit->set_upper_bound(data[2].toDouble());
}

void	Grid_filter_coord_bounded_delegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const{
  Grid_filter_coord_bounded_editor* edit = static_cast<Grid_filter_coord_bounded_editor*>(editor);

  QStringList data;
  data.append( QString("%1").arg(edit->get_lower_bound()) );
  data.append( QString("%1").arg(edit->get_upper_bound()) );
   
  model->setData(index, data, Qt::EditRole );

}


QSize Grid_filter_coord_bounded_delegate::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);

}

void Grid_filter_coord_bounded_delegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }


void Grid_filter_coord_bounded_delegate::commitAndCloseEditor()
{
    Grid_filter_coord_bounded_editor *editor = qobject_cast<Grid_filter_coord_bounded_editor *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void Grid_filter_coord_bounded_delegate::set_grid(const Geostat_grid* grid){
  grid_ = grid;
  if(grid_) grid_name_ = QString::fromStdString(grid_->name());

}


void Grid_filter_coord_bounded_delegate::set_grid(const QString& grid_name){
  grid_name_ = grid_name;
  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + grid_name.toStdString() );
  grid_ = dynamic_cast<Geostat_grid*>(ni.raw_ptr());
}



// 
//       ------------------------------
//


Grid_filter_threshold_property_editor::Grid_filter_threshold_property_editor(const Geostat_grid* grid, QWidget *parent)
:QFrame(parent)
{
  bound_spin_ = new QDoubleSpinBox(this);
  prop_selector_ = new SinglePropertySelector(this);
  QString grid_name = QString::fromStdString(grid->name());
  prop_selector_->show_properties( grid_name );

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->addWidget(prop_selector_);
  layout->addStretch(1);
  layout->addWidget(new QLabel("Threshold", this));
  layout->addWidget(bound_spin_);
  layout->addStretch(9);


  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  this->setAutoFillBackground(true);

  //this->setFocusPolicy( Qt::StrongFocus );

}


Grid_filter_threshold_property_delegate::Grid_filter_threshold_property_delegate( QObject *parent)
  : Grid_filter_abstract_delegate(parent), grid_(0)
{


}

Grid_filter_threshold_property_delegate::~Grid_filter_threshold_property_delegate()
{

}

 void Grid_filter_threshold_property_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
 {
   QStyledItemDelegate::paint(painter, option, index);
}



 QWidget *Grid_filter_threshold_property_delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const

 {
   Grid_filter_threshold_property_editor* editor = new Grid_filter_threshold_property_editor(grid_, parent);
         connect(editor, SIGNAL(editingFinished()),
                 this, SLOT(commitAndCloseEditor()));

   return editor;

 }

void	Grid_filter_threshold_property_delegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const{
  Grid_filter_threshold_property_editor* edit = static_cast<Grid_filter_threshold_property_editor*>(editor);
  QStringList data;
  // could be less or greater
  QString current_data = index.data().toString();
  if( current_data.contains("<") ) {
    data = index.data().toString().split("<");
  }
  else if( current_data.contains(">") ) {
    data = index.data().toString().split(">");
  }
  else {
    return;
  }
  if( data.size() !=2 ) return;

  edit->set_property(data[0]);
  if(data[1].contains("nan",Qt::CaseInsensitive) ) {
    edit->set_bound(Grid_continuous_property::no_data_value);
  }
  else {
    edit->set_bound(data[1].toDouble());
  }
}

void	Grid_filter_threshold_property_delegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const{
  Grid_filter_threshold_property_editor* edit = static_cast<Grid_filter_threshold_property_editor*>(editor);

  QStringList data;

  data.append( QString("%1").arg(edit->get_property()) );
  data.append( QString("%1").arg(edit->get_bound()) );
   
  model->setData(index, data, Qt::EditRole );

}


QSize Grid_filter_threshold_property_delegate::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
  /*
    if (qVariantCanConvert<QColor>(index.data())) {
        QColor color = qVariantValue<QColor>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    */
}

void Grid_filter_threshold_property_delegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }


void Grid_filter_threshold_property_delegate::commitAndCloseEditor()
{
    Grid_filter_coord_bounded_editor *editor = qobject_cast<Grid_filter_coord_bounded_editor *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

void Grid_filter_threshold_property_delegate::set_grid(const Geostat_grid* grid){
  grid_ = grid;
  grid_name_ = QString::fromStdString(grid_->name() );

}


void Grid_filter_threshold_property_delegate::set_grid(const QString& grid_name){
  grid_name_ = grid_name;
  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + grid_name.toStdString() );
  grid_ = dynamic_cast<Geostat_grid*>(ni.raw_ptr());
}

//
//---------------------------------------------
//


Grid_filter_categorical_property_editor::Grid_filter_categorical_property_editor(const Geostat_grid* grid, QWidget *parent)
:QFrame(parent)
{
  prop_selector_ = new SingleCategoricalPropertySelector(this);
  category_selector_ = new SingleCategorySelector( this);

  QString grid_name = QString::fromStdString(grid->name());
  prop_selector_->show_properties( grid_name );

  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->addWidget(prop_selector_);
  layout->addStretch(1);
  layout->addWidget(category_selector_);
  layout->addStretch(9);


  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  this->setAutoFillBackground(true);

  QObject::connect(prop_selector_, SIGNAL( categorical_property_selected( Grid_categorical_property*) ), 
                   category_selector_, SLOT(show_categories(  Grid_categorical_property*) ) );

  //this->setFocusPolicy( Qt::StrongFocus );

}


Grid_filter_cateorical_property_delegate::Grid_filter_cateorical_property_delegate( QObject *parent)
  : Grid_filter_abstract_delegate(parent), grid_(0)
{

}

Grid_filter_cateorical_property_delegate::~Grid_filter_cateorical_property_delegate()
{

}

 void Grid_filter_cateorical_property_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
 {
   QStyledItemDelegate::paint(painter, option, index);
}



 QWidget *Grid_filter_cateorical_property_delegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const

 {
   //if( grid_ == 0 ) return  0;
   if(grid_name_.isEmpty()) return 0;
   Grid_filter_categorical_property_editor* editor = new Grid_filter_categorical_property_editor(grid_, parent );
   return editor;

 }

void	Grid_filter_cateorical_property_delegate::setEditorData ( QWidget * editor, const QModelIndex & index ) const{
  Grid_filter_categorical_property_editor* edit = static_cast<Grid_filter_categorical_property_editor*>(editor);
  QString data = index.data().toString();
  QStringList params = data.split(" : ");
  if(params.size() !=2) return;
  edit->set_property( params.at(1) );
  edit->set_category( params.at(0) );
}

void	Grid_filter_cateorical_property_delegate::setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const{
  Grid_filter_categorical_property_editor* edit = static_cast<Grid_filter_categorical_property_editor*>(editor);

  QStringList data;

  data.append( QString("%1").arg(edit->get_property()) );
  data.append( QString("%1").arg(edit->get_category()) );

  model->setData(index, data, Qt::EditRole );

}


QSize Grid_filter_cateorical_property_delegate::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);

}

void Grid_filter_cateorical_property_delegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }

void Grid_filter_cateorical_property_delegate::set_grid(const Geostat_grid* grid){
  grid_ = grid;
  if(grid_) grid_name_ = QString::fromStdString(grid_->name());

}


void Grid_filter_cateorical_property_delegate::set_grid(const QString& grid_name){
  grid_name_ = grid_name;
  SmartPtr< Named_interface > ni = 
    Root::instance()->interface( gridModels_manager + "/" + grid_name.toStdString() );
  grid_ = dynamic_cast<Geostat_grid*>(ni.raw_ptr());
}
