
#include <QLayout>
#include <QStatusBar>

#include <nsxlib/DataSet.h>
#include <nsxlib/FitProfile.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/ShapeIntegrator.h>
#include <nsxlib/ShapeLibrary.h>
#include <nsxlib/Logger.h>

#include "ProgressView.h"

#include "DialogProfileFit.h"
#include "ui_ProfileFitDialog.h"

DialogProfileFit::DialogProfileFit(nsx::sptrExperiment experiment,
                                    nsx::sptrUnitCell unitCell,
                                    const nsx::PeakList& peaks,
                                    QWidget *parent):
    QDialog(parent),
    ui(new Ui::ProfileFitDialog),
    _experiment(std::move(experiment)),
    _unitCell(std::move(unitCell)),
    _peaks(peaks), 
    _cmap()
{
    ui->setupUi(this);

    // get list of datasets
    for (auto p: _peaks) {
        _data.insert(p->data());
    }  

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

    connect(ui->drawFrame, SIGNAL(sliderMoved(int)), this, SLOT(drawFrame(int)));
}

DialogProfileFit::~DialogProfileFit()
{
    delete ui;
}

void DialogProfileFit::build()
{
    nsx::PeakList fit_peaks;

    for (auto peak: _peaks) {
        if (!peak->isSelected()) {
            continue;
        }
        double d = 1.0 / peak->q().rowVector().norm();

        if (d > ui->dMax->value() || d < ui->dMin->value()) {
            continue;
        }

        auto inten = peak->correctedIntensity();

        if (inten.value() <= ui->Isigma->value() * inten.sigma()) {
            continue;
        }
        fit_peaks.push_back(peak);
    }

    auto scale = ui->scale->value();
    auto nx = ui->nx->value();
    auto ny = ui->ny->value();
    auto nz = ui->nz->value();  
    auto subdivide = ui->subdivide->value();

    auto sigmaM = ui->sigmaM->value();
    auto sigmaD = ui->sigmaD->value();

    // update the frame slider if necessary
    if (ui->drawFrame->maximum() != nz) {
        ui->drawFrame->setMaximum(nz-1);
    }

    // todo: add to dialog
    bool detector_space = true;
    nsx::AABB aabb;

    if (detector_space) {
        Eigen::Vector3d dx(nx, ny, nz);
        dx *= -0.5;
        aabb.setLower(-dx);
        aabb.setUpper(dx);
    } else {
        Eigen::Vector3d sigma(sigmaD, sigmaD, sigmaM);
        aabb.setLower(-scale*sigma);
        aabb.setUpper(scale*sigma);
    }

    // free memory of old library
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(this);
    view.watch(handler);
    _library = nsx::sptrShapeLibrary(new nsx::ShapeLibrary);
    nsx::ShapeIntegrator integrator(aabb, nx, ny, nz, detector_space);    
    integrator.setHandler(handler);

    for (auto data: _data) {
        nsx::info() << "Fitting profiles in dataset " << data->filename();
        integrator.integrate(fit_peaks, data, scale, scale+1, scale+2);
    }
    nsx::info() << "Done fitting profiles";


    _library = integrator.library();

    nsx::info() << "Updating peak shape model...";
    _library->updateFit(1000);
    nsx::info() << "Done, mean pearson is " << _library->meanPearson();

    calculate();
}

void DialogProfileFit::calculate()
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
            for (auto k = 0; k < nz; ++k) {
                _maximum = std::max(_maximum, _profile(i, j, k));
            }
        }
    }

    nsx::Ellipsoid e = _profile.ellipsoid();

    nsx::info() << "Mean profile has inertia tensor";
    nsx::info() << e.inverseMetric();

    // for debugging purposes
    std::vector<nsx::Intensity> profile1d = _library->meanIntegratedProfile(ev, ui->radius->value(), ui->nframes->value());

    // draw the updated frame
    drawFrame(ui->drawFrame->value());
}

void DialogProfileFit::drawFrame(int value)
{
    if (value < 0 || value >= _profile.shape()[2]) {
        throw std::runtime_error("DialogProfileFit::drawFrame(): invalid frame value");
    }
   
    auto shape = _profile.shape();
    auto scene = ui->graphicsView->scene();

    if (!scene) {
        scene = new QGraphicsScene();
        ui->graphicsView->setScene(scene);
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
    ui->graphicsView->fitInView(0, 0, shape[0], shape[1]);
}

const nsx::FitProfile& DialogProfileFit::profile()
{
    return _profile;
}


nsx::sptrShapeLibrary DialogProfileFit::library() const
{
    return _library;
}