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
        _integrator_combo,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::toggleUnsafeWidgets);
    connect(
        _integration_region_type,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        _detector_widget, &DetectorWidget::refresh);
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

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeIntegrate::toggleUnsafeWidgets);

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
    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeIntegrate::refreshPeakTable);

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

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeIntegrate::refreshPeakTable()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _peak_collection = _peak_combo->currentPeakCollection();
    _peak_collection_item.setPeakCollection(_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    _detector_widget->refresh();
}

void SubframeIntegrate::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
    _detector_widget->refresh();
    _peak_combo->refresh();
    refreshPeakTable();
    grabIntegrationParameters();
    toggleUnsafeWidgets();
}

void SubframeIntegrate::grabIntegrationParameters()
{

    auto* expt = gSession->currentProject()->experiment();
    auto* integrator = expt->integrator();
    auto* params = integrator->parameters();

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
    _radius_int->setValue(params->neighbour_range_pixels);
    _n_frames_int->setValue(params->neighbour_range_frames);
    _fit_center->setChecked(params->fit_center);
    _fit_covariance->setChecked(params->fit_cov);
    _min_neighbours->setValue(params->min_neighbors);
    _compute_gradient->setChecked(params->use_gradient);
    _fft_gradient->setChecked(params->fft_gradient);
    _gradient_kernel->setCurrentIndex(static_cast<int>(params->gradient_type));
    _remove_masked->setChecked(params->skip_masked);
    _remove_overlaps->setChecked(params->remove_overlaps);

    _integrator_combo->setCurrentIndex(static_cast<int>(params->integrator_type));

    for (auto it = ohkl::regionTypeDescription.begin(); it != ohkl::regionTypeDescription.end();
         ++it)
        if (it->first == params->region_type)
            _integration_region_type->setCurrentText(QString::fromStdString(it->second));
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
    params->neighbour_range_pixels = _radius_int->value();
    params->neighbour_range_frames = _n_frames_int->value();
    params->fit_center = _fit_center->isChecked();
    params->fit_cov = _fit_covariance->isChecked();
    params->min_neighbors = _min_neighbours->value();
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

    for (auto it = ohkl::regionTypeDescription.begin(); it != ohkl::regionTypeDescription.end();
         ++it)
        if (it->second == _integration_region_type->currentText().toStdString())
            params->region_type = it->first;
}

