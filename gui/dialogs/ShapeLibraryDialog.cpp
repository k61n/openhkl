//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/ShapeLibraryDialog.cpp
//! @brief     Implements class ShapeLibraryDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/dialogs/ShapeLibraryDialog.h"
#include "gui/models/ExperimentModel.h"
#include "gui/models/PeaksTable.h"
#include "gui/models/Session.h"
#include <QCR/engine/logger.h>

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStatusBar>
#include <QVBoxLayout>

#include "core/experiment/DataSet.h"
#include "core/integration/Profile3D.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/integration/ShapeLibrary.h"
#include "base/logger/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/quantify/PeakCoordinateSystem.h"
#include "core/peak/PeakFilter.h"
#include "gui/frames/ProgressView.h"
#include "gui/models/ColorMap.h"


ShapeLibraryDialog::ShapeLibraryDialog() : QDialog {}
{
    setAttribute(Qt::WA_DeleteOnClose);

    if (gSession->selectedExperimentNum() < 0) {
        gLogger->log("[ERROR] No experiment selected");
        return;
    }
    if (gSession->selectedExperiment()->peaks()->allPeaks().empty()) {
        gLogger->log("[ERROR] No peaks in selected experiment");
        return;
    }
    _peaks = gSession->selectedExperiment()->peaks()->allPeaks();
    //_unitCell = gSession->selectedExperiment()->unitCells()->allUnitCells().at(0);
    for (auto p : _peaks)
        _data.insert(p->data());
    layout();
}

