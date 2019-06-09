//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/tab_peaks.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/panels/tab_peaks.h"
#include "gui/models/experimentmodel.h"
#include "gui/models/session.h"
#include <QCR/engine/logger.h>
#include <QTreeView>
#include <QVBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class TabPeaks

TabPeaks::TabPeaks() : QcrWidget {"peaks"}
{
    filtered = new QcrTabWidget("filteredPeaks");
    foundPeaksLists = new QcrComboBox("foundLists", new QcrCell<int>(0), []() {
        if (gSession->selectedExperimentNum() < 0)
            return QStringList {};
        return gSession->selectedExperiment()->peaks()->peaklistNames();
    });
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(foundPeaksLists);
    layout->addWidget(filtered);

    foundPeaksLists->setHook([this](int i) { slotSelectedListChanged(i); });

    setRemake([this]() {
        int numLists = foundPeaksLists->count();
        int selected = foundPeaksLists->doGetValue();
        if (selected < 0)
            selected = 0;
        PeakListsModel* model =
            gSession->selectedExperiment()->peaks()->selectedPeakLists(selected);
        if (numLists > 0) {
            if (filtered->count() != model->numberFilteredLists()) {
                int oldTabs = filtered->count();

                for (int j = 0; j < model->numberFilteredLists(); j++) {
                    FilteredPeaksModel* peaks = model->getPeaksAt(j);
                    filtered->addTab(new ListTab(peaks), peaks->getName());
                }
                for (int t = 0; t < oldTabs; t++)
                    filtered->removeTab(0);
            }
        }
    });
}

void TabPeaks::slotSelectedListChanged(int i)
{
    filtered->clear();
    PeakListsModel* model = gSession->selectedExperiment()->peaks()->selectedPeakLists(i);
    for (int j = 0; j < model->numberFilteredLists(); j++) {
        FilteredPeaksModel* peaks = model->getPeaksAt(j);
        filtered->addTab(new ListTab(peaks), peaks->getName());
    }
}

ListTab::ListTab(FilteredPeaksModel* filteredModel)
    : QcrWidget {"adhoc_" + filteredModel->getName()}
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    view = new PeaksTableView;
    PeaksTableModel* model = new PeaksTableModel(
        "adhoc_" + filteredModel->getName() + "Model", gSession->selectedExperiment()->experiment(),
        filteredModel->getPeaks());
    view->setModel(model);
    layout->addWidget(view);
}
