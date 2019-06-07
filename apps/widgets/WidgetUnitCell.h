//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/WidgetUnitCell.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QWidget>

#include <core/CrystalTypes.h>

namespace Ui {
class WidgetUnitCell;
}

class WidgetUnitCell : public QWidget {

    Q_OBJECT

public:
    WidgetUnitCell(nsx::sptrUnitCell);

    ~WidgetUnitCell();

    nsx::sptrUnitCell unitCell() const;

private:
    Ui::WidgetUnitCell* _ui;

    nsx::sptrUnitCell _unit_cell;
};
