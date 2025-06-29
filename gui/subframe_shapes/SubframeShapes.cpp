//  ************************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_shapes/SubframeShapes.cpp

//! @brief     Implements class SubframeShapes
//!
//! @homepage  https://openhkl.org
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
#include "core/experiment/Integrator.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/peak/RegionData.h"
#include "core/experiment/ShapeModelBuilder.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeModel.h"
#include "gui/MainWin.h" // gGui
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics_items/CircleGraphic.h"
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

#include <QCheckBox>
#include <QComboBox>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

SubframeShapes::SubframeShapes()
    : QWidget()
    , _peak_pixmap(nullptr)
    , _profile_pixmap(nullptr)
    , _shape_model(nullptr)
    , _current_peak(nullptr)
    , _preview_peak(nullptr)
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setInputUp();
    setComputeShapesUp();
    setPreviewUp();
    setFigureUp();
    setShapePreviewUp();
    setPeakTableUp();
    toggleUnsafeWidgets();

    _right_element->setSizes(QList<int>({200, 100, 100}));
    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);
    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, _detector_widget,
        &DetectorWidget::refresh);
    connect(
        _integration_region_type,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), _detector_widget,
        &DetectorWidget::refresh);

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
    _peak_combo =
        f.addPeakCombo(ComboType::PeakCollection, "Peaks for shapes", "Used to build shape model");

    _nx = f.addSpinBox("Histogram bins x", "Number of histogram bins in x direction");
    _ny = f.addSpinBox("Histogram bins y", "Number of histogram bins in y direction");
    _nz = f.addSpinBox("Histogram bins frames", "Number of histogram bins about rotation axis");
    _nsubdiv = f.addSpinBox("Subdivisions", "Number of subdivisions along each axis per pixel");

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
    _max_width = f.addSpinBox("Maximum width", "Maximum number of images peak can span");
    std::tie(_min_d, _max_d) = f.addDoubleSpinBoxPair(
        "Resolution (d) range", "(\u212B) - resolution range for peaks to include in model");

    _integration_region_type = f.addCombo("Integration region type");
    for (int i = 0; i < static_cast<int>(ohkl::RegionType::Count); ++i)
        for (const auto& [key, val] : ohkl::regionTypeDescription)
            if (i == static_cast<int>(key))
                _integration_region_type->addItem(QString::fromStdString(val));

    _show_single_region = f.addCheckBox(
        "Show single integration region",
        "When checked, only show single integration region of clicked peak", 1);
    _peak_end = f.addDoubleSpinBox(
        "Peak end", "(" + QString(QChar(0x03C3)) + ") - scaling factor for peak region");
    _bkg_begin = f.addDoubleSpinBox(
        "Background begin",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for lower limit of background region");
    _bkg_end = f.addDoubleSpinBox(
        "Background end",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for upper limit of background region");

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
    _max_width->setMaximum(500);
    _peak_end->setMinimum(0.1);
    _peak_end->setMaximum(50);
    _peak_end->setSingleStep(0.1);
    _bkg_begin->setMinimum(0.1);
    _bkg_begin->setMaximum(50);
    _bkg_begin->setSingleStep(0.1);
    _bkg_end->setMinimum(0.1);
    _bkg_end->setMaximum(50);
    _bkg_end->setSingleStep(0.1);

    connect(_build_collection, &QPushButton::clicked, this, &SubframeShapes::buildShapeModel);
    connect(_save_shapes, &QPushButton::clicked, this, &SubframeShapes::saveShapes);
    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeShapes::grabShapeParameters);
    connect(_peak_combo, &QComboBox::currentTextChanged, this, &SubframeShapes::refreshPeakTable);
    connect(_data_combo, &QComboBox::currentTextChanged, this, &SubframeShapes::refreshPeakTable);
    connect(
        _show_single_region, &QCheckBox::stateChanged, this, &SubframeShapes::onRegionModeChanged);

    _left_layout->addWidget(input_box);
}

