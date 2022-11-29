//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_filter/SubframeRefiner.cpp
//! @brief     Implements class SubframeRefiner
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_refiner/SubframeRefiner.h"

#include "core/experiment/Experiment.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/PeakFilter.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/frames/ProgressView.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/SXPlot.h"
#include "gui/models/Meta.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_refiner/RefinerTables.h"
#include "gui/subwindows/DetectorWindow.h"
#include "gui/utility/CellComboBox.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PeakComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PeakViewWidget.h"
#include "gui/widgets/PlotCheckBox.h"

#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLabel>
#include <QScrollBar>
#include <QSpacerItem>

SubframeRefiner::SubframeRefiner()
    : _refine_success(false)
    , _refined_collection_item()
    , _refined_model()
    , _unrefined_collection_item()
    , _unrefined_model()
{
    auto main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    _left_layout = new QVBoxLayout();

    // Tabs to switch betwen tables and detector
    _tab_widget = new QTabWidget(this);
    QWidget* tables_tab = new QWidget(_tab_widget);
    QWidget* detector_tab = new QWidget(_tab_widget);
    _tab_widget->addTab(tables_tab, "Tables of refined parameters");
    _tab_widget->addTab(detector_tab, "Detector image");

    QHBoxLayout* table_layout = new QHBoxLayout();

    _tables_widget = new RefinerTables();
    _tables_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    table_layout->addWidget(_tables_widget);
    tables_tab->setLayout(table_layout);

    _peak_view_widget_1 = new PeakViewWidget("Valid peaks", "Invalid Peaks");
    _peak_view_widget_2 = new PeakViewWidget("Valid peaks", "Invalid Peaks");
    _peak_view_widget_2->set1.setColor(Qt::darkGreen);
    _peak_view_widget_2->set2.setColor(Qt::darkRed);
    _peak_view_widget_2->set1.setIntegrationRegionColors(Qt::darkGreen, Qt::darkYellow, 0.5);

    setInputUp();
    setRefinerFlagsUp();
    setUpdateUp();
    setPlotUp();
    setPeakViewWidgetUp(_peak_view_widget_1, "View refined_peaks");
    setPeakViewWidgetUp(_peak_view_widget_2, "View unrefined_peaks");
    refreshAll();

    _detector_widget = new DetectorWidget(false, true);
    _detector_widget->linkPeakModel(&_unrefined_model, &_refined_model);
    detector_tab->setLayout(_detector_widget);

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeRefiner::refreshPeakVisual);
    connect(
        _predicted_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, [=]() {
            updatePeaks();
            refreshPeakVisual();
        });
    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);

    _right_element->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _right_element->addWidget(_tab_widget);

    _plot_widget = new SXPlot;
    _plot_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _right_element->addWidget(_plot_widget);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    QRandomGenerator _rng(0);
}

void SubframeRefiner::setInputUp()
{
    auto input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _peak_combo = f.addPeakCombo(ComboType::FoundPeaks, "Peaks");
    _data_combo = f.addDataCombo("Data set");
    _cell_combo = f.addCellCombo("Unit cell");
    _batch_cell_check = f.addCheckBox(
        "Use refined cells", "Use unit cells generated per batch during previous refinement", 1);
    _n_batches_spin = f.addSpinBox(
        "Number of batches", "Number of batches of equal numbers of peaks for refinement");
    _max_iter_spin =
        f.addSpinBox("Maximum iterations", "Maximum number of iterations for NLLS minimsation");

    _batch_cell_check->setChecked(false);
    _n_batches_spin->setMinimum(1);
    _n_batches_spin->setMaximum(1000); // updated on setBatchesUp
    _max_iter_spin->setMinimum(100);
    _max_iter_spin->setMaximum(10000000);

    connect(
        _batch_cell_check, &QCheckBox::stateChanged, this, &SubframeRefiner::toggleUnsafeWidgets);

    _left_layout->addWidget(input_box);
}

