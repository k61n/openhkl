//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogPredictPeaks.cpp
//! @brief     Implements class DialogPredictPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "base/utils/ProgressHandler.h"
#include "core/shape/ShapeLibrary.h"
#include "tables/crystal/UnitCell.h"

#include "apps/dialogs/DialogPredictPeaks.h"
#include "apps/models/CollectedPeaksModel.h"
#include "apps/models/DataItem.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/LibraryItem.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/PeakListItem.h"
#include "apps/models/PeaksItem.h"
#include "ui_DialogPredictPeaks.h"

#include <QDebug>
#include <QtGlobal>

DialogPredictPeaks* DialogPredictPeaks::_instance = nullptr;

DialogPredictPeaks* DialogPredictPeaks::create(
    ExperimentItem* experiment_item, const nsx::UnitCellList& unit_cells, QWidget* parent)
{
    if (!_instance)
        _instance = new DialogPredictPeaks(experiment_item, unit_cells, parent);

    return _instance;
}

DialogPredictPeaks* DialogPredictPeaks::Instance()
{
    return _instance;
}

DialogPredictPeaks::DialogPredictPeaks(
    ExperimentItem* experiment_item, const nsx::UnitCellList& unit_cells, QWidget* parent)
    : QDialog(parent), _ui(new Ui::DialogPredictPeaks), _experiment_item(experiment_item)
{
    _ui->setupUi(this);

    setModal(false);

    setWindowModality(Qt::NonModal);

    setAttribute(Qt::WA_DeleteOnClose);

    for (auto unit_cell : unit_cells) {
        _ui->unitCells->addItem(
            QString::fromStdString(unit_cell->name()), QVariant::fromValue(unit_cell));
    }

    _peaks_model =
        new CollectedPeaksModel(_experiment_item->model(), _experiment_item->experiment(), {});
    _ui->predictedPeaks->setModel(_peaks_model);

    connect(
        _ui->actions, SIGNAL(clicked(QAbstractButton*)), this,
        SLOT(slotActionClicked(QAbstractButton*)));
}

DialogPredictPeaks::~DialogPredictPeaks()
{
    if (_peaks_model)
        delete _peaks_model;

    delete _ui;

    if (_instance)
        _instance = nullptr;
}

void DialogPredictPeaks::slotActionClicked(QAbstractButton* button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch (button_role) {
        case QDialogButtonBox::StandardButton::Apply: {
            predictPeaks();
            break;
        }
        case QDialogButtonBox::StandardButton::Cancel: {
            reject();
            break;
        }
        case QDialogButtonBox::StandardButton::Ok: {
            accept();
            break;
        }
        default: {
            return;
        }
    }
}

void DialogPredictPeaks::accept()
{
    auto& predicted_peaks = _peaks_model->peaks();

    if (!predicted_peaks.empty()) {
        auto peaks_item = _experiment_item->peaksItem();
        auto item = new PeakListItem(predicted_peaks);
        item->setText("Predicted peaks");
        peaks_item->appendRow(item);
    }

    QDialog::accept();
}

void DialogPredictPeaks::predictPeaks()
{
    qInfo() << "Started peak prediction...";

    auto data_item = _experiment_item->dataItem();
    nsx::DataList data = data_item->selectedData();

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    auto library = _experiment_item->libraryItem()->library();

    auto unit_cell = _ui->unitCells->currentData().value<nsx::sptrUnitCell>();

    auto&& d_min = _ui->dMinSpinBox->value();
    auto&& d_max = _ui->dMaxSpinBox->value();
    auto&& radius = _ui->radius->value();
    auto&& n_frames = _ui->nframes->value();
    auto&& min_neighbors = _ui->neighborSpinBox->value();
    auto&& interpolation = _ui->interpolation->currentIndex();

    nsx::PeakInterpolation peak_interpolation = static_cast<nsx::PeakInterpolation>(interpolation);

    nsx::PeakList predicted_peaks;

    int current_numor = 0;
    for (auto d : data) {
        qInfo() << "Predicting peaks for numor " << ++current_numor << " of " << data.size();

        auto&& predicted = nsx::predictPeaks(
            *library, d, unit_cell, d_min, d_max, radius, n_frames, min_neighbors,
            peak_interpolation);

        for (auto peak : predicted)
            predicted_peaks.push_back(peak);

        qInfo() << "Added " << predicted.size() << " predicted peaks.";
    }

    qInfo() << "Completed  peak prediction. Added " << predicted_peaks.size() << " peaks";

    _peaks_model->setPeaks(predicted_peaks);
}
