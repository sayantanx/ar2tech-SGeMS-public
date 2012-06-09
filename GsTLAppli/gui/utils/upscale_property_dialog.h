#ifndef UPSCALE_PROPERTY_DIALOG_H
#define UPSCALE_PROPERTY_DIALOG_H

#include <QDialog>

class GridSelector;
class OrderedPropertySelector;
class SinglePropertyGroupSelector;
class QCheckBox;
class QRadioButton;

class Upscale_property_dialog : public QDialog
{
  Q_OBJECT

public:
    Upscale_property_dialog(QWidget *parent);
    ~Upscale_property_dialog();

protected slots:
  void create_and_close();
  void create();

private:

  GridSelector* target_grid_selector_;
  GridSelector* source_grid_selector_;
  OrderedPropertySelector* prop_selector_;
  SinglePropertyGroupSelector* group_selector_;
  QRadioButton* use_group_;
  QRadioButton* use_properties_;
  QCheckBox* compute_variance_;


  
    
};

#endif // UPSCALE_PROPERTY_DIALOG_H
