//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/TabPeaks.cpp
//! @brief     Implements classes ListTab, TabPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/panels/TabPeaks.h"

#include "gui/models/Session.h"
#include <QCR/engine/logger.h>
#include <QTreeView>
#include <QVBoxLayout>

//-------------------------------------------------------------------------------------------------
//! @class TabPeaks

TabPeaks::TabPeaks() : QcrWidget {"peaks"}
{
    foundPeaksLists = new QcrComboBox("adhoc_foundLists", new QcrCell<int>(0), []() {
        if (gSession->selectedExperimentNum() < 0)
            return QStringList {""};
        return gSession->selectedExperiment()->getPeakListNames(1);
    });
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(foundPeaksLists);
    foundPeaksLists->setHook([this](int i) { selectedListChanged(i); });

    peaksTable = new PeaksTableView;
    peaksTable->show();
    if (gSession->selectedExperimentNum() > 0) {
        if (!gSession->selectedExperiment()->getPeakListNames().empty()) {
            PeaksTableModel* model =
                    new PeaksTableModel("adhoc_tabpeaksmodel",
                                        gSession->selectedExperiment()->experiment(),
                                        gSession->selectedExperiment()->getPeaks(0));
            peaksTable->setModel(model);
        }
    }
    layout->addWidget(peaksTable);
}

void TabPeaks::selectedListChanged(int i)
{
    Q_UNUSED(i)

    QString selectedPeaks = foundPeaksLists->currentText();
    gSession->selectedExperiment()->selectPeaks(selectedPeaks);
    PeaksTableModel* model = dynamic_cast<PeaksTableModel*>(peaksTable->model());
    if (!model) {
        model = new PeaksTableModel("adhoc_tabpeaksmodel",
                                    gSession->selectedExperiment()->experiment(),
                                    gSession->selectedExperiment()->getPeaks(selectedPeaks));
        peaksTable->setModel(model);
        return;
    }
    model->setPeaks(gSession->selectedExperiment()->getPeaks(selectedPeaks));
}

void TabPeaks::selectedExperimentChanged()
{
    if (gSession->selectedExperimentNum() < 0)
        return;
    PeaksTableModel* model = new PeaksTableModel("adhoc_tabpeaksmodel",
                                                 gSession->selectedExperiment()->experiment());
    peaksTable->setModel(model);
    if (gSession->selectedExperiment()->getPeakListNames().empty())
        return;
    model->setPeaks(gSession->selectedExperiment()->getPeaks(foundPeaksLists->currentText()));
}
