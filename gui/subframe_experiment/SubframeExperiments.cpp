//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeExperiments.cpp
//! @brief     Implements classes ExperimentsModel, ExperimentsView, SubframeExperiments
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/SubframeExperiments.h"

#include "gui/MainWin.h"
#include "gui/actions/Triggers.h"
#include "gui/models/Session.h"
#include <QCR/widgets/tables.h>
#include <QHBoxLayout>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

//  ***********************************************************************************************
//! @class ExperimentsModel

int ExperimentsModel::highlighted() const
{
    int selected = gSession->selectedExperimentNum();
    if (selected < 0)
        return -1;
    return selected;
}

QVariant ExperimentsModel::entry(int row, int col) const
{
    if (col == COL_NAME)
        return QString::fromStdString(gSession->experimentAt(row)->experiment()->name());
    if (col == COL_INSTRUMENT) {
        return QString::fromStdString(
            gSession->experimentAt(row)->experiment()->diffractometer()->name());
    }
    return {};
}

QString ExperimentsModel::tooltip(int row, int col) const
{
    nsx::sptrExperiment exp = gSession->experimentAt(row)->experiment();
    if (col == 2)
        return QString("Experiment %1 on instrument %2")
            .arg(QString::fromStdString(exp->name()))
            .arg(QString::fromStdString(exp->diffractometer()->name()));
    return {};
}

QVariant ExperimentsModel::headerData(int col, Qt::Orientation ori, int role) const
{
    if (ori != Qt::Horizontal)
        return {};
    if (role != Qt::DisplayRole)
        return {};
    if (col == COL_NAME)
        return "experiment";
    if (col == COL_INSTRUMENT)
        return "instrument";
    return {};
}


//  ***********************************************************************************************
//! @class ExperimentsView

ExperimentsView::ExperimentsView() : QcrCheckTableView {new ExperimentsModel {}}
{
    setSelectionMode(QAbstractItemView::NoSelection);
    onData();
}

void ExperimentsView::onData()
{
    setHeaderHidden(false);
    setColumnWidth(0, 0);
    setColumnWidth(1, 3 * dWidth());
    for (int i = 2; i < model_->columnCount(); ++i)
        setColumnWidth(i, 7. * dWidth());
    model_->refreshModel();
    emit model_->layoutChanged();
    updateScroll();
}


//  ***********************************************************************************************
//! @class SubframeExperiments
SubframeExperiments::SubframeExperiments() : QcrDockWidget {"Experiments"}
{
    view = new ExperimentsView();
    setWidget(view);
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->triggers->viewExperiment,
        SLOT(setChecked(bool)));
}

void SubframeExperiments::experimentChanged()
{
    view->onData();
}
