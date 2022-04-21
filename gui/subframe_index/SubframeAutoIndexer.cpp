//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_index/SubframeAutoIndexer.cpp
//! @brief     Implements class SubframeAutoIndexer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_index/SubframeAutoIndexer.h"

#include "base/utils/Logger.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"
#include "core/algo/AutoIndexer.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Qs2Events.h"
#include "gui/MainWin.h" // gGui
#include "gui/connect/Sentinel.h"
#include "gui/dialogs/UnitCellDialog.h"
#include "gui/frames/UnitCellWidget.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/utility/SpoilerCheck.h"
#include "gui/views/PeakTableView.h"
#include "gui/views/UnitCellTableView.h"
#include "gui/widgets/DetectorWidget.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QSplitter>
#include <QTabWidget>
#include <QVBoxLayout>
#include <qobject.h>
#include <stdexcept>


SubframeAutoIndexer::SubframeAutoIndexer()
    : QWidget()
    , _peak_collection(nsx::kw_autoindexingCollection, nsx::listtype::INDEXING)
    , _peak_collection_item()
    , _peak_collection_model()
    , _show_direct_beam(true)
    , _size_policy_right(QSizePolicy::Expanding, QSizePolicy::Expanding)
{
    QHBoxLayout* main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);
    _left_layout = new QVBoxLayout();

    QTabWidget* tab_widget = new QTabWidget(this);
    QWidget* tables_tab = new QWidget(tab_widget);
    QWidget* detector_tab = new QWidget(tab_widget);
    tab_widget->addTab(tables_tab, "Solutions");
    tab_widget->addTab(detector_tab, "Detector");

    setInputUp();
    setAdjustBeamUp();
    setParametersUp();
    setProceedUp();
    setPeakViewWidgetUp();
    setPeakTableUp();
    setSolutionTableUp();
    setFigureUp();
    toggleUnsafeWidgets();

    connect(
        _detector_widget->scene(), &DetectorScene::beamPosChanged, this,
        &SubframeAutoIndexer::onBeamPosChanged);
    connect(
        this, &SubframeAutoIndexer::beamPosChanged, _detector_widget->scene(),
        &DetectorScene::setBeamSetterPos);
    connect(
        this, &SubframeAutoIndexer::crosshairChanged, _detector_widget->scene(),
        &DetectorScene::onCrosshairChanged);

    _detector_widget->scene()->linkDirectBeamPositions(&_direct_beam_events);
    _detector_widget->scene()->linkOldDirectBeamPositions(&_old_direct_beam_events);

    tables_tab->setLayout(_solution_layout);
    detector_tab->setLayout(_detector_widget);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);

    _right_element->addWidget(tab_widget);
    _right_element->addWidget(_peak_group);
    _right_element->setSizePolicy(_size_policy_right);
    _set_initial_ki->setChecked(false);
}


void SubframeAutoIndexer::setInputUp()
{
    Spoiler* input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _data_combo = f.addLinkedCombo(ComboType::DataSet, "Data set");
    _peak_combo = f.addLinkedCombo(ComboType::FoundPeaks, "Peak collection");

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeAutoIndexer::refreshPeakTable);

    _left_layout->addWidget(input_box);
}

