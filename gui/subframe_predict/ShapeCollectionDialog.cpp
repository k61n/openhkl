//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_predict/ShapeCollectionDialog.cpp
//! @brief     Implements class ShapeCollectionDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_predict/ShapeCollectionDialog.h"

#include "core/data/DataSet.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeCollection.h"
#include "gui/frames/ProgressView.h"
#include "gui/models/ColorMap.h"
#include "gui/models/Session.h"
#include "gui/MainWin.h" // gGui

#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QtGlobal>

ShapeCollectionDialog::ShapeCollectionDialog(
    nsx::PeakCollection* peak_collection, std::shared_ptr<nsx::ShapeCollectionParameters> params)
    : QDialog(), _peak_collection_model() , _peak_collection_item(),
      _collection_ptr(peak_collection), _params(params)
{
    _collection = std::make_unique<nsx::ShapeCollection>();

    setModal(true);
    setSizePolicies();
    setParametersUp();
    setPreviewUp();
    setUpParametrization(peak_collection);
    grabShapeCollectionParameters();

    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QHBoxLayout* widget_layout = new QHBoxLayout();
    widget_layout->addWidget(_parameter_widget);
    widget_layout->addWidget(_preview_widget);
    main_layout->addLayout(widget_layout);

    QHBoxLayout* button_layout = new QHBoxLayout();
    button_layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Fixed));
    QPushButton* accept_button = new QPushButton("Accept");
    button_layout->addWidget(accept_button);
    QPushButton* reject_button = new QPushButton("Cancel");
    button_layout->addWidget(reject_button);
    main_layout->addLayout(button_layout);

    connect(accept_button, &QPushButton::clicked, this, &ShapeCollectionDialog::accept);

    connect(reject_button, &QPushButton::clicked, this, &ShapeCollectionDialog::reject);

}

void ShapeCollectionDialog::setUpParametrization(nsx::PeakCollection* peak_collection)
{
    _peaks = peak_collection->getPeakList();
    for (nsx::Peak3D* peak : _peaks)
        _data.insert(peak->dataSet());

    _peak_collection_item.setPeakCollection(peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _table->setModel(&_peak_collection_model);
    _table->verticalHeader()->show();
    _table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    connect(_draw_frame, &QSlider::valueChanged, this, &ShapeCollectionDialog::drawFrame);
    connect(_table, &QTableView::clicked, [this](QModelIndex index) {
        selectTargetPeak(index.row());
    });
    connect(
        _table->verticalHeader(), &QHeaderView::sectionClicked, this,
        &ShapeCollectionDialog::selectTargetPeak);
}

void ShapeCollectionDialog::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);
}

