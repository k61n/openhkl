//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/frames/GlobalOffsetsFrame.cpp
//! @brief     Implements classes offsetMode, GlobalOffsetsFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/GlobalOffsetsFrame.h"
#include "base/utils/Units.h"
#include "core/algo/GonioFit.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

GlobalOffsetsFrame::GlobalOffsetsFrame(offsetMode mode) : QFrame()
{
    if (gSession->currentProjectNum() < 0) {
        // gLogger->log("[ERROR] No experiment selected");
        return;
    }
    if (gSession->currentProject()->getDataNames().empty()) {
        // gLogger->log("[ERROR] No data loaded to selected experiment");
        return;
    }

    mode_ = mode;
    layout();
}

void GlobalOffsetsFrame::layout()
{
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* whole = new QVBoxLayout(this);
    QHBoxLayout* above = new QHBoxLayout;
    QVBoxLayout* left = new QVBoxLayout;
    selectedData = new QListWidget;
    std::vector<nsx::sptrDataSet> all_data = gSession->currentProject()->allData();
    for (const nsx::sptrDataSet& data : all_data) {
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(data->name()));
        item->setData(Qt::UserRole, QVariant::fromValue(data));
        selectedData->addItem(item);
    }
    selectedData->setSelectionMode(QAbstractItemView::MultiSelection);

    left->addWidget(selectedData);
    QGroupBox* fitparams = new QGroupBox("Fit parameters");
    QFormLayout* form = new QFormLayout(fitparams);

    iterations = new QSpinBox();
    iterations->setMaximum(1000);
    iterations->setValue(5);
    form->addRow("#iterations", iterations);

    tolerance = new QDoubleSpinBox();
    tolerance->setMaximum(100);
    tolerance->setDecimals(6);
    tolerance->setValue(4);
    form->addRow("tolerance", tolerance);

    left->addWidget(fitparams);
    above->addLayout(left);
    offsets = new QTableWidget;
    offsets->setColumnCount(2);
    nsx::Gonio& detector_gonio =
        gSession->currentProject()->experiment()->getDiffractometer()->detector()->gonio();
    size_t n_axes = detector_gonio.nAxes();
    offsets->setRowCount(n_axes);
    for (size_t i = 0; i < n_axes; ++i) {
        const nsx::Axis& axis = detector_gonio.axis(i);
        offsets->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(axis.name())));
    }

    offsets->setHorizontalHeaderItem(
        1, new QTableWidgetItem("Offset (" + QString(QChar(0x00B0)) + ")"));
    // offsets->setItemDelegateForColumn(1, new DoubleItemDelegate());

    tolerance->setValue(1.0e-6);

    above->addWidget(offsets);
    whole->addLayout(above);
    plot = new SXPlot(this);
    whole->addWidget(plot);
    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, Qt::Horizontal);
    whole->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::clicked, this, &GlobalOffsetsFrame::actionClicked);

    show();
}

void GlobalOffsetsFrame::fit()
{
    QList<QListWidgetItem*> selected_items = selectedData->selectedItems();

    // No item selected, just return
    if (selected_items.empty()) {
        // gLogger->log("[ERROR] No data selected for the fit.");
        return;
    }

    nsx::DataList selected_data;
    for (QListWidgetItem* item : selected_items)
        selected_data.push_back(item->data(Qt::UserRole).value<nsx::sptrDataSet>());

    QVector<double> xValues;
    QVector<double> yValues;

    if (mode_ == offsetMode::DETECTOR) {
        // Fit the detector offsets with the selected data
        const nsx::Detector* detector =
            gSession->currentProject()->experiment()->getDiffractometer()->detector();
        const nsx::GonioFit fit_results = nsx::fitDetectorGonioOffsets(
            detector->gonio(), selected_data, iterations->value(), tolerance->value());

        // The fit failed for whatever reason, return
        if (!fit_results.success) {
            // gLogger->log("[ERROR] Could not fit the detector offsets.");
            return;
        }

        int comp(0);
        for (double offset : fit_results.offsets) {
            QTableWidgetItem* offset_item = new QTableWidgetItem;
            offset_item->setData(Qt::DisplayRole, offset / nsx::deg);
            offsets->setItem(comp++, 1, offset_item);
        }
        std::copy(
            fit_results.cost_function.begin(), fit_results.cost_function.end(),
            std::back_insert_iterator(yValues));

        xValues.resize(yValues.size());
        std::iota(xValues.begin(), xValues.end(), 0);
    } else if (mode_ == offsetMode::SAMPLE) {
        const nsx::Sample& sample =
            gSession->currentProject()->experiment()->getDiffractometer()->sample();
        nsx::GonioFit fit_results = nsx::fitSampleGonioOffsets(
            sample.gonio(), selected_data, iterations->value(), tolerance->value());

        // The fit failed for whatever reason, return
        if (!fit_results.success) {
            // gLogger->log("[ERROR] Could not fit the sample offsets.");
            return;
        }

        int comp(0);
        for (double offset : fit_results.offsets) {
            QTableWidgetItem* offset_item = new QTableWidgetItem;
            offset_item->setData(Qt::DisplayRole, offset / nsx::deg);
            offsets->setItem(comp++, 1, offset_item);
        }
        std::copy(
            fit_results.cost_function.begin(), fit_results.cost_function.end(),
            std::back_insert_iterator(yValues));
        xValues.resize(yValues.size());
        std::iota(xValues.begin(), xValues.end(), 0);
    } else {
        // gLogger->log("[ERROR] invalide offset mode. Should be DETECTOR or SAMPLE");
        return;
    }

    plot->clearGraphs();

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    plot->addGraph();
    plot->graph(0)->setPen(pen);

    // Get the cost function for this batch
    plot->graph(0)->addData(xValues, yValues);
    plot->xAxis->setLabel("# iterations");
    plot->yAxis->setLabel("Cost function");
    plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    plot->xAxis->setTickLabelFont(font);
    plot->yAxis->setTickLabelFont(font);
    plot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);
    plot->rescaleAxes();
    plot->replot();
}

void GlobalOffsetsFrame::actionClicked(QAbstractButton* button)
{
    auto button_role = buttons->standardButton(button);

    switch (button_role) {
        case QDialogButtonBox::StandardButton::Apply: {
            fit();
            break;
        }
        case QDialogButtonBox::StandardButton::Cancel: {
            close();
            break;
        }
        case QDialogButtonBox::StandardButton::Ok: {
            close();
            break;
        }
        default: {
            return;
        }
    }
}
