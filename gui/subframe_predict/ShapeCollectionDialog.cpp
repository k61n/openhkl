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
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeCollection.h"
#include "gui/frames/ProgressView.h"
#include "gui/models/ColorMap.h"
#include "gui/models/Session.h"

#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QtGlobal>

ShapeCollectionDialog::ShapeCollectionDialog(nsx::PeakCollection* peak_collection)
    : QDialog(), _peak_collection_model(), _peak_collection_item()
{
    setModal(true);
    setSizePolicies();
    setParametersUp();
    setPreviewUp();
    setUpParametrization(peak_collection);

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

    connect(reject_button, &QPushButton::clicked, this, &ShapeCollectionDialog::rejected);
}

void ShapeCollectionDialog::setUpParametrization(nsx::PeakCollection* peak_collection)
{
    _collection_ptr = peak_collection;

    _peaks = peak_collection->getPeakList();
    for (nsx::Peak3D* peak : _peaks)
        _data.insert(peak->dataSet());

    peak_collection->computeSigmas();
    _sigma_D->setValue(peak_collection->sigmaD());
    _sigma_M->setValue(peak_collection->sigmaM());

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

    _nx = new QSpinBox();
    _nx->setMinimum(5);
    _nx->setMaximum(10000);
    _nx->setMaximumWidth(10000);
    _nx->setValue(20);
    _nx->setSizePolicy(*_size_policy_widgets);

    _ny = new QSpinBox();
    _ny->setMinimum(5);
    _ny->setMaximum(10000);
    _ny->setMaximumWidth(10000);
    _ny->setValue(20);
    _ny->setSizePolicy(*_size_policy_widgets);

    _nz = new QSpinBox();
    _nz->setMinimum(5);
    _nz->setMaximum(10000);
    _nz->setMaximumWidth(10000);
    _nz->setValue(20);
    _nz->setSizePolicy(*_size_policy_widgets);

    _kabsch = new QGroupBox("Kabsch coordinates");
    _kabsch->setSizePolicy(*_size_policy_widgets);
    _kabsch->setCheckable(true);

    QFormLayout* kabschform = new QFormLayout(_kabsch);

    _sigma_D = new QDoubleSpinBox();
    _sigma_D->setMaximum(10000);
    _sigma_D->setSizePolicy(*_size_policy_widgets);

    _sigma_M = new QDoubleSpinBox();
    _sigma_M->setMaximum(10000);
    _sigma_M->setSizePolicy(*_size_policy_widgets);

    _min_I_sigma = new QDoubleSpinBox();
    _min_I_sigma->setMaximum(10000);
    _min_I_sigma->setValue(1);
    _min_I_sigma->setSizePolicy(*_size_policy_widgets);

    _min_d = new QDoubleSpinBox();
    _min_d->setMaximum(10000);
    _min_d->setValue(1.5);
    _min_d->setSizePolicy(*_size_policy_widgets);

    _max_d = new QDoubleSpinBox();
    _max_d->setMaximum(10000);
    _max_d->setValue(50);
    _max_d->setSizePolicy(*_size_policy_widgets);

    _peak_scale = new QDoubleSpinBox();
    _peak_scale->setMaximum(10000);
    _peak_scale->setValue(3);
    _peak_scale->setSizePolicy(*_size_policy_widgets);

    _background_begin = new QDoubleSpinBox();
    _background_begin->setMaximum(10000);
    _background_begin->setValue(3);
    _background_begin->setSizePolicy(*_size_policy_widgets);

    _background_end = new QDoubleSpinBox();
    _background_end->setMaximum(10000);
    _background_end->setValue(4.5);
    _background_end->setSizePolicy(*_size_policy_widgets);

    _build_collection = new QPushButton("Build shape collection");
    _build_collection->setSizePolicy(*_size_policy_widgets);

    form->addRow("Number along x:", _nx);
    form->addRow("Number along y:", _ny);
    form->addRow("Number along z:", _nz);
    kabschform->addRow("Sigma D:", _sigma_D);
    kabschform->addRow("Sigma M:", _sigma_M);
    form->addRow(_kabsch);
    form->addRow("Minimum I/Sigma:", _min_I_sigma);
    form->addRow("Minimum d:", _min_d);
    form->addRow("Maximum d:", _max_d);
    form->addRow("Peak scale:", _peak_scale);
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

    _table = new QTableView;
    vertical->addWidget(_table);

    QHBoxLayout* horizontal = new QHBoxLayout;
    QVBoxLayout* left = new QVBoxLayout;
    QFormLayout* left_up = new QFormLayout;

    _x = new QDoubleSpinBox();
    _x->setMaximum(10000);
    _x->setValue(500);

    _y = new QDoubleSpinBox();
    _y->setMaximum(10000);
    _y->setValue(700);

    _frame = new QDoubleSpinBox();
    _frame->setMaximum(10000);
    _frame->setValue(5);

    _radius = new QDoubleSpinBox();
    _radius->setMaximum(10000);
    _radius->setMinimum(1);
    _radius->setValue(500);

    _n_frames = new QDoubleSpinBox();
    _n_frames->setMaximum(10000);
    _n_frames->setMinimum(1);
    _n_frames->setValue(5.0);

    left_up->addRow("x:", _x);
    left_up->addRow("y:", _y);
    left_up->addRow("Frame:", _frame);
    left_up->addRow("Radius:", _radius);
    left_up->addRow("N. frames:", _n_frames);

    left->addLayout(left_up);

    _calculate_mean_profile = new QPushButton("Calculate Profile");
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
    std::vector<nsx::Peak3D*> fit_peaks;

    for (nsx::Peak3D* peak : _peaks) {
        if (!peak->enabled())
            continue;
        double d = 1.0 / peak->q().rowVector().norm();

        if (d > _max_d->value() || d < _min_d->value())
            continue;

        nsx::Intensity intensity = peak->correctedIntensity();

        if (intensity.value() <= _min_I_sigma->value() * intensity.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    const int nx_val = _nx->value();
    const int ny_val = _ny->value();
    const int nz_val = _nz->value();

    // update the frame slider if necessary
    if (_draw_frame->maximum() != nz_val)
        _draw_frame->setMaximum(nz_val - 1);

    nsx::AABB aabb;

    bool kabsch_coords = _kabsch->isChecked();

    double peak_scale_val = _peak_scale->value();

    if (kabsch_coords) {
        double sigma_d_val = _sigma_D->value();
        double sigma_m_val = _sigma_M->value();
        Eigen::Vector3d sigma(sigma_d_val, sigma_d_val, sigma_m_val);
        aabb.setLower(-peak_scale_val * sigma);
        aabb.setUpper(peak_scale_val * sigma);
    } else {
        Eigen::Vector3d dx(nx_val, ny_val, nz_val);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }

    // free memory of old collection
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(this);
    view.watch(handler);

    const double bkg_begin_val = _background_begin->value();
    const double bkg_end_val = _background_end->value();
    _collection = nsx::ShapeCollection(!kabsch_coords, peak_scale_val, bkg_begin_val, bkg_end_val);

    nsx::ShapeIntegrator integrator(&_collection, aabb, nx_val, ny_val, nz_val);
    integrator.setHandler(handler);
    nsx::IntegrationParameters params{};
    params.peak_end = peak_scale_val;
    params.bkg_begin = bkg_begin_val;
    params.bkg_end = bkg_end_val;
    integrator.setParameters(params);

    int n_numor = 1;
    for (nsx::sptrDataSet data : _data) {
        integrator.integrate(fit_peaks, &_collection, data, n_numor);
        ++n_numor;
    }

    _collection = *integrator.collection();
    _collection.updateFit(1000); // This does nothing!! - zamaan
}

void ShapeCollectionDialog::calculate()
{
    const int nx_val = _nx->value();
    const int ny_val = _ny->value();
    const int nz_val = _nz->value();

    nsx::DetectorEvent ev(_x->value(), _y->value(), _frame->value());
    // update maximum value, used for drawing
    _profile = _collection.meanProfile(ev, _radius->value(), _n_frames->value());
    _maximum = 0;

    for (int i = 0; i < nx_val; ++i) {
        for (int j = 0; j < ny_val; ++j) {
            for (int k = 0; k < nz_val; ++k)
                _maximum = std::max(_maximum, _profile(i, j, k));
        }
    }

    nsx::Ellipsoid e = _profile.ellipsoid();

    qDebug() << "Mean profile has inertia tensor";
    std::ostringstream os;
    os << e.inverseMetric();
    qDebug() << QString::fromStdString(os.str());

    // draw the updated frame
    drawFrame(_draw_frame->value());
}

void ShapeCollectionDialog::drawFrame(int value)
{
    if (value < 0 || value >= _profile.shape()[2]) {
        qInfo() << "SLD: drawFrame(): value = " << value;
        qInfo() << "SLD: drawFrame(): _profile.shape()[2] = " << _profile.shape()[2];
        return;
    }

    const Eigen::Vector3i shape = _profile.shape();

    QImage img(shape[0], shape[1], QImage::Format_ARGB32);

    for (int i = 0; i < shape[0]; ++i) {
        for (int j = 0; j < shape[1]; ++j) {
            const double value = _profile.at(i, j, _draw_frame->value());
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
