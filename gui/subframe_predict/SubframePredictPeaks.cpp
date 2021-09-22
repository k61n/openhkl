//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_predict/SubframePredictPeaks.cpp
//! @brief     Implements classes FoundPeaks, SubframePredictPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_predict/SubframePredictPeaks.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/Experiment.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeCollection.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/items/PeakItem.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_predict/ShapeCollectionDialog.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PeakViewWidget.h"
#include "tables/crystal/UnitCell.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QMessageBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>

SubframePredictPeaks::SubframePredictPeaks()
    : QWidget()
    , _peak_collection("temp", nsx::listtype::PREDICTED)
    , _peak_collection_item()
    , _peak_collection_model()
    , _peaks_predicted(false)
    , _shapes_assigned(false)
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    setParametersUp();
    setShapeCollectionUp();
    setPreviewUp();
    setSaveUp();
    setFigureUp();
    setPeakTableUp();

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _shape_collection = std::make_unique<nsx::ShapeCollection>();
    _shape_params = std::make_shared<nsx::ShapeCollectionParameters>();
}

void SubframePredictPeaks::setParametersUp()
{
    _para_box = new Spoiler("Predict peaks");
    GridFiller f(_para_box, true);

    _exp_combo = f.addLinkedCombo(ComboType::Experiment, "Experiment");
    _cell_combo = f.addLinkedCombo(ComboType::UnitCell, "Unit cell:");
    _found_peaks_combo =
        f.addLinkedCombo(ComboType::PeakCollection, "Found peak collection",
                         "Found peaks from which to construct shape collection");
    _d_min = f.addDoubleSpinBox("d min:", QString::fromUtf8("(\u212B) - minimum d (Bragg's law)"));
    _d_max = f.addDoubleSpinBox("d max:", QString::fromUtf8("(\u212B) - maximum d (Bragg's law)"));
    _direct_beam = f.addCheckBox(
        "Show direct beam", "Show position of direct beam computed from instrument states", 1);
    _refine_ki_button = f.addButton(
        "Refine incident wavevector", "Refine the position of the  direct beam");
    _predict_button = f.addButton("Predict");
    _predict_button->setEnabled(false);

    _d_min->setMaximum(100);
    _d_min->setDecimals(2);

    _d_max->setMaximum(100);
    _d_max->setDecimals(2);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::updateUnitCellList);
    connect(_predict_button, &QPushButton::clicked, this, &SubframePredictPeaks::runPrediction);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframePredictPeaks::setPredictorParameters);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframePredictPeaks::setShapeCollectionParameters);
    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframePredictPeaks::refreshPeakCombo);
    connect(
        _direct_beam, &QCheckBox::stateChanged, this, &SubframePredictPeaks::showDirectBeamEvents);
    connect(_refine_ki_button, &QPushButton::clicked, this, &SubframePredictPeaks::refineKi);

    _left_layout->addWidget(_para_box);
}

