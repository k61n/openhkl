//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_filter/SubframeIntegrate.cpp

//! @brief     Implements class SubframeIntegrate
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_integrate/SubframeIntegrate.h"

#include "core/experiment/Experiment.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "gui/MainWin.h" // gGui
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_predict/ShapeCollectionDialog.h"
#include "gui/subwindows/DetectorWindow.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
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

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _shape_params = nullptr;
}

void SubframeIntegrate::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _data_combo = f.addDataCombo("Data set");
    _peak_combo = f.addPeakCombo(
        ComboType::FoundPeaks, "Peaks for shapes", "Used to build shape collection");
    _build_shape_lib_button = f.addButton(
        "Build shape collection",
        "<font>A shape collection is a collection of averaged peaks attached to a peak"
        "collection. A shape is the averaged peak shape of a peak and its neighbours within a "
        "specified cutoff.</font>"); // Rich text to force line break in tooltip
    _int_peak_combo = f.addPeakCombo(ComboType::PeakCollection, "Peaks to integrate");

    connect(
        _int_peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::toggleUnsafeWidgets);

    _left_layout->addWidget(input_box);
}

void SubframeIntegrate::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(false, false, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model);

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeIntegrate::changeSelected);
    connect(
        _int_peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::refreshPeakTable);

    _right_element->addWidget(figure_group);
}

void SubframeIntegrate::refreshPeakVisual()
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

void SubframeIntegrate::setPeakTableUp()
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

void SubframeIntegrate::refreshPeakTable()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _peak_collection = _int_peak_combo->currentPeakCollection();
    _peak_collection_item.setPeakCollection(_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    refreshPeakVisual();
}

void SubframeIntegrate::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
    _detector_widget->updateDatasetList(gSession->currentProject()->experiment()->getAllData());
    _detector_widget->refresh();
    _peak_combo->refresh();
    _int_peak_combo->refresh();
    refreshPeakTable();
    grabIntegrationParameters();
    toggleUnsafeWidgets();
}

void SubframeIntegrate::grabIntegrationParameters()
{

    auto* expt = gSession->currentProject()->experiment();
    auto* integrator = expt->integrator();
    auto* params = integrator->parameters();

    _peak_end->setValue(params->peak_end);
    _bkg_begin->setValue(params->bkg_begin);
    _bkg_end->setValue(params->bkg_end);
    _radius_int->setValue(params->neighbour_range_pixels);
    _n_frames_int->setValue(params->neighbour_range_frames);
    _fit_center->setChecked(params->fit_center);
    _fit_covariance->setChecked(params->fit_cov);
    _min_neighbours->setValue(params->min_neighbors);

    for (auto it = _integrator_strings.begin(); it != _integrator_strings.end(); ++it)
        if (it->second == params->integrator_type)
            _integrator_combo->setCurrentText(QString::fromStdString(it->first));

    for (auto it = nsx::regionTypeDescription.begin(); it != nsx::regionTypeDescription.end(); ++it)
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

    params->peak_end = _peak_end->value();
    params->bkg_begin = _bkg_begin->value();
    params->bkg_end = _bkg_end->value();
    params->neighbour_range_pixels = _radius_int->value();
    params->neighbour_range_frames = _n_frames_int->value();
    params->fit_center = _fit_center->isChecked();
    params->fit_cov = _fit_covariance->isChecked();
    params->min_neighbors = _min_neighbours->value();
    params->region_type = static_cast<nsx::RegionType>(_integration_region_type->currentIndex());
    params->integrator_type =
        _integrator_strings.find(_integrator_combo->currentText().toStdString())->second;

    for (auto it = nsx::regionTypeDescription.begin(); it != nsx::regionTypeDescription.end(); ++it)
        if (it->second == _integration_region_type->currentText().toStdString())
            params->region_type = it->first;
}

