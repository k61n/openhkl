//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/frames/unitcellwidget.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_FRAMES_UNITCELLWIDGET_H
#define NSXGUI_GUI_FRAMES_UNITCELLWIDGET_H

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>
#include "core/crystal/CrystalTypes.h"

class UnitCellWidget : public QcrWidget {
    Q_OBJECT
public:
    UnitCellWidget(nsx::sptrUnitCell, const QString&);
    nsx::sptrUnitCell unitCell() const { return unitCell_; }

private:
    nsx::sptrUnitCell unitCell_;
};

#endif // NSXGUI_GUI_FRAMES_UNITCELLWIDGET_H
