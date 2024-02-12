//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_filter/SubframeIntegrate.cpp

//! @brief     Implements class SubframeIntegrate
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_integrate/SubframeIntegrate.h"

#include "core/data/ImageGradient.h"
#include "core/experiment/Experiment.h"
#include "core/integration/IIntegrator.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/shape/ShapeModel.h"
#include "gui/MainWin.h" // gGui
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/items/PeakItem.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subwindows/DetectorWindow.h"
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
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include <QSpacerItem>

SubframeIntegrate::SubframeIntegrate() : QWidget()
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setInputUp();
    setIntegrationRegionUp();
    setIntegrateUp();
    setPreviewUp();
    setFigureUp();
    setPeakTableUp();
    toggleUnsafeWidgets();

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
        _integrator_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::toggleUnsafeWidgets);
    connect(
        _integration_region_type,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), _detector_widget,
        &DetectorWidget::refresh);
    connect(
        _gradient_kernel, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::onGradientSettingsChanged);
    connect(
        _fft_gradient, &QCheckBox::stateChanged, this,
        &SubframeIntegrate::onGradientSettingsChanged);
    connect(
        _compute_gradient, &QGroupBox::clicked, this,
        &SubframeIntegrate::onGradientSettingsChanged);
    connect(
        this, &SubframeIntegrate::signalGradient, _detector_widget->scene(),
        &DetectorScene::onGradientSetting);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);

    _shape_params = nullptr;
}

void SubframeIntegrate::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _data_combo = f.addDataCombo("Data set");
    _peak_combo = f.addPeakCombo(ComboType::PeakCollection, "Peaks to integrate");

    connect(_peak_combo, &QComboBox::currentTextChanged, this, &SubframeIntegrate::toggleUnsafeWidgets);

    _left_layout->addWidget(input_box);
}

void SubframeIntegrate::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(1, false, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeIntegrate::changeSelected);
    connect(_peak_combo, &QComboBox::currentTextChanged, this, &SubframeIntegrate::refreshPeakTable);
    connect(_data_combo, &QComboBox::currentTextChanged, this, &SubframeIntegrate::refreshPeakTable);

    _right_element->addWidget(figure_group);
}

void SubframeIntegrate::setPeakTableUp()
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

    _peak_table->sortByColumn(PeakColumn::d, Qt::DescendingOrder);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeIntegrate::refreshPeakTable()
{
    if (gSession->currentProject()->hasPeakCollection()) {
        _peak_collection = _peak_combo->currentPeakCollection();
        _peak_collection_item.setPeakCollection(_peak_collection);
        _peak_collection_model.setRoot(&_peak_collection_item);
        _peak_table->resizeColumnsToContents();
    }

    _detector_widget->refresh();
}

void SubframeIntegrate::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _detector_widget->refresh();
    refreshPeakTable();
    grabIntegrationParameters();
    toggleUnsafeWidgets();
}

void SubframeIntegrate::grabIntegrationParameters()
{

    auto* expt = gSession->currentProject()->experiment();
    auto* integrator = expt->integrator();
    auto* params = integrator->parameters();

    QSignalBlocker blocker1(_integration_region_type);
    QSignalBlocker blocker2(_peak_end);
    QSignalBlocker blocker3(_bkg_begin);
    QSignalBlocker blocker4(_bkg_end);
    QSignalBlocker blocker5(_integrator_combo);

    _integration_region_type->setCurrentIndex(static_cast<int>(params->region_type));
    if (params->region_type == ohkl::RegionType::VariableEllipsoid) {
        _peak_end->setValue(params->peak_end);
        _bkg_begin->setValue(params->bkg_begin);
        _bkg_end->setValue(params->bkg_end);
    } else {
        _peak_end->setValue(params->fixed_peak_end);
        _bkg_begin->setValue(params->fixed_bkg_begin);
        _bkg_end->setValue(params->fixed_bkg_end);
    }
    _discard_saturated->setChecked(params->discard_saturated);
    _max_counts->setValue(params->max_counts);
    _fit_center->setChecked(params->fit_center);
    _fit_covariance->setChecked(params->fit_cov);
    _compute_gradient->setChecked(params->use_gradient);
    _fft_gradient->setChecked(params->fft_gradient);
    _gradient_kernel->setCurrentIndex(static_cast<int>(params->gradient_type));
    _remove_masked->setChecked(params->skip_masked);
    _remove_overlaps->setChecked(params->remove_overlaps);
    _use_max_strength->setChecked(params->use_max_strength);
    _max_strength->setValue(params->max_strength);
    _use_max_d->setChecked(params->use_max_d);
    _max_d->setValue(params->max_d);
    _use_max_width->setChecked(params->use_max_width);
    _max_width->setValue(params->max_width);
    _integrator_combo->setCurrentIndex(static_cast<int>(params->integrator_type));

    if (!gSession->currentProject()->hasShapeModel())
        return;

    auto* shape_params = _shape_combo->currentShapes()->parameters();
    _radius_int->setValue(shape_params->neighbour_range_pixels);
    _n_frames_int->setValue(shape_params->neighbour_range_frames);
    _interpolation_combo->setCurrentIndex(static_cast<int>(shape_params->interpolation));
}