void ShapeCollectionDialog::setParametersUp()
{
    // Set up the parameters
    _parameter_widget = new QWidget();
    QFormLayout* form = new QFormLayout(_parameter_widget);
    QString tooltip;

    _nx = new QSpinBox();
    _nx->setMinimum(5);
    _nx->setMaximum(10000);
    _nx->setMaximumWidth(10000);
    _nx->setSizePolicy(*_size_policy_widgets);
    tooltip = "Number of histogram bins in x direction";
    _nx->setToolTip(tooltip);

    _ny = new QSpinBox();
    _ny->setMinimum(5);
    _ny->setMaximum(10000);
    _ny->setMaximumWidth(10000);
    _ny->setSizePolicy(*_size_policy_widgets);
    tooltip = "Number of histogram bins in y direction";
    _ny->setToolTip(tooltip);

    _nz = new QSpinBox();
    _nz->setMinimum(5);
    _nz->setMaximum(10000);
    _nz->setMaximumWidth(10000);
    _nz->setSizePolicy(*_size_policy_widgets);
    tooltip = "Number of histogram bins in z (frame) direction";
    _nz->setToolTip(tooltip);

    _kabsch = new QGroupBox("Kabsch coordinates");
    _kabsch->setSizePolicy(*_size_policy_widgets);
    _kabsch->setCheckable(true);
    tooltip = "Toggle between Kabsch and detector coordinates";
    _kabsch->setToolTip(tooltip);

    QFormLayout* kabschform = new QFormLayout(_kabsch);

    _sigma_D = new QDoubleSpinBox();
    _sigma_D->setMaximum(10000);
    _sigma_D->setSingleStep(0.1);
    _sigma_D->setSizePolicy(*_size_policy_widgets);
    tooltip = "Variance due to beam divergence";
    _sigma_D->setToolTip(tooltip);

    _sigma_M = new QDoubleSpinBox();
    _sigma_M->setMaximum(10000);
    _sigma_M->setSingleStep(0.1);
    _sigma_M->setSizePolicy(*_size_policy_widgets);
    tooltip = "Variance due to crystal mosaicity";
    _sigma_M->setToolTip(tooltip);

    _min_I_sigma = new QDoubleSpinBox();
    _min_I_sigma->setMaximum(10000);
    _min_I_sigma->setSingleStep(1.0);
    _min_I_sigma->setSizePolicy(*_size_policy_widgets);
    tooltip = QString::fromUtf8("Minimum strength (I/\u03C3) of peak to include in average");
    _min_I_sigma->setToolTip(tooltip);

    _min_d = new QDoubleSpinBox();
    _min_d->setMaximum(10000);
    _min_d->setSingleStep(0.1);
    _min_d->setSizePolicy(*_size_policy_widgets);
    tooltip = QString::fromUtf8("Minimum d (\u212B) of peak to include in average");
    _min_d->setToolTip(tooltip);

    _max_d = new QDoubleSpinBox();
    _max_d->setMaximum(10000);
    _max_d->setSingleStep(0.1);
    _max_d->setSizePolicy(*_size_policy_widgets);
    tooltip = QString::fromUtf8("Maximum d (\u212B) of peak to include in average");
    _max_d->setToolTip(tooltip);

    _peak_end = new QDoubleSpinBox();
    _peak_end->setMaximum(10000);
    _peak_end->setSingleStep(0.1);
    _peak_end->setSizePolicy(*_size_policy_widgets);
    tooltip = "(sigmas) - scaling factor for peak region";
    _peak_end->setToolTip(tooltip);

    _background_begin = new QDoubleSpinBox();
    _background_begin->setMaximum(10000);
    _background_begin->setSingleStep(0.1);
    _background_begin->setSizePolicy(*_size_policy_widgets);
    tooltip = "(sigmas) - scaling factor for lower limit of background";
    _background_begin->setToolTip(tooltip);

    _background_end = new QDoubleSpinBox();
    _background_end->setMaximum(10000);
    _background_end->setSingleStep(0.1);
    _background_end->setSizePolicy(*_size_policy_widgets);
    tooltip = "(sigmas) - scaling factor for upper limit of background";
    _background_begin->setToolTip(tooltip);

    _build_collection = new QPushButton("Build shape collection");
    _build_collection->setSizePolicy(*_size_policy_widgets);
    tooltip = "<font>A shape collection is a collection of averaged peaks attached to a peak \
        collection. A shape is the averaged peak shape of a peak and its neighbours within a \
        specified cutoff.</font>";
    _build_collection->setToolTip(tooltip);

    form->addRow("histogram bins x:", _nx);
    form->addRow("histogram bins y:", _ny);
    form->addRow("histogram bins frames:", _nz);
    kabschform->addRow("Beam divergence " + QString(QChar(0x03C3)) + ":", _sigma_D);
    kabschform->addRow("Mosaicity " + QString(QChar(0x03C3)) + ":", _sigma_M);
    form->addRow(_kabsch);
    form->addRow("Minimum I/" + QString(QChar(0x03C3)) + ":", _min_I_sigma);
    form->addRow("Minimum d:", _min_d);
    form->addRow("Maximum d:", _max_d);
    form->addRow("Peak end:", _peak_end);
    form->addRow("Background begin:", _background_begin);
    form->addRow("Background end:", _background_end);
    form->addRow(_build_collection);

    connect(_build_collection, &QPushButton::clicked, this, &ShapeCollectionDialog::build);
}