void SubframeRefiner::setRefinerFlagsUp()
{
    auto refiner_flags_box = new Spoiler("Parameters to refine");
    GridFiller f(refiner_flags_box, true);

    _residual_combo =
        f.addCombo("Residual type", "Type of residual to use in least squares refinement");
    _refineUB = f.addCheckBox("Cell vectors");
    _refineSamplePosition = f.addCheckBox("Sample position");
    _refineSampleOrientation = f.addCheckBox("Sample orientation");
    _refineDetectorPosition = f.addCheckBox("Detector position");
    _refineKi = f.addCheckBox("Incident wavevector");
    _refine_button = f.addButton("Refine");

    for (const auto& [key, val] : _residual_strings)
        _residual_combo->addItem(QString::fromStdString(key));

    connect(_refine_button, &QPushButton::clicked, this, &SubframeRefiner::refine);
    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this, &SubframeRefiner::setRefinerParameters);

    _left_layout->addWidget(refiner_flags_box);
}

void SubframeRefiner::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _data_combo->refresh();
    _predicted_combo->refresh();
    _peak_combo->refresh();
    _cell_combo->refresh();


    updatePeaks();
    grabRefinerParameters();
    refreshPeakVisual();
    toggleUnsafeWidgets();
}

void SubframeRefiner::refreshTables()
{
    const auto expt = gSession->currentProject()->experiment();
    const auto data = _data_combo->currentData();
    const auto refiner = expt->refiner();
    _tables_widget->refreshTables(refiner, data.get());
}

void SubframeRefiner::setBatchesUp()
{
    const auto dataset = _data_combo->currentData();
    if (dataset)
        _n_batches_spin->setMaximum(dataset->nFrames());
}

void SubframeRefiner::refine()
{
    gGui->setReady(false);
    try {
        auto expt = gSession->currentProject()->experiment();
        auto* peaks = _peak_combo->currentPeakCollection();
        const auto data = _data_combo->currentData();
        auto cell = _cell_combo->currentCell();
        auto states = data->instrumentStates();
        auto* refiner = expt->refiner();
        auto* params = refiner->parameters();

        _detector_widget->scene()->showDirectBeam(true);
        auto* detector = data->diffractometer()->detector();
        _old_direct_beam_events = ohkl::algo::getDirectBeamEvents(states, *detector);
        _detector_widget->scene()->linkOldDirectBeamPositions(&_old_direct_beam_events);

        setRefinerParameters();

        int n_checked = 0;
        if (params->refine_ub)
            ++n_checked;
        if (params->refine_sample_position)
            ++n_checked;
        if (params->refine_sample_orientation)
            ++n_checked;
        if (params->refine_detector_offset)
            ++n_checked;
        if (params->refine_ki)
            ++n_checked;
        if (n_checked > 0) { // Check that we have selected at least one parameter set
            // TODO: Work out why calling the refiner directly doesn't work
            // refiner->makeBatches(states, peaks->getPeakList(), cell);
            // _refine_success = refiner->refine();
            if (_batch_cell_check->isChecked())
                _refine_success = expt->refine(peaks, data.get());
            else
                _refine_success = expt->refine(peaks, data.get(), cell);
        }

        states = data->instrumentStates();
        _direct_beam_events = ohkl::algo::getDirectBeamEvents(states, *detector);
        _detector_widget->scene()->linkDirectBeamPositions(&_direct_beam_events);
        refreshPeakVisual();
        gSession->onUnitCellChanged();
        _cell_combo->refresh();
        gGui->detector_window->refreshAll();

        _tables_widget->refreshTables(refiner, data.get());
        refreshPlot();
        toggleUnsafeWidgets();
    } catch (const std::exception& ex) {
        gGui->statusBar()->showMessage("Error: " + QString(ex.what()));
        // QMessageBox::critical(this, "Error", QString(ex.what()));
    }
    if (_refine_success)
        gGui->statusBar()->showMessage("Refinement success");
    else
        gGui->statusBar()->showMessage("Refinement failed");
    emit gGui->sentinel->instrumentStatesChanged();
    _tab_widget->setCurrentIndex(0);
    gGui->setReady(true);
}

