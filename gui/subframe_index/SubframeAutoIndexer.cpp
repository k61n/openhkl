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
#include "core/experiment/Experiment.h"
#include "gui/dialogs/UnitCellDialog.h"
#include "gui/frames/UnitCellWidget.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/GridFiller.h"
#include "gui/utility/PropertyScrollArea.h"
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/views/UnitCellTableView.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>


SubframeAutoIndexer::SubframeAutoIndexer()
    : QWidget()
    , _peak_collection("temp", nsx::listtype::FOUND)
    , _peak_collection_item()
    , _peak_collection_model()
    , _size_policy_right(QSizePolicy::Expanding, QSizePolicy::Expanding)
{
    QHBoxLayout* main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);
    _left_layout = new QVBoxLayout();

    setInputUp();
    setParametersUp();
    setProceedUp();
    setPeakTableUp();
    setSolutionTableUp();
    _right_element->setSizePolicy(_size_policy_right);

    auto propertyScrollArea = new PropertyScrollArea(this);
    propertyScrollArea->setContentLayout(_left_layout);
    main_layout->addWidget(propertyScrollArea);
    main_layout->addWidget(_right_element);
}

void SubframeAutoIndexer::setInputUp()
{
    Spoiler* input_box = new Spoiler("Input");
    GridFiller f(input_box, true);

    _exp_combo = f.addCombo("Experiment");
    _data_combo = f.addCombo("Data set");
    _peak_combo = f.addCombo("Peak collection");

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeAutoIndexer::updatePeakList);

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeAutoIndexer::refreshPeakTable);

    _left_layout->addWidget(input_box);
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

    _str_min->setMaximum(100);
    _str_min->setDecimals(2);

    _str_max->setMaximum(10000000);
    _str_max->setDecimals(2);

    _gruber->setMaximum(100000);
    _gruber->setDecimals(6);

    _niggli->setMaximum(100000);
    _niggli->setDecimals(6);

    _max_cell_dimension->setMaximum(100000);
    _max_cell_dimension->setDecimals(2);

    _number_vertices->setMaximum(10000);

    _number_solutions->setMaximum(100000);

    _min_cell_volume->setMaximum(100000);
    _min_cell_volume->setDecimals(2);

    _indexing_tolerance->setMaximum(100000);
    _indexing_tolerance->setDecimals(6);

    _frequency_tolerance->setMaximum(1);
    _frequency_tolerance->setDecimals(3);

    _left_layout->addWidget(para_box);
}

void SubframeAutoIndexer::setProceedUp()
{
    QPushButton* solve_button = new QPushButton("Find unit cells");
    _left_layout->addWidget(solve_button);

    QPushButton* save_button = new QPushButton("Assign selected unit cell");
    _left_layout->addWidget(save_button);

    connect(solve_button, &QPushButton::clicked, this, &SubframeAutoIndexer::runAutoIndexer);
    connect(save_button, &QPushButton::clicked, this, &SubframeAutoIndexer::acceptSolution);
}

void SubframeAutoIndexer::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(_size_policy_right);

    _peak_table = new PeakTableView(this);
    _peak_collection_model.setRoot(&_peak_collection_item);
    _peak_table->setModel(&_peak_collection_model);

    peak_grid->addWidget(_peak_table, 0, 0, 0, 0);

    _right_element->addWidget(peak_group);
}

void SubframeAutoIndexer::setSolutionTableUp()
{
    QGroupBox* solution_group = new QGroupBox("Solutions");
    QGridLayout* solution_grid = new QGridLayout(solution_group);

    solution_group->setSizePolicy(_size_policy_right);

    _solution_table = new UnitCellTableView(this);

    solution_grid->addWidget(_solution_table, 0, 0, 0, 0);

    connect(
        _solution_table->verticalHeader(), &QHeaderView::sectionClicked, this,
        &SubframeAutoIndexer::selectSolutionHeader);

    connect(
        _solution_table, &UnitCellTableView::clicked, this,
        &SubframeAutoIndexer::selectSolutionTable);

    _right_element->addWidget(solution_group);
}

void SubframeAutoIndexer::refreshAll()
{
    setExperiments();
    const auto dataset =
        gSession->experimentAt(_exp_combo->currentIndex())->getData(_data_combo->currentIndex());
    _max_frame->setMaximum(dataset->nFrames() - 1);
}

void SubframeAutoIndexer::setExperiments()
{
    _exp_combo->blockSignals(true);
    _exp_combo->clear();

    if (gSession->experimentNames().empty())
        return;

    for (const QString& exp : gSession->experimentNames())
        _exp_combo->addItem(exp);

    _exp_combo->blockSignals(false);

    updatePeakList();
    updateDatasetList();
    grabIndexerParameters();
}

