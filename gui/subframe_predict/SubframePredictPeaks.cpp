//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_predict/SubframePredictPeaks.cpp
//! @brief     Implements classes FoundPeaks, SubframePredictPeaks
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_predict/SubframePredictPeaks.h"

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/detector/DetectorEvent.h"
#include "core/experiment/Experiment.h"
#include "core/instrument/InstrumentState.h"
#include "core/integration/IIntegrator.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/raw/DataKeys.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/Predictor.h"
#include "core/shape/ShapeModel.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/dialogs/ListNameDialog.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/items/PeakItem.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_refiner/SubframeRefiner.h"
#include "gui/utility/CellComboBox.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/ShapeComboBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"
#include "gui/views/PeakTableView.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/DirectBeamWidget.h"
#include "gui/widgets/PeakViewWidget.h"
#include "tables/crystal/UnitCell.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QMessageBox>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTableWidgetItem>

SubframePredictPeaks::SubframePredictPeaks()
    : QWidget()
    , _peak_collection("temp", ohkl::PeakCollectionType::PREDICTED, nullptr)
    , _peak_collection_item()
    , _peak_collection_model()
    , _peaks_predicted(false)
    , _shapes_assigned(false)
{
    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    _detector_widget = new DetectorWidget(1, false, true, figure_group);
    _beam_setter_widget = new DirectBeamWidget(_detector_widget->scene());
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid peaks");

    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);
    _left_layout = new QVBoxLayout();

    _right_element->addWidget(figure_group);

    setAdjustBeamUp();
    setRefineKiUp();
    setParametersUp();
    setShapeModelUp();
    setPreviewUp();
    setSaveUp();
    setPeakTableUp();
    toggleUnsafeWidgets();

    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);
    _detector_widget->scene()->linkDirectBeam(&_direct_beam_events, &_old_direct_beam_events);

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);
    _set_initial_ki->setChecked(false);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);

    _shape_params = std::make_shared<ohkl::ShapeModelParameters>();

    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframePredictPeaks::changeSelected);
}

void SubframePredictPeaks::setAdjustBeamUp()
{
    _set_initial_ki = new Spoiler("Set initial direct beam position");
    _set_initial_ki->setContentLayout(*_beam_setter_widget, true);

    connect(
        _beam_setter_widget->crosshairOn(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);
    connect(
        _beam_setter_widget->crosshairOn(), &QCheckBox::stateChanged, this,
        &SubframePredictPeaks::toggleCursorMode);

    _left_layout->addWidget(_set_initial_ki);
}

void SubframePredictPeaks::setRefineKiUp()
{
    Spoiler* ki_box = new Spoiler("Refine direct beam position");
    GridFiller f(ki_box, true);

    _peak_combo = f.addPeakCombo(
        ComboType::FoundPeaks, "Found peaks", "Peak collection to be used in refinement");
    _n_batches_spin =
        f.addSpinBox("Number of batches", "Number of batches for refining incident wavevector");
    _max_iter_spin = f.addSpinBox(
        "Maximum iterations", "Maximum number of iterations for least squares minimisation");
    _residual_combo = f.addCombo("Residual type", "Residual type for refinement");
    _direct_beam = f.addCheckBox(
        "Show direct beam", "Show position of direct beam computed from instrument states", 1);
    _refine_ki_button =
        f.addButton("Refine incident wavevector", "Refine the position of the  direct beam");

    for (const auto& [key, val] : _residual_strings)
        _residual_combo->addItem(QString::fromStdString(key));

    _direct_beam->setChecked(true);
    _n_batches_spin->setValue(10);
    _max_iter_spin->setMaximum(1000000);
    _max_iter_spin->setValue(1000);

    connect(
        _direct_beam, &QCheckBox::stateChanged, this, &SubframePredictPeaks::showDirectBeamEvents);
    connect(_refine_ki_button, &QPushButton::clicked, this, &SubframePredictPeaks::refineKi);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframePredictPeaks::setRefinerParameters);

    _left_layout->addWidget(ki_box);
}

void SubframePredictPeaks::toggleCursorMode()
{
    if (_beam_setter_widget->crosshairOn()->isChecked()) {
        _stored_cursor_mode = _detector_widget->scene()->mode();
        _detector_widget->scene()->changeInteractionMode(7);
    } else {
        _detector_widget->scene()->changeInteractionMode(_stored_cursor_mode);
    }
}