void SubframeRefiner::setPlotUp()
{
    // Shortener for creating PlotCheckBox
    const auto cb = [](const QString& text, TableType table, int column) {
        return new PlotCheckBox(text, table, column);
    };

    _plot_box = new Spoiler("Plot changes in cell or instrument states");
    QVBoxLayout* plot_layout = new QVBoxLayout();

    // Create group & gridlayout for checkboxes. Add it already to the main layout
    // and return its grid layout
    const auto addGroup = [&](const QString& title) -> QGridLayout* {
        QGroupBox* group = new QGroupBox(title);
        QGridLayout* grid = new QGridLayout(group);
        group->setLayout(grid);
        plot_layout->addWidget(group);
        return grid;
    };

    // -- Lattice checks group
    auto lattice_grid = addGroup("Cell parameters");
    lattice_grid->addWidget(cb("a", TableType::Lattice, 1), 0, 0);
    lattice_grid->addWidget(cb("b", TableType::Lattice, 2), 0, 1);
    lattice_grid->addWidget(cb("c", TableType::Lattice, 3), 0, 2);
    lattice_grid->addWidget(cb(QString((QChar)0x03B1), TableType::Lattice, 4), 1, 0);
    lattice_grid->addWidget(cb(QString((QChar)0x03B2), TableType::Lattice, 5), 1, 1);
    lattice_grid->addWidget(cb(QString((QChar)0x03B3), TableType::Lattice, 6), 1, 2);

    // -- Sample position checks group
    auto sample_pos_grid = addGroup("Sample position");
    sample_pos_grid->addWidget(cb("x", TableType::SamplePos, 1), 0, 0);
    sample_pos_grid->addWidget(cb("y", TableType::SamplePos, 2), 0, 1);
    sample_pos_grid->addWidget(cb("z", TableType::SamplePos, 3), 0, 2);

    // -- Sample orientation checks group
    auto sample_orn_grid = addGroup("Sample orientation");
    sample_orn_grid->addWidget(cb("xx", TableType::SampleOrn, 1), 0, 0);
    sample_orn_grid->addWidget(cb("xy", TableType::SampleOrn, 2), 0, 1);
    sample_orn_grid->addWidget(cb("xz", TableType::SampleOrn, 3), 0, 2);
    sample_orn_grid->addWidget(cb("yx", TableType::SampleOrn, 4), 1, 0);
    sample_orn_grid->addWidget(cb("yy", TableType::SampleOrn, 5), 1, 1);
    sample_orn_grid->addWidget(cb("yz", TableType::SampleOrn, 6), 1, 2);
    sample_orn_grid->addWidget(cb("zx", TableType::SampleOrn, 7), 2, 0);
    sample_orn_grid->addWidget(cb("zy", TableType::SampleOrn, 8), 2, 1);
    sample_orn_grid->addWidget(cb("zz", TableType::SampleOrn, 9), 2, 2);

    // - Detector offset checks group
    auto detector_pos_grid = addGroup("Detector offset");
    detector_pos_grid->addWidget(cb("x", TableType::DetectorPos, 1), 0, 0);
    detector_pos_grid->addWidget(cb("y", TableType::DetectorPos, 2), 0, 1);
    detector_pos_grid->addWidget(cb("z", TableType::DetectorPos, 3), 0, 2);

    // -- Incident wavevector checks group
    auto ki_grid = addGroup("Incident wavevector");
    ki_grid->addWidget(cb("x", TableType::Ki, 1), 0, 0);
    ki_grid->addWidget(cb("y", TableType::Ki, 2), 0, 1);
    ki_grid->addWidget(cb("z", TableType::Ki, 3), 0, 2);

    _plot_box->setContentLayout(*plot_layout, true);

    _left_layout->addWidget(_plot_box);

    // refresh the plot whenever a PlotCheckBox state changes
    for (auto checkbox : plotCheckBoxes())
        connect(checkbox, &QCheckBox::stateChanged, this, &SubframeRefiner::refreshPlot);
}

