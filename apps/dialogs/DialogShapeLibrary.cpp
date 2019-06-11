//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/dialogs/DialogShapeLibrary.cpp
//! @brief     Implements class DialogShapeLibrary
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include <QHeaderView>
#include <QLayout>
#include <QStatusBar>

#include "core/experiment/DataSet.h"
#include "core/shape/Profile3D.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/shape/ShapeLibrary.h"
#include "base/logger/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/quantify/PeakCoordinateSystem.h"
#include "core/merge_and_filter/PeakFilter.h"

#include "apps/models/CollectedPeaksModel.h"
#include "apps/models/ExperimentItem.h"
#include "apps/views/ProgressView.h"

#include "apps/dialogs/DialogShapeLibrary.h"
#include "ui_DialogShapeLibrary.h"

DialogShapeLibrary::DialogShapeLibrary(
    ExperimentItem* experiment_item, nsx::sptrUnitCell unitCell, const nsx::PeakList& peaks,
    QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogShapeLibrary)
    , _unitCell(std::move(unitCell))
    , _peaks(peaks)
    , _cmap()
    , _library(nullptr)
{
    ui->setupUi(this);

    ui->preview->resize(400, 400);

    ui->kabsch->setStyleSheet("font-weight: normal;");
    ui->kabsch->setCheckable(true);

    // get list of datasets
    for (auto p : _peaks)
        _data.insert(p->data());

    connect(ui->calculate, SIGNAL(released()), this, SLOT(calculate()));
    connect(ui->build, SIGNAL(released()), this, SLOT(build()));

    // setup slider
    ui->drawFrame->setMaximum(0);
    ui->drawFrame->setMaximum(0);

    ui->x->setMaximum(10000);
    ui->y->setMaximum(10000);
    ui->frame->setMaximum(10000);
    ui->radius->setMaximum(10000);
    ui->nframes->setMaximum(10000);

    // calculate reasonable values of sigmaD and sigmaM
    Eigen::Matrix3d cov;
    cov.setZero();

    for (auto peak : peaks) {
        nsx::PeakCoordinateSystem coord(peak);
        auto shape = peak->shape();
        Eigen::Matrix3d J = coord.jacobian();
        cov += J * shape.inverseMetric() * J.transpose();
    }

    cov /= peaks.size();

    // check this
    ui->sigmaD->setValue(std::sqrt(0.5 * (cov(0, 0) + cov(1, 1))));
    ui->sigmaM->setValue(std::sqrt(cov(2, 2)));

    auto peaks_model =
        new CollectedPeaksModel(experiment_item->model(), experiment_item->experiment(), peaks);
    ui->peaks->setModel(peaks_model);
    ui->peaks->verticalHeader()->show();

    ui->peaks->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->peaks->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    connect(ui->drawFrame, SIGNAL(valueChanged(int)), this, SLOT(drawFrame(int)));
    connect(ui->peaks, &QTableView::clicked, [this](QModelIndex index) {
        selectTargetPeak(index.row());
    });
    connect(
        ui->peaks->verticalHeader(), SIGNAL(sectionClicked(int)), this,
        SLOT(selectTargetPeak(int)));
}

DialogShapeLibrary::~DialogShapeLibrary()
{
    delete ui;
}

void DialogShapeLibrary::selectTargetPeak(int row)
{
    auto model = dynamic_cast<CollectedPeaksModel*>(ui->peaks->model());

    auto& peaks = model->peaks();

    auto selected_peak = peaks[row];

    auto&& center = selected_peak->shape().center();

    ui->x->setValue(center[0]);
    ui->y->setValue(center[1]);
    ui->frame->setValue(center[2]);
}