void SubframePredictPeaks::setShapeCollectionUp()
{
    _shapes_box = new Spoiler("Generate shapes");
    GridFiller f(_shapes_box, true);

    _nx = f.addSpinBox("histogram bins x", "Number of bins in x direction");
    _ny = f.addSpinBox("histogram bins y", "Number of bins in x direction");
    _nz = f.addSpinBox("histogram bins f", "Number of bins in frames direction");
    _kabsch = f.addCheckBox(
        "Kabsch coordinates", "Use Kabsch coordinate to mitigate effects of detector geometry", 1);
    _sigma_m = f.addDoubleSpinBox(
        QString("Mosaicity ") + QChar(0x03C3), "Variance arising from crystal mosaicity");
    _sigma_d = f.addDoubleSpinBox(
        QString("Beam Divergence ") + QString(QChar(0x03C3)),
        "Variance arising from beam divergence");
    _min_strength = f.addDoubleSpinBox("Minimum I/" + QString(QChar(0x03C3)),
            "Minimum strength for peak to be included in shape collection");
    _min_d =
        f.addDoubleSpinBox("Minimum d", "Minimum d for peak to be included in shape collection");
    _max_d =
        f.addDoubleSpinBox("Maximum d", "Minimum d for peak to be included in shape collection");
    _peak_end = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");
    _bkg_begin =
        f.addDoubleSpinBox("Bkg begin:", "(sigmas) - scaling factor for lower limit of background");
    _bkg_end =
        f.addDoubleSpinBox("Bkg end:", "(sigmas) - scaling factor for upper limit of background");
    _radius_pix =
        f.addDoubleSpinBox("Search radius:", "(pixels) - neighbour search radius in pixels");
    _radius_frames =
        f.addDoubleSpinBox("N. of frames:", "(frames) - neighbour search radius in frames");
    _min_neighbours = f.addSpinBox(
        "Min. neighbours", "Minimum number of neighbouring shapes to predict peak shape");
    _interpolation_combo = f.addCombo("Interpolation", "Interpolation type for peak shape");
    _assign_peak_shapes = f.addButton(
        "Assign peak shapes", "Assign peak shapes from shape collection to a predicted collection");

    _nx->setMaximum(100);
    _ny->setMaximum(100);
    _nz->setMaximum(100);

    _sigma_m->setMaximum(10);
    _sigma_m->setDecimals(2);

    _sigma_d->setMaximum(10);
    _sigma_d->setDecimals(2);

    _min_d->setMaximum(100);
    _min_d->setDecimals(2);

    _max_d->setMaximum(100);
    _max_d->setDecimals(2);

    _peak_end->setMaximum(100);
    _peak_end->setDecimals(2);

    _bkg_begin->setMaximum(100);
    _bkg_begin->setDecimals(2);

    _bkg_end->setMaximum(100);
    _bkg_end->setDecimals(2);

    _radius_pix->setMaximum(1000);
    _radius_pix->setDecimals(2);

    _radius_frames->setMaximum(50);
    _radius_frames->setDecimals(2);

    _interpolation_combo->addItem("None");
    _interpolation_combo->addItem("Inverse distance");
    _interpolation_combo->addItem("Intensity");

    connect(
        _assign_peak_shapes, &QPushButton::clicked, this, &SubframePredictPeaks::assignPeakShapes);
    connect(_kabsch, &QCheckBox::clicked, this, &SubframePredictPeaks::toggleUnsafeWidgets);

    _left_layout->addWidget(_shapes_box);
    grabShapeCollectionParameters();
}

void SubframePredictPeaks::setPreviewUp()
{
    _preview_box = new Spoiler("Show/hide peaks");
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);

    _preview_box->setContentLayout(*_peak_view_widget);
    _left_layout->addWidget(_preview_box);
}

void SubframePredictPeaks::setSaveUp()
{
    _save_button = new QPushButton("Create peak collection");
    _save_button->setEnabled(false);
    _left_layout->addWidget(_save_button);
    connect(_save_button, &QPushButton::clicked, this, &SubframePredictPeaks::accept);
}

void SubframePredictPeaks::setFigureUp()
{
    QGroupBox* figure_group = new QGroupBox("Preview");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(false, false, true, figure_group);
    _detector_widget->linkPeakModel(&_peak_collection_model);

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframePredictPeaks::changeSelected);

    _right_element->addWidget(figure_group);
}

void SubframePredictPeaks::setPeakTableUp()
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

void SubframePredictPeaks::refreshAll()
{
    setExperiments();
    toggleUnsafeWidgets();
}

void SubframePredictPeaks::setExperiments()
{
    _exp_combo->blockSignals(true);
    QString current_exp = _exp_combo->currentText();
    _exp_combo->clear();
    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);
    _exp_combo->setCurrentText(current_exp);

    if (!(_exp_combo->count() == 0)) {
        updateUnitCellList();
        updateDatasetList();
        refreshPeakCombo();
        grabPredictorParameters();
        grabShapeCollectionParameters();
        refreshPeakTable();
        computeSigmas();
    }

    _exp_combo->blockSignals(false);
}

