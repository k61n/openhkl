//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/subframe_experiments.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/subframe_experiments.h"
#include "nsxgui/gui/actions/triggers.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/models/session.h"
#include "nsxgui/gui/view/toggles.h"
#include <QCR/widgets/tables.h>
#include <QHBoxLayout>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

//  ***********************************************************************************************
//! @class FilesModel (local scope)

//! The model for FilesView

class ExperimentsModel : public CheckTableModel { // < QAbstractTableModel < QAbstractItemModel
public:
    ExperimentsModel() : CheckTableModel {"experiments"} {}

    enum { COL_CHECK = 1, COL_NAME, COL_INSTRUMENT, COL_ATTRS };

private:
    void setActivated(int i, bool on) final {}

    int highlighted() const final;
    void onHighlight(int i) final { gSession->selectExperiment(i); }
    bool activated(int i) const final { return i == gSession->selectedExperimentNum(); }
    Qt::CheckState state(int i) const final { return Qt::Unchecked; }

    int columnCount() const final { return COL_ATTRS; }
    int rowCount() const final { return gSession->numExperiments(); }
    QVariant entry(int, int) const final;
    QString tooltip(int, int) const final;
    QVariant headerData(int, Qt::Orientation, int) const final;
};

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
//! @class FilesView (local scope)

//! Main item in `SubframeFiles`: View and control the list of `DataFile`s

class ExperimentsView : public CheckTableView {
public:
    ExperimentsView();

private:
    ExperimentsModel* model() { return static_cast<ExperimentsModel*>(model_); }
    void onData() override;
};

ExperimentsView::ExperimentsView() : CheckTableView {new ExperimentsModel {}}
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
//! @class SubframeFiles
SubframeExperiments::SubframeExperiments() : QcrDockWidget {"Experiments"}
{
    setWidget(new ExperimentsView());
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewExperiment,
        SLOT(setChecked(bool)));
}
