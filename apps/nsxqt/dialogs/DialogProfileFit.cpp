
#include <QLayout>
#include <QStatusBar>

#include <nsxlib/DataSet.h>
#include <nsxlib/FitProfile.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/ShapeIntegrator.h>
#include <nsxlib/Logger.h>

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

    // TODO: support multiple datasets for a single crystal
    if (_data.size() != 1) {
        throw std::runtime_error("DialogProfileFit: currently we only support one dataset at a time!!");
    }

    // setup slider
    ui->frame->setMaximum(0);
    ui->frame->setMaximum(0);

    connect(ui->frame, SIGNAL(sliderMoved(int)), this, SLOT(drawFrame(int)));
}

DialogProfileFit::~DialogProfileFit()
{
    delete ui;
}

void DialogProfileFit::calculate()
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

    auto data = *_data.begin();

    auto scale = ui->scale->value();
    auto nx = ui->nx->value();
    auto ny = ui->ny->value();
    auto nz = ui->nz->value();  
    auto subdivide = ui->subdivide->value();

    auto sigmaM = ui->sigmaM->value();
    auto sigmaD = ui->sigmaD->value();

    // update the frame slider if necessary
    if (ui->frame->maximum() != nz) {
        ui->frame->setMaximum(nz-1);
    }

    Eigen::Vector3d sigma(sigmaD, sigmaD, sigmaM);
    nsx::AABB aabb(-scale*sigma, scale*sigma);

    nsx::ShapeIntegrator integrator(aabb, nx, ny, nz);
    
    nsx::info() << "Fitting profiles...";
    integrator.integrate(fit_peaks, data, scale, scale+1, scale+2);
    nsx::info() << "Done fitting profiles";

    // update maximum value, used for drawing
    _profile = integrator.library()->meanShape();
    _maximum = 0;

    for (auto i = 0; i < nx; ++i) {
        for (auto j = 0; j < ny; ++j) {
            for (auto k = 0; k < nz; ++k) {
                _maximum = std::max(_maximum, _profile(i, j, k));
            }
        }
    }

    // draw the updated frame
    drawFrame(ui->frame->value());
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
            const double value = _profile(i, j, ui->frame->value());
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
