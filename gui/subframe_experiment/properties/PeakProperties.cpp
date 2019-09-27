//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/PeakProperties.cpp
//! @brief     Implements classes ListTab, PeakProperties
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/properties/PeakProperties.h"

#include "gui/models/Session.h"
#include <QCR/engine/logger.h>
#include <QFormLayout>
#include <QTreeView>
#include <QVBoxLayout>
#include <QStandardItemModel>

PeakProperties::PeakProperties() : QWidget()
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
    left_side->addRow("Peaks:", peak_num);
    left_side->addRow("Valid:", valid);
    left_side->addRow("Not valid:", non_valid);

    QHBoxLayout* meta_box = new QHBoxLayout;
    meta_box->addLayout(left_side);

    layout->addLayout(meta_box);

    connect(
        peak_list_combo, static_cast<void (QComboBox::*) (int) >(&QComboBox::currentIndexChanged),
        this, &PeakProperties::selectedPeaksChanged
    );
}

void PeakProperties::refreshInput()
{
    peak_list_combo->blockSignals(true);
    peak_list_combo->clear();
    peak_list_combo->addItems(
        gSession->selectedExperiment()->getPeakListNames());
    peak_list_combo->blockSignals(false);

    if (!gSession->selectedExperiment()->getPeakListNames().isEmpty())
        selectedPeaksChanged();
}

void PeakProperties::selectedPeaksChanged()
{
    PeakCollectionModel* model = gSession->selectedExperiment()->peakModel(peak_list_combo->currentIndex());
    peak_table->setModel(model);

    if (!model){
        type->setText("");
        peak_num->setText("");
        valid->setText("");
        non_valid->setText("");
        return;
    }

    nsx::listtype listType = model->root()->peakCollection()->type();
    switch (listType) {
        case nsx::listtype::FILTERED : {
            type->setText("Filtered");
            break;
        }
        case nsx::listtype::FOUND : {
            type->setText("Found");
            break;
        }
        case nsx::listtype::PREDICTED : {
            type->setText("Predicted");
            break;
        }
    }

    peak_num->setText(QString::number(
        model->root()->peakCollection()->numberOfPeaks()));
    valid->setText(QString::number(
        model->root()->peakCollection()->numberOfValid()));
    non_valid->setText(QString::number(
        model->root()->peakCollection()->numberOfInvalid()));

}

void PeakProperties::selectedExperimentChanged()
{
    if (gSession->selectedExperimentNum() < 0)
        return;

    selectedPeaksChanged();
}
