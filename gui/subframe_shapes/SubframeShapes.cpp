//  ************************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_shapes/SubframeShapes.cpp

//! @brief     Implements class SubframeShapes
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_shapes/SubframeShapes.h"

#include "base/geometry/Ellipsoid.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/Experiment.h"
#include "core/shape/Profile3D.h"
#include "gui/MainWin.h" // gGui
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/ColorMap.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/ShapeComboBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/DetectorWidget.h"

#include <QFileInfo>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QSpacerItem>
#include <qobject.h>

SubframeShapes::SubframeShapes() : QWidget(), _preview_peak(nullptr)
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setInputUp();
    setComputeShapesUp();
    setAssignShapesUp();
    setPreviewUp();
    setFigureUp();
    setShapePreviewUp();
    setPeakTableUp();

    _right_element->setSizes(QList<int>({200, 100, 100}));
    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);
}

void SubframeShapes::setInputUp()
{
    auto input_box = new Spoiler("Build shape model");
    GridFiller f(input_box, true);

    _data_combo = f.addDataCombo("Data set");
    _peak_combo = f.addPeakCombo(
        ComboType::PeakCollection, "Peaks for shapes", "Used to build shape model");

    _nx = f.addSpinBox("histogram bins x", "Number of histogram bins in x direction");
    _ny = f.addSpinBox("histogram bins y", "Number of histogram bins in y direction");
    _nz = f.addSpinBox("histogram bins frames", "Number of histogram bins about rotation axis");

    _sigma_d = new SafeDoubleSpinBox();
    _sigma_m = new SafeDoubleSpinBox();

    QGridLayout* grid = new QGridLayout();
    _kabsch = new QGroupBox("Kabsch coordinates");
    _kabsch->setToolTip("Toggle between Kabsch and detector coordinates");
    _kabsch->setCheckable(true);
    _kabsch->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _kabsch->setLayout(grid);
    QLabel* label1 = new QLabel("Beam divergence " + QString(QChar(0x03C3)));
    label1->setToolTip("Variance due to beam divergence");
    QLabel* label2 = new QLabel("Mosaicity " + QString(QChar(0x03C3)));
    label2->setToolTip("Variance due to sample mosaicity");
    grid->addWidget(label1, 0, 0, 1, 1);
    grid->addWidget(_sigma_d, 0, 1, 1, 1);
    grid->addWidget(label2, 1, 0, 1, 1);
    grid->addWidget(_sigma_m, 1, 1, 1, 1);
    f.addWidget(_kabsch);

    // _sigma_d = f.addDoubleSpinBox(
    //     ("Beam divergence " + QString(QChar(0x03C3)), "Variance due to beam divergence"));
    // _sigma_m = f.addDoubleSpinBox(
    //     ("Mosaicity " + QString(QChar(0x03C3)), "Variance due to sample mosaicity"));
    _min_strength = f.addDoubleSpinBox(
        ("Minimum I/" + QString(QChar(0x03C3))),
        "Minimum strength (I/\u03C3) of peak to include in average");
    _min_d = f.addDoubleSpinBox("Minimum d", "Minimum d (\u212B) of peak to include in average");
    _max_d = f.addDoubleSpinBox("Maximum d", "Maximum d (\u212B) of peak to include in average");
    _peak_end = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");
    _bkg_begin = f.addDoubleSpinBox(
        "Background begin", "(sigmas) - scaling factor for lower limit of background region");
    _bkg_end = f.addDoubleSpinBox(
        "Background end", "(sigmas) - scaling factor for upper limit of background region");

    _build_collection = f.addButton(
        "Build shape model",
        "<font>A shape model is a collection of strong peak shapes, to be averaged within a"
        "specified cutoff in order to infer the shape of a weak peak.</font>");
        // Rich text to force line break in tooltip
    _save_shapes =
        f.addButton("Save shape model", "Add the generated shape model to the experiment");

    _nx->setMinimum(5);
    _nx->setMaximum(10000);
    _ny->setMinimum(5);
    _ny->setMaximum(10000);
    _nz->setMinimum(5);
    _nz->setMaximum(10000);
    _sigma_d->setMaximum(10.0);
    _sigma_d->setSingleStep(0.1);
    _sigma_m->setMaximum(10.0);
    _sigma_m->setSingleStep(0.1);
    _min_strength->setMaximum(10000);
    _min_d->setMaximum(1000);
    _max_d->setMaximum(10000);
    _peak_end->setMinimum(0.1);
    _peak_end->setMaximum(10);
    _peak_end->setSingleStep(0.1);
    _bkg_begin->setMinimum(0.1);
    _bkg_begin->setMaximum(100);
    _bkg_begin->setSingleStep(0.1);
    _bkg_end->setMinimum(0.1);
    _bkg_end->setMaximum(100);
    _bkg_end->setSingleStep(0.1);

    connect(_build_collection, &QPushButton::clicked, this, &SubframeShapes::buildShapeModel);
    connect(_save_shapes, &QPushButton::clicked, this, &SubframeShapes::saveShapes);

    _left_layout->addWidget(input_box);
}