void SubframeAutoIndexer::setAdjustBeamUp()
{
    _set_initial_ki = new SpoilerCheck("Set initial direct beam position");
    GridFiller f(_set_initial_ki, true);

    _beam_offset_x = f.addDoubleSpinBox("x offset", "Direct beam offset in x direction (pixels)");

    _beam_offset_y = f.addDoubleSpinBox("y offset", "Direct beam offset in y direction (pixels)");

    _crosshair_size = new QSlider(Qt::Horizontal);
    QLabel* crosshair_label = new QLabel("Crosshair size");
    crosshair_label->setToolTip("Radius of crosshair (pixels)");
    crosshair_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    _crosshair_size->setMinimum(5);
    _crosshair_size->setMaximum(200);
    _crosshair_size->setValue(15);
    f.addLabel("Crosshair size", "Radius of crosshair (pixels)");
    f.addWidget(_crosshair_size, 1);

    _crosshair_linewidth = f.addSpinBox("Crosshair linewidth", "Line width of crosshair");

    _beam_offset_x->setValue(0.0);
    _beam_offset_x->setMaximum(1000.0);
    _beam_offset_x->setMinimum(-1000.0);
    _beam_offset_x->setDecimals(4);
    _beam_offset_y->setValue(0.0);
    _beam_offset_y->setMaximum(1000.0);
    _beam_offset_y->setMinimum(-1000.0);
    _beam_offset_y->setDecimals(4);
    _crosshair_linewidth->setValue(2);
    _crosshair_linewidth->setMinimum(1);
    _crosshair_linewidth->setMaximum(10);

    connect(
        _set_initial_ki->checkBox(), &QCheckBox::stateChanged, this,
        &SubframeAutoIndexer::refreshPeakVisual);
    connect(
        _set_initial_ki->checkBox(), &QCheckBox::stateChanged, this,
        &SubframeAutoIndexer::toggleCursorMode);
    connect(
        _beam_offset_x,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeAutoIndexer::onBeamPosSpinChanged);
    connect(
        _beam_offset_y,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeAutoIndexer::onBeamPosSpinChanged);
    connect(
        _crosshair_size, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this,
        &SubframeAutoIndexer::changeCrosshair);
    connect(
        _crosshair_linewidth, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeAutoIndexer::changeCrosshair);

    _left_layout->addWidget(_set_initial_ki);
}

void SubframeAutoIndexer::setParametersUp()
{
    Spoiler* para_box = new Spoiler("Parameters");
    GridFiller f(para_box, true);

    std::tie(_min_frame, _max_frame) =
        f.addSpinBoxPair("Frames:", "(frames) - range of frames over which to index");

    std::tie(_d_min, _d_max) = f.addDoubleSpinBoxPair(
        "d range:",
        QString::fromUtf8(
            "(\u212B) - only attempt to index peaks over this d (Bragg's law) range "));

    std::tie(_str_min, _str_max) = f.addDoubleSpinBoxPair(
        "Strength:",
        QString::fromUtf8("(I/\u03C3) - only attempt to index peaks in this strength range"));

    _gruber = f.addDoubleSpinBox("Gruber tol.:", "Tolerance for Gruber reduction");

    _niggli = f.addDoubleSpinBox("Niggli tol.:", "Tolerance for Niggli reduction");

    _only_niggli = f.addCheckBox("Find Niggli cell only", 1);

    _max_cell_dimension = f.addDoubleSpinBox(
        "Max. Cell dim.:", QString::fromUtf8("(\u212B) - maximum length of any lattice vector"));

    _number_vertices = f.addSpinBox(
        "Q Vertices:",
        "Number of points on reciprocal space unit sphere to test against candidate lattice "
        "vector");

    _number_subdivisions = f.addSpinBox("Subdivisions:", "Number of histogram bins for FFT");

    _number_solutions = f.addSpinBox("Unit Cells:", "Number of unit cell solutions to find");

    _min_cell_volume = f.addDoubleSpinBox(
        "Min. Volume:",
        QString::fromUtf8("(\u212B^3) - discard candidate cells below this volume"));

    _indexing_tolerance = f.addDoubleSpinBox("Indexing tol.:");

    _frequency_tolerance = f.addDoubleSpinBox(
        "Frequency tol.:",
        "Minimum fraction of amplitude of the zeroth Fourier frequency to accept as a candidate "
        "lattice vector");

    _min_frame->setMaximum(1000);
    _max_frame->setMaximum(1000);

    _d_min->setMaximum(100);
    _d_min->setDecimals(2);

    _d_max->setMaximum(100);
    _d_max->setDecimals(2);

    _str_min->setMaximum(10000);
    _str_min->setDecimals(2);

    _str_max->setMaximum(1000000000);
    _str_max->setDecimals(2);

    _gruber->setMaximum(10);
    _gruber->setDecimals(6);

    _niggli->setMaximum(10);
    _niggli->setDecimals(6);

    _max_cell_dimension->setMaximum(1000);
    _max_cell_dimension->setDecimals(2);

    _number_vertices->setMaximum(100000);

    _number_solutions->setMaximum(1000);

    _min_cell_volume->setMaximum(1000000);
    _min_cell_volume->setDecimals(2);

    _indexing_tolerance->setMaximum(10);
    _indexing_tolerance->setDecimals(6);

    _frequency_tolerance->setMaximum(1);
    _frequency_tolerance->setDecimals(3);

    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this,
        &SubframeAutoIndexer::setIndexerParameters);

    _left_layout->addWidget(para_box);
}

