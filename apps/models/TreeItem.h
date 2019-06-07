//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/TreeItem.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QStandardItem>
#include <QVariant>

#include <core/Experiment.h>
#include <core/InstrumentTypes.h>

#include "SessionModel.h"

class QWidget;
class ExperimentItem;

class TreeItem : public QStandardItem {
public:
    explicit TreeItem();

    virtual ~TreeItem();

    virtual void setData(const QVariant& value, int role = Qt::UserRole + 1) override;

    virtual SessionModel* model() const;

    nsx::sptrExperiment experiment();

    ExperimentItem* experimentItem() const;
};
