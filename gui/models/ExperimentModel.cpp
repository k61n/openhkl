//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/ExperimentModel.cpp
//! @brief     Defines classes ExperimentTableModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/ExperimentModel.h"
#include "core/experiment/Experiment.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QColor>

ExperimentModel::ExperimentModel(QObject* parent) : QAbstractTableModel(parent) { }

int ExperimentModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 6;
}

int ExperimentModel::rowCount(const QModelIndex& /*parent*/) const
{
    return gSession->numExperiments();
}

QVariant ExperimentModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();

    if (row < 0 || rowCount() <= row)
        return QVariant();

    switch (role) {
        case Qt::DisplayRole: {
            return valueOutput(index);
        }
        case Qt::ForegroundRole: {
            return formatOutput(index);
        }
        case Qt::BackgroundRole: {
            if (row == gSession->currentProjectNum())
                return QColor(Qt::gray);
            return QVariant();
        }
        case Qt::ToolTipRole: {
            QString tooltip =
                QString::fromStdString(gSession->experimentAt(row)->experiment()->name());
            tooltip += " with data: " + gSession->experimentAt(row)->getDataNames().first();
            return tooltip;
        }
        default: return QVariant();
    }
}

bool ExperimentModel::indexIsValid(const QModelIndex& index) const
{
    return index.isValid() && (index.row() < rowCount());
}

Qt::ItemFlags ExperimentModel::flags(const QModelIndex& index) const
{
    if (!indexIsValid(index))
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index);
}

QVariant ExperimentModel::formatOutput(const QModelIndex& index) const
{
    int row = index.row();

    if (!gSession->experimentAt(row)->saved())
        return QColor(Qt::red);

    return QVariant();
}

QVariant ExperimentModel::valueOutput(const QModelIndex& index) const
{
    int col = index.column();
    int row = index.row();

    if (col < 0 || columnCount() <= col)
        return QVariant();

    switch (col) {
        case 0: {
            return QString::fromStdString(gSession->experimentAt(row)->experiment()->name());
        }
        case 1: {
            return QString::fromStdString(
                gSession->experimentAt(row)->experiment()->getDiffractometer()->name());
        }
        case 2: {
            return QVariant(gSession->experimentAt(row)->experiment()->numData());
        }
        case 3: {
            return QVariant(gSession->experimentAt(row)->experiment()->numPeakCollections());
        }
        case 4: {
            return QVariant(gSession->experimentAt(row)->experiment()->numUnitCells());
        }
        default: return QVariant();
    }
}

QVariant ExperimentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: {
                return QString("Name");
            }
            case 1: {
                return QString("Instrument");
            }
            case 2: {
                return QString("Data sets");
            }
            case 3: {
                return QString("Peak collections");
            }
            case 4: {
                return QString("Unit cells");
            }
            default: return QVariant();
        }
    } else {
        return QVariant(section + 1);
    }
}