void SubframeRefiner::refreshPlot()
{
    _plot_widget->clearGraphs();
    _plot_widget->xAxis->setLabel("frame");
    _plot_widget->yAxis->setLabel(QString(QChar(0x0394)));
    _plot_widget->setNotAntialiasedElements(QCP::aeAll);

    QPen pen;
    pen.setWidth(2.0);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _plot_widget->xAxis->setTickLabelFont(font);
    _plot_widget->yAxis->setTickLabelFont(font);
    _plot_widget->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    for (PlotCheckBox* check : plotCheckBoxes()) {
        if (check->isChecked()) {
            QVector<double> xvals = _tables_widget->getXVals();
            QVector<double> yvals = _tables_widget->getYVals(check->getTable(), check->getColumn());

            QColor color(_rng.bounded(256), _rng.bounded(256), _rng.bounded(256));
            pen.setColor(color);

            _plot_widget->addGraph();
            _plot_widget->graph()->setPen(pen);
            _plot_widget->graph()->addData(xvals, yvals);
            _plot_widget->graph()->setName(check->getLabel());
        }
    }
    _plot_widget->rescaleAxes();
    _plot_widget->replot();
}

void SubframeRefiner::setUpdateUp()
{
    auto update_box = new Spoiler("Update predicted peak centres");
    GridFiller f(update_box, true);

    _predicted_combo = f.addPeakCombo(ComboType::PredictedPeaks, "Predicted peaks");

    _update_button =
        f.addButton("Update", "Update predicted peak positions given refined unit cell");

    _left_layout->addWidget(update_box);

    connect(_update_button, &QPushButton::clicked, this, &SubframeRefiner::updatePredictions);
}

void SubframeRefiner::updatePeaks()
{
    QSignalBlocker blocker(_predicted_combo);
    if (_predicted_combo->count() == 0)
        return;

    // Peaks centres are refined in-place
    _refined_peaks = _predicted_combo->currentPeakCollection();
    _refined_collection_item.setPeakCollection(_refined_peaks);
    _refined_model.setRoot(&_refined_collection_item);

    _unrefined_collection_item.setPeakCollection(&_unrefined_peaks);
    _unrefined_model.setRoot(&_unrefined_collection_item);
}

void SubframeRefiner::setPeakViewWidgetUp(PeakViewWidget* peak_widget, QString name)
{

    Spoiler* preview_spoiler = new Spoiler(name);
    preview_spoiler->setContentLayout(*peak_widget, true);
    _left_layout->addWidget(preview_spoiler);
    preview_spoiler->setExpanded(false);

    connect(
        peak_widget, &PeakViewWidget::settingsChanged, this, &SubframeRefiner::refreshPeakVisual);
}

void SubframeRefiner::refreshPeakVisual()
{
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget_1->set1);
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget_2->set1, true);
    _detector_widget->refresh();

    if (_refined_collection_item.childCount() == 0)
        return;

    for (int i = 0; i < _refined_collection_item.childCount(); i++) {
        PeakItem* peak = _refined_collection_item.peakItemAt(i);
        auto graphic = peak->peakGraphic();

        graphic->showLabel(false);
        graphic->setColor(Qt::transparent);
        graphic->initFromPeakViewWidget(
            peak->peak()->enabled() ? _peak_view_widget_1->set1 : _peak_view_widget_1->set2);
    }

    if (!(_unrefined_collection_item.childCount() == 0)) {
        for (int i = 0; i < _unrefined_collection_item.childCount(); i++) {
            PeakItem* peak = _unrefined_collection_item.peakItemAt(i);
            auto graphic = peak->peakGraphic();

            graphic->showLabel(false);
            graphic->setColor(Qt::transparent);
            graphic->initFromPeakViewWidget(
                peak->peak()->enabled() ? _peak_view_widget_2->set1 : _peak_view_widget_2->set2);
        }
    }

    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget_1->set1);
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget_2->set1, true);
    _detector_widget->refresh();
    _detector_widget->scene()->drawPeakitems();
}