void SubframeShapes::setComputeShapesUp()
{
    auto compute_box = new Spoiler("Preview peak shapes");
    GridFiller f(compute_box, true);

    _x = f.addDoubleSpinBox("x coordinate", "(pixels) x coordinate of peak shape to preview");
    _y = f.addDoubleSpinBox("y coordinate", "(pixels) y coordinate of peak shape to preview");
    _frame = f.addDoubleSpinBox(
        "frame coordinate", "(frames) frame coordinate of peak shape to preview");

    _min_neighbours = f.addSpinBox(
        "Minimum neighbours", "Fewest possible neighbours to compute a mean profile");
    _pixel_radius = f.addDoubleSpinBox("Radius", "(pixels) - radius for neighbour search");
    _frame_radius = f.addDoubleSpinBox("Frames", "(frames) - angular radius for neighbour search");
    _interpolation_combo = f.addCombo(
        "Interpolation type", "Weighting strategy for mean covariance calculation");

    _calculate_mean_profile = f.addButton(
        "Calculate profile",
        "Compute mean profile at position (x, y, frame) within specified radius");

    _x->setMaximum(10000);
    _x->setValue(500);
    _x->setSingleStep(1);
    _y->setMaximum(10000);
    _y->setValue(500);
    _y->setSingleStep(1);
    _frame->setMaximum(100);
    _frame->setValue(5);
    _frame->setSingleStep(1);
    _min_neighbours->setMaximum(1000);
    _min_neighbours->setValue(10);
    _pixel_radius->setMaximum(10000);
    _pixel_radius->setValue(500);
    _pixel_radius->setSingleStep(10);
    _frame_radius->setMaximum(100);
    _frame_radius->setValue(10);

    _interpolation_combo->addItem("None");
    _interpolation_combo->addItem("Inverse distance");
    _interpolation_combo->addItem("Intensity");

    connect(
        _x, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _y, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _frame, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _min_neighbours, qOverload<int>(&QSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _pixel_radius, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _frame_radius, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _interpolation_combo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeShapes::computeProfile);
    connect(
        _x, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::onShapeChanged);
    connect(
        _y, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::onShapeChanged);
    connect(
        _frame, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::onShapeChanged);
    connect(
        _min_neighbours, qOverload<int>(&QSpinBox::valueChanged), this,
        &SubframeShapes::onShapeChanged);
    connect(
        _pixel_radius, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::onShapeChanged);
    connect(
        _frame_radius, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::onShapeChanged);
    connect(
        _interpolation_combo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeShapes::onShapeChanged);
    connect(_calculate_mean_profile, &QPushButton::clicked, this, &SubframeShapes::computeProfile);

    _left_layout->addWidget(compute_box);
}

void SubframeShapes::setAssignShapesUp()
{
    auto assign_box = new Spoiler("Apply shape model");
    GridFiller f(assign_box, true);
    _predicted_combo =
        f.addPeakCombo(ComboType::PredictedPeaks, "Target peak collection");
    _shape_combo = f.addShapeCombo("Shape model");
    _assign_peak_shapes =
        f.addButton("Apply shape model", "Apply selected shape model to a peak collection");

    connect(_assign_peak_shapes, &QPushButton::clicked, this, &SubframeShapes::assignPeakShapes);

    _left_layout->addWidget(assign_box);
}

void SubframeShapes::setShapePreviewUp()
{
    QGroupBox* shape_group = new QGroupBox("Shape preview");
    QHBoxLayout* layout = new QHBoxLayout;
    shape_group->setLayout(layout);
    shape_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _graphics_view = new QGraphicsView();
    _graphics_view->scale(1, -1);
    layout->addWidget(_graphics_view);

    _right_element->addWidget(shape_group);
}

void SubframeShapes::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(false, false, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model);
    _detector_widget->scene()->drawSinglePeakIntegrationRegion(true);

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeShapes::changeSelected);
    connect(
        _predicted_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeShapes::refreshPeakTable);
    connect(
        _detector_widget->scene(), &DetectorScene::signalPeakSelected,
        this, &SubframeShapes::onPeakSelected);

    _right_element->addWidget(figure_group);
}

