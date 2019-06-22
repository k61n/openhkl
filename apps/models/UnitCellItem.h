//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/UnitCellItem.h
//! @brief     Defines class UnitCellItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include "apps/models/InspectableTreeItem.h"

#include "core/instrument/InstrumentTypes.h"
#include "core/peak/Peak3D.h"

class QWidget;

class UnitCellItem : public InspectableTreeItem {
 public:
    UnitCellItem(nsx::sptrUnitCell unit_cell);
    ~UnitCellItem();

    virtual QVariant data(int role) const override;
    virtual void setData(const QVariant& value, int role) override;

    QWidget* inspectItem();

    void info() const;

 public slots:
    void openIndexingToleranceDialog();
    void openTransformationMatrixDialog();
    void openChangeUnitCellDialog();

    void openSpaceGroupDialog();

 private:
    nsx::sptrUnitCell _unit_cell;
};