void ShapeLibraryDialog::layout()
{
    tabs = new QcrTabWidget("adhoc_ShapeLibraryTabs");
    libraryTab = new QcrWidget("adhoc_libraryTab");
    QFormLayout* form = new QFormLayout(libraryTab);
    nx = new QcrSpinBox("adhoc_nx", new QcrCell<int>(0), 5);
    ny = new QcrSpinBox("adhoc_ny", new QcrCell<int>(0), 5);
    nz = new QcrSpinBox("adhoc_nz", new QcrCell<int>(0), 5);
    form->addRow("nx", nx);
    form->addRow("ny", ny);
    form->addRow("nz", nz);
    kabsch = new QGroupBox("Kabsch coordinates");
    kabsch->setCheckable(true);
    QFormLayout* kabschform = new QFormLayout(kabsch);
    sigmaD = new QcrDoubleSpinBox("adhoc_sigmaD", new QcrCell<double>(0.0), 8, 2);
    sigmaM = new QcrDoubleSpinBox("adhoc_sigmaM", new QcrCell<double>(0.0), 8, 2);
    kabschform->addRow("sigmaD", sigmaD);
    kabschform->addRow("sigmaM", sigmaM);
    form->addRow(kabsch);
    minISigma = new QcrDoubleSpinBox("adhoc_minISigma", new QcrCell<double>(0.0), 8, 2);
    minD = new QcrDoubleSpinBox("adhoc_minD", new QcrCell<double>(0.0), 8, 2);
    maxD = new QcrDoubleSpinBox("adhoc_maxD", new QcrCell<double>(0.0), 8, 2);
    peakscale = new QcrDoubleSpinBox("adhoc_peakscale", new QcrCell<double>(0.0), 8, 2);
    backgroundbegin = new QcrDoubleSpinBox("adhoc_backgroundbegin", new QcrCell<double>(0.0), 8, 2);
    backgroundend = new QcrDoubleSpinBox("adhoc_backgroundend", new QcrCell<double>(0.0), 8, 2);
    form->addRow("min I/Sigma", minISigma);
    form->addRow("min d", minD);
    form->addRow("max d", maxD);
    form->addRow("peak scale", peakscale);
    form->addRow("background begin", backgroundbegin);
    form->addRow("background end", backgroundend);
    buildShapeLibrary = new QcrTextTriggerButton("adhoc_buildShapeLib", "Build shape library");
    form->addRow(buildShapeLibrary);
    tabs->addTab(libraryTab, "Library");

    previewTab = new QcrWidget("adhoc_previewTab");
    QVBoxLayout* vertical = new QVBoxLayout(previewTab);
    table = new QTableView;
    vertical->addWidget(table);
    QHBoxLayout* horizontal = new QHBoxLayout;
    QVBoxLayout* horileft = new QVBoxLayout;
    QFormLayout* horileftup = new QFormLayout;
    x = new QcrDoubleSpinBox("adhoc_X", new QcrCell<double>(0.0), 8, 2);
    y = new QcrDoubleSpinBox("adhoc_Y", new QcrCell<double>(0.0), 8, 2);
    frame = new QcrDoubleSpinBox("adhoc_frame", new QcrCell<double>(0.0), 8, 2);
    radius = new QcrDoubleSpinBox("adhoc_radius", new QcrCell<double>(0.0), 8, 2);
    nframes = new QcrDoubleSpinBox("adhoc_nFrames", new QcrCell<double>(0.0), 8, 2);
    horileftup->addRow("x", x);
    horileftup->addRow("y", y);
    horileftup->addRow("frame", frame);
    horileftup->addRow("radius", radius);
    horileftup->addRow("nframes", nframes);
    horileft->addLayout(horileftup);
    calculateMeanProfile =
        new QcrTextTriggerButton("adhoc_calcMeanProfile", "Calculate Mean Profile");
    horileft->addWidget(calculateMeanProfile);
    horizontal->addLayout(horileft);
    graphics = new QGraphicsView;
    drawFrame = new QSlider(previewTab);
    drawFrame->setObjectName(QStringLiteral("drawFrame"));
    drawFrame->setOrientation(Qt::Horizontal);
    QVBoxLayout* graphicsview = new QVBoxLayout;
    graphicsview->addWidget(graphics);
    graphicsview->addWidget(drawFrame);
    horizontal->addLayout(graphicsview);
    vertical->addLayout(horizontal);
    tabs->addTab(previewTab, "Preview");

    buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    QVBoxLayout* whole = new QVBoxLayout(this);
    whole->addWidget(tabs);
    whole->addWidget(buttons);

    // values
    Eigen::Matrix3d cov;
    cov.setZero();

    for (auto peak : _peaks) {
        nsx::PeakCoordinateSystem coord(peak);
        auto shape = peak->shape();
        Eigen::Matrix3d J = coord.jacobian();
        cov += J * shape.inverseMetric() * J.transpose();
    }
    cov /= _peaks.size();
    sigmaD->setCellValue(std::sqrt(0.5 * (cov(0, 0) + cov(1, 1))));
    sigmaM->setCellValue(std::sqrt(cov(2, 2)));

    auto peaks_model = new PeaksTableModel(
        "adhoc_shapeTable", gSession->selectedExperiment()->experiment(), _peaks);
    table->setModel(peaks_model);
    table->verticalHeader()->show();

    table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    table->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    connect(drawFrame, &QSlider::valueChanged, this, &ShapeLibraryDialog::drawframe);
    connect(
        table, &QTableView::clicked, [this](QModelIndex index) { selectTargetPeak(index.row()); });
    connect(
        table->verticalHeader(), &QHeaderView::sectionClicked, this,
        &ShapeLibraryDialog::selectTargetPeak);

    show();
}

void ShapeLibraryDialog::calculate()
{
    if (!_library) {
        gLogger->log("Error: must build shape library before calculating a mean profile");
        return;
    }

    auto nxval = nx->value();
    auto nyval = ny->value();
    auto nzval = nz->value();

    nsx::DetectorEvent ev(x->value(), y->value(), frame->value());
    // update maximum value, used for drawing
    _profile = _library->meanProfile(ev, radius->value(), nframes->value());
    _maximum = 0;

    for (auto i = 0; i < nxval; ++i) {
        for (auto j = 0; j < nyval; ++j) {
            for (auto k = 0; k < nzval; ++k)
                _maximum = std::max(_maximum, _profile(i, j, k));
        }
    }

    nsx::Ellipsoid e = _profile.ellipsoid();

    gLogger->log("Mean profile has inertia tensor");
    // nsx::info() << e.inverseMetric();

    // draw the updated frame
    drawframe(drawFrame->value());
}

