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



#ifndef CHART_MDI_AREA_H
#define CHART_MDI_AREA_H

#include <GsTLAppli/gui/common.h>

#include <QMdiArea>

class GUI_DECL Chart_mdi_area : public QMdiArea
{
  Q_OBJECT

public:
    Chart_mdi_area(QWidget *parent);
    ~Chart_mdi_area();

public slots:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

private:

  enum chart_mode { HISTO, SCATTER, QQ };

  int current_chart_mode_;
    
};

#endif // CHART_MDI_AREA_H
