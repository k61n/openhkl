//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_index/SubframeAutoIndexer.cpp
//! @brief     Implements class SubframeAutoIndexer
//!
//! @homepage  https://openhkl.org
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
#include "gui/utility/CellComboBox.h"
#include "gui/utility/DataComboBox.h"
#include "gui/utility/FoundPeakComboBox.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/SafeSpinBox.h"
#include "gui/utility/SideBar.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/views/UnitCellTableView.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/DirectBeamWidget.h"

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
#include <stdexcept>


SubframeAutoIndexer::SubframeAutoIndexer()
    : QWidget()
    , _peak_collection(ohkl::kw_autoindexingCollection, ohkl::PeakCollectionType::INDEXING, nullptr)
    , _peak_collection_item()
    , _peak_collection_model()
    , _show_direct_beam(true)
    , _size_policy_right(QSizePolicy::Expanding, QSizePolicy::Expanding)
{
    QHBoxLayout* main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);
    _left_layout = new QVBoxLayout();

    _tab_widget = new QTabWidget(this);
    QWidget* tables_tab = new QWidget(_tab_widget);
    QWidget* detector_tab = new QWidget(_tab_widget);
    _tab_widget->addTab(tables_tab, "Autoindexer solutions");
    _tab_widget->addTab(detector_tab, "Detector image");

    _detector_widget = new DetectorWidget(1, false, true);
    _beam_setter_widget = new DirectBeamWidget(_detector_widget->scene());
    _peak_view_widget = new PeakViewWidget("Valid peaks", "Invalid Peaks");

    setFigureUp();
    setInputUp();
    setAdjustBeamUp();
    setParametersUp();
    setProceedUp();
    setPeakViewWidgetUp();
    setPeakTableUp();
    setSolutionTableUp();
    toggleUnsafeWidgets();

    connect(
        _data_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        _detector_widget->dataCombo(), &QComboBox::setCurrentIndex);
    connect(
        _detector_widget->dataCombo(), QOverload<int>::of(&QComboBox::currentIndexChanged),
        _data_combo, &QComboBox::setCurrentIndex);

    _detector_widget->scene()->linkDirectBeam(&_direct_beam_events, &_old_direct_beam_events);

    tables_tab->setLayout(_solution_layout);
    detector_tab->setLayout(_detector_widget);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);

    _peak_collection_item.setPeakCollection(&_peak_collection);
    _peak_collection_model.setRoot(&_peak_collection_item);

    _right_element->addWidget(_tab_widget);
    _right_element->addWidget(_peak_group);
    _right_element->setSizePolicy(_size_policy_right);

    _right_element->setStretchFactor(0, 2);
    _right_element->setStretchFactor(1, 1);
}

void SubframeAutoIndexer::setInputUp()
{
    Spoiler* input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _data_combo = f.addDataCombo("Data set");
    _peak_combo = f.addPeakCombo(ComboType::FoundPeaks, "Peak collection");

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeAutoIndexer::refreshPeakTable);

    _left_layout->addWidget(input_box);
}

void SubframeAutoIndexer::setAdjustBeamUp()
{
    _set_initial_ki = new Spoiler("Set initial direct beam position");
    _set_initial_ki->setContentLayout(*_beam_setter_widget, true);

    connect(
        _beam_setter_widget->crosshairOn(), &QCheckBox::stateChanged, this,
        &SubframeAutoIndexer::refreshPeakVisual);
    connect(
        _beam_setter_widget->crosshairOn(), &QCheckBox::stateChanged, this,
        &SubframeAutoIndexer::toggleCursorMode);

    _left_layout->addWidget(_set_initial_ki);
}

