//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_experiments.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H
#define NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H

#include <QCR/widgets/views.h>
#include <QCR/widgets/tables.h>
#include "gui/models/session.h"

//! The model for ExperimentsView

class ExperimentsModel : public CheckTableModel {
public:
    ExperimentsModel() : CheckTableModel {"experiments"} {}

    enum { COL_CHECK = 1, COL_NAME, COL_INSTRUMENT, COL_ATTRS };

private:
    void setActivated(int i, bool on) final {}

    int highlighted() const final;
    void onHighlight(int i) final { gSession->selectExperiment(i); }
    bool activated(int i) const final { return i == gSession->selectedExperimentNum(); }
    Qt::CheckState state(int) const final { return Qt::Unchecked; }

    int columnCount() const final { return COL_ATTRS; }
    int rowCount() const final { return gSession->numExperiments(); }
    QVariant entry(int, int) const final;
    QString tooltip(int, int) const final;
    QVariant headerData(int, Qt::Orientation, int) const final;
};

//! Main item in `SubframeExperiments`: View and control the list of `Experiment`s
class ExperimentsView : public CheckTableView {
public:
    ExperimentsView();
    void onData() override;
private:
    ExperimentsModel* model() { return static_cast<ExperimentsModel*>(model_); }
};


//! Part of the main window that controls the experiments.
class SubframeExperiments : public QcrDockWidget {
public:
    SubframeExperiments();
    void experimentChanged();
private:
    ExperimentsView* view;
};

#endif // NSXGUI_GUI_PANELS_SUBFRAME_EXPERIMENTS_H