void SubframeShapes::setComputeShapesUp()
{
    _compute_box = new Spoiler("Preview shapes and apply model");
    GridFiller f(_compute_box, true);

    _x = f.addDoubleSpinBox("x coordinate", "(pixels) x coordinate of peak shape to preview");
    _y = f.addDoubleSpinBox("y coordinate", "(pixels) y coordinate of peak shape to preview");
    _frame = f.addDoubleSpinBox(
        "frame coordinate", "(frames) frame coordinate of peak shape to preview");

    _pixel_radius =
        f.addDoubleSpinBox("Search radius (pixels)", "(pixels) - radius for neighbour search");
    _frame_radius = f.addDoubleSpinBox(
        "Search radius (images)", "(detector images) - angular \"radius\"\" for neighbour search");
    _interpolation_combo =
        f.addCombo("Interpolation type", "Weighting strategy for mean covariance calculation");

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
        _pixel_radius, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _frame_radius, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
        &SubframeShapes::computeProfile);
    connect(
        _interpolation_combo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeShapes::computeProfile);
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

    _predicted_combo = f.addPeakCombo(ComboType::PredictedPeaks, "Target peak collection");
    _shape_combo = f.addShapeCombo("Shape model");
    _assign_peak_shapes =
        f.addButton("Apply shape model", "Apply selected shape model to a peak collection");

    _left_layout->addWidget(_compute_box);

    connect(_assign_peak_shapes, &QPushButton::clicked, this, &SubframeShapes::assignPeakShapes);
    connect(
        _shape_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        [=]() { _detector_widget->scene()->clearPixmapItems(); });
}

void SubframeShapes::setShapePreviewUp()
{
    // QGroupBox* shape_group = new QGroupBox("Shape preview");
    QWidget* shape_widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout;
    shape_widget->setLayout(layout);
    shape_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGroupBox* image_group = new QGroupBox("Peak");
    QGroupBox* profile_group = new QGroupBox("Profile preview");
    _image_view = new QGraphicsView();
    _profile_view = new QGraphicsView();
    QVBoxLayout* image_layout = new QVBoxLayout();
    QVBoxLayout* profile_layout = new QVBoxLayout();
    image_group->setLayout(image_layout);
    profile_group->setLayout(profile_layout);
    image_layout->addWidget(_image_view);
    profile_layout->addWidget(_profile_view);

    layout->addWidget(image_group);
    layout->addWidget(profile_group);

    _right_element->addWidget(shape_widget);
}

void SubframeShapes::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(1, false, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeShapes::changeSelected);
    connect(
        _predicted_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeShapes::refreshPeakTable);
    connect(
        _detector_widget->scene(), &DetectorScene::signalPeakSelected, this,
        &SubframeShapes::onPeakSelected);

    _right_element->addWidget(figure_group);
    onRegionModeChanged();
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
    _peak_table->setColumnHidden(PeakColumn::Enabled, true);
    _peak_table->setColumnHidden(PeakColumn::Count, true);
    _peak_table->setColumnHidden(PeakColumn::BkgGradient, true);
    _peak_table->setColumnHidden(PeakColumn::BkgGradientSigma, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileIntensity, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileSigma, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileStrength, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileBkg, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileBkgSigma, true);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeShapes::refreshPeakTable()
{
    if (gSession->currentProject()->hasPeakCollection()) {
        if (_predicted_combo->count() == 0)
            _peak_collection_item.setPeakCollection(_peak_combo->currentPeakCollection());
        else
            _peak_collection_item.setPeakCollection(_predicted_combo->currentPeakCollection());
        _peak_collection_model.setRoot(&_peak_collection_item);
        _peak_table->resizeColumnsToContents();
    }

    _detector_widget->refresh();
}

void SubframeShapes::refreshAll()
{
    toggleUnsafeWidgets();
    if (!gSession->hasProject())
        return;

    refreshPeakTable();
    grabShapeParameters();
}

void SubframeShapes::grabShapeParameters()
{
    if (!gSession->hasProject())
        return;
    auto* experiment = gSession->currentProject()->experiment();
    auto* integration_params = experiment->integrator()->parameters();
    auto* shape_params = experiment->shapeModelBuilder()->parameters();

    QSignalBlocker blocker1(_integration_region_type);
    QSignalBlocker blocker2(_peak_end);
    QSignalBlocker blocker3(_bkg_begin);
    QSignalBlocker blocker4(_bkg_end);

    _min_d->setValue(shape_params->d_min);
    _max_d->setValue(shape_params->d_max);
    _max_width->setValue(shape_params->max_width);
    if (integration_params->region_type == ohkl::RegionType::VariableEllipsoid) {
        _peak_end->setValue(integration_params->peak_end);
        _bkg_begin->setValue(integration_params->bkg_begin);
        _bkg_end->setValue(integration_params->bkg_end);
    } else {
        _peak_end->setValue(integration_params->fixed_peak_end);
        _bkg_begin->setValue(integration_params->fixed_bkg_begin);
        _bkg_end->setValue(integration_params->fixed_bkg_end);
    }
    _min_strength->setValue(shape_params->strength_min);
    _kabsch->setChecked(shape_params->kabsch_coords);
    _nx->setValue(shape_params->nbins_x);
    _ny->setValue(shape_params->nbins_y);
    _nz->setValue(shape_params->nbins_z);
    _nsubdiv->setValue(shape_params->n_subdiv);
    _pixel_radius->setValue(shape_params->neighbour_range_pixels);
    _frame_radius->setValue(shape_params->neighbour_range_frames);
    _interpolation_combo->setCurrentIndex(static_cast<int>(shape_params->interpolation));
    _integration_region_type->setCurrentIndex(static_cast<int>(integration_params->region_type));

    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _peak_combo->currentPeakCollection()->computeSigmas();
    _sigma_m->setValue(_peak_combo->currentPeakCollection()->sigmaM());
    _sigma_d->setValue(_peak_combo->currentPeakCollection()->sigmaD());
}