void SubframeAutoIndexer::setParametersUp()
{
    Spoiler* para_box = new Spoiler("Autoindexer parameters");
    GridFiller f(para_box, true);

    std::tie(_min_frame, _max_frame) =
        f.addSpinBoxPair("Image range:", "(images) - range of detector images over which to index");

    std::tie(_d_min, _d_max) = f.addDoubleSpinBoxPair(
        "Resolution (d) range:",
        QString::fromUtf8(
            "(\u212B) - only attempt to index peaks over this d (from Bragg's law) range"));

    std::tie(_str_min, _str_max) = f.addDoubleSpinBoxPair(
        "Strength range:",
        QString::fromUtf8("(I/\u03C3) - only attempt to index peaks in this strength range"));

    _gruber = f.addDoubleSpinBox("Gruber tolerance:", "Tolerance for Gruber reduction");

    _niggli = f.addDoubleSpinBox("Niggli tolerance:", "Tolerance for Niggli reduction");

    _only_niggli = f.addCheckBox("Find Niggli cell only", 1);

    _max_cell_dimension = f.addDoubleSpinBox(
        "Max. Cell dimension:",
        QString::fromUtf8("(\u212B) - maximum length of any lattice vector"));

    _number_vertices = f.addSpinBox(
        "Num. Q-space trial vectors:",
        "Number of points on reciprocal space unit sphere to test against candidate lattice "
        "vector");

    _number_subdivisions = f.addSpinBox(
        "Num. FFT histogram bins:", "Number of histogram bins for Fast Fourier transform");

    _number_solutions =
        f.addSpinBox("Number of solutions:", "Number of unit cell solutions to find");

    _min_cell_volume = f.addDoubleSpinBox(
        "Minimum Volume:",
        QString::fromUtf8("(\u212B^3) - discard candidate cells below this volume"));

    _indexing_tolerance = f.addDoubleSpinBox("Indexing tolerance:");

    _frequency_tolerance = f.addDoubleSpinBox(
        "Frequency tolerance:",
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
    _peak_table->setColumnHidden(PeakColumn::Enabled, true);
    _peak_table->setColumnHidden(PeakColumn::Count, true);
    _peak_table->setColumnHidden(PeakColumn::BkgGradient, true);
    _peak_table->setColumnHidden(PeakColumn::BkgGradientSigma, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileIntensity, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileSigma, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileStrength, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileBkg, true);
    _peak_table->setColumnHidden(PeakColumn::ProfileBkgSigma, true);

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

    _data_combo->refresh();
    _peak_combo->refresh();
    refreshPeakTable();
    _detector_widget->refresh();
    _solution_table->setModel(nullptr);
    _solutions.clear();
    _selected_unit_cell = nullptr;
    grabIndexerParameters();

    auto dataset = _data_combo->currentData();
    _max_frame->setMaximum(dataset->nFrames() - 1);
    _beam_setter_widget->setSpinLimits(dataset->nCols(), dataset->nRows());

    toggleUnsafeWidgets();
}

void SubframeAutoIndexer::setPeakViewWidgetUp()
{
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
    _detector_widget->linkPeakModel(&_peak_collection_model, _peak_view_widget);

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

    showDirectBeamEvents();
    _detector_widget->refresh();
}

void SubframeAutoIndexer::grabIndexerParameters()
{
    if (!gSession->hasProject())
        return;

    auto* params = gSession->currentProject()->experiment()->autoIndexer()->parameters();
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

    auto* params = gSession->currentProject()->experiment()->autoIndexer()->parameters();

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

    if (!gSession->currentProject()->hasPeakCollection())
        return;
    auto* peaks = _peak_combo->currentPeakCollection();
    params->peaks_integrated = peaks->isIntegrated();
}

void SubframeAutoIndexer::runAutoIndexer()
{
    gGui->setReady(false);
    setIndexerParameters();

    auto* expt = gSession->currentProject()->experiment();
    auto* autoindexer = expt->autoIndexer();

    // Manally adjust the direct beam position
    if (_beam_setter_widget->crosshairOn()->isChecked()) {
        auto data = _detector_widget->currentData();
        emit gGui->sentinel->instrumentStatesChanged();
        data->adjustDirectBeam(_beam_setter_widget->xOffset(), _beam_setter_widget->yOffset());
        auto* detector = data->diffractometer()->detector();
        auto& states = data->instrumentStates();
        _old_direct_beam_events = ohkl::algo::getDirectBeamEvents(states, *detector);
    }

    std::shared_ptr<ohkl::ProgressHandler> handler(new ohkl::ProgressHandler());
    autoindexer->setHandler(handler);

    _solutions.clear();

    bool success = autoindexer->autoIndex(_peak_combo->currentPeakCollection());
    if (!success) {
        gGui->statusBar()->showMessage("Indexing failed");
        return;
    }

    _peak_collection.populate(*autoindexer->filteredPeaks());
    refreshPeakTable();

    _solutions = autoindexer->solutions();
    buildSolutionsTable();

    autoindexer->unsetHandler();
    handler.reset();
    toggleUnsafeWidgets();

    if (_solutions.empty())
        gGui->statusBar()->showMessage("WARNING: No unit cells found");
    else
        gGui->statusBar()->showMessage(QString::number(_solutions.size()) + " unit cells found");

    _tab_widget->setCurrentIndex(0);
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
        const ohkl::sptrUnitCell cell = _solutions[i].first;
        const double quality = _solutions[i].second;
        const double volume = cell->volume();

        const ohkl::UnitCellCharacter ch = cell->character();
        const ohkl::UnitCellCharacter sigma = cell->characterSigmas();

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
            QString::number(ch.alpha / ohkl::deg, 'f', 3) + "("
                + QString::number(sigma.alpha / ohkl::deg * 1000, 'f', 0) + ")",
            ch.alpha, sigma.alpha);
        ValueTupleItem* col5 = new ValueTupleItem(
            QString::number(ch.beta / ohkl::deg, 'f', 3) + "("
                + QString::number(sigma.beta / ohkl::deg * 1000, 'f', 0) + ")",
            ch.beta, sigma.beta);
        ValueTupleItem* col6 = new ValueTupleItem(
            QString::number(ch.gamma / ohkl::deg, 'f', 3) + "("
                + QString::number(sigma.gamma / ohkl::deg * 1000, 'f', 0) + ")",
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
    toggleUnsafeWidgets();
}

void SubframeAutoIndexer::selectSolutionHeader(int index)
{
    _selected_unit_cell = _solutions[index].first;

    const ohkl::PeakCollection* collection = _peak_combo->currentPeakCollection();

    std::vector<ohkl::Peak3D*> peaks = collection->getPeakList();
    for (ohkl::Peak3D* peak : peaks) {
        peak->setUnitCell(_selected_unit_cell);
        peak->setMillerIndices();
    }
    refreshPeakTable();
}

void SubframeAutoIndexer::acceptSolution()
{
    if (_selected_unit_cell) {
        ohkl::Experiment* expt = gSession->currentProject()->experiment();
        QStringList collections =
            gSession->currentProject()->getPeakCollectionNames(ohkl::PeakCollectionType::FOUND);

        QStringList space_groups;
        for (const std::string& name : _selected_unit_cell->compatibleSpaceGroups())
            space_groups.push_back(QString::fromStdString(name));

        std::unique_ptr<UnitCellDialog> dlg(new UnitCellDialog(
            QString::fromStdString(expt->generateUnitCellName()), space_groups, collections));
        dlg->exec();
        if (dlg->unitCellName().isEmpty())
            return;
        if (dlg->result() == QDialog::Rejected)
            return;

        std::string cellName = dlg->unitCellName().toStdString();
        _selected_unit_cell->setName(cellName);
        expt->addUnitCell(dlg->unitCellName().toStdString(), *_selected_unit_cell.get());
        gSession->onUnitCellChanged();

        gSession->onUnitCellChanged();

        ohkl::PeakCollection* collection =
            expt->getPeakCollection(dlg->peakCollectionName().toStdString());
        expt->assignUnitCell(collection, cellName);
        ohkl::UnitCell* cell = expt->getUnitCell(cellName);
        cell->setSpaceGroup(dlg->spaceGroup().toStdString());
        collection->setMillerIndices();
        gGui->refreshMenu();
    }
}

void SubframeAutoIndexer::toggleUnsafeWidgets()
{
    _solve_button->setEnabled(false);
    _save_button->setEnabled(false);
    _beam_setter_widget->setEnabled(false);

    if (!gSession->hasProject())
        return;

    _solve_button->setEnabled(gSession->currentProject()->hasPeakCollection());
    _save_button->setEnabled(!_solutions.empty());
    _beam_setter_widget->setEnabled(gSession->currentProject()->hasDataSet());
}

void SubframeAutoIndexer::toggleCursorMode()
{
    if (_beam_setter_widget->crosshairOn()->isChecked()) {
        _stored_cursor_mode = _detector_widget->scene()->mode();
        _detector_widget->scene()->changeInteractionMode(7);
    } else {
        _detector_widget->scene()->changeInteractionMode(_stored_cursor_mode);
    }
}

void SubframeAutoIndexer::showDirectBeamEvents()
{
    if (!_show_direct_beam)
        return;

    _detector_widget->scene()->params()->directBeam = true;
    auto data_name = _detector_widget->dataCombo()->currentText().toStdString();
    if (data_name.empty()) {
        return;
    }
    const auto data = _detector_widget->currentData();

    _direct_beam_events.clear();
    const auto& states = data->instrumentStates();
    auto* detector = data->diffractometer()->detector();
    std::vector<ohkl::DetectorEvent> events = ohkl::algo::getDirectBeamEvents(states, *detector);

    for (auto&& event : events)
        _direct_beam_events.push_back(event);
}