void ShapeLibraryDialog::build()
{
    nsx::PeakList fit_peaks;

    for (auto peak : _peaks) {
        if (!peak->enabled())
            continue;
        double d = 1.0 / peak->q().rowVector().norm();

        if (d > maxD->value() || d < minD->value())
            continue;

        auto inten = peak->correctedIntensity();

        if (inten.value() <= minISigma->value() * inten.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    auto nxval = nx->value();
    auto nyval = ny->value();
    auto nzval = nz->value();

    // update the frame slider if necessary
    if (drawFrame->maximum() != nzval)
        drawFrame->setMaximum(nzval - 1);

    nsx::AABB aabb;

    bool kabsch_coords = kabsch->isChecked();

    auto peakScale = peakscale->value();

    if (kabsch_coords) {
        auto sigmaDval = sigmaD->value();
        auto sigmaMval = sigmaM->value();
        Eigen::Vector3d sigma(sigmaDval, sigmaDval, sigmaMval);
        aabb.setLower(-peakScale * sigma);
        aabb.setUpper(peakScale * sigma);
    } else {
        Eigen::Vector3d dx(nxval, nyval, nzval);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }

    // free memory of old library
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(this);
    view.watch(handler);

    auto bkgBegin = backgroundbegin->value();
    auto bkgEnd = backgroundend->value();
    _library =
        nsx::sptrShapeLibrary(new nsx::ShapeLibrary(!kabsch_coords, peakScale, bkgBegin, bkgEnd));

    nsx::ShapeIntegrator integrator(_library, aabb, nxval, nyval, nzval);
    integrator.setHandler(handler);

    for (auto data : _data) {
        gLogger->log(
            "[INFO]Fitting profiles in dataset " + QString::fromStdString(data->filename()));
        integrator.integrate(
            fit_peaks, data, _library->peakScale(), _library->bkgBegin(), _library->bkgEnd());
    }
    gLogger->log("[INFO]Done fitting profiles");

    _library = integrator.library();

    gLogger->log("[INFO]Updating peak shape model...");
    _library->updateFit(1000);
    gLogger->log("[INFO]Done, mean pearson is " + QString::number(_library->meanPearson()));

    calculate();
}

void ShapeLibraryDialog::drawframe(int value)
{
    if (value < 0 || value >= _profile.shape()[2])
        throw std::runtime_error("DialogShapeLibrary::drawFrame(): invalid frame value");

    auto shape = _profile.shape();
    auto scene = graphics->scene();

    if (!scene) {
        scene = new QGraphicsScene();
        graphics->setScene(scene);
    }

    QImage img(shape[0], shape[1], QImage::Format_ARGB32);

    for (auto i = 0; i < shape[0]; ++i) {
        for (auto j = 0; j < shape[1]; ++j) {
            const double value = _profile.at(i, j, drawFrame->value());
            auto color = _cmap.color(value, _maximum);
            img.setPixel(i, j, color);
        }
    }
    scene->clear();
    scene->setSceneRect(QRectF(0, 0, shape[0], shape[1]));
    scene->addPixmap(QPixmap::fromImage(img));
    graphics->fitInView(0, 0, shape[0], shape[1]);
}

void ShapeLibraryDialog::selectTargetPeak(int row)
{
    auto model = dynamic_cast<PeaksTableModel*>(table->model());

    auto& peaks = model->peaks();

    auto selected_peak = peaks[row];

    auto&& center = selected_peak->shape().center();

    x->setCellValue(center[0]);
    y->setCellValue(center[1]);
    frame->setCellValue(center[2]);
}
