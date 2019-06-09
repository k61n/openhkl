//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/globaloffsets.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/frames/globaloffsets.h"
#include "gui/models/session.h"

#include "apps/models/MetaTypes.h"
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "core/experiment/DataSet.h"
#include "core/utils/Units.h"

GlobalOffsets::GlobalOffsets(offsetMode mode) : QcrFrame {"adhoc_detectorOffsets"}
{
    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[ERROR] No experiment selected");
        return;
    }
    if (gSession->selectedExperiment()->data()->allData().empty()) {
        gLogger->log("[ERROR] No data loaded to selected experiment");
        return;
    }

    mode_ = mode;
    layout();
}

void GlobalOffsets::layout()
{
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* whole = new QVBoxLayout(this);
    QHBoxLayout* above = new QHBoxLayout;
    QVBoxLayout* left = new QVBoxLayout;
    selectedData = new QListWidget;
    auto all_data = gSession->selectedExperiment()->data()->allData();
    for (auto data : all_data) {
        QFileInfo fileinfo(QString::fromStdString(data->filename()));

        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole, QVariant::fromValue(data));
        selectedData->addItem(item);
    }
    selectedData->setSelectionMode(QAbstractItemView::MultiSelection);

    left->addWidget(selectedData);
    QGroupBox* fitparams = new QGroupBox("Fit parameters");
    QFormLayout* form = new QFormLayout(fitparams);
    iterations = new QcrSpinBox("adhoc_detectorOffsetsIterations", new QcrCell<int>(1000), 5);
    form->addRow("#iterations", iterations);
    tolerance =
        new QcrDoubleSpinBox("adhoc_detectorOffsetsTolerance", new QcrCell<double>(0.0), 6, 4);
    form->addRow("tolerance", tolerance);
    left->addWidget(fitparams);
    above->addLayout(left);
    offsets = new QTableWidget;
    offsets->setColumnCount(2);
    const auto& detector_gonio =
        gSession->selectedExperiment()->experiment()->diffractometer()->detector()->gonio();
    size_t n_axes = detector_gonio.nAxes();
    offsets->setRowCount(n_axes);
    for (size_t i = 0; i < n_axes; ++i) {
        const auto& axis = detector_gonio.axis(i);
        offsets->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(axis.name())));
    }

    offsets->setHorizontalHeaderItem(
        1, new QTableWidgetItem("Offset (" + QString(QChar(0x00B0)) + ")"));
    // offsets->setItemDelegateForColumn(1, new DoubleItemDelegate());

    tolerance->setCellValue(1.0e-6);

    above->addWidget(offsets);
    whole->addLayout(above);
    whole->addWidget(new QLabel("in the future, here is a sxplot..."));
    buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, Qt::Horizontal);
    whole->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::clicked, this, &GlobalOffsets::actionClicked);

    show();
}

void GlobalOffsets::fit()
{
    auto selected_items = selectedData->selectedItems();

    // No item selected, just return
    if (selected_items.empty()) {
        gLogger->log("[ERROR] No data selected for the fit.");
        return;
    }

    nsx::DataList selected_data;
    for (auto item : selected_items) {
        selected_data.push_back(item->data(Qt::UserRole).value<nsx::sptrDataSet>());
    }

    if (mode_ == offsetMode::DETECTOR) {
        // Fit the detector offsets with the selected data
        const auto* detector =
            gSession->selectedExperiment()->experiment()->diffractometer()->detector();
        auto fit_results =
            detector->fitGonioOffsets(selected_data, iterations->value(), tolerance->value());

        // The fit failed for whatever reason, return
        if (!fit_results.success) {
            gLogger->log("[ERROR] Could not fit the detector offsets.");
        }

        int comp(0);
        for (auto&& offset : fit_results.offsets) {
            QTableWidgetItem* offset_item = new QTableWidgetItem();
            offset_item->setData(Qt::DisplayRole, offset / nsx::deg);
            offsets->setItem(comp++, 1, offset_item);
        }
    } else if (mode_ == offsetMode::SAMPLE) {
        const auto& sample =
            gSession->selectedExperiment()->experiment()->diffractometer()->sample();
        auto fit_results =
            sample.fitGonioOffsets(selected_data, iterations->value(), tolerance->value());

        // The fit failed for whatever reason, return
        if (!fit_results.success) {
            gLogger->log("[ERROR] Could not fit the sample offsets.");
        }

        int comp(0);
        for (auto&& offset : fit_results.offsets) {
            QTableWidgetItem* offset_item = new QTableWidgetItem();
            offset_item->setData(Qt::DisplayRole, offset / nsx::deg);
            offsets->setItem(comp++, 1, offset_item);
        }
    } else {
        gLogger->log("[ERROR] invalide offset mode. Should be DETECTOR or SAMPLE");
    }

    //    plot->clearGraphs();

    //    QPen pen;
    //    pen.setColor(QColor("black"));
    //    pen.setWidth(2.0);

    //    plot->addGraph();
    //    plot->graph(0)->setPen(pen);

    //    // Get the cost function for this batch
    //    const auto &cost_function = fit_results.cost_function;

    //    std::vector<double> iterationsval(cost_function.size());
    //    std::iota(iterationsval.begin(), iterationsval.end(), 0);

    //    QVector<double> x_values = QVector<double>::fromStdVector(iterationsval);
    //    QVector<double> y_values = QVector<double>::fromStdVector(cost_function);

    //    plot->graph(0)->addData(x_values, y_values);

    //    plot->xAxis->setLabel("# iterations");
    //    plot->yAxis->setLabel("Cost function");

    //    plot->setNotAntialiasedElements(QCP::aeAll);

    //    QFont font;
    //    font.setStyleStrategy(QFont::NoAntialias);
    //    plot->xAxis->setTickLabelFont(font);
    //    plot->yAxis->setTickLabelFont(font);

    //    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom |
    //                               QCP::iSelectAxes | QCP::iSelectLegend |
    //                               QCP::iSelectPlottables);

    //    plot->rescaleAxes();

    //    plot->replot();
}

void GlobalOffsets::actionClicked(QAbstractButton* button)
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
