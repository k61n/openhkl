//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogUnitCellParameters.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QDialog>

#include "core/experiment/CrystalTypes.h"

namespace Ui {
class DialogUnitCellParameters;
}

class Widget;

class DialogUnitCellParameters : public QDialog {
    Q_OBJECT

public:
    explicit DialogUnitCellParameters(nsx::sptrUnitCell unitCell, QWidget* parent = 0);
    ~DialogUnitCellParameters();

public slots:

    void setUnitCellParameters();

private:
    Ui::DialogUnitCellParameters* ui;
    nsx::sptrUnitCell _unitCell;
};