void SubframeAutoIndexer::updateDatasetList()
{
    _data_combo->blockSignals(true);
    _data_combo->clear();

    auto data_list = gSession->experimentAt(_exp_combo->currentIndex())->allData();

    if (!data_list.empty()) {
        for (const nsx::sptrDataSet& data : data_list) {
            QFileInfo fileinfo(QString::fromStdString(data->filename()));
            _data_combo->addItem(fileinfo.baseName() /*absoluteFilePath()*/);
        }
        _data_combo->setCurrentIndex(0);
    }
    _data_combo->blockSignals(false);
}

void SubframeAutoIndexer::updatePeakList()
{
    _peak_combo->blockSignals(true);

    _peak_combo->clear();
    _peak_list = gSession->experimentAt(_exp_combo->currentIndex())->getPeakListNames();
    _peak_list.clear();

    QStringList tmp = gSession->experimentAt(_exp_combo->currentIndex())
                          ->getPeakCollectionNames(nsx::listtype::FOUND);
    _peak_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_combo->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::FILTERED);
    _peak_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_combo->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::INDEXING);
    _peak_list.append(tmp);

    if (!_peak_list.empty()) {
        _peak_combo->addItems(_peak_list);
        _peak_combo->setCurrentIndex(0);

        refreshPeakTable();
        _solution_table->setModel(nullptr);
        _solutions.clear();
        _selected_unit_cell = nullptr;
    }
    _peak_combo->blockSignals(false);
}

void SubframeAutoIndexer::refreshPeakTable()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    const nsx::PeakCollection* collection =
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());
    _peak_collection_item.setPeakCollection(collection);
    _peak_collection_model.setRoot(&_peak_collection_item);
}

void SubframeAutoIndexer::grabIndexerParameters()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    _params = gSession->experimentAt(_exp_combo->currentIndex())->experiment()->indexer_params;
    _min_frame->setValue(_params.first_frame);
    _max_frame->setValue(_params.last_frame);
    _d_min->setValue(_params.d_min);
    _d_max->setValue(_params.d_max);
    _str_min->setValue(_params.strength_min);
    _str_max->setValue(_params.strength_max);
    _niggli->setValue(_params.niggliTolerance);
    _only_niggli->setChecked(_params.niggliReduction);
    _gruber->setValue(_params.gruberTolerance);
    _number_vertices->setValue(_params.nVertices);
    _number_subdivisions->setValue(_params.subdiv);
    _number_solutions->setValue(_params.nSolutions);
    _max_cell_dimension->setValue(_params.maxdim);
    _indexing_tolerance->setValue(_params.indexingTolerance);
    _frequency_tolerance->setValue(_params.frequencyTolerance);
    _min_cell_volume->setValue(_params.minUnitCellVolume);
}

void SubframeAutoIndexer::setIndexerParameters()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    _params.first_frame = _min_frame->value();
    _params.last_frame = _max_frame->value();
    _params.d_min = _d_min->value();
    _params.d_max = _d_max->value();
    _params.strength_min = _str_min->value();
    _params.strength_max = _str_max->value();
    _params.niggliTolerance = _niggli->value();
    _params.niggliReduction = _only_niggli->isChecked();
    _params.gruberTolerance = _gruber->value();
    _params.nVertices = _number_vertices->value();
    _params.subdiv = _number_subdivisions->value();
    _params.nSolutions = _number_solutions->value();
    _params.maxdim = _max_cell_dimension->value();
    _params.indexingTolerance = _indexing_tolerance->value();
    _params.frequencyTolerance = _frequency_tolerance->value();
    _params.minUnitCellVolume = _min_cell_volume->value();
}

void SubframeAutoIndexer::runAutoIndexer()
{
    if (_peak_list.empty() || _exp_combo->count() < 1) {
        QMessageBox::critical(this, "Error", "No peaks or experiments defined.");
        return;
    }

    setIndexerParameters();

    nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
    nsx::AutoIndexer* auto_indexer = expt->autoIndexer();
    nsx::PeakCollection* collection =
        expt->getPeakCollection(_peak_combo->currentText().toStdString());

    std::shared_ptr<nsx::ProgressHandler> handler(new nsx::ProgressHandler());
    auto_indexer->setHandler(handler);

    _solutions.clear();

    try {
        expt->autoIndex(collection, _params);
    } catch (const std::exception& e) {
        nsx::nsxlog(nsx::Level::Error, "Autoindexer:", e.what());
        QMessageBox::critical(this, "Indexing Error", e.what());
        return;
    }

    _solutions = auto_indexer->solutions();
    if (_solutions.empty())
        QMessageBox::warning(this, "Warning", "No solution found.");

    buildSolutionsTable();

    auto_indexer->unsetHandler();
    handler.reset();
}