void SubframeShapes::refreshPeakVisual()
{
    if (_peak_collection_item.childCount() == 0)
        return;

    for (int i = 0; i < _peak_collection_item.childCount(); i++) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget->set1 : _peak_view_widget->set2);
    }
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    _detector_widget->refresh();
}

void SubframeShapes::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);
    _peak_table->resizeColumnsToContents();

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeShapes::refreshPeakTable()
{
    if (_predicted_combo->count() == 0)
        return;

    _peak_collection_item.setPeakCollection(_predicted_combo->currentPeakCollection());
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    refreshPeakVisual();
}

void SubframeShapes::refreshAll()
{
    toggleUnsafeWidgets();
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
    _detector_widget->refresh();
    _peak_combo->refresh();
    _predicted_combo->refresh();
    refreshPeakTable();
    grabShapeParameters();
}

void SubframeShapes::grabShapeParameters()
{
    auto* params = _shape_model.parameters();
    _peak_combo->currentPeakCollection()->computeSigmas();

    _min_d->setValue(params->d_min);
    _max_d->setValue(params->d_max);
    _peak_end->setValue(params->peak_end);
    _bkg_begin->setValue(params->bkg_begin);
    _bkg_end->setValue(params->bkg_end);
    _min_strength->setValue(params->strength_min);
    _kabsch->setChecked(params->kabsch_coords);
    _nx->setValue(params->nbins_x);
    _ny->setValue(params->nbins_y);
    _nz->setValue(params->nbins_z);
    _pixel_radius->setValue(params->neighbour_range_pixels);
    _frame_radius->setValue(params->neighbour_range_frames);
    _sigma_m->setValue(_peak_combo->currentPeakCollection()->sigmaM());
    _sigma_d->setValue(_peak_combo->currentPeakCollection()->sigmaD());
    _interpolation_combo->setCurrentIndex(static_cast<int>(params->interpolation));
}

void SubframeShapes::setShapeParameters()
{
    if (!gSession->hasProject())
        return;

    auto* params = _shape_model.parameters();

    params->d_min = _min_d->value();
    params->d_max = _max_d->value();
    params->peak_end = _peak_end->value();
    params->bkg_begin = _bkg_begin->value();
    params->bkg_end = _bkg_end->value();
    params->strength_min = _min_strength->value();
    params->kabsch_coords = _kabsch->isChecked();
    params->nbins_x = _nx->value();
    params->nbins_y = _ny->value();
    params->nbins_z = _nz->value();
    params->neighbour_range_pixels = _pixel_radius->value();
    params->neighbour_range_frames = _frame_radius->value();
    params->sigma_m = _sigma_m->value();
    params->sigma_d = _sigma_d->value();
    params->interpolation =
        static_cast<nsx::PeakInterpolation>(_interpolation_combo->currentIndex());
}