void SubframeIntegrate::setIntegrationParameters()
{
    if (!gSession->hasProject())
        return;

    auto* expt = gSession->currentProject()->experiment();
    auto* integrator = expt->integrator();
    auto* params = integrator->parameters();

    params->discard_saturated = _discard_saturated->isChecked();
    params->max_counts = _max_counts->value();
    params->fit_center = _fit_center->isChecked();
    params->fit_cov = _fit_covariance->isChecked();
    params->region_type = static_cast<ohkl::RegionType>(_integration_region_type->currentIndex());
    params->integrator_type = static_cast<ohkl::IntegratorType>(_integrator_combo->currentIndex());
    if (params->region_type == ohkl::RegionType::VariableEllipsoid) {
        params->peak_end = _peak_end->value();
        params->bkg_begin = _bkg_begin->value();
        params->bkg_end = _bkg_end->value();
    } else {
        params->fixed_peak_end = _peak_end->value();
        params->fixed_bkg_begin = _bkg_begin->value();
        params->fixed_bkg_end = _bkg_end->value();
    }
    params->use_gradient = _compute_gradient->isChecked();
    params->fft_gradient = _fft_gradient->isChecked();
    params->gradient_type = static_cast<ohkl::GradientKernel>(_gradient_kernel->currentIndex());
    params->skip_masked = _remove_masked->isChecked();
    params->remove_overlaps = _remove_overlaps->isChecked();
    params->use_max_strength = _use_max_strength->isChecked();
    params->max_strength = _max_strength->value();
    params->use_max_d = _use_max_d->isChecked();
    params->max_d = _max_d->value();
    params->use_max_width = _use_max_width->isChecked();
    params->max_width = _max_width->value();
    params->region_type = static_cast<ohkl::RegionType>(_integration_region_type->currentIndex());

    if (!gSession->currentProject()->hasShapeModel())
        return;

    auto* shape_params = _shape_combo->currentShapes()->parameters();
    shape_params->neighbour_range_pixels = _radius_int->value();
    shape_params->neighbour_range_frames = _n_frames_int->value();
    shape_params->interpolation =
        static_cast<ohkl::PeakInterpolation>(_interpolation_combo->currentIndex());
}

