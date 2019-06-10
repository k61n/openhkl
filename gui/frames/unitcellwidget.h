//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/unitcellwidget.h
//! @brief     Defines class UnitCellWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_FRAMES_UNITCELLWIDGET_H
#define GUI_FRAMES_UNITCELLWIDGET_H

#include "core/crystal/UnitCell.h"

#include <QCR/widgets/controls.h>
#include <QCR/widgets/views.h>

class UnitCellWidget : public QcrWidget {
    Q_OBJECT
public:
    UnitCellWidget(nsx::sptrUnitCell, const QString&);
    nsx::sptrUnitCell unitCell() const { return unitCell_; }

private:
    nsx::sptrUnitCell unitCell_;
};

#endif // GUI_FRAMES_UNITCELLWIDGET_H