void SubframeAutoIndexer::buildSolutionsTable()
{
    // Create table with 9 columns
    QStandardItemModel* model = new QStandardItemModel(_solutions.size(), 10, this);
    model->setHorizontalHeaderItem(0, new QStandardItem("a"));
    model->setHorizontalHeaderItem(1, new QStandardItem("b"));
    model->setHorizontalHeaderItem(2, new QStandardItem("c"));
    model->setHorizontalHeaderItem(3, new QStandardItem(QString((QChar)0x03B1)));
    model->setHorizontalHeaderItem(4, new QStandardItem(QString((QChar)0x03B2)));
    model->setHorizontalHeaderItem(5, new QStandardItem(QString((QChar)0x03B3)));
    model->setHorizontalHeaderItem(6, new QStandardItem("Volume"));
    model->setHorizontalHeaderItem(7, new QStandardItem("Bravais type"));
    model->setHorizontalHeaderItem(8, new QStandardItem("Quality"));

    // Display solutions
    for (unsigned int i = 0; i < _solutions.size(); ++i) {
        const nsx::sptrUnitCell cell = _solutions[i].first;
        const double quality = _solutions[i].second;

        const nsx::UnitCellCharacter ch = cell->character();
        const nsx::UnitCellCharacter sigma = cell->characterSigmas();

        QStandardItem* col1 = new QStandardItem(
            QString::number(ch.a, 'f', 3) + "(" + QString::number(sigma.a * 1000, 'f', 0) + ")");
        QStandardItem* col2 = new QStandardItem(
            QString::number(ch.b, 'f', 3) + "(" + QString::number(sigma.b * 1000, 'f', 0) + ")");
        QStandardItem* col3 = new QStandardItem(
            QString::number(ch.c, 'f', 3) + "(" + QString::number(sigma.c * 1000, 'f', 0) + ")");
        QStandardItem* col4 = new QStandardItem(
            QString::number(ch.alpha / nsx::deg, 'f', 3) + "("
            + QString::number(sigma.alpha / nsx::deg * 1000, 'f', 0) + ")");
        QStandardItem* col5 = new QStandardItem(
            QString::number(ch.beta / nsx::deg, 'f', 3) + "("
            + QString::number(sigma.beta / nsx::deg * 1000, 'f', 0) + ")");
        QStandardItem* col6 = new QStandardItem(
            QString::number(ch.gamma / nsx::deg, 'f', 3) + "("
            + QString::number(sigma.gamma / nsx::deg * 1000, 'f', 0) + ")");
        QStandardItem* col7 = new QStandardItem(QString::number(cell->volume(), 'f', 3));
        QStandardItem* col8 = new QStandardItem(QString::fromStdString(cell->bravaisTypeSymbol()));
        QStandardItem* col9 = new QStandardItem(QString::number(quality, 'f', 2) + "%");

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
        gSession->experimentAt(_exp_combo->currentIndex())
            ->experiment()
            ->getPeakCollection(_peak_combo->currentText().toStdString());

    std::vector<nsx::Peak3D*> peaks = collection->getPeakList();
    for (nsx::Peak3D* peak : peaks)
        peak->setUnitCell(_selected_unit_cell.get());
    refreshPeakTable();
}

void SubframeAutoIndexer::acceptSolution()
{
    if (_peak_list.empty() || _exp_combo->count() < 1)
        return;

    if (_selected_unit_cell) {
        nsx::Experiment* expt = gSession->experimentAt(_exp_combo->currentIndex())->experiment();
        QStringList collections = gSession->experimentAt(_exp_combo->currentIndex())
                                      ->getPeakCollectionNames(nsx::listtype::FOUND);
        QStringList space_groups;
        for (const std::string& name : _selected_unit_cell->compatibleSpaceGroups())
            space_groups.push_back(QString::fromStdString(name));
        std::unique_ptr<UnitCellDialog> dlg(new UnitCellDialog(collections, space_groups));
        dlg->exec();
        if (!dlg->unitCellName().isEmpty()) {
            std::string cellName = dlg->unitCellName().toStdString();
            _selected_unit_cell->setName(cellName);
            expt->addUnitCell(dlg->unitCellName().toStdString(), *_selected_unit_cell.get());
            gSession->onUnitCellChanged();

            nsx::PeakCollection* collection =
                expt->getPeakCollection(dlg->peakCollectionName().toStdString());
            expt->assignUnitCell(collection, cellName);
            nsx::UnitCell* cell = expt->getUnitCell(cellName);
            cell->setSpaceGroup(dlg->spaceGroup().toStdString());
        }
    }
}
