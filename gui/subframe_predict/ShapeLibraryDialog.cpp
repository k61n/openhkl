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

#include "gui/subframe_predict/ShapeLibraryDialog.h"

#include "core/data/DataSet.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeLibrary.h"
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

ShapeLibraryDialog::ShapeLibraryDialog(nsx::PeakCollection* peak_collection)
    : QDialog(), _peak_collection_item(), _peak_collection_model()
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

    connect(accept_button, &QPushButton::clicked, this, &ShapeLibraryDialog::accept);

    connect(reject_button, &QPushButton::clicked, this, &ShapeLibraryDialog::rejected);
}

void ShapeLibraryDialog::setUpParametrization(nsx::PeakCollection* peak_collection)
{
    _collection_ptr = peak_collection;

    _peaks = peak_collection->getPeakList();
    for (nsx::Peak3D* peak : _peaks)
        _data.insert(peak->dataSet());

    Eigen::Matrix3d cov;
    cov.setZero();

    for (nsx::Peak3D* peak : _peaks) {
        nsx::PeakCoordinateSystem coord(peak);
        const nsx::Ellipsoid& shape = peak->shape();
        Eigen::Matrix3d J = coord.jacobian();
        cov += J * shape.inverseMetric() * J.transpose();
    }
    cov /= _peaks.size();
    _sigma_D->setValue(std::sqrt(0.5 * (cov(0, 0) + cov(1, 1))));
    _sigma_M->setValue(std::sqrt(cov(2, 2)));

    _peak_collection_item.setPeakCollection(peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _table->setModel(&_peak_collection_model);
    _table->verticalHeader()->show();
    _table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    _table->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);

    connect(_draw_frame, &QSlider::valueChanged, this, &ShapeLibraryDialog::drawFrame);
    connect(
        _table, &QTableView::clicked, [this](QModelIndex index) { selectTargetPeak(index.row()); });
    connect(
        _table->verticalHeader(), &QHeaderView::sectionClicked, this,
        &ShapeLibraryDialog::selectTargetPeak);
}

void ShapeLibraryDialog::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);
}

void ShapeLibraryDialog::setParametersUp()
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

    _build_library = new QPushButton("Build Library");
    _build_library->setSizePolicy(*_size_policy_widgets);

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
    form->addRow(_build_library);

    connect(_build_library, &QPushButton::clicked, this, &ShapeLibraryDialog::build);
}

void ShapeLibraryDialog::setPreviewUp()
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
    connect(_calculate_mean_profile, &QPushButton::clicked, this, &ShapeLibraryDialog::calculate);

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

void ShapeLibraryDialog::build()
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

    int nx_val = _nx->value();
    int ny_val = _ny->value();
    int nz_val = _nz->value();

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

    // free memory of old library
    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView view(this);
    view.watch(handler);

    double bkg_begin_val = _background_begin->value();
    double bkg_end_val = _background_end->value();
    _library = nsx::ShapeLibrary(!kabsch_coords, peak_scale_val, bkg_begin_val, bkg_end_val);

    nsx::ShapeIntegrator integrator(&_library, aabb, nx_val, ny_val, nz_val);
    integrator.setHandler(handler);
    integrator.setPeakEnd(peak_scale_val);
    integrator.setBkgBegin(bkg_begin_val);
    integrator.setBkgEnd(bkg_end_val);

    for (nsx::sptrDataSet data : _data) {
        // gLogger->log(
        //     "[INFO]Fitting profiles in dataset " + QString::fromStdString(data->filename()));

        integrator.integrate(fit_peaks, &_library, data);
    }
    // gLogger->log("[INFO]Done fitting profiles");

    _library = *integrator.library();

    // gLogger->log("[INFO]Updating peak shape model...");
    _library.updateFit(1000);
    // gLogger->log("[INFO]Done, mean pearson is " + QString::number(_library.meanPearson()));
}

void ShapeLibraryDialog::calculate()
{
    int nx_val = _nx->value();
    int ny_val = _ny->value();
    int nz_val = _nz->value();

    nsx::DetectorEvent ev(_x->value(), _y->value(), _frame->value());
    // update maximum value, used for drawing
    _profile = _library.meanProfile(ev, _radius->value(), _n_frames->value());
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

void ShapeLibraryDialog::drawFrame(int value)
{
    if (value < 0 || value >= _profile.shape()[2]) {
        qInfo() << "SLD: drawFrame(): value = " << value;
        qInfo() << "SLD: drawFrame(): _profile.shape()[2] = " << _profile.shape()[2];
        return;
    }

    const Eigen::Vector3i shape = _profile.shape();
    QGraphicsScene* scene = _graphics->scene();

    if (!scene) {
        scene = new QGraphicsScene();
        _graphics->setScene(scene);
    }

    QImage img(shape[0], shape[1], QImage::Format_ARGB32);

    for (int i = 0; i < shape[0]; ++i) {
        for (int j = 0; j < shape[1]; ++j) {
            const double value = _profile.at(i, j, _draw_frame->value());
            QRgb color = _cmap.color(value, _maximum);
            img.setPixel(i, j, color);
        }
    }
    scene->clear();
    scene->setSceneRect(QRectF(0, 0, shape[0], shape[1]));
    scene->addPixmap(QPixmap::fromImage(img));
    _graphics->fitInView(0, 0, shape[0], shape[1]);
}

void ShapeLibraryDialog::selectTargetPeak(int row)
{
    nsx::Peak3D* selected_peak = _peak_collection_item.peakCollection()->getPeak(row);

    const Eigen::Vector3d& center = selected_peak->shape().center();

    _x->setValue(center[0]);
    _y->setValue(center[1]);
    _frame->setValue(center[2]);
}

void ShapeLibraryDialog::accept()
{
    _collection_ptr->setShapeLibrary(_library);
    QDialog::accept();
}
