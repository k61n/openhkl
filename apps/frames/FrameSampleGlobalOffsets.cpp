//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameSampleGlobalOffsets.cpp
//! @brief     Implements class FrameSampleGlobalOffsets
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>

#include <QFileInfo>
#include <QPen>

#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/experiment/GonioFit.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Sample.h"
#include "base/logger/Logger.h"
#include "base/utils/Units.h"

#include "apps/delegates/DoubleItemDelegate.h"
#include "apps/frames/FrameSampleGlobalOffsets.h"
#include "apps/models/DataItem.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/MetaTypes.h"

#include "ui_FrameSampleGlobalOffsets.h"

FrameSampleGlobalOffsets* FrameSampleGlobalOffsets::_instance = nullptr;

FrameSampleGlobalOffsets* FrameSampleGlobalOffsets::create(ExperimentItem* experiment_item)
{
    if (!_instance)
        _instance = new FrameSampleGlobalOffsets(experiment_item);

    return _instance;
}

FrameSampleGlobalOffsets* FrameSampleGlobalOffsets::Instance()
{
    return _instance;
}

FrameSampleGlobalOffsets::FrameSampleGlobalOffsets(ExperimentItem* experiment_item)
    : NSXQFrame(), _ui(new Ui::FrameSampleGlobalOffsets), _experiment_item(experiment_item)
{
    _ui->setupUi(this);

    // Fill the data list widgets
    auto all_data = _experiment_item->dataItem()->allData();
    for (auto data : all_data) {
        QFileInfo fileinfo(QString::fromStdString(data->filename()));

        QListWidgetItem* item = new QListWidgetItem(fileinfo.baseName());
        item->setData(Qt::UserRole, QVariant::fromValue(data));
        _ui->selected_data->addItem(item);
    }
    _ui->selected_data->setSelectionMode(QAbstractItemView::MultiSelection);

    // Fill the offset table with the name of the sample goniometer axes
    const auto& sample_gonio = _experiment_item->experiment()->diffractometer()->sample().gonio();
    size_t n_axes = sample_gonio.nAxes();
    _ui->offsets->setRowCount(n_axes);
    for (size_t i = 0; i < n_axes; ++i) {
        const auto& axis = sample_gonio.axis(i);
        _ui->offsets->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(axis.name())));
    }

    _ui->offsets->setHorizontalHeaderItem(
        1, new QTableWidgetItem("Offset (" + QString(QChar(0x00B0)) + ")"));
    _ui->offsets->setItemDelegateForColumn(1, new DoubleItemDelegate());

    _ui->tolerance->setValue(1.0e-6);

    connect(
        _ui->actions, SIGNAL(clicked(QAbstractButton*)), this,
        SLOT(slotActionClicked(QAbstractButton*)));
}

FrameSampleGlobalOffsets::~FrameSampleGlobalOffsets()
{
    delete _ui;

    if (_instance)
        _instance = nullptr;
}

void FrameSampleGlobalOffsets::slotActionClicked(QAbstractButton* button)
{
    auto button_role = _ui->actions->standardButton(button);

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

void FrameSampleGlobalOffsets::fit()
{
    auto selected_items = _ui->selected_data->selectedItems();

    // No item selected, just return
    if (selected_items.empty()) {
        nsx::error() << "No data selected for the fit.";
        return;
    }

    nsx::DataList selected_data;
    for (auto item : selected_items)
        selected_data.push_back(item->data(Qt::UserRole).value<nsx::sptrDataSet>());

    // Fit the sample offsets with the selected data
    const auto& sample = _experiment_item->experiment()->diffractometer()->sample();
    auto fit_results = fitSampleGonioOffsets(
        sample.gonio(), selected_data, _ui->n_iterations->value(), _ui->tolerance->value());

    // The fit failed for whatever reason, return
    if (!fit_results.success)
        nsx::error() << "Could not fit the sample offsets.";

    int comp(0);
    for (auto&& offset : fit_results.offsets) {
        QTableWidgetItem* offset_item = new QTableWidgetItem();
        offset_item->setData(Qt::DisplayRole, offset / nsx::deg);
        _ui->offsets->setItem(comp++, 1, offset_item);
    }

    _ui->plot->clearGraphs();

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    _ui->plot->addGraph();
    _ui->plot->graph(0)->setPen(pen);

    // Get the cost function for this batch
    const auto& cost_function = fit_results.cost_function;

    std::vector<double> iterations(cost_function.size());
    std::iota(iterations.begin(), iterations.end(), 0);

    QVector<double> x_values = QVector<double>::fromStdVector(iterations);
    QVector<double> y_values = QVector<double>::fromStdVector(cost_function);

    _ui->plot->graph(0)->addData(x_values, y_values);

    _ui->plot->xAxis->setLabel("# iterations");
    _ui->plot->yAxis->setLabel("Cost function");

    _ui->plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _ui->plot->xAxis->setTickLabelFont(font);
    _ui->plot->yAxis->setTickLabelFont(font);

    _ui->plot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    _ui->plot->rescaleAxes();

    _ui->plot->replot();
}