void SubframePredictPeaks::updateUnitCellList()
{
    _cell_combo->blockSignals(true);
    QString current_cell = _cell_combo->currentText();
    _cell_combo->clear();

    _unit_cell_list = gSession->experimentAt(_exp_combo->currentIndex())->getUnitCellNames();

    if (!_unit_cell_list.empty()) {
        _cell_combo->addItems(_unit_cell_list);
        _cell_combo->setCurrentText(current_cell);
    }
    _cell_combo->blockSignals(false);
}

void SubframePredictPeaks::updateDatasetList()
{
    _data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();
    if (!_data_list.empty())
        _detector_widget->updateDatasetList(_data_list);
}

void SubframePredictPeaks::grabPredictorParameters()
{
    auto params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->predictor()->parameters();

    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
}

void SubframePredictPeaks::setPredictorParameters()
{
    if (_exp_combo->count() == 0 || _cell_combo->count() == 0)
        return;

    auto params =
        gSession->experimentAt(_exp_combo->currentIndex())->experiment()->predictor()->parameters();

    params->d_min = _d_min->value();
    params->d_max = _d_max->value();
}

void SubframePredictPeaks::computeSigmas()
{
    if (!(_peak_collection.numberOfPeaks() == 0)) {
        _peak_collection.computeSigmas();
        _sigma_m->setValue(_peak_collection.sigmaM());
        _sigma_d->setValue(_peak_collection.sigmaD());
    }
}

void SubframePredictPeaks::grabShapeCollectionParameters()
{
    if (_exp_combo->count() == 0)
        return;

    _nx->setValue(_shape_params->nbins_x);
    _ny->setValue(_shape_params->nbins_y);
    _nz->setValue(_shape_params->nbins_z);
    _kabsch->setChecked(_shape_params->kabsch_coords);
    _min_strength->setValue(_shape_params->strength_min);
    _min_d->setValue(_shape_params->d_min);
    _max_d->setValue(_shape_params->d_max);
    _peak_end->setValue(_shape_params->peak_end);
    _bkg_begin->setValue(_shape_params->bkg_begin);
    _bkg_end->setValue(_shape_params->bkg_end);
    _radius_pix->setValue(_shape_params->neighbour_range_pixels);
    _radius_frames->setValue(_shape_params->neighbour_range_frames);
    _min_neighbours->setValue(_shape_params->min_neighbors);
    _interpolation_combo->setCurrentIndex(static_cast<int>(_shape_params->interpolation));
}

void SubframePredictPeaks::setShapeCollectionParameters()
{
    if (_exp_combo->count() == 0)
        return;

    if (!(_peak_collection.numberOfPeaks() == 0)) {
        _peak_collection.computeSigmas();
        _sigma_m->setValue(_peak_collection.sigmaM());
        _sigma_d->setValue(_peak_collection.sigmaD());
    }
    _shape_params->nbins_x = _nx->value();
    _shape_params->nbins_y = _ny->value();
    _shape_params->nbins_z = _nz->value();
    _shape_params->kabsch_coords = _kabsch->isChecked();
    _shape_params->strength_min = _min_strength->value();
    _shape_params->d_min = _min_d->value();
    _shape_params->d_max = _max_d->value();
    _shape_params->peak_end = _peak_end->value();
    _shape_params->bkg_begin = _bkg_begin->value();
    _shape_params->bkg_end = _bkg_end->value();
    _shape_params->neighbour_range_pixels = _radius_pix->value();
    _shape_params->neighbour_range_frames = _radius_frames->value();
    _shape_params->min_neighbors = _min_neighbours->value();
    _shape_params->interpolation =
        static_cast<nsx::PeakInterpolation>(_interpolation_combo->currentIndex());
}