void ShapeCollectionDialog::setPreviewUp()
{
    // Set up the preview
    _preview_widget = new QWidget();
    QVBoxLayout* vertical = new QVBoxLayout(_preview_widget);
    QString tooltip;

    _table = new QTableView;
    vertical->addWidget(_table);

    QHBoxLayout* horizontal = new QHBoxLayout;
    QVBoxLayout* left = new QVBoxLayout;
    QFormLayout* left_up = new QFormLayout;

    _x = new QDoubleSpinBox();
    _x->setMaximum(10000);
    _x->setValue(500);
    tooltip = "(pixels) x coordinate of peak shape to preview";
    _x->setToolTip(tooltip);

    _y = new QDoubleSpinBox();
    _y->setMaximum(10000);
    _y->setValue(700);
    tooltip = "(pixels) y coordinate of peak shape to preview";
    _y->setToolTip(tooltip);

    _frame = new QDoubleSpinBox();
    _frame->setMaximum(10000);
    _frame->setValue(5);
    tooltip = "(pixels) frame coordinate of peak shape to preview";
    _frame->setToolTip(tooltip);

    _radius = new QDoubleSpinBox();
    _radius->setMaximum(10000);
    _radius->setMinimum(1);
    _radius->setValue(500);
    tooltip = "(pixels) - radius for neighbour search in pixels";
    _radius->setToolTip(tooltip);

    _n_frames = new QDoubleSpinBox();
    _n_frames->setMaximum(10000);
    _n_frames->setMinimum(1);
    _n_frames->setValue(5.0);
    tooltip = "(frames) - radius for neighbour search in frames";
    _n_frames->setToolTip(tooltip);

    left_up->addRow("x:", _x);
    left_up->addRow("y:", _y);
    left_up->addRow("Frame:", _frame);
    left_up->addRow("Radius:", _radius);
    left_up->addRow("N. frames:", _n_frames);

    left->addLayout(left_up);

    _calculate_mean_profile = new QPushButton("Calculate Profile");
    tooltip = "Compute mean profile at position (x, y, frame) within specified radius";
    _calculate_mean_profile->setToolTip(tooltip);
    connect(
        _calculate_mean_profile, &QPushButton::clicked, this, &ShapeCollectionDialog::calculate);

    left->addWidget(_calculate_mean_profile);
    horizontal->addLayout(left);

    _graphics = new QGraphicsView;
    _draw_frame = new QSlider(_preview_widget);

    _draw_frame->setObjectName(QStringLiteral("drawFrame"));
    _draw_frame->setOrientation(Qt::Horizontal);
    _draw_frame->setMinimum(1);
    _draw_frame->setValue(1);

    QVBoxLayout* graphicsview = new QVBoxLayout;
    graphicsview->addWidget(_graphics);
    graphicsview->addWidget(_draw_frame);

    horizontal->addLayout(graphicsview);
    vertical->addLayout(horizontal);
}

