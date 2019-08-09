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
#include <QStandardItemModel>

TabPeaks::TabPeaks() : QcrWidget {"peaks"}
{
    peak_list_combo = new QComboBox();
    peak_table = new PeaksTableView;

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(peak_list_combo);
    layout->addWidget(peak_table);

    type = new QLabel;
    file_name = new QLabel;
    kernel_name = new QLabel;
    parent_name = new QLabel;
    peak_num = new QLabel;
    valid = new QLabel;
    non_valid = new QLabel;

    QFormLayout* left_side = new QFormLayout;
    left_side->addRow("Type:", type);
    left_side->addRow("File:", file_name);
    left_side->addRow("Kernel:", kernel_name);
    left_side->addRow("Parent:  ", parent_name);

    QFormLayout* right_side = new QFormLayout;
    right_side->addRow("Peaks:", peak_num);
    right_side->addRow("Valid:", valid);
    right_side->addRow("Not valid:", non_valid);

    QHBoxLayout* meta_box = new QHBoxLayout;
    meta_box->addLayout(left_side);
    meta_box->addLayout(right_side);

    layout->addLayout(meta_box);

    // peaksTable->show();
    // // if (gSession->selectedExperimentNum() > 0) {
    // //     if (!gSession->selectedExperiment()->getPeakListNames().empty()) {
    // //         PeaksTableModel* model = new PeaksTableModel(
    // //             "adhoc_tabpeaksmodel", gSession->selectedExperiment()->experiment(),
    // //             gSession->selectedExperiment()->getPeaks(0)->peaks_);
    // //         peaksTable->setModel(model);
    // //     }
    // // }

    QObject::connect(
        peak_list_combo, SIGNAL(currentTextChanged(const QString&)), 
        this, SLOT(changedPeakSelection(const QString&)));
}

void TabPeaks::selectedPeaksChanged()
{
    PeakCollectionModel* model = gSession->selectedExperiment()->selected();

    if (model == nullptr)
        return;

    peak_table->setModel(model);

    // QString selectedPeaks = gSession->selectedExperiment()->getPeakListNames(1).at(i);
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

    QStandardItemModel* list_model = 
        gSession->selectedExperiment()->peakListModel();

    // PeakCollectionModel* peak_model = 
    //     gSession->selectedExperiment();
    
    peak_list_combo->setModel(list_model);
    selectedPeaksChanged();

}

void TabPeaks::changedPeakSelection(const QString& name)
{
    std::cout << name.toStdString()<< std::endl;
    gSession->selectedExperiment()->setSelected(name.toStdString());
    selectedPeaksChanged();
}