void DialogShapeLibrary::build()
{
    nsx::PeakList fit_peaks;

    for (auto peak : _peaks) {
        if (!peak->enabled())
            continue;
        double d = 1.0 / peak->q().rowVector().norm();

        if (d > ui->dMax->value() || d < ui->dMin->value())
            continue;

        auto inten = peak->correctedIntensity();

        if (inten.value() <= ui->Isigma->value() * inten.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    auto nx = ui->nx->value();
    auto ny = ui->ny->value();
    auto nz = ui->nz->value();

    // update the frame slider if necessary
    if (ui->drawFrame->maximum() != nz)
        ui->drawFrame->setMaximum(nz - 1);

    nsx::AABB aabb;

    bool kabsch_coords = ui->kabsch->isChecked();

    auto peakScale = ui->peakScale->value();

    if (kabsch_coords) {
        auto sigmaD = ui->sigmaD->value();
        auto sigmaM = ui->sigmaM->value();
        Eigen::Vector3d sigma(sigmaD, sigmaD, sigmaM);
        aabb.setLower(-peakScale * sigma);
        aabb.setUpper(peakScale * sigma);
    } else {
        Eigen::Vector3d dx(nx, ny, nz);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }

    // free memory of old library
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(this);
    view.watch(handler);

    auto bkgBegin = ui->bkgBegin->value();
    auto bkgEnd = ui->bkgEnd->value();
    _library =
        nsx::sptrShapeLibrary(new nsx::ShapeLibrary(!kabsch_coords, peakScale, bkgBegin, bkgEnd));

    nsx::ShapeIntegrator integrator(_library, aabb, nx, ny, nz);
    integrator.setHandler(handler);

    for (auto data : _data) {
        nsx::info() << "Fitting profiles in dataset " << data->filename();
        integrator.integrate(
            fit_peaks, data, _library->peakScale(), _library->bkgBegin(), _library->bkgEnd());
    }
    nsx::info() << "Done fitting profiles";

    _library = integrator.library();

    nsx::info() << "Updating peak shape model...";
    _library->updateFit(1000);
    nsx::info() << "Done, mean pearson is " << _library->meanPearson();

    calculate();
}

void DialogShapeLibrary::calculate()
{
    if (!_library) {
        nsx::info() << "Error: must build shape library before calculating a mean profile";
        return;
    }

    auto nx = ui->nx->value();
    auto ny = ui->ny->value();
    auto nz = ui->nz->value();

    nsx::DetectorEvent ev(ui->x->value(), ui->y->value(), ui->frame->value());
    // update maximum value, used for drawing
    _profile = _library->meanProfile(ev, ui->radius->value(), ui->nframes->value());
    _maximum = 0;

    for (auto i = 0; i < nx; ++i) {
        for (auto j = 0; j < ny; ++j) {
            for (auto k = 0; k < nz; ++k)
                _maximum = std::max(_maximum, _profile(i, j, k));
        }
    }

    nsx::Ellipsoid e = _profile.ellipsoid();

    nsx::info() << "Mean profile has inertia tensor";
    nsx::info() << e.inverseMetric();

    // draw the updated frame
    drawFrame(ui->drawFrame->value());
}

void DialogShapeLibrary::drawFrame(int value)
{
    if (value < 0 || value >= _profile.shape()[2])
        throw std::runtime_error("DialogShapeLibrary::drawFrame(): invalid frame value");

    auto shape = _profile.shape();
    auto scene = ui->preview->scene();

    if (!scene) {
        scene = new QGraphicsScene();
        ui->preview->setScene(scene);
    }

    QImage img(shape[0], shape[1], QImage::Format_ARGB32);

    for (auto i = 0; i < shape[0]; ++i) {
        for (auto j = 0; j < shape[1]; ++j) {
            const double value = _profile.at(i, j, ui->drawFrame->value());
            auto color = _cmap.color(value, _maximum);
            img.setPixel(i, j, color);
        }
    }
    scene->clear();
    scene->setSceneRect(QRectF(0, 0, shape[0], shape[1]));
    scene->addPixmap(QPixmap::fromImage(img));
    ui->preview->fitInView(0, 0, shape[0], shape[1]);
}

const nsx::Profile3D& DialogShapeLibrary::profile()
{
    return _profile;
}

nsx::sptrShapeLibrary DialogShapeLibrary::library() const
{
    return _library;
}
