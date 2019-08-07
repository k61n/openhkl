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
#include <QFormLayout>
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
    // if (gSession->selectedExperimentNum() > 0) {
    //     if (!gSession->selectedExperiment()->getPeakListNames().empty()) {
    //         PeaksTableModel* model = new PeaksTableModel(
    //             "adhoc_tabpeaksmodel", gSession->selectedExperiment()->experiment(),
    //             gSession->selectedExperiment()->getPeaks(0)->peaks_);
    //         peaksTable->setModel(model);
    //     }
    // }
    layout->addWidget(peaksTable);

    type = new QLabel;
    filename = new QLabel;
    kernelname = new QLabel;
    parentname = new QLabel;
    numPeaks = new QLabel;
    valid = new QLabel;
    nonValid = new QLabel;

    QHBoxLayout* metaBox = new QHBoxLayout;
    QFormLayout* leftSide = new QFormLayout;
    leftSide->addRow("listtype:", type);
    leftSide->addRow("file:", filename);
    leftSide->addRow("kernel:", kernelname);
    leftSide->addRow("parent:  ", parentname);

    QFormLayout* rightSide = new QFormLayout;
    rightSide->addRow("peaks:", numPeaks);
    rightSide->addRow("valid:", valid);
    rightSide->addRow("not valid:", nonValid);
    metaBox->addLayout(leftSide);
    metaBox->addLayout(rightSide);

    layout->addLayout(metaBox);
}

void TabPeaks::selectedListChanged(int i)
{
    QString selectedPeaks = gSession->selectedExperiment()->getPeakListNames(1).at(i);
    // gSession->selectedExperiment()->selectPeaks(selectedPeaks);
    // PeaksTableModel* model = dynamic_cast<PeaksTableModel*>(peaksTable->model());
    // const Peaks* peaks = gSession->selectedExperiment()->getPeaks(selectedPeaks);
    // if (!peaks)
    //     return;
    // if (!model) {
    //     model = new PeaksTableModel(
    //         "adhoc_tabpeaksmodel", gSession->selectedExperiment()->experiment(), peaks->peaks_);
    //     peaksTable->setModel(model);
    //     return;
    // }
    // model->setPeaks(peaks->peaks_);
    // listtype listType = peaks->type_;
    // switch (listType) {
    // case listtype::FILTERED : {
    //     type->setText("filtered");
    //     break;
    // }
    // case listtype::FOUND : {
    //     type->setText("found");
    //     break;
    // }
    // case listtype::PREDICTED : {
    //     type->setText("predicted");
    //     break;
    // }
    // }
    // filename->setText(peaks->file_);
    // kernelname->setText(peaks->convolutionkernel_);
    // numPeaks->setText(QString::number(peaks->numberPeaks()));
    // valid->setText(QString::number(peaks->numberValid()));
    // nonValid->setText(QString::number(peaks->numberInvalid()));
    // parentname->setText(peaks->parent);
}

void TabPeaks::selectedExperimentChanged()
{
    if (gSession->selectedExperimentNum() < 0)
        return;
    PeaksTableModel* model =
        new PeaksTableModel("adhoc_tabpeaksmodel", gSession->selectedExperiment()->experiment());
    peaksTable->setModel(model);
    if (gSession->selectedExperiment()->getPeakListNames().empty())
        return;
    // model->setPeaks(
    //             gSession->selectedExperiment()->getPeaks(foundPeaksLists->currentText())->peaks_);
}
