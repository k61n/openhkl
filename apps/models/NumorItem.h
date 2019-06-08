//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/NumorItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <string>

#include <QJsonObject>

#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentTypes.h"

#include "apps/models/InspectableTreeItem.h"

class QWidget;

class NumorItem : public InspectableTreeItem {
public:
    explicit NumorItem(nsx::sptrDataSet data);

    ~NumorItem();

    virtual QVariant data(int role) const override;

    QWidget* inspectItem() override;

    void exportHDF5(const std::string& filename) const;

private:
    nsx::sptrDataSet _data;
};