void SubframeIntegrate::setIntegrationRegionUp()
{
    _integration_region_box = new Spoiler("Integration region");
    GridFiller f(_integration_region_box, true);

    _integration_region_type = f.addCombo("Integration region type");
    for (int i = 0; i < static_cast<int>(ohkl::RegionType::Count); ++i)
        _integration_region_type->addItem(QString::fromStdString(
            ohkl::regionTypeDescription.at(static_cast<ohkl::RegionType>(i))));

    _peak_end = f.addDoubleSpinBox(
        "Peak end", "(" + QString(QChar(0x03C3)) + ") - scaling factor for peak region");

    _bkg_begin = f.addDoubleSpinBox(
        "Background begin:",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for lower limit of background");

    _bkg_end = f.addDoubleSpinBox(
        "Background end:",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for upper limit of background");

    _peak_end->setMaximum(50);
    _peak_end->setDecimals(2);

    _bkg_begin->setMaximum(50);
    _bkg_begin->setDecimals(2);

    _bkg_end->setMaximum(50);
    _bkg_end->setDecimals(2);

    _left_layout->addWidget(_integration_region_box);
}

void SubframeIntegrate::setIntegrateUp()
{
    _integrate_box = new Spoiler("Integrate peaks");
    GridFiller f(_integrate_box, true);

    _integrator_combo = f.addCombo();

    _fit_center =
        f.addCheckBox("Fit peak center", "Allow the peak center to move during integration", 1);

    _fit_covariance = f.addCheckBox(
        "Fit peak covariance", "Allow the peak covariance matrix to vary during integration", 1);

    _remove_overlaps = f.addCheckBox(
        "Remove overlaps", "Remove peaks with overlapping adjacent background regions", 1);

    _remove_masked =
        f.addCheckBox("Remove masked peaks", "Remove peaks intersecting detector image masks", 1);

    _compute_gradient = new QGroupBox("Compute gradient");
    _compute_gradient->setAlignment(Qt::AlignLeft);
    _compute_gradient->setCheckable(true);
    _compute_gradient->setChecked(false);
    _compute_gradient->setToolTip("Discard peaks with high mean background gradient");

    _gradient_kernel = new QComboBox();

    _fft_gradient = new QCheckBox("FFT gradient");
    _fft_gradient->setToolTip("Use Fourier transform for image filtering");
    _fft_gradient->setChecked(false);

    QGridLayout* grid = new QGridLayout();
    _compute_gradient->setLayout(grid);
    QLabel* label = new QLabel("Kernel");
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_gradient_kernel, 0, 1, 1, 1);
    grid->addWidget(_fft_gradient, 1, 1, 1, -1);
    f.addWidget(_compute_gradient);

    _discard_saturated = new QGroupBox("Discard saturated");
    _discard_saturated->setAlignment(Qt::AlignLeft);
    _discard_saturated->setCheckable(true);
    _discard_saturated->setChecked(false);
    _discard_saturated->setToolTip("Discard peaks containing saturated pixels");

    _max_counts = new SafeDoubleSpinBox();
    _max_counts->setMaximum(100000);

    label = new QLabel("Maximum count");
    label->setToolTip("Maximum count for a pixel in a single peak");
    grid = new QGridLayout();
    _discard_saturated->setLayout(grid);
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_max_counts, 0, 1, 1, 1);
    f.addWidget(_discard_saturated);

    _use_max_strength = new QGroupBox("Maximum strength for profile integration");
    _use_max_strength->setAlignment(Qt::AlignLeft);
    _use_max_strength->setCheckable(true);
    _use_max_strength->setChecked(false);
    _use_max_strength->setToolTip(
        "Skip profile integration of peaks with sum intensity strength above maximum");

    _max_strength = new SafeDoubleSpinBox();
    _max_strength->setMaximum(1e9);

    label = new QLabel("Maximum strength");
    label->setToolTip("Maximum strength for peak to be profile integrated");
    grid = new QGridLayout();
    _use_max_strength->setLayout(grid);
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_max_strength, 0, 1, 1, 1);
    f.addWidget(_use_max_strength);

    _use_max_d = new QGroupBox("Maximum d for profile integration");
    _use_max_d->setAlignment(Qt::AlignLeft);
    _use_max_d->setCheckable(true);
    _use_max_d->setChecked(false);
    _use_max_d->setToolTip("Skip profile integration of peaks with d below maximum");

    _max_d = new SafeDoubleSpinBox();
    _max_d->setMaximum(100);

    label = new QLabel("Maximum d");
    label->setToolTip("Maximum d for peak to be profile integrated");
    grid = new QGridLayout();
    _use_max_d->setLayout(grid);
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_max_d, 0, 1, 1, 1);
    f.addWidget(_use_max_d);

    _use_max_width = new QGroupBox("Maximum width for integration");
    _use_max_width->setAlignment(Qt::AlignLeft);
    _use_max_width->setCheckable(true);
    _use_max_width->setChecked(false);
    _use_max_width->setToolTip("Skip integration of peaks spanning too many images");

    _max_width = new SafeSpinBox();
    _max_width->setMaximum(20);

    label = new QLabel("Maximum width");
    label->setToolTip("Maximum width for peak to be integrated");
    grid = new QGridLayout();
    _use_max_width->setLayout(grid);
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_max_width, 0, 1, 1, 1);
    f.addWidget(_use_max_width);

    for (const auto& [kernel, description] : _kernel_description)
        _gradient_kernel->addItem(description);
    _gradient_kernel->setCurrentIndex(1);

    _radius_int = f.addDoubleSpinBox(
        "Search radius (pixels):", "(pixels) - neighbour search radius in pixels");

    _n_frames_int = f.addDoubleSpinBox(
        "Search radius (images)", "(detector images) - neighbour search radius in detector images");

    _interpolation_combo =
        f.addCombo("Interpolation type", "Interpolation strategy for computing mean covariance");

    _shape_combo = f.addShapeCombo("Shape model", "The shape model used in integraton");

    _integrate_button = f.addButton("Integrate peaks");

    // -- Initialize controls
    for (std::size_t idx = 0; idx < static_cast<int>(ohkl::IntegratorType::Count); ++idx) {
        const std::string integrator_type =
            _integrator_strings.at(static_cast<ohkl::IntegratorType>(idx));
        _integrator_combo->addItem(QString::fromStdString(integrator_type));
    }

    _interpolation_combo->addItem("None");
    _interpolation_combo->addItem("Inverse distance");
    _interpolation_combo->addItem("Intensity");

    _radius_int->setMaximum(1000);
    _radius_int->setDecimals(2);

    _n_frames_int->setMaximum(20);
    _n_frames_int->setDecimals(2);

    connect(_integrate_button, &QPushButton::clicked, this, &SubframeIntegrate::runIntegration);
    connect(
        _integrator_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::toggleUnsafeWidgets);

    _left_layout->addWidget(_integrate_box);
}

