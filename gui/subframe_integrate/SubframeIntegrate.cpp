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
#include "gui/MainWin.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_predict/ShapeCollectionDialog.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>

SubframeIntegrate::SubframeIntegrate() : QWidget()
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout(this);

    setInputUp();
    setPreviewUp();
    setIntegrateUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);
}

void SubframeIntegrate::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _exp_combo = f.addCombo("Experiment");
    _data_combo = f.addCombo("Data set");
    _peak_combo = f.addCombo("Found peaks", "Used to build shape collection");
    _int_peak_combo = f.addCombo("Peaks to integrate");

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeIntegrate::updateDatasetList);
    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeIntegrate::updatePeakList);
    connect(
        _int_peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::refreshPeakTable);

    _left_layout->addWidget(input_box);
}

void SubframeIntegrate::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    QGridLayout* figure_grid = new QGridLayout(figure_group);

    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _figure_view = new DetectorView(this);
    _figure_view->getScene()->linkPeakModel1(&_peak_collection_model);
    _figure_view->scale(1, -1);
    figure_grid->addWidget(_figure_view, 0, 0, 1, 2);

    _figure_scroll = new QScrollBar(this);
    _figure_scroll->setOrientation(Qt::Horizontal);
    _figure_scroll->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    figure_grid->addWidget(_figure_scroll, 1, 0, 1, 1);

    _figure_spin = new QSpinBox(this);
    _figure_spin->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    figure_grid->addWidget(_figure_spin, 1, 1, 1, 1);

    connect(
        _figure_scroll, SIGNAL(valueChanged(int)), _figure_view->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(_figure_scroll, SIGNAL(valueChanged(int)), _figure_spin, SLOT(setValue(int)));

    connect(_figure_spin, SIGNAL(valueChanged(int)), _figure_scroll, SLOT(setValue(int)));

    connect(
        _figure_view->getScene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeIntegrate::changeSelected);

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
    _figure_view->getScene()->update();
    _figure_view->getScene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    _figure_view->getScene()->drawPeakitems();
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
    if (_int_peak_combo->count() == 0)
        return;

    _peak_collection =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->
        getPeakCollection(_int_peak_combo->currentText().toStdString());

    _figure_view->getScene()->clearPeakItems();
    _peak_collection_item.setPeakCollection(_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();

    refreshPeakVisual();
}

void SubframeIntegrate::refreshAll()
{
    updateExptList();
}

void SubframeIntegrate::updateExptList()
{
    _exp_combo->blockSignals(true);
    QString current_exp = _exp_combo->currentText();
    _exp_combo->clear();

    if (!gSession->experimentNames().empty()) {
        for (const QString& exp : gSession->experimentNames())
            _exp_combo->addItem(exp);
        _exp_combo->setCurrentText(current_exp);

        updateDatasetList();
        updatePeakList();
        refreshPeakTable();

        _integration_params =
            gSession->experimentAt(_exp_combo->currentIndex())->experiment()->int_params;
        _shape_params =
            gSession->experimentAt(_exp_combo->currentIndex())->experiment()->shape_params;
    }
    _exp_combo->blockSignals(false);
}

void SubframeIntegrate::updateDataset(const QString& dataname)
{
    nsx::sptrDataSet data =
        gSession->experimentAt(
            _exp_combo->currentIndex())->experiment()->getData(dataname.toStdString());

    _figure_view->getScene()->slotChangeSelectedData(data, _figure_spin->value());
    //_figure_view->getScene()->setMaxIntensity(3000);
    emit _figure_view->getScene()->dataChanged();
    _figure_view->getScene()->update();

    _figure_scroll->setMaximum(data->nFrames() - 1);
    _figure_scroll->setMinimum(0);

    _figure_spin->setMaximum(data->nFrames() - 1);
    _figure_spin->setMinimum(0);
}

void SubframeIntegrate::updateDatasetList()
{
    _data_combo->blockSignals(true);
    QString current_data = _data_combo->currentText();
    _data_combo->clear();
    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    const QStringList& datanames{gSession->currentProject()->getDataNames()};
    if (!datanames.empty()) {
        _data_combo->addItems(datanames);
        _data_combo->setCurrentText(current_data);
        updateDataset(_data_combo->currentText());
    }
    _data_combo->blockSignals(false);
}

void SubframeIntegrate::updatePeakList()
{
    _peak_combo->blockSignals(true);
    QString current_peaks = _peak_combo->currentText();
    _peak_combo->clear();

    QStringList tmp = gSession->experimentAt(_exp_combo->currentIndex())
                          ->getPeakCollectionNames(nsx::listtype::FOUND);
    if (!tmp.empty()) {
        _peak_combo->addItems(tmp);
        tmp.clear();
    }

    tmp = gSession->experimentAt(_exp_combo->currentIndex())
        ->getPeakCollectionNames(nsx::listtype::FILTERED);

    if (!tmp.empty()) {
        _peak_combo->addItems(tmp);
        _peak_combo->setCurrentText(current_peaks);
    }

    tmp = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();
    if (!tmp.empty()) {
        current_peaks = _int_peak_combo->currentText();
        _int_peak_combo->addItems(tmp);
        _peak_combo->setCurrentText(current_peaks);
    }

    _peak_combo->blockSignals(false);
    refreshShapeStatus();
}

void SubframeIntegrate::setIntegrateUp()
{
    _integrate_box = new Spoiler("Integrate peaks");
    GridFiller f(_integrate_box, true);

    // -- Create controls
    _integrator_combo = f.addCombo();

    auto build_shape_lib = f.addButton(
        "Build shape collection",
        "<font>A shape collection is a collection of averaged peaks attached to a peak"
        "collection. A shape is the averaged peak shape of a peak and its neighbours within a "
        "specified cutoff.</font>"); // Rich text to force line break in tooltip

    _fit_center =
        f.addCheckBox("Fit the center", "Allow the peak center to move during integration");

    _fit_covariance = f.addCheckBox(
        "Fit the covariance", "Allow the peak covariance matrix to vary during integration");

    _peak_end = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");

    _bkg_begin =
        f.addDoubleSpinBox("Bkg begin:", "(sigmas) - scaling factor for lower limit of background");

    _bkg_end =
        f.addDoubleSpinBox("Bkg end:", "(sigmas) - scaling factor for upper limit of background");

    _radius_int =
        f.addDoubleSpinBox("Search radius:", "(pixels) - neighbour search radius in pixels");

    _n_frames_int =
        f.addDoubleSpinBox("N. of frames:", "(frames) - neighbour search radius in frames");

    _min_neighbours = f.addSpinBox(
        "Min. neighbours", "Minimum number of neighbouring shapes to predict peak shape");

    _interpolation_combo = f.addCombo("Interpolation", "Interpolation type for peak shape");

    _integrate = f.addButton("Integrate peaks");

    // -- Initialize controls
    _integrator_combo->addItem("Pixel sum integrator");
    _integrator_combo->addItem("Gaussian integrator");
    _integrator_combo->addItem("I/Sigma integrator");
    _integrator_combo->addItem("1d profile integrator");
    _integrator_combo->addItem("3d profile integrator");

    _interpolation_combo->addItem("None");
    _interpolation_combo->addItem("Inverse distance");
    _interpolation_combo->addItem("Intensity");

    _fit_center->setChecked(_integration_params.fit_center);

    _fit_covariance->setChecked(_integration_params.fit_cov);

    _peak_end->setMaximum(100000);
    _peak_end->setDecimals(2);
    _peak_end->setValue(_integration_params.peak_end);

    _bkg_begin->setMaximum(100000);
    _bkg_begin->setDecimals(2);
    _bkg_begin->setValue(_integration_params.bkg_begin);

    _bkg_end->setMaximum(100000);
    _bkg_end->setDecimals(2);
    _bkg_end->setValue(_integration_params.bkg_end);

    _radius_int->setMaximum(100000);
    _radius_int->setDecimals(2);
    _radius_int->setValue(_integration_params.neighbour_range_pixels);

    _n_frames_int->setMaximum(100000);
    _n_frames_int->setDecimals(2);
    _n_frames_int->setValue(_integration_params.neighbour_range_frames);

    _min_neighbours->setMaximum(100000);
    _min_neighbours->setValue(_integration_params.min_neighbors);

    connect(_integrate, &QPushButton::clicked, this, &SubframeIntegrate::runIntegration);
    connect(build_shape_lib, &QPushButton::clicked, this, &SubframeIntegrate::openShapeBuilder);
    connect(
        _integrator_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeIntegrate::refreshShapeStatus);
    connect(
        _peak_view_widget->set1.peakEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_end, &QDoubleSpinBox::setValue);
    connect(
        _peak_view_widget->set1.bkgBegin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_begin, &QDoubleSpinBox::setValue);
    connect(
        _peak_view_widget->set1.bkgEnd, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _bkg_end, &QDoubleSpinBox::setValue);
    connect(
        _peak_end, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.peakEnd, &QDoubleSpinBox::setValue);
    connect(
        _bkg_begin, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.bkgBegin, &QDoubleSpinBox::setValue);
    connect(
        _bkg_end, qOverload<double>(&QDoubleSpinBox::valueChanged),
        _peak_view_widget->set1.bkgEnd, &QDoubleSpinBox::setValue);
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

    _peak_view_widget->set1.drawIntegrationRegion->setChecked(true);
    _peak_view_widget->set1.previewIntRegion->setChecked(true);
    _left_layout->addWidget(preview_spoiler);
    preview_spoiler->setExpanded(true);
}

void SubframeIntegrate::runIntegration()
{
    try {
        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        nsx::IPeakIntegrator* integrator =
            expt->getIntegrator(_integrator_combo->currentText().toStdString());
        nsx::PeakCollection* peaks_to_integrate =
            expt->getPeakCollection(_int_peak_combo->currentText().toStdString());
        nsx::ShapeCollection* shapes =
            expt->getPeakCollection(_peak_combo->currentText().toStdString())->shapeCollection();

        int interpol = _interpolation_combo->currentIndex();
        nsx::PeakInterpolation peak_interpolation = static_cast<nsx::PeakInterpolation>(interpol);

        _integration_params.peak_end = _peak_end->value();
        _integration_params.bkg_begin = _bkg_begin->value();
        _integration_params.bkg_end = _bkg_end->value();
        _integration_params.neighbour_range_pixels = _radius_int->value();
        _integration_params.neighbour_range_frames = _n_frames_int->value();
        _integration_params.fit_center = _fit_center->isChecked();
        _integration_params.fit_cov = _fit_covariance->isChecked();
        _integration_params.min_neighbors = _min_neighbours->value();

        shapes->setPredictedShapes(peaks_to_integrate, peak_interpolation, handler);
        integrator->setHandler(handler);
        expt->integratePeaks(integrator, peaks_to_integrate, &_integration_params, shapes);
    } catch (std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }
}

void SubframeIntegrate::openShapeBuilder()
{
    // #nsxAudit Crash if no experiment existing. Disable btn if no experiment loaded?
    nsx::PeakCollection* peak_collection =
        gSession->experimentAt(_exp_combo->currentIndex())
        ->experiment()
        ->getPeakCollection(_peak_combo->currentText().toStdString());

    std::unique_ptr<ShapeCollectionDialog> dialog(
        new ShapeCollectionDialog(peak_collection, _shape_params));

    dialog->exec();
    refreshShapeStatus();
}

void SubframeIntegrate::refreshShapeStatus()
{
    bool shape_collection_present = true;

    if (_peak_combo->count() == 0 || _exp_combo->count() == 0)
        shape_collection_present = false;

    if (shape_collection_present) {
        nsx::PeakCollection* collection =
            gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());
        if (collection->shapeCollection() == nullptr)
            shape_collection_present = false;
    }

    if (_integrator_combo->currentText().toStdString() == "Pixel sum integrator") {
        _integrate->setEnabled(true);
        _interpolation_combo->setEnabled(false);
        _radius_int->setEnabled(false);
        _n_frames_int->setEnabled(false);
        _min_neighbours->setEnabled(false);
    } else {
        _integrate->setEnabled(shape_collection_present);
        _interpolation_combo->setEnabled(true);
        _radius_int->setEnabled(true);
        _n_frames_int->setEnabled(true);
        _min_neighbours->setEnabled(true);
    }
}

void SubframeIntegrate::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}