void SubframeIntegrate::setIntegrationRegionUp()
{
    _integration_region_box = new Spoiler("Integration region");
    GridFiller f(_integration_region_box, true);

    _integration_region_type = f.addCombo(
        "Integration region type",
        "<font>Specify integration region in Pixels (peak end), and"
        "scaling factors for background region (bkg begin, bkg end)</font>");
    for (int i = 0; i < static_cast<int>(ohkl::RegionType::Count); ++i)
        for (const auto& [key, val] : ohkl::regionTypeDescription)
            if (i == static_cast<int>(key))
                _integration_region_type->addItem(QString::fromStdString(val));

    _peak_end = f.addDoubleSpinBox(
        "Peak end", "(" + QString(QChar(0x03C3)) + ") - scaling factor for peak region");

    _bkg_begin = f.addDoubleSpinBox(
        "Background begin:",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for lower limit of background");

    _bkg_end = f.addDoubleSpinBox(
        "Background end:",
        "(" + QString(QChar(0x03C3)) + ") - scaling factor for upper limit of background");

    _peak_end->setMaximum(20);
    _peak_end->setDecimals(2);

    _bkg_begin->setMaximum(10);
    _bkg_begin->setDecimals(2);

    _bkg_end->setMaximum(10);
    _bkg_end->setDecimals(2);

    _left_layout->addWidget(_integration_region_box);
}

void SubframeIntegrate::setIntegrateUp()
{
    _integrate_box = new Spoiler("Integrate peaks");
    GridFiller f(_integrate_box, true);

    _integrator_combo = f.addCombo();

    _discard_saturated = new QGroupBox("Discard saturated");
    _discard_saturated->setAlignment(Qt::AlignLeft);
    _discard_saturated->setCheckable(true);
    _discard_saturated->setChecked(false);
    _discard_saturated->setToolTip("Discard peaks containing saturated pixels");

    _max_counts = new SafeDoubleSpinBox();
    _max_counts->setMaximum(100000);

    QLabel* label = new QLabel("Maximum count");
    label->setToolTip("Maximum count for a pixel in a single peak");
    QGridLayout* grid = new QGridLayout();
    _discard_saturated->setLayout(grid);
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_max_counts, 0, 1, 1, 1);
    f.addWidget(_discard_saturated);

    _compute_gradient = new QGroupBox("Compute gradient");
    _compute_gradient->setAlignment(Qt::AlignLeft);
    _compute_gradient->setCheckable(true);
    _compute_gradient->setChecked(false);
    _compute_gradient->setToolTip("Discard peaks with high mean background gradient");

    _gradient_kernel = new QComboBox();

    _fft_gradient = new QCheckBox("FFT gradient");
    _fft_gradient->setToolTip("Use Fourier transform for image filtering");
    _fft_gradient->setChecked(false);

    grid = new QGridLayout();
    _compute_gradient->setLayout(grid);
    label = new QLabel("Kernel");
    grid->addWidget(label, 0, 0, 1, 1);
    grid->addWidget(_gradient_kernel, 0, 1, 1, 1);
    grid->addWidget(_fft_gradient, 1, 1, 1, -1);
    f.addWidget(_compute_gradient);

    for (const auto& [kernel, description] : _kernel_description)
        _gradient_kernel->addItem(description);
    _gradient_kernel->setCurrentIndex(1);

    _fit_center =
        f.addCheckBox("Fit peak center", "Allow the peak center to move during integration", 1);

    _fit_covariance = f.addCheckBox(
        "Fit peak covariance", "Allow the peak covariance matrix to vary during integration", 1);

    _radius_int = f.addDoubleSpinBox(
        "Search radius (pixels):", "(pixels) - neighbour search radius in pixels");

    _n_frames_int = f.addDoubleSpinBox(
        "Search radius (images)", "(detector images) - neighbour search radius in detector images");

    _min_neighbours = f.addSpinBox(
        "Min. neighbours", "Minimum number of neighbouring shapes to predict peak shape");

    _interpolation_combo =
        f.addCombo("Interpolation type", "Interpolation strategy for computing mean covariance");

    _shape_combo = f.addShapeCombo("Shape model", "The shape model used in integraton");

    _remove_overlaps = f.addCheckBox(
        "Remove overlaps", "Remove peaks with overlapping adjacent background regions", 1);

    _remove_masked =
        f.addCheckBox("Remove masked peaks", "Remove peaks intersecting detector image masks", 1);

    _integrate_button = f.addButton("Integrate peaks");

    // -- Initialize controls
    for (std::size_t idx = 0; idx < static_cast<int>(ohkl::IntegratorType::Count); ++idx) {
        const std::string integrator_type = _integrator_strings.at(static_cast<ohkl::IntegratorType>(idx));
        _integrator_combo->addItem(QString::fromStdString(integrator_type));
    }

    _interpolation_combo->addItem("None");
    _interpolation_combo->addItem("Inverse distance");
    _interpolation_combo->addItem("Intensity");

    _radius_int->setMaximum(1000);
    _radius_int->setDecimals(2);

    _n_frames_int->setMaximum(20);
    _n_frames_int->setDecimals(2);

    _min_neighbours->setMaximum(1000);

    connect(_integrate_button, &QPushButton::clicked, this, &SubframeIntegrate::runIntegration);
    connect(
        _integrator_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::toggleUnsafeWidgets);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframeIntegrate::setIntegrationParameters);

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
    _integrate_button->setEnabled(false);
    _remove_overlaps->setEnabled(false);

    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _remove_overlaps->setEnabled(true);

    if (_integrator_combo->currentIndex() == 0)
        _integrate_button->setEnabled(true);
    else
        _integrate_button->setEnabled(gSession->currentProject()->hasShapeModel());
}


DetectorWidget* SubframeIntegrate::detectorWidget()
{
    return _detector_widget;
}

void SubframeIntegrate::onGradientSettingsChanged()
{
    emit signalGradient(_gradient_kernel->currentIndex(), _fft_gradient->isChecked());
}