void SubframeShapes::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeShapes::refreshPeakVisual);

    preview_spoiler->setContentLayout(*_peak_view_widget);

    _peak_view_widget->set1.drawIntegrationRegion->setChecked(false);
    _peak_view_widget->set1.previewIntRegion->setChecked(false);

    connect(
        _peak_view_widget->set1.peakEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_end, &QDoubleSpinBox::setValue);
    connect(
        _peak_view_widget->set1.bkgBegin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_begin, &QDoubleSpinBox::setValue);
    connect(
        _peak_view_widget->set1.bkgEnd, qOverload<double>(&QDoubleSpinBox::valueChanged), _bkg_end,
        &QDoubleSpinBox::setValue);
    connect(
        _peak_end, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.peakEnd, &QDoubleSpinBox::setValue);
    connect(
        _bkg_begin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.bkgBegin, &QDoubleSpinBox::setValue);
    connect(
        _bkg_end, qOverload<double>(&QDoubleSpinBox::valueChanged), _peak_view_widget->set1.bkgEnd,
        &QDoubleSpinBox::setValue);

    _left_layout->addWidget(preview_spoiler);
}

void SubframeShapes::buildShapeModel()
{
    gGui->setReady(false);
    setShapeParameters();
    try {
        auto* params = _shape_model.parameters();
        std::vector<nsx::Peak3D*> fit_peaks;

        for (nsx::Peak3D* peak : _peak_combo->currentPeakCollection()->getPeakList()) {
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

        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView view(this);
        view.watch(handler);

        std::set<nsx::sptrDataSet> data;
        for (auto dataset : gSession->currentProject()->experiment()->getAllData())
            data.insert(dataset);

        _shape_model.integrate(fit_peaks, data, handler);
        _shape_model.updateFit(1000); // This does nothing!! - zamaan
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->statusBar()->showMessage(
        QString::number(_shape_model.numberOfPeaks()) + " shapes generated");
    toggleUnsafeWidgets();
    gGui->setReady(true);
}

void SubframeShapes::computeProfile()
{
    if (!gSession->hasProject())
        return;

    setShapeParameters();

    auto* params = _shape_model.parameters();
    const nsx::DetectorEvent ev(_x->value(), _y->value(), _frame->value());

    std::optional<nsx::Profile3D> profile = _shape_model.meanProfile(
        ev, params->neighbour_range_pixels, params->neighbour_range_frames);
    if (!profile) {
        return;
    }

    _profile = profile.value();

    int xmax = _profile.shape()[0];
    int ymax = _profile.shape()[1];
    int nframes = _profile.shape()[2];
    // update maximum value, used for drawing
    double intensity_maximum = 0;
    for (int i = 0; i < xmax; ++i) {
        for (int j = 0; j < ymax; ++j) {
            for (int k = 0; k < nframes; ++k)
                intensity_maximum = std::max(intensity_maximum, _profile.at(i, j, k));
        }
    }

    QImage img(xmax * nframes, ymax, QImage::Format_ARGB32);
    if (!_graphics_view->scene())
        _graphics_view->setScene(new QGraphicsScene());

    _graphics_view->scene()->setSceneRect(QRectF(0, 0, xmax * nframes, ymax));

    ColorMap cmap;
    for (int frame = 0; frame < nframes; ++frame ) {
        int xmin = frame * xmax;
        for (int i = 0; i < xmax; ++i) {
            for (int j = 0; j < ymax; ++j) {
                const double value = _profile.at(i, j, frame);
                QRgb color = cmap.color(value, intensity_maximum);
                img.setPixel(i + xmin, j, color);
            }
        }
    }
    _graphics_view->scene()->addPixmap(QPixmap::fromImage(img));
    _graphics_view->fitInView(_graphics_view->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void SubframeShapes::getPreviewPeak(nsx::Peak3D* selected_peak)
{
    setShapeParameters();
    auto* params = _shape_model.parameters();
    int interpol = _interpolation_combo->currentIndex();
    nsx::PeakInterpolation peak_interpolation = static_cast<nsx::PeakInterpolation>(interpol);

    auto cov = _shape_model.meanCovariance(
        selected_peak, params->neighbour_range_pixels, params->neighbour_range_frames,
        params->min_n_neighbors, peak_interpolation);
    if (cov) {
        Eigen::Vector3d center = selected_peak->shape().center();
        nsx::Ellipsoid shape = nsx::Ellipsoid(center, cov.value().inverse());
        _preview_peak = std::make_unique<nsx::Peak3D>(selected_peak->dataSet(), shape);
    }
}

void SubframeShapes::saveShapes()
{
    if (!gSession->hasProject())
        return;

    std::string suggestion = gSession->currentProject()->experiment()->generateShapeModelName();
    std::unique_ptr<ListNameDialog> dlg =
        std::make_unique<ListNameDialog>(QString::fromStdString(suggestion));
    dlg->exec();
    if (dlg->listName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;
    if (!gSession->currentProject()->experiment()->addShapeModel(
            dlg->listName().toStdString(), _shape_model)) {
        QMessageBox::warning(
            this, "Unable to add ShapeModel", "Collection with this name already exists!");
        return;
    }
    std::string name = gSession->currentProject()->experiment()->getShapeModels()[0]->name();
    gSession->onShapesChanged();
    toggleUnsafeWidgets();
}

void SubframeShapes::assignPeakShapes()
{
    gGui->setReady(false);
    try {
        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);


        int interpol = _interpolation_combo->currentIndex();
        nsx::PeakInterpolation peak_interpolation = static_cast<nsx::PeakInterpolation>(interpol);
        nsx::PeakCollection* peaks = _predicted_combo->currentPeakCollection();

        _shape_combo->currentShapes()->setHandler(handler);
        _shape_combo->currentShapes()->setPredictedShapes(peaks, peak_interpolation);
        gGui->statusBar()->showMessage(
            QString::number(peaks->numberOfValid()) + "/" + QString::number(peaks->numberOfPeaks())
            + " predicted peaks with valid shapes");
        refreshPeakTable();
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
}

void SubframeShapes::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframeShapes::toggleUnsafeWidgets()
{
    _build_collection->setEnabled(false);
    _save_shapes->setEnabled(false);
    _calculate_mean_profile->setEnabled(false);
    _assign_peak_shapes->setEnabled(false);

    if (!gSession->hasProject())
        return;

    if (gSession->currentProject()->hasPeakCollection())
        _build_collection->setEnabled(true);

    if (_shape_model.numberOfPeaks() > 0) {
        _save_shapes->setEnabled(true);
        _calculate_mean_profile->setEnabled(true);
    }

    if (gSession->currentProject()->hasShapeModel() && (_predicted_combo->count() > 0))
        _assign_peak_shapes->setEnabled(true);

}


DetectorWidget* SubframeShapes::detectorWidget()
{
    return _detector_widget;
}

void SubframeShapes::onPeakSelected(nsx::Peak3D* peak)
{
    QSignalBlocker block_x(_x) ;
    QSignalBlocker block_y(_y);
    QSignalBlocker block_frame(_frame);
    _current_peak = peak;
    _x->setValue(peak->shape().center()[0]);
    _y->setValue(peak->shape().center()[1]);
    _frame->setValue(peak->shape().center()[2]);

    if (_shape_model.numberOfPeaks() == 0)
        return;
    computeProfile();
    getPreviewPeak(peak);
    _detector_widget->scene()->setPeak(_preview_peak.get());
}

void SubframeShapes::onShapeChanged()
{
    QSignalBlocker block_x(_x);
    QSignalBlocker block_y(_y);
    QSignalBlocker block_frame(_frame);
    if (_shape_model.numberOfPeaks() == 0)
        return;
    if (!_preview_peak)
        return;
    computeProfile();
    Eigen::Vector3d new_centre = {_x->value(), _y->value(), _frame->value()};
    nsx::Ellipsoid new_shape = {new_centre, _preview_peak->shape().metric()};
    _preview_peak->setShape(new_shape);
    getPreviewPeak(_preview_peak.get());
    _detector_widget->scene()->setPeak(_preview_peak.get());
}