void SubframeIntegrate::setIntegrationRegionUp()
{
    _integration_region_box = new Spoiler("Integration region");
    GridFiller f(_integration_region_box, true);

    _integration_region_type = f.addLinkedCombo(
        ComboType::RegionType, "Integration region type",
        "<font>Specify integration region in Pixels (peak end), and"
        "scaling factors for background region (bkg begin, bkg end)</font>");
    for (int i = 0; i < static_cast<int>(nsx::RegionType::Count); ++i)
        for (const auto& [key, val] : nsx::regionTypeDescription)
            if (i == static_cast<int>(key))
                _integration_region_type->addItem(QString::fromStdString(val));

    _peak_end = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");

    _bkg_begin =
        f.addDoubleSpinBox("Bkg begin:", "(sigmas) - scaling factor for lower limit of background");

    _bkg_end =
        f.addDoubleSpinBox("Bkg end:", "(sigmas) - scaling factor for upper limit of background");

    _peak_end->setMaximum(20);
    _peak_end->setDecimals(2);

    _bkg_begin->setMaximum(10);
    _bkg_begin->setDecimals(2);

    _bkg_end->setMaximum(10);
    _bkg_end->setDecimals(2);

    _left_layout->addWidget(_integration_region_box);

    connect(
        _integration_region_type,
        static_cast<void (LinkedComboBox::*)(int)>(&LinkedComboBox::currentIndexChanged), this,
        &SubframeIntegrate::refreshPeakVisual);
}

void SubframeIntegrate::setIntegrateUp()
{
    _integrate_box = new Spoiler("Integrate peaks");
    GridFiller f(_integrate_box, true);

    // -- Create controls
    _integrator_combo = f.addCombo();

    _fit_center =
        f.addCheckBox("Fit the center", "Allow the peak center to move during integration", 1);

    _fit_covariance = f.addCheckBox(
        "Fit the covariance", "Allow the peak covariance matrix to vary during integration", 1);

    _radius_int =
        f.addDoubleSpinBox("Search radius:", "(pixels) - neighbour search radius in pixels");

    _n_frames_int =
        f.addDoubleSpinBox("N. of frames:", "(frames) - neighbour search radius in frames");

    _min_neighbours = f.addSpinBox(
        "Min. neighbours", "Minimum number of neighbouring shapes to predict peak shape");

    _interpolation_combo = f.addCombo("Interpolation", "Interpolation type for peak shape");

    _assign_peak_shapes = f.addButton(
        "Assign peak shapes", "Assign peak shapes from shape collection to a predicted collection");

    _remove_overlaps = f.addCheckBox(
        "Remove overlaps", "Remove peaks with overlapping adjacent background regions", 1);

    _integrate_button = f.addButton("Integrate peaks");

    // -- Initialize controls
    for (const auto& [key, val] : _integrator_strings)
        _integrator_combo->addItem(QString::fromStdString(key));

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
        _remove_overlaps, &QCheckBox::stateChanged, this,
        &SubframeIntegrate::removeOverlappingPeaks);
    connect(
        _peak_end, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &SubframeIntegrate::removeOverlappingPeaks);
    connect(
        _build_shape_lib_button, &QPushButton::clicked, this, &SubframeIntegrate::openShapeBuilder);
    connect(_assign_peak_shapes, &QPushButton::clicked, this, &SubframeIntegrate::assignPeakShapes);
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

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeIntegrate::refreshPeakVisual);

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