void SubframeAutoIndexer::setProceedUp()
{
    _solve_button = new QPushButton("Find unit cells");
    _left_layout->addWidget(_solve_button);

    _save_button = new QPushButton("Assign selected unit cell");
    _left_layout->addWidget(_save_button);

    connect(_solve_button, &QPushButton::clicked, this, &SubframeAutoIndexer::runAutoIndexer);
    connect(_save_button, &QPushButton::clicked, this, &SubframeAutoIndexer::acceptSolution);
}

void SubframeAutoIndexer::setPeakTableUp()
{
    _peak_group = new QGroupBox("Peaks used in indexing");
    QGridLayout* peak_layout = new QGridLayout(_peak_group);

    _peak_group->setSizePolicy(_size_policy_right);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);

    peak_layout->addWidget(_peak_table, 0, 0, 0, 0);
}

void SubframeAutoIndexer::setSolutionTableUp()
{
    QGroupBox* solution_group = new QGroupBox("Solutions");
    _solution_layout = new QVBoxLayout(solution_group);

    solution_group->setSizePolicy(_size_policy_right);

    _solution_table = new UnitCellTableView(this);

    _solution_layout->addWidget(_solution_table);

    connect(
        _solution_table->verticalHeader(), &QHeaderView::sectionClicked, this,
        &SubframeAutoIndexer::selectSolutionHeader);

    connect(
        _solution_table, &UnitCellTableView::clicked, this,
        &SubframeAutoIndexer::selectSolutionTable);
}

void SubframeAutoIndexer::refreshAll()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasDataSet())
        return;

    updateDatasetList();
    updatePeakList();
    grabIndexerParameters();

    const auto all_data = gSession->currentProject()->allData();
    _detector_widget->updateDatasetList(all_data);
    const auto dataset =
        gSession->currentProject()->experiment()->getData(_data_combo->currentText().toStdString());
    _max_frame->setMaximum(dataset->nFrames() - 1);

    _beam_offset_x->setMaximum(static_cast<double>(dataset->nCols()) / 2.0);
    _beam_offset_x->setMinimum(-static_cast<double>(dataset->nCols()) / 2.0);
    _beam_offset_y->setMaximum(static_cast<double>(dataset->nRows()) / 2.0);
    _beam_offset_y->setMinimum(-static_cast<double>(dataset->nRows()) / 2.0);

    toggleUnsafeWidgets();
}

void SubframeAutoIndexer::setPeakViewWidgetUp()
{
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    Spoiler* preview_spoiler = new Spoiler("Show/hide peaks");
    preview_spoiler->setContentLayout(*_peak_view_widget, true);
    _left_layout->addWidget(preview_spoiler);
    preview_spoiler->setExpanded(false);

    connect(
        _peak_view_widget, &PeakViewWidget::settingsChanged, this,
        &SubframeAutoIndexer::refreshPeakVisual);
}

void SubframeAutoIndexer::setFigureUp()
{
    _detector_widget = new DetectorWidget(false, false, true);
    _detector_widget->linkPeakModel(&_peak_collection_model);

    connect(
        _detector_widget->spin(), static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this, &SubframeAutoIndexer::refreshPeakVisual);
    connect(
        _detector_widget->scene(), &DetectorScene::signalUpdateDetectorScene, this,
        &SubframeAutoIndexer::refreshPeakTable);
    connect(
        _detector_widget->scene(), &DetectorScene::signalSelectedPeakItemChanged, this,
        &SubframeAutoIndexer::changeSelected);
}