void SubframePredictPeaks::setParametersUp()
{
    Spoiler* para_box = new Spoiler("Predict peaks");
    GridFiller f(para_box, true);

    _cell_combo = f.addCellCombo("Unit cell:");
    _d_min = f.addDoubleSpinBox(
        "Maximum resolution (min. d):", QString::fromUtf8("(\u212B) - minimum d (Bragg's law)"));
    _d_max = f.addDoubleSpinBox(
        "Minimum resolution (max. d):", QString::fromUtf8("(\u212B) - maximum d (Bragg's law)"));
    _predict_button = f.addButton("Predict");
    _predict_button->setEnabled(false);

    _d_min->setMaximum(100);
    _d_min->setDecimals(2);

    _d_max->setMaximum(100);
    _d_max->setDecimals(2);

    connect(_predict_button, &QPushButton::clicked, this, &SubframePredictPeaks::runPrediction);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframePredictPeaks::setPredictorParameters);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframePredictPeaks::setShapeModelParameters);

    _left_layout->addWidget(para_box);
}

void SubframePredictPeaks::setShapeModelUp()
{
    Spoiler* shapes_box = new Spoiler("Shape model");
    GridFiller f(shapes_box, true);

    _shape_combo =
        f.addShapeCombo("Shape model", "Shape model to predict shapes of predicted peaks");
    _peak_end = f.addDoubleSpinBox("Peak end", "(sigmas) - scaling factor for peak region");
    _bkg_begin = f.addDoubleSpinBox(
        "Background begin:", "(sigmas) - scaling factor for lower limit of background");
    _bkg_end = f.addDoubleSpinBox(
        "Background end:", "(sigmas) - scaling factor for upper limit of background");
    _radius_pix = f.addDoubleSpinBox(
        "Search radius (pixels):",
        "(pixels) - neighbour search radius in pixels for generating mean covariance");
    _radius_frames = f.addDoubleSpinBox(
        "Search radius (images):",
        "(frames) - neighbour search radius in frames for generating mean covariance");
    _min_neighbours = f.addSpinBox(
        "Min. neighbours", "Minimum number of neighbouring shapes to generate mean covariance");
    _interpolation_combo =
        f.addCombo("Interpolation type", "Interpolation strategy for determining mean covariance");
    _apply_shape_model =
        f.addButton("Apply shape model", "Apply shape model to a predicted peak collection");


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
        _apply_shape_model, &QPushButton::clicked, this, &SubframePredictPeaks::applyShapeModel);

    _left_layout->addWidget(shapes_box);
    grabShapeModelParameters();
}

void SubframePredictPeaks::setPreviewUp()
{
    Spoiler* preview_box = new Spoiler("Show/hide peaks");

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframePredictPeaks::refreshPeakVisual);

    preview_box->setContentLayout(*_peak_view_widget);
    _left_layout->addWidget(preview_box);
}