void SubframeIntegrate::assignPeakShapes()
{
    gGui->setReady(false);
    try {
        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::PeakCollection* peaks_to_integrate =_int_peak_combo->currentPeakCollection();
        nsx::ShapeCollection* shapes = _peak_combo->currentPeakCollection()->shapeCollection();

        int interpol = _interpolation_combo->currentIndex();
        nsx::PeakInterpolation peak_interpolation = static_cast<nsx::PeakInterpolation>(interpol);

        shapes->setHandler(handler);
        shapes->setPredictedShapes(peaks_to_integrate, peak_interpolation);
        gGui->statusBar()->showMessage(
            QString::number(peaks_to_integrate->numberOfValid()) + "/"
            + QString::number(peaks_to_integrate->numberOfPeaks())
            + " predicted peaks with valid shapes");
        refreshPeakTable();
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
}

void SubframeIntegrate::removeOverlappingPeaks()
{
    gGui->setReady(false);

    if (_int_peak_combo->count() == 0)
        return;

    nsx::PeakCollection* peaks_to_integrate = _int_peak_combo->currentPeakCollection();
    nsx::PeakFilter filter;
    filter.resetFiltering(peaks_to_integrate);
    if (_remove_overlaps->isChecked()) {
        filter.parameters()->peak_end = _peak_end->value();
        filter.parameters()->bkg_end = _peak_end->value();
        filter.filterOverlapping(peaks_to_integrate);
        for (auto* peak : peaks_to_integrate->getPeakList()) {
            if (!peak->caughtByFilter()) {
                peak->setSelected(false);
                peak->setRejectionFlag(nsx::RejectionFlag::OverlappingPeak);
            }
        }
    } else {
        for (auto* peak : peaks_to_integrate->getPeakList()) {
            if (!(peak->selected())
                && peak->rejectionFlag() == nsx::RejectionFlag::OverlappingPeak) {
                peak->setSelected(true);
                peak->setRejectionFlag(nsx::RejectionFlag::NotRejected, true);
            }
        }
    }
    refreshPeakTable();
    gGui->setReady(true);
}

void SubframeIntegrate::runIntegration()
{
    gGui->setReady(false);
    try {
        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::Experiment* expt = gSession->currentProject()->experiment();
        nsx::Integrator* integrator = expt->integrator();
        nsx::sptrDataSet data = _data_combo->currentData();
        nsx::PeakCollection* peaks_to_integrate = _int_peak_combo->currentPeakCollection();
        nsx::ShapeCollection* shapes = _peak_combo->currentPeakCollection()->shapeCollection();

        setIntegrationParameters();
        auto* params = gSession->currentProject()->experiment()->integrator()->parameters();

        integrator->getIntegrator(params->integrator_type)->setHandler(handler);
        integrator->integratePeaks(data, peaks_to_integrate, params, shapes);
        gGui->detector_window->refreshAll();
        gGui->statusBar()->showMessage(
            QString::number(integrator->numberOfValidPeaks()) + "/"
            + QString::number(integrator->numberOfPeaks()) + " peaks integrated");
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
    gGui->setReady(true);
}

void SubframeIntegrate::openShapeBuilder()
{
    gGui->setReady(false);
    nsx::PeakCollection* peak_collection = _peak_combo->currentPeakCollection();
    std::unique_ptr<ShapeCollectionDialog> dialog(
        new ShapeCollectionDialog(peak_collection, _shape_params));

    dialog->exec();
    toggleUnsafeWidgets();
    if (peak_collection->shapeCollection())
        gGui->statusBar()->showMessage(
            QString::number(peak_collection->shapeCollection()->numberOfPeaks())
            + " shapes generated");
    refreshPeakVisual();
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
    _radius_int->setEnabled(true);
    _n_frames_int->setEnabled(true);
    _min_neighbours->setEnabled(true);
    _interpolation_combo->setEnabled(true);
    _build_shape_lib_button->setEnabled(true);
    _assign_peak_shapes->setEnabled(true);
    _remove_overlaps->setEnabled(true);
    _integrate_button->setEnabled(true);

    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet() ||
        !gSession->currentProject()->hasPeakCollection()) {
        _integrate_button->setEnabled(false);
        _remove_overlaps->setEnabled(false);
        _assign_peak_shapes->setEnabled(false);
    }

    if (_int_peak_combo->count() == 0) {
        _integrate_button->setEnabled(false);
        _assign_peak_shapes->setEnabled(false);
        _remove_overlaps->setEnabled(false);
    }

    if (!_peak_combo->count() == 0) {
        nsx::PeakCollection* peaks = _peak_combo->currentPeakCollection();
        if (peaks->shapeCollection() == nullptr) {
            _assign_peak_shapes->setEnabled(false);
            _integrate_button->setEnabled(false);
            _remove_overlaps->setEnabled(false);
        }
    }

    if (_integrator_strings.find(_integrator_combo->currentText().toStdString())->second
        == nsx::IntegratorType::PixelSum) {
        _integrate_button->setEnabled(true);
        _interpolation_combo->setEnabled(false);
        _radius_int->setEnabled(false);
        _n_frames_int->setEnabled(false);
        _min_neighbours->setEnabled(false);
        _assign_peak_shapes->setEnabled(false);
        _build_shape_lib_button->setEnabled(false);
    }
}


DetectorWidget* SubframeIntegrate::detectorWidget()
{
    return _detector_widget;
}