void SubframeAutoIndexer::updateDatasetList()
{
    if (!gSession->currentProject()->hasDataSet())
        return;

    QSignalBlocker blocker(_data_combo);
    QString current_data = _data_combo->currentText();
    _data_combo->clear();
    _data_list = gSession->currentProject()->allData();
    _detector_widget->updateDatasetList(_data_list);

    const QStringList& datanames{gSession->currentProject()->getDataNames()};
    _data_combo->addItems(datanames);
    _data_combo->setCurrentText(current_data);
}

void SubframeAutoIndexer::updatePeakList()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;

    QSignalBlocker blocker(_peak_combo);
    QStringList peak_list = gSession->currentProject()->getPeakListNames();
    if (peak_list.empty())
        return;
    peak_list.clear();

    QString current_peaks = _peak_combo->currentText();
    _peak_combo->clear();

    QStringList tmp = gSession->currentProject() ->getPeakCollectionNames(nsx::listtype::FOUND);
    peak_list.append(tmp);
    tmp.clear();
    tmp = gSession->currentProject() ->getPeakCollectionNames(nsx::listtype::FILTERED);
    peak_list.append(tmp);
    tmp.clear();
    tmp = gSession->currentProject() ->getPeakCollectionNames(nsx::listtype::INDEXING);
    peak_list.append(tmp);

    _peak_combo->addItems(peak_list);
    _peak_combo->setCurrentText(current_peaks);

    refreshPeakTable();
    _solution_table->setModel(nullptr);
    _solutions.clear();
    _selected_unit_cell = nullptr;
}

void SubframeAutoIndexer::refreshPeakTable()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;

    _peak_collection.setMillerIndices();
    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->resizeColumnsToContents();
    refreshPeakVisual();
}

void SubframeAutoIndexer::changeSelected(PeakItemGraphic* peak_graphic)
{
    int row = _peak_collection_item.returnRowOfVisualItem(peak_graphic);
    QModelIndex index = _peak_collection_model.index(row, 0);
    _peak_table->selectRow(row);
    _peak_table->scrollTo(index, QAbstractItemView::PositionAtTop);
}