void SubframePredictPeaks::refreshPeakCombo()
{
    _found_peaks_combo->blockSignals(true);
    QString current_peaks = _found_peaks_combo->currentText();
    _found_peaks_combo->clear();
    _found_peaks_combo->addItems(
        gSession->experimentAt(_exp_combo->currentIndex())->
        getPeakCollectionNames(nsx::listtype::FOUND));
    _found_peaks_combo->setCurrentText(current_peaks);
    _found_peaks_combo->blockSignals(false);
}

void SubframePredictPeaks::refineKi()
{
    gGui->setReady(false);
    auto expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    auto* peaks = expt->getPeakCollection(_found_peaks_combo->currentText().toStdString());
    const auto data = _detector_widget->currentData();
    auto* detector = data->diffractometer()->detector();
    auto states = data->instrumentStates();
    auto refiner = expt->refiner();
    auto* params = refiner->parameters();
    auto* cell = expt->getUnitCell(_cell_combo->currentText().toStdString());

    nsx::RefinerParameters tmp_params = *params;

    std::vector<nsx::DetectorEvent> old_beam =
        nsx::algo::getDirectBeamEvents(states, *detector);
    _detector_widget->scene()->linkOldDirectBeamPositions(old_beam);
    refreshPeakVisual();

    params->refine_ki = true;
    params->refine_ub = false;
    params->refine_detector_offset = false;
    params->refine_sample_position = false;
    params->refine_sample_orientation = false;
    params->nbatches = data->nFrames();
    params->residual_type = nsx::ResidualType::RealSpace;

    bool success = expt->refine(peaks, data.get(), cell);
    if (success) {
        gGui->statusBar()->showMessage("Direct beam positions refined");
        showDirectBeamEvents();
    } else
        gGui->statusBar()->showMessage("Direct beam position refinement failed");

    refiner->setParameters(tmp_params);
    gGui->setReady(true);
}

void SubframePredictPeaks::runPrediction()
{
    gGui->setReady(false);
    try {
        auto* experiment = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        auto data = experiment->getData(_detector_widget->dataCombo()->currentText().toStdString());
        auto* predictor = experiment->predictor();
        setPredictorParameters();

        nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        nsx::UnitCell* cell = gSession->currentProject()->experiment()->getUnitCell(
            _cell_combo->currentText().toStdString());

        predictor->predictPeaks(data, cell, handler);

        std::vector<nsx::Peak3D*> predicted_peaks;
        for (nsx::Peak3D* peak : predictor->peaks())
            predicted_peaks.push_back(peak);

        _peak_collection.populate(predicted_peaks);
        for (nsx::Peak3D* peak : predicted_peaks)
            delete peak;
        predicted_peaks.clear();

        _peak_collection_item.setPeakCollection(&_peak_collection);
        _peak_collection_model.setRoot(&_peak_collection_item);
        refreshPeakTable();
        _peaks_predicted = true;
        toggleUnsafeWidgets();

        gGui->statusBar()->showMessage(
            QString::number(predictor->numberOfPredictedPeaks()) + " peaks predicted");
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString(e.what()));
    }

    gGui->setReady(true);
}

void SubframePredictPeaks::showDirectBeamEvents()
{
    if (_direct_beam->isChecked()){
        _detector_widget->scene()->showDirectBeam(true);

        auto* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        auto data_name = _detector_widget->dataCombo()->currentText();
        const auto data = expt->getData(data_name.toStdString());

        std::vector<nsx::DetectorEvent> direct_beam_events;
        const auto& states = data->instrumentStates();
        auto* detector = data->diffractometer()->detector();
        std::vector<nsx::DetectorEvent> events = nsx::algo::getDirectBeamEvents(states, *detector);

        for (auto&& event : events)
            direct_beam_events.push_back(event);

        _detector_widget->scene()->linkDirectBeamPositions(direct_beam_events);
    } else {
        _detector_widget->scene()->showDirectBeam(false);
    }
    refreshPeakVisual();
}

