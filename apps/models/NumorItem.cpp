//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/NumorItem.cpp
//! @brief     Implements class NumorItem
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <string>

#include <QJsonArray>

#include "base/geometry/AABB.h"
#include "base/logger/Logger.h"
#include "base/mask/BoxMask.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"

#include "apps/models/ExperimentItem.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/NumorItem.h"
#include "apps/tree/NumorPropertyWidget.h"

NumorItem::NumorItem(nsx::sptrDataSet data) : InspectableTreeItem(), _data(data)
{
    setText("Numor");

    QIcon icon(":/resources/numorIcon.png");
    setIcon(icon);

    setEditable(false);

    setDragEnabled(false);
    setDropEnabled(false);

    setSelectable(false);

    setCheckable(true);
}

NumorItem::~NumorItem() {}

QVariant NumorItem::data(int role) const
{
    switch (role) {
        case (Qt::UserRole): {
            return QVariant::fromValue(_data);
        }
    }
    return InspectableTreeItem::data(role);
}

void NumorItem::exportHDF5(const std::string& filename) const
{
    if (filename.empty())
        return;

    if (filename.compare(_data->filename()) == 0)
        return;

    try {
        _data->saveHDF5(filename);
    } catch (...) {
        nsx::error() << "The filename " << filename
                     << " could not be saved. Maybe a permission problem.";
    }
}

QWidget* NumorItem::inspectItem()
{
    return new NumorPropertyWidget(this);
}
