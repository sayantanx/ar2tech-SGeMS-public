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




#include <GsTLAppli/extra/qtplugins/grid_filter_delegate.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/appli/manager_repository.h>

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

}


void Grid_filter_region_delegate::set_grid(const QString& grid_name){
  grid_name_ = grid_name;
}

// 
//       ------------------------------
//


Grid_filter_coord_bounded_editor::Grid_filter_coord_bounded_editor(const Geostat_grid* grid, QWidget *parent)
:QFrame(parent)
{
  lower_bound_spin_ = new QDoubleSpinBox(this);
  upper_bound_spin_ = new QDoubleSpinBox(this);

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


  std::stringstream str;
  str<<"temp "<<rand()<<".txt";

  ostream_.open(str.str().c_str());
  //ostream_.open("temp.txt");

}

Grid_filter_coord_bounded_delegate::~Grid_filter_coord_bounded_delegate()
{
  ostream_.close();
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
  /*
    if (qVariantCanConvert<QColor>(index.data())) {
        QColor color = qVariantValue<QColor>(index.data());
        return starRating.sizeHint();
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
    */
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

}


void Grid_filter_coord_bounded_delegate::set_grid(const QString& grid_name){
  grid_name_ = grid_name;
}


bool Grid_filter_coord_bounded_delegate::eventFilter(QObject *object, QEvent *event)
{
    Grid_filter_coord_bounded_editor *editor = qobject_cast<Grid_filter_coord_bounded_editor*>(object);
    if (!editor)
        return false;

    QWidget *w_focused = QApplication::focusWidget();
    QEvent::Type t = event->type();

    std::string class_name = (w_focused!=0)?w_focused->metaObject()->className():std::string("000");
    ostream_<<w_focused<<" "<<class_name <<" "<< QString("%1").arg(t).toStdString()+"\n";

    if (event->type() == QEvent::KeyPress) {
        switch (static_cast<QKeyEvent *>(event)->key()) {
        case Qt::Key_Tab:
            emit commitData(editor);
            emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
            return true;
        case Qt::Key_Backtab:
            emit commitData(editor);
            emit closeEditor(editor, QAbstractItemDelegate::EditPreviousItem);
            return true;
        case Qt::Key_Enter:
        case Qt::Key_Return:

            QMetaObject::invokeMethod(this, "_q_commitDataAndCloseEditor",
                                      Qt::QueuedConnection, Q_ARG(QWidget*, editor));
            return false;
        case Qt::Key_Escape:
            // don't commit data
            emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
            break;
        default:
            return false;
        }
        if (editor->parentWidget())
            editor->parentWidget()->setFocus();
        return true;
    } else if (event->type() == QEvent::FocusOut || (event->type() == QEvent::Hide && editor->isWindow())) {
        //the Hide event will take care of he editors that are in fact complete dialogs
        if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
        //if (!editor->isActiveWindow() || (w_focused != editor)) {
            //QWidget *w = w_focused;
            QWidget *w = QApplication::focusWidget();
            while (w) { // don't worry about focus changes internally in the editor
                if (w == editor)
                    return false;
                w = w->parentWidget();
            }
            emit commitData(editor);
            emit closeEditor(editor, NoHint);
        }
    } else if (event->type() == QEvent::ShortcutOverride) {
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
            event->accept();
            return true;
        }
    }
    return false;
}

/*
bool Grid_filter_coord_bounded_delegate::editorEvent(QEvent *event,
                                QAbstractItemModel *model,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(option.state & QStyle::State_Enabled)
        || !(flags & Qt::ItemIsEnabled))
        return false;

    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;

    const QWidget *widget = QStyledItemDelegatePrivate::widget(option);
    QStyle *style = widget ? widget->style() : QApplication::style();

    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease)
        || (event->type() == QEvent::MouseButtonDblClick)
        || (event->type() == QEvent::MouseButtonPress)) {
        QStyleOptionViewItemV4 viewOpt(option);
        initStyleOption(&viewOpt, index);
        QRect checkRect = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &viewOpt, widget);
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (me->button() != Qt::LeftButton || !checkRect.contains(me->pos()))
            return false;

        if ((event->type() == QEvent::MouseButtonPress)
            || (event->type() == QEvent::MouseButtonDblClick))
            return true;

    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space
         && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    } else {
        return false;
    }

    Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
                            ? Qt::Unchecked : Qt::Checked);
    return model->setData(index, state, Qt::CheckStateRole);

}
*/