void SubframePredictPeaks::assignPeakShapes()
{
    gGui->setReady(false);
    auto* experiment = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    auto data = experiment->getData(_detector_widget->dataCombo()->currentText().toStdString());
    auto* found_peaks = experiment->getPeakCollection(_found_peaks_combo->currentText().toStdString());

    setShapeCollectionParameters();

    std::set<nsx::sptrDataSet> datalist;
    std::vector<nsx::Peak3D*> fit_peaks;
    for (nsx::Peak3D* peak : found_peaks->getPeakList()) {
        datalist.insert(peak->dataSet());
        if (!peak->enabled())
            continue;
        const double d = 1.0 / peak->q().rowVector().norm();

        if (d > _shape_params->d_max || d < _shape_params->d_min)
            continue;

        const nsx::Intensity intensity = peak->correctedIntensity();

        if (intensity.value() <= _shape_params->strength_min * intensity.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    nsx::sptrProgressHandler handler(new nsx::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);
    experiment->integrator()->setHandler(handler);

    _shape_collection->setParameters(_shape_params);
    _shape_collection->integrate(fit_peaks, datalist, handler);

    found_peaks->setShapeCollection(_shape_collection);

    nsx::ShapeCollection* shapes = found_peaks->shapeCollection();
    shapes->setPredictedShapes(&_peak_collection, _shape_params->interpolation, handler);

    refreshPeakTable();
    _shapes_assigned = true;
    toggleUnsafeWidgets();
    gGui->statusBar()->showMessage(
        QString::number(_peak_collection.numberOfValid()) + "/" +
        QString::number(_peak_collection.numberOfPeaks()) + " predicted peaks with valid shapes");
    gGui->setReady(true);
}

void SubframePredictPeaks::accept()
{
    std::unique_ptr<ListNameDialog> dlg(new ListNameDialog());
    dlg->exec();
    if (!dlg->listName().isEmpty()) {
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->addPeakCollection(
                dlg->listName().toStdString(), nsx::listtype::PREDICTED,
                _peak_collection.getPeakList());
        gSession->experimentAt(_exp_combo->currentIndex())->generatePeakModel(dlg->listName());
        gGui->sentinel->addLinkedComboItem(ComboType::PredictedPeaks, dlg->listName());
        gGui->sentinel->addLinkedComboItem(ComboType::PeakCollection, dlg->listName());
    }
}

void SubframePredictPeaks::refreshPeakTable()
{
    if (_exp_combo->count() == 0)
        return;

    computeSigmas();
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();
    showDirectBeamEvents();
    refreshPeakVisual();
}

void SubframePredictPeaks::refreshPeakVisual()
{
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    _detector_widget->refresh();
    if (_peak_collection_item.childCount() == 0)
        return;

    for (int i = 0; i < _peak_collection_item.childCount(); ++i) {
        PeakItem* peak = _peak_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget->set1 : _peak_view_widget->set2);
    }
}

void SubframePredictPeaks::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframePredictPeaks::toggleUnsafeWidgets()
{
    _predict_button->setEnabled(true);
    _save_button->setEnabled(true);
    _assign_peak_shapes->setEnabled(true);
    if (_cell_combo->count() == 0 || _exp_combo->count() == 0) {
        _predict_button->setEnabled(false);
        _save_button->setEnabled(false);
        _assign_peak_shapes->setEnabled(false);
    }

    if (!_peaks_predicted) {
        _assign_peak_shapes->setEnabled(false);
        _save_button->setEnabled(false);
    }

    // if (!_shapes_assigned) // TODO: reenable later
    //     _save_button->setEnabled(false);

    _sigma_d->setEnabled(true);
    _sigma_m->setEnabled(true);
    if (!_kabsch->isChecked()) {
        _sigma_d->setEnabled(false);
        _sigma_m->setEnabled(false);
    }
}

DetectorWidget* SubframePredictPeaks::detectorWidget()
{
    return _detector_widget;
}