void SubframeAutoIndexer::refreshPeakVisual()
{
    auto data = _detector_widget->currentData();
    _detector_widget->scene()->initIntRegionFromPeakWidget(_peak_view_widget->set1);
    if (_set_initial_ki->isChecked()) {
        _detector_widget->scene()->addBeamSetter(
            _crosshair_size->value(), _crosshair_linewidth->value());
        changeCrosshair();
    }
    showDirectBeamEvents();

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

void SubframeAutoIndexer::grabIndexerParameters()
{
    if (!gSession->hasProject())
        return;

    auto params = gSession->currentProject()->experiment()->autoIndexer()->parameters();
    _min_frame->setValue(params->first_frame);
    _max_frame->setValue(params->last_frame);
    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
    _str_min->setValue(params->strength_min);
    _str_max->setValue(params->strength_max);
    _niggli->setValue(params->niggliTolerance);
    _only_niggli->setChecked(params->niggliReduction);
    _gruber->setValue(params->gruberTolerance);
    _number_vertices->setValue(params->nVertices);
    _number_subdivisions->setValue(params->subdiv);
    _number_solutions->setValue(params->nSolutions);
    _max_cell_dimension->setValue(params->maxdim);
    _indexing_tolerance->setValue(params->indexingTolerance);
    _frequency_tolerance->setValue(params->frequencyTolerance);
    _min_cell_volume->setValue(params->minUnitCellVolume);
}

void SubframeAutoIndexer::setIndexerParameters()
{
    if (!gSession->hasProject())
        return;

    auto params = gSession->currentProject()->experiment()->autoIndexer()->parameters();

    params->first_frame = _min_frame->value();
    params->last_frame = _max_frame->value();
    params->d_min = _d_min->value();
    params->d_max = _d_max->value();
    params->strength_min = _str_min->value();
    params->strength_max = _str_max->value();
    params->niggliTolerance = _niggli->value();
    params->niggliReduction = _only_niggli->isChecked();
    params->gruberTolerance = _gruber->value();
    params->nVertices = _number_vertices->value();
    params->subdiv = _number_subdivisions->value();
    params->nSolutions = _number_solutions->value();
    params->maxdim = _max_cell_dimension->value();
    params->indexingTolerance = _indexing_tolerance->value();
    params->frequencyTolerance = _frequency_tolerance->value();
    params->minUnitCellVolume = _min_cell_volume->value();
}

void SubframeAutoIndexer::runAutoIndexer()
{
    gGui->setReady(false);
    setIndexerParameters();

    auto* expt = gSession->currentProject()->experiment();
    auto* autoindexer = expt->autoIndexer();
    auto* params = autoindexer->parameters();
    auto* filter = expt->peakFilter();
    nsx::PeakCollection* collection =
        expt->getPeakCollection(_peak_combo->currentText().toStdString());


    // Manally adjust the direct beam position
    if (_set_initial_ki->isChecked()) {
        auto data = _detector_widget->currentData();
        auto* detector = data->diffractometer()->detector();
        auto& states = data->instrumentStates();
        _old_direct_beam_events = nsx::algo::getDirectBeamEvents(states, *detector);
        setInitialKi(data);
    }

    std::shared_ptr<nsx::ProgressHandler> handler(new nsx::ProgressHandler());
    autoindexer->setHandler(handler);

    filter->resetFiltering(collection);
    filter->resetFilterFlags();
    filter->flags()->strength = true;
    filter->flags()->d_range = true;
    filter->flags()->frames = true;
    filter->parameters()->d_min = params->d_min;
    filter->parameters()->d_max = params->d_max;
    filter->parameters()->strength_min = params->strength_min;
    filter->parameters()->strength_max = params->strength_max;
    filter->parameters()->frame_min = params->first_frame;
    filter->parameters()->frame_max = params->last_frame;

    filter->filter(collection);
    _peak_collection.populateFromFiltered(collection);
    _solutions.clear();
    refreshPeakTable();

    bool success = autoindexer->autoIndex(&_peak_collection);
    if (!success) {
        gGui->statusBar()->showMessage("Indexing failed");
        return;
    }

    _solutions = autoindexer->solutions();
    buildSolutionsTable();

    autoindexer->unsetHandler();
    handler.reset();
    toggleUnsafeWidgets();

    if (_solutions.empty())
        gGui->statusBar()->showMessage("WARNING: No unit cells found");
    else
        gGui->statusBar()->showMessage(QString::number(_solutions.size()) + " unit cells found");

    gGui->setReady(true);
}

void SubframeAutoIndexer::buildSolutionsTable()
{
    // Create table with 9 columns
    QStandardItemModel* model = new QStandardItemModel(_solutions.size(), 9, this);
    model->setHorizontalHeaderItem(0, new QStandardItem("a"));
    model->setHorizontalHeaderItem(1, new QStandardItem("b"));
    model->setHorizontalHeaderItem(2, new QStandardItem("c"));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString((QChar)0x03B1))); // alpha
    model->setHorizontalHeaderItem(4, new QStandardItem(QString((QChar)0x03B2))); // beta
    model->setHorizontalHeaderItem(5, new QStandardItem(QString((QChar)0x03B3))); // gamma
    model->setHorizontalHeaderItem(6, new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7, new QStandardItem("Bravais type"));
    model->setHorizontalHeaderItem(8, new QStandardItem("Quality"));

    // Display solutions
    for (unsigned int i = 0; i < _solutions.size(); ++i) {
        const nsx::sptrUnitCell cell = _solutions[i].first;
        const double quality = _solutions[i].second;
        const double volume = cell->volume();

        const nsx::UnitCellCharacter ch = cell->character();
        const nsx::UnitCellCharacter sigma = cell->characterSigmas();

        ValueTupleItem* col1 = new ValueTupleItem(
            QString::number(ch.a, 'f', 3) + "(" + QString::number(sigma.a * 1000, 'f', 0) + ")",
            ch.a, sigma.a);
        ValueTupleItem* col2 = new ValueTupleItem(
            QString::number(ch.b, 'f', 3) + "(" + QString::number(sigma.b * 1000, 'f', 0) + ")",
            ch.b, sigma.b);
        ValueTupleItem* col3 = new ValueTupleItem(
            QString::number(ch.c, 'f', 3) + "(" + QString::number(sigma.c * 1000, 'f', 0) + ")",
            ch.c, sigma.c);
        ValueTupleItem* col4 = new ValueTupleItem(
            QString::number(ch.alpha / nsx::deg, 'f', 3) + "("
                + QString::number(sigma.alpha / nsx::deg * 1000, 'f', 0) + ")",
            ch.alpha, sigma.alpha);
        ValueTupleItem* col5 = new ValueTupleItem(
            QString::number(ch.beta / nsx::deg, 'f', 3) + "("
                + QString::number(sigma.beta / nsx::deg * 1000, 'f', 0) + ")",
            ch.beta, sigma.beta);
        ValueTupleItem* col6 = new ValueTupleItem(
            QString::number(ch.gamma / nsx::deg, 'f', 3) + "("
                + QString::number(sigma.gamma / nsx::deg * 1000, 'f', 0) + ")",
            ch.gamma, sigma.gamma);
        ValueTupleItem* col7 = new ValueTupleItem(QString::number(volume, 'f', 3), volume);
        QStandardItem* col8 = new QStandardItem(QString::fromStdString(cell->bravaisTypeSymbol()));
        ValueTupleItem* col9 = new ValueTupleItem(QString::number(quality, 'f', 2) + "%", quality);

        model->setItem(i, 0, col1);
        model->setItem(i, 1, col2);
        model->setItem(i, 2, col3);
        model->setItem(i, 3, col4);
        model->setItem(i, 4, col5);
        model->setItem(i, 5, col6);
        model->setItem(i, 6, col7);
        model->setItem(i, 7, col8);
        model->setItem(i, 8, col9);
    }
    _solution_table->setModel(model);
}