void SubframeRefiner::grabRefinerParameters()
{
    auto* params = gSession->currentProject()->experiment()->refiner()->parameters();

    _n_batches_spin->setValue(params->nbatches);
    _max_iter_spin->setValue(params->max_iter);
    _refineUB->setChecked(params->refine_ub);
    _refineSamplePosition->setChecked(params->refine_sample_position);
    _refineSampleOrientation->setChecked(params->refine_sample_orientation);
    _refineDetectorPosition->setChecked(params->refine_detector_offset);
    _refineKi->setChecked(params->refine_ki);
    _batch_cell_check->setChecked(params->use_batch_cells);
    for (const auto& [key, val] : _residual_strings) {
        if (val == params->residual_type) {
            _residual_combo->setCurrentText(QString::fromStdString(key));
            break;
        }
    }
}

void SubframeRefiner::setRefinerParameters()
{
    if (!gSession->hasProject())
        return;
    auto* params = gSession->currentProject()->experiment()->refiner()->parameters();

    params->nbatches = _n_batches_spin->value();
    params->max_iter = _max_iter_spin->value();
    params->refine_ub = _refineUB->isChecked();
    params->refine_sample_position = _refineSamplePosition->isChecked();
    params->refine_sample_orientation = _refineSampleOrientation->isChecked();
    params->refine_detector_offset = _refineDetectorPosition->isChecked();
    params->refine_ki = _refineKi->isChecked();
    params->use_batch_cells = _batch_cell_check->isChecked();
    for (const auto& [key, val] : _residual_strings) {
        if (key == _residual_combo->currentText().toStdString())
            params->residual_type = val;
    }
}

void SubframeRefiner::updatePredictions()
{
    gGui->setReady(false);
    if (_refine_success) {

        // A local copy to compare positions pre- and post-refinement
        _unrefined_peaks.reset();
        _unrefined_peaks.populate(_refined_peaks->getPeakList());
        updatePeaks();

        auto* expt = gSession->currentProject()->experiment();
        auto* refiner = expt->refiner();
        auto* peaks = _predicted_combo->currentPeakCollection();
        auto peak_list = peaks->getPeakList();

        int n_updated = refiner->updatePredictions(peak_list);
        gGui->statusBar()->showMessage(QString::number(n_updated) + " peaks updated");
        refreshPeakVisual();
        gGui->detector_window->refreshAll();
    } else {
        QMessageBox::critical(this, "Error", "Cannot update predictions: refinement failed");
    }
    _tab_widget->setCurrentIndex(1);
    gGui->setReady(true);
}

QList<PlotCheckBox*> SubframeRefiner::plotCheckBoxes() const
{
    return _plot_box->findChildren<PlotCheckBox*>();
}

void SubframeRefiner::toggleUnsafeWidgets()
{
    _refine_button->setEnabled(true);
    _update_button->setEnabled(false);
    _cell_combo->setEnabled(true);

    if (!gSession->hasProject())
        return;

    if (!(_predicted_combo->count() == 0))
        _update_button->setEnabled(true);

    if (_batch_cell_check->isChecked())
        _cell_combo->setEnabled(false);

    if (!gSession->currentProject()->hasDataSet()
        || !gSession->currentProject()->hasPeakCollection()) {
        _refine_button->setEnabled(false);
        _update_button->setEnabled(false);
    }
    if (!_refine_success)
        _update_button->setEnabled(false);

    ohkl::PeakCollection* pc = nullptr;
    std::string current_pc = _peak_combo->currentText().toStdString();
    if (current_pc.size() == 0)
        return;
    pc = _peak_combo->currentPeakCollection();

    if (!pc->isIndexed()) {
        _refine_button->setEnabled(false);
        _update_button->setEnabled(false);
    }
}