void SubframePredictPeaks::setSaveUp()
{
    _save_button = new QPushButton("Create peak collection");
    _save_button->setEnabled(false);
    _left_layout->addWidget(_save_button);
    connect(_save_button, &QPushButton::clicked, this, &SubframePredictPeaks::accept);
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

void SubframePredictPeaks::refreshAll()
{
    toggleUnsafeWidgets();
    if (!gSession->hasProject())
        return;

    _cell_combo->refresh();
    _peak_combo->refresh();
    grabRefinerParameters();
    grabPredictorParameters();
    grabShapeModelParameters();
    if (!gSession->currentProject()->hasDataSet())
        return;
    refreshPeakTable();
    const auto data = _detector_widget->currentData();
    if (data) {
        _n_batches_spin->setMaximum(data->nFrames());
        _beam_setter_widget->setSpinLimits(data->nCols(), data->nRows());
    }
}

void SubframePredictPeaks::grabPredictorParameters()
{
    auto params = gSession->currentProject()->experiment()->predictor()->parameters();

    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
}

void SubframePredictPeaks::setPredictorParameters()
{
    if (!gSession->hasProject())
        return;
    if (!gSession->currentProject()->hasUnitCell())
        return;

    auto params = gSession->currentProject()->experiment()->predictor()->parameters();

    params->d_min = _d_min->value();
    params->d_max = _d_max->value();
}

void SubframePredictPeaks::grabRefinerParameters()
{
    auto* params = gSession->currentProject()->experiment()->refiner()->parameters();

    _n_batches_spin->setValue(params->nbatches);
    _max_iter_spin->setValue(params->max_iter);
    for (const auto& [key, val] : _residual_strings) {
        if (val == params->residual_type) {
            _residual_combo->setCurrentText(QString::fromStdString(key));
            break;
        }
    }
}

void SubframePredictPeaks::setRefinerParameters()
{
    if (!gSession->hasProject())
        return;
    auto* params = gSession->currentProject()->experiment()->refiner()->parameters();

    params->nbatches = _n_batches_spin->value();
    params->max_iter = _max_iter_spin->value();
    for (const auto& [key, val] : _residual_strings) {
        if (key == _residual_combo->currentText().toStdString())
            params->residual_type = val;
    }
}

void SubframePredictPeaks::grabShapeModelParameters()
{
    if (!gSession->hasProject())
        return;

    _peak_end->setValue(_shape_params->peak_end);
    _bkg_begin->setValue(_shape_params->bkg_begin);
    _bkg_end->setValue(_shape_params->bkg_end);
    _radius_pix->setValue(_shape_params->neighbour_range_pixels);
    _radius_frames->setValue(_shape_params->neighbour_range_frames);
    _min_neighbours->setValue(_shape_params->min_neighbors);
    _interpolation_combo->setCurrentIndex(static_cast<int>(_shape_params->interpolation));
}

void SubframePredictPeaks::setShapeModelParameters()
{
    if (!gSession->hasProject())
        return;

    _shape_params->peak_end = _peak_end->value();
    _shape_params->bkg_begin = _bkg_begin->value();
    _shape_params->bkg_end = _bkg_end->value();
    _shape_params->neighbour_range_pixels = _radius_pix->value();
    _shape_params->neighbour_range_frames = _radius_frames->value();
    _shape_params->min_neighbors = _min_neighbours->value();
    _shape_params->interpolation =
        static_cast<ohkl::PeakInterpolation>(_interpolation_combo->currentIndex());
}

void SubframePredictPeaks::refineKi()
{
    gGui->setReady(false);
    auto expt = gSession->currentProject()->experiment();
    auto* peaks = _peak_combo->currentPeakCollection();
    auto data = _detector_widget->currentData();
    auto* detector = data->diffractometer()->detector();
    auto& states = data->instrumentStates();
    auto refiner = expt->refiner();
    auto* params = refiner->parameters();
    auto cell = _cell_combo->currentCell();

    ohkl::RefinerParameters tmp_params = *params;
    setRefinerParameters();

    // Manally adjust the direct beam position
    if (_beam_setter_widget->crosshairOn()->isChecked()) {
        auto data = _detector_widget->currentData();
        emit gGui->sentinel->instrumentStatesChanged();
        data->adjustDirectBeam(_beam_setter_widget->xOffset(), _beam_setter_widget->yOffset());
    }

    _old_direct_beam_events = ohkl::algo::getDirectBeamEvents(states, *detector);
    refreshPeakVisual();

    params->refine_ki = true;
    params->refine_ub = false;
    params->refine_detector_offset = false;
    params->refine_sample_position = false;
    params->refine_sample_orientation = false;

    refiner->makeBatches(states, peaks->getPeakList(), cell);
    bool success = refiner->refine();
    if (success) {
        gGui->statusBar()->showMessage("Direct beam positions refined");
        showDirectBeamEvents();
    } else
        gGui->statusBar()->showMessage("Direct beam position refinement failed");

    refiner->setParameters(tmp_params);
    for (auto* peak : peaks->getPeakList()) // Assign original unit cell to all peaks
        peak->setUnitCell(cell);

    _old_direct_beam_events = _direct_beam_events;
    emit gGui->sentinel->instrumentStatesChanged();

    gGui->setReady(true);
}

void SubframePredictPeaks::runPrediction()
{
    gGui->setReady(false);
    // Manally adjust the direct beam position
    auto data = _detector_widget->currentData();
    if (_beam_setter_widget->crosshairOn()->isChecked()) {
        data->adjustDirectBeam(_beam_setter_widget->xOffset(), _beam_setter_widget->yOffset());
        emit gGui->sentinel->instrumentStatesChanged();
    }
    try {
        auto* experiment = gSession->currentProject()->experiment();
        auto* predictor = experiment->predictor();
        setPredictorParameters();

        ohkl::sptrProgressHandler handler(new ohkl::ProgressHandler);
        ProgressView progressView(nullptr);
        progressView.watch(handler);

        ohkl::sptrUnitCell cell = _cell_combo->currentCell();

        predictor->setHandler(handler);
        predictor->predictPeaks(data, cell);

        std::vector<ohkl::Peak3D*> predicted_peaks;
        for (ohkl::Peak3D* peak : predictor->peaks())
            predicted_peaks.push_back(peak);

        _peak_collection.populate(predicted_peaks);
        _peak_collection.setData(data);
        for (ohkl::Peak3D* peak : predicted_peaks)
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
    if (_direct_beam->isChecked()) {
        _detector_widget->scene()->params()->directBeam = true;

        const auto data = _detector_widget->currentData();

        _direct_beam_events.clear();
        const auto& states = data->instrumentStates();
        auto* detector = data->diffractometer()->detector();
        std::vector<ohkl::DetectorEvent> events =
            ohkl::algo::getDirectBeamEvents(states, *detector);

        for (auto&& event : events)
            _direct_beam_events.push_back(event);

    } else {
        _detector_widget->scene()->params()->directBeam = false;
    }
    refreshPeakVisual();
}

void SubframePredictPeaks::applyShapeModel()
{

    gGui->setReady(false);
    ohkl::ShapeModel* shapes = _shape_combo->currentShapes();

    setShapeModelParameters();

    ohkl::sptrProgressHandler handler(new ohkl::ProgressHandler);
    ProgressView progressView(nullptr);
    progressView.watch(handler);

    shapes->setParameters(_shape_params);
    shapes->setHandler(handler);
    shapes->setPredictedShapes(&_peak_collection);

    refreshPeakTable();
    _shapes_assigned = true;
    toggleUnsafeWidgets();
    gGui->statusBar()->showMessage(
        QString::number(_peak_collection.numberOfValid()) + "/"
        + QString::number(_peak_collection.numberOfPeaks()) + " predicted peaks with valid shapes");
    gGui->setReady(true);
}

void SubframePredictPeaks::accept()
{
    // suggest name to user
    auto* project = gSession->currentProject();
    auto* expt = project->experiment();
    auto data = _detector_widget->currentData();
    auto cell = _cell_combo->currentCell();
    std::string suggestion = expt->generatePeakCollectionName();
    std::unique_ptr<ListNameDialog> dlg(new ListNameDialog(QString::fromStdString(suggestion)));
    dlg->exec();
    if (dlg->listName().isEmpty())
        return;
    if (dlg->result() == QDialog::Rejected)
        return;

    if (!expt->addPeakCollection(
            dlg->listName().toStdString(), ohkl::PeakCollectionType::PREDICTED,
            _peak_collection.getPeakList(), data, cell)) {
        QMessageBox::warning(
            this, "Unable to add PeakCollection",
            "Unable to add PeakCollection, please use a unique name");
        return;
    }

    gSession->onPeaksChanged();
    _peak_combo->refresh();
    auto* collection = expt->getPeakCollection(dlg->listName().toStdString());
    collection->setIndexed(true);
    project->generatePeakModel(dlg->listName());
}

void SubframePredictPeaks::refreshPeakTable()
{
    if (!gSession->hasProject())
        return;

    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();
    showDirectBeamEvents();
    refreshPeakVisual();
}

void SubframePredictPeaks::refreshPeakVisual()
{
    _detector_widget->refresh();
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
    _predict_button->setEnabled(false);
    _save_button->setEnabled(false);
    _apply_shape_model->setEnabled(false);
    _refine_ki_button->setEnabled(false);
    _direct_beam->setEnabled(false);

    if (!gSession->hasProject())
        return;

    if (gSession->currentProject()->hasPeakCollection()) {
        if (_peak_combo->currentPeakCollection()->isIndexed())
            _refine_ki_button->setEnabled(true);
    }

    _predict_button->setEnabled(gSession->currentProject()->hasUnitCell());
    _apply_shape_model->setEnabled(_peaks_predicted);
    _save_button->setEnabled(_peaks_predicted);
}

DetectorWidget* SubframePredictPeaks::detectorWidget()
{
    return _detector_widget;
}
