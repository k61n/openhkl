//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      nsxgui/gui/panels/tab_peaks.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "nsxgui/gui/panels/tab_peaks.h"
#include "nsxgui/gui/models/experimentmodel.h"
#include "nsxgui/gui/models/session.h"
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

    connect(foundPeaksLists, SIGNAL(currentIndexChanged), this, SLOT(slotSelectedListChanged));
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
    layout->addWidget(view);
}