void SubframeAutoIndexer::selectSolutionTable()
{
    const QItemSelectionModel* select = _solution_table->selectionModel();
    QModelIndexList indices = select->selectedRows();
    if (!indices.empty())
        selectSolutionHeader(indices[0].row());
}

void SubframeAutoIndexer::selectSolutionHeader(int index)
{
    _selected_unit_cell = _solutions[index].first;

    const nsx::PeakCollection* collection =
        gSession->currentProject()->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());

    std::vector<nsx::Peak3D*> peaks = collection->getPeakList();
    for (nsx::Peak3D* peak : peaks) {
        peak->setUnitCell(_selected_unit_cell);
        peak->setMillerIndices();
    }
    refreshPeakTable();
}

void SubframeAutoIndexer::acceptSolution()
{
    if (_selected_unit_cell) {
        nsx::Experiment* expt = gSession->currentProject()->experiment();
        QStringList collections =
            gSession->currentProject()->getPeakCollectionNames(nsx::listtype::FOUND);

        QStringList space_groups;
        for (const std::string& name : _selected_unit_cell->compatibleSpaceGroups())
            space_groups.push_back(QString::fromStdString(name));

        std::unique_ptr<UnitCellDialog> dlg(new UnitCellDialog(
            QString::fromStdString(expt->generateUnitCellName()), collections, space_groups));
        dlg->exec();
        if (dlg->unitCellName().isEmpty())
            return;
        if (dlg->result() == QDialog::Rejected)
            return;

        std::string cellName = dlg->unitCellName().toStdString();
        _selected_unit_cell->setName(cellName);
        if (!expt->addUnitCell(dlg->unitCellName().toStdString(), *_selected_unit_cell.get())) {
            QMessageBox::warning(
                this, "Unable to add Unit Cell", "UnitCell with same name already exists");
            return;
        }

        gSession->onUnitCellChanged();

        nsx::PeakCollection* collection =
            expt->getPeakCollection(dlg->peakCollectionName().toStdString());
        expt->assignUnitCell(collection, cellName);
        nsx::UnitCell* cell = expt->getUnitCell(cellName);
        cell->setSpaceGroup(dlg->spaceGroup().toStdString());
        collection->setMillerIndices();
        gGui->sentinel->addLinkedComboItem(ComboType::UnitCell, dlg->unitCellName());
    }
}