void SubframeShapes::setShapeParameters()
{
    if (!gSession->hasProject())
        return;

    auto* experiment = gSession->currentProject()->experiment();
    auto* integration_params = experiment->integrator()->parameters();
    auto* shape_params = experiment->shapeModelBuilder()->parameters();

    shape_params->d_min = _min_d->value();
    shape_params->d_max = _max_d->value();
    shape_params->max_width = _max_width->value();
    shape_params->use_max_width = true;

    integration_params->region_type =
        static_cast<ohkl::RegionType>(_integration_region_type->currentIndex());
    shape_params->region_type = static_cast<ohkl::RegionType>(_integration_region_type->currentIndex());
    if (integration_params->region_type == ohkl::RegionType::VariableEllipsoid) {
        integration_params->peak_end = _peak_end->value();
        integration_params->bkg_begin = _bkg_begin->value();
        integration_params->bkg_end = _bkg_end->value();
        shape_params->peak_end = _peak_end->value();
        shape_params->bkg_begin = _bkg_begin->value();
        shape_params->bkg_end = _bkg_end->value();
    } else {
        integration_params->fixed_peak_end = _peak_end->value();
        integration_params->fixed_bkg_begin = _bkg_begin->value();
        integration_params->fixed_bkg_end = _bkg_end->value();
        shape_params->fixed_peak_end = _peak_end->value();
        shape_params->fixed_bkg_begin = _bkg_begin->value();
        shape_params->fixed_bkg_end = _bkg_end->value();
    }

    shape_params->strength_min = _min_strength->value();
    shape_params->kabsch_coords = _kabsch->isChecked();
    shape_params->nbins_x = _nx->value();
    shape_params->nbins_y = _ny->value();
    shape_params->nbins_z = _nz->value();
    shape_params->n_subdiv = _nsubdiv->value();
    shape_params->sigma_m = _sigma_m->value();
    shape_params->sigma_d = _sigma_d->value();
    shape_params->neighbour_range_pixels = _pixel_radius->value();
    shape_params->neighbour_range_frames = _frame_radius->value();
    shape_params->interpolation =
        static_cast<ohkl::PeakInterpolation>(_interpolation_combo->currentIndex());
}

void SubframeShapes::setPreviewUp()
{
    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    preview_spoiler->setContentLayout(*_peak_view_widget);

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
        _peak_view_widget->set1.regionType, &QComboBox::currentTextChanged,
        _integration_region_type, &QComboBox::setCurrentText);
    connect(
        _peak_end, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.peakEnd, &QDoubleSpinBox::setValue);
    connect(
        _bkg_begin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.bkgBegin, &QDoubleSpinBox::setValue);
    connect(
        _bkg_end, qOverload<double>(&QDoubleSpinBox::valueChanged), _peak_view_widget->set1.bkgEnd,
        &QDoubleSpinBox::setValue);
    connect(
        _integration_region_type, &QComboBox::currentTextChanged,
        _peak_view_widget->set1.regionType, &QComboBox::setCurrentText);

    _left_layout->addWidget(preview_spoiler);
}