void ShapeCollectionDialog::build()
{
    setShapeCollectionParameters();
    auto* params = _collection->parameters();
    std::vector<nsx::Peak3D*> fit_peaks;

    for (nsx::Peak3D* peak : _peaks) {
        if (!peak->enabled())
            continue;
        const double d = 1.0 / peak->q().rowVector().norm();

        if (d > params->d_max || d < params->d_min)
            continue;

        const nsx::Intensity intensity = peak->correctedIntensity();

        if (intensity.value() <= params->strength_min * intensity.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    // update the frame slider if necessary
    if (_draw_frame->maximum() != _nz->value())
        _draw_frame->setMaximum(_nz->value() - 1);
    _draw_frame->setValue(_nz->value() / 2);

    // free memory of old collection
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(this);
    view.watch(handler);

    _collection->integrate(fit_peaks, _data, handler);

    // _collection.updateFit(1000); // This does nothing!! - zamaan
}

void ShapeCollectionDialog::calculate()
{
    setShapeCollectionParameters();
    auto* params = _collection->parameters();

    const nsx::DetectorEvent ev(_x->value(), _y->value(), _frame->value());
    // update maximum value, used for drawing
    _profile = _collection->meanProfile(
        ev, params->neighbour_range_pixels, params->neighbour_range_frames);
    if (!_profile) {
        gGui->statusBar()->showMessage(
            "Failed to calculate profile. Insufficient neighbouring peaks");
        return;
    }
    _maximum = 0;

    for (int i = 0; i < params->nbins_x; ++i) {
        for (int j = 0; j < params->nbins_y; ++j) {
            for (int k = 0; k < params->nbins_z; ++k)
                _maximum = std::max(_maximum, _profile.value()(i, j, k));
        }
    }
    drawFrame(_draw_frame->value()); // draw the updated frame
}

void ShapeCollectionDialog::drawFrame(int value)
{
    if (!_profile)
        return;

    if (value < 0 || value >= _profile.value().shape()[2])
        return;

    const Eigen::Vector3i shape = _profile.value().shape();

    QImage img(shape[0], shape[1], QImage::Format_ARGB32);

    for (int i = 0; i < shape[0]; ++i) {
        for (int j = 0; j < shape[1]; ++j) {
            const double value = _profile.value().at(i, j, _draw_frame->value());
            QRgb color = _cmap.color(value, _maximum);
            img.setPixel(i, j, color);
        }
    }

    if (!_graphics->scene())
        _graphics->setScene(new QGraphicsScene());
    _graphics->scene()->clear();
    _graphics->scene()->setSceneRect(QRectF(0, 0, shape[0], shape[1]));
    _graphics->scene()->addPixmap(QPixmap::fromImage(img));

    _graphics->fitInView(0, 0, shape[0], shape[1]);
}

void ShapeCollectionDialog::selectTargetPeak(int row)
{
    const nsx::Peak3D* selected_peak = _peak_collection_item.peakCollection()->getPeak(row);

    const Eigen::Vector3d& center = selected_peak->shape().center();

    _x->setValue(center[0]);
    _y->setValue(center[1]);
    _frame->setValue(center[2]);
}

void ShapeCollectionDialog::accept()
{
    _collection_ptr->setShapeCollection(_collection);
    QDialog::accept();
}

void ShapeCollectionDialog::setShapeCollectionParameters()
{
    auto* params = _collection->parameters();

    params->d_min = _min_d->value();
    params->d_max = _max_d->value();
    params->peak_end = _peak_end->value();
    params->bkg_begin = _background_begin->value();
    params->bkg_end = _background_end->value();
    params->strength_min = _min_I_sigma->value();
    params->kabsch_coords = _kabsch->isChecked();
    params->nbins_x = _nx->value();
    params->nbins_y = _ny->value();
    params->nbins_z = _nz->value();
    params->neighbour_range_pixels = _radius->value();
    params->neighbour_range_frames = _n_frames->value();
    params->sigma_m = _sigma_M->value();
    params->sigma_d = _sigma_D->value();
}

void ShapeCollectionDialog::grabShapeCollectionParameters()
{
    auto* params = _collection->parameters();
    _collection_ptr->computeSigmas();

    _min_d->setValue(params->d_min);
    _max_d->setValue(params->d_max);
    _peak_end->setValue(params->peak_end);
    _background_begin->setValue(params->bkg_begin);
    _background_end->setValue(params->bkg_end);
    _min_I_sigma->setValue(params->strength_min);
    _kabsch->setChecked(params->kabsch_coords);
    _nx->setValue(params->nbins_x);
    _ny->setValue(params->nbins_y);
    _nz->setValue(params->nbins_z);
    _radius->setValue(params->neighbour_range_pixels);
    _n_frames->setValue(params->neighbour_range_frames);
    _sigma_M->setValue(_collection_ptr->sigmaM());
    _sigma_D->setValue(_collection_ptr->sigmaD());
}

void ShapeCollectionDialog::toggleUnsafeWidgets()
{
    _calculate_mean_profile->setEnabled(true);
    if (!_collection)
        _calculate_mean_profile->setEnabled(false);

}