void SubframeAutoIndexer::toggleUnsafeWidgets()
{
    _solve_button->setEnabled(true);
    _save_button->setEnabled(true);
    if (!gSession->hasProject() || !gSession->currentProject()->hasPeakCollection() ||
        !gSession->currentProject()->hasDataSet()) {
        _solve_button->setEnabled(false);
        _save_button->setEnabled(false);
    }
    if (_peak_collection_model.rowCount() == 0 || _solutions.empty())
        _save_button->setEnabled(false);

    nsx::PeakCollection* pc = nullptr;
    std::string current_pc = _peak_combo->currentText().toStdString();
    if (current_pc.size() == 0)
        return;
    pc = gSession->currentProject()->experiment()->getPeakCollection(current_pc);

    _solve_button->setEnabled(pc->isIntegrated());
}

void SubframeAutoIndexer::onBeamPosChanged(QPointF pos)
{
    const QSignalBlocker blocker(this);
    auto data = _detector_widget->currentData();
    _beam_offset_x->setValue(pos.x() - (static_cast<double>(data->nCols()) / 2.0));
    _beam_offset_y->setValue(-pos.y() + (static_cast<double>(data->nRows()) / 2.0));
}

void SubframeAutoIndexer::onBeamPosSpinChanged()
{
    auto data = _detector_widget->currentData();
    double x = _beam_offset_x->value() + static_cast<double>(data->nCols()) / 2.0;
    double y = -_beam_offset_y->value() + static_cast<double>(data->nRows()) / 2.0;
    emit beamPosChanged({x, y});
}

void SubframeAutoIndexer::changeCrosshair()
{
    emit crosshairChanged(_crosshair_size->value(), _crosshair_linewidth->value());
}

void SubframeAutoIndexer::toggleCursorMode()
{
    if (_set_initial_ki->isChecked()) {
        _stored_cursor_mode = _detector_widget->scene()->mode();
        _detector_widget->scene()->changeInteractionMode(7);
    } else {
        _detector_widget->scene()->changeInteractionMode(_stored_cursor_mode);
    }
}

void SubframeAutoIndexer::setInitialKi(nsx::sptrDataSet data)
{
    const auto* detector = data->diffractometer()->detector();
    const auto coords = _detector_widget->scene()->beamSetterCoords();

    nsx::DirectVector direct = detector->pixelPosition(coords.x(), coords.y());
    for (nsx::InstrumentState& state : data->instrumentStates())
        state.adjustKi(direct);
    emit gGui->sentinel->instrumentStatesChanged();
}

void SubframeAutoIndexer::showDirectBeamEvents()
{
    if (!_show_direct_beam)
        return;

    updateDatasetList();
    _detector_widget->scene()->showDirectBeam(true);
    auto data_name = _detector_widget->dataCombo()->currentText().toStdString();
    if (data_name.empty()) { // to prevent crash
        QMessageBox::warning(
            nullptr, "Empty Experimentname",
            "Unable to retrieve data for an empty experiment name!");
        return;
    }
    const auto data = _detector_widget->currentData();

    _direct_beam_events.clear();
    const auto& states = data->instrumentStates();
    auto* detector = data->diffractometer()->detector();
    std::vector<nsx::DetectorEvent> events = nsx::algo::getDirectBeamEvents(states, *detector);

    for (auto&& event : events)
        _direct_beam_events.push_back(event);
}