void SubframeIntegrate::setPreviewUp()
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

void SubframeIntegrate::runIntegration()
{
    gGui->setReady(false);
    try {
        ohkl::sptrProgressHandler handler(new ohkl::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        ohkl::Experiment* expt = gSession->currentProject()->experiment();
        ohkl::Integrator* integrator = expt->integrator();
        ohkl::sptrDataSet data = _data_combo->currentData();
        ohkl::PeakCollection* peaks_to_integrate = _peak_combo->currentPeakCollection();
        ohkl::ShapeModel* shapes = nullptr;
        if (gSession->currentProject()->hasShapeModel())
            shapes = _shape_combo->currentShapes();

        setIntegrationParameters();
        auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

        integrator->getIntegrator(params->integrator_type)->setHandler(handler);
        integrator->integratePeaks(data, peaks_to_integrate, params, shapes);
        gGui->detector_window->refreshAll();
        gGui->statusBar()->showMessage(
            QString::number(integrator->numberOfValidPeaks()) + "/"
            + QString::number(integrator->numberOfPeaks()) + " peaks integrated");
        refreshPeakTable();
        gSession->onPeaksChanged();
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
}

void SubframeIntegrate::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframeIntegrate::toggleUnsafeWidgets()
{
    bool isPxsum = _integrator_combo->currentIndex() == 0;

    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _integrate_button->setToolTip("");
    _integrate_button->setEnabled(isPxsum);
    _use_max_strength->setEnabled(!isPxsum);
    _use_max_d->setEnabled(!isPxsum);
    _compute_gradient->setEnabled(isPxsum);
    _fit_center->setEnabled(isPxsum);
    _fit_covariance->setEnabled(isPxsum);
    _radius_int->setEnabled(!isPxsum);
    _n_frames_int->setEnabled(!isPxsum);
    _interpolation_combo->setEnabled(!isPxsum);
    _shape_combo->setEnabled(!isPxsum);
    if (!isPxsum) {
        _integrate_button->setEnabled(gSession->currentProject()->hasShapeModel());
        _integrate_button->setToolTip("No shape model available");
    }
}


DetectorWidget* SubframeIntegrate::detectorWidget()
{
    return _detector_widget;
}

void SubframeIntegrate::onGradientSettingsChanged()
{
    emit signalGradient(_gradient_kernel->currentIndex(), _fft_gradient->isChecked());
}