void SubframeShapes::buildShapeModel()
{
    gGui->setReady(false);
    auto* shape_builder = gSession->currentProject()->experiment()->shapeModelBuilder();
    auto data = _data_combo->currentData();
    auto* peaks = _peak_combo->currentPeakCollection();
    setShapeParameters();

    try {
        _shape_model = std::make_unique<ohkl::ShapeModel>(shape_builder->build(peaks, data));

        ohkl::sptrProgressHandler handler(new ohkl::ProgressHandler);
        ProgressView view(this);
        view.watch(handler);
        shape_builder->setHandler(handler);
        shape_builder->build(peaks, data);

        // _shape_model->updateFit(1000);
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    _detector_widget->scene()->clearPixmapItems();
    toggleUnsafeWidgets();
    gGui->statusBar()->showMessage(
        QString::number(_shape_model->numberOfPeaks()) + " shapes generated");
    gGui->setReady(true);
}

void SubframeShapes::computeProfile()
{
    if (!gSession->hasProject())
        return;

    auto* model = shapeModel();
    if (!model)
        return;

    if (!_current_peak)
        return;

    auto* shape_params = gSession->currentProject()->experiment()->shapeModelBuilder()->parameters();
    setShapeParameters();

    // const ohkl::DetectorEvent ev(_x->value(), _y->value(), _frame->value());

    if (!_current_peak)
        return;

    double peak_end, bkg_begin, bkg_end;
    if (shape_params->region_type == ohkl::RegionType::VariableEllipsoid) {
        peak_end = shape_params->peak_end;
        bkg_begin = shape_params->peak_end;
        bkg_end = shape_params->peak_end;
    } else {
        peak_end = shape_params->fixed_peak_end;
        bkg_begin = 1.0;
        bkg_end = 1.0;
    }
    // construct the integration region
    ohkl::IntegrationRegion region(
        _current_peak, peak_end, bkg_begin, bkg_end, shape_params->region_type);

    ohkl::RegionData* region_data = region.getRegion();
    region_data->buildProfile(
        model, shape_params->neighbour_range_pixels, shape_params->neighbour_range_frames);
    QPointF pos = {_current_peak->shape().center()[0], _current_peak->shape().center()[1]};
    auto* circle = _detector_widget->scene()->addCircle(pos, shape_params->neighbour_range_pixels);

    regionData2Image(region_data);

    for (auto item : _profile_view->scene()->items())
        if (dynamic_cast<QGraphicsTextItem*>(item) != nullptr)
            _profile_view->scene()->removeItem(item);

    int nprofiles = region_data->nProfiles();
    std::ostringstream oss;
    oss << nprofiles << " shapes";
    circle->setLabel(QString::fromStdString(oss.str()));
}

void SubframeShapes::regionData2Image(ohkl::RegionData* region_data)
{
    int nframes = region_data->nFrames();
    int ncols = region_data->cols();
    int nrows = region_data->rows();

    double image_max = region_data->dataMax();
    double profile_max = region_data->profileMax();

    QImage peak_img(ncols * nframes, nrows, QImage::Format_ARGB32);
    QImage profile_img(ncols * nframes, nrows, QImage::Format_ARGB32);

    if (!_image_view->scene()) {
        _image_view->setScene(new QGraphicsScene());
    } else {
        _image_view->scene()->clear();
        _image_view->viewport()->update();
    }

    if (!_profile_view->scene()) {
        _profile_view->setScene(new QGraphicsScene());
    } else {
        _profile_view->scene()->clear();
        _profile_view->viewport()->update();
    }

    _image_view->scene()->setSceneRect(QRectF(0, 0, ncols * nframes, nrows));
    _profile_view->scene()->setSceneRect(QRectF(0, 0, ncols * nframes, nrows));

    ColorMap cmap;
    for (int frame = 0; frame < nframes; ++frame) {
        auto image_mat = region_data->frame(frame);
        auto profile_mat = region_data->profileData(frame);
        int xmin = frame * ncols;
        for (int i = 0; i < ncols; ++i) {
            for (int j = 0; j < nrows; ++j) {
                const double image_value = image_mat(i, j);
                const double profile_value = profile_mat(i, j);
                QRgb image_color = cmap.color(image_value, image_max);
                QRgb profile_color = cmap.color(profile_value, profile_max);

                peak_img.setPixel(i + xmin, j, image_color);
                profile_img.setPixel(i + xmin, j, profile_color);
            }
        }
    }

    // black line borders around peak/profile bounding box
    QPen pen(QColor(0, 0, 0), 1);
    pen.setCosmetic(true);
    for (int idx = 0; idx <= nframes; ++idx) {
        QGraphicsLineItem* line =
            _image_view->scene()->addLine(ncols * idx, 0, ncols * idx, nrows, pen);
        line->setZValue(20);
        line = _profile_view->scene()->addLine(ncols * idx, 0, ncols * idx, nrows, pen);
        line->setZValue(20);
    }
    QGraphicsLineItem* line = _image_view->scene()->addLine(0, 0, ncols * nframes, 0, pen);
    line->setZValue(20);
    line = _profile_view->scene()->addLine(0, 0, ncols * nframes, 0, pen);
    line->setZValue(20);
    line = _image_view->scene()->addLine(0, nrows, ncols * nframes, nrows, pen);
    line->setZValue(20);
    line = _profile_view->scene()->addLine(0, nrows, ncols * nframes, nrows, pen);
    line->setZValue(20);

    _peak_pixmap = _image_view->scene()->addPixmap(QPixmap::fromImage(peak_img));
    _profile_pixmap = _profile_view->scene()->addPixmap(QPixmap::fromImage(profile_img));
    _image_view->fitInView(_image_view->scene()->sceneRect(), Qt::KeepAspectRatio);
    _profile_view->fitInView(_profile_view->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void SubframeShapes::getPreviewPeak(ohkl::Peak3D* selected_peak)
{
    auto* model = shapeModel();
    if (!model)
        return;

    auto* shape_params = gSession->currentProject()->experiment()->shapeModelBuilder()->parameters();
    setShapeParameters();
    model->setParameters(*shape_params);

    auto cov = model->meanCovariance(selected_peak);
    Eigen::Vector3d center = selected_peak->shape().center();
    ohkl::Ellipsoid shape = ohkl::Ellipsoid(center, cov.inverse());
    _preview_peak = std::make_unique<ohkl::Peak3D>(selected_peak->dataSet(), shape);
}

void SubframeShapes::saveShapes()
{
    if (!gSession->hasProject())
        return;

    std::string suggestion = gSession->currentProject()->experiment()->generateShapeModelName();
    std::unique_ptr<ListNameDialog> dlg = std::make_unique<ListNameDialog>(
        QString::fromStdString(suggestion), QString("Shape model"), QString("New shape model"));
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
    // ShapeHandler uses std::move so _shape_model is no longer a valid pointer!
    _shape_model = nullptr;
    gSession->onShapesChanged();
    toggleUnsafeWidgets();
}

void SubframeShapes::assignPeakShapes()
{
    gGui->setReady(false);
    try {
        ohkl::sptrProgressHandler handler(new ohkl::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        ohkl::PeakCollection* peaks = _predicted_combo->currentPeakCollection();

        _shape_combo->currentShapes()->setHandler(handler);
        _shape_combo->currentShapes()->setPredictedShapes(peaks);
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
    _compute_box->setEnabled(false);
    _calculate_mean_profile->setEnabled(false);
    _assign_peak_shapes->setEnabled(false);

    if (!gSession->hasProject())
        return;

    _build_collection->setEnabled(gSession->currentProject()->hasPeakCollection());

    if (_shape_model) {
        _save_shapes->setEnabled(true);
        _compute_box->setEnabled(true);
    }

    if (gSession->currentProject()->hasShapeModel()
        && gSession->currentProject()->hasPeakCollection()) {
        _calculate_mean_profile->setEnabled(true);
        _assign_peak_shapes->setEnabled(true);
    }
}


DetectorWidget* SubframeShapes::detectorWidget()
{
    return _detector_widget;
}

void SubframeShapes::onPeakSelected(ohkl::Peak3D* peak)
{
    QSignalBlocker block_x(_x);
    QSignalBlocker block_y(_y);
    QSignalBlocker block_frame(_frame);
    _current_peak = peak;
    _x->setValue(peak->shape().center()[0]);
    _y->setValue(peak->shape().center()[1]);
    _frame->setValue(peak->shape().center()[2]);

    computeProfile();
    getPreviewPeak(peak);
    _detector_widget->scene()->setPeak(_preview_peak.get());
}

void SubframeShapes::onShapeChanged()
{
    QSignalBlocker block_x(_x);
    QSignalBlocker block_y(_y);
    QSignalBlocker block_frame(_frame);
    if (!_shape_model)
        return;
    if (!_preview_peak)
        return;
    computeProfile();
    Eigen::Vector3d new_centre = {_x->value(), _y->value(), _frame->value()};
    ohkl::Ellipsoid new_shape = {new_centre, _preview_peak->shape().metric()};
    _preview_peak->setShape(new_shape);
    getPreviewPeak(_preview_peak.get());
    _detector_widget->scene()->setPeak(_preview_peak.get());
}

void SubframeShapes::onRegionModeChanged()
{
    _detector_widget->scene()->clearPixmapItems();
    _detector_widget->scene()->params()->singlePeakIntRegion = _show_single_region->isChecked();
    _detector_widget->refresh();
}

ohkl::ShapeModel* SubframeShapes::shapeModel()
{
    ohkl::ShapeModel* model = nullptr;
    if (_shape_model != nullptr)
        model = _shape_model.get();
    else {
        if (gSession->currentProject()->hasShapeModel())
            model = _shape_combo->currentShapes();
    }
    return model;
}
