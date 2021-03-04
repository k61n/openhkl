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
#include "gui/utility/Spoiler.h"
#include "gui/views/PeakTableView.h"
#include "gui/views/UnitCellTableView.h"

#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>

SubframeAutoIndexer::SubframeAutoIndexer()
    : QWidget()
    , _peak_collection("temp", nsx::listtype::FOUND)
    , _peak_collection_item()
    , _peak_collection_model()
{
    setSizePolicies();
    _main_layout = new QHBoxLayout(this);
    _right_element = new QSplitter(Qt::Vertical, this);

    QScrollArea* scroll_area = new QScrollArea(this);
    QWidget* scroll_widget = new QWidget();

    scroll_area->setSizePolicy(*_size_policy_box);
    scroll_widget->setSizePolicy(*_size_policy_box);
    _left_layout = new QVBoxLayout(scroll_widget);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(scroll_widget);

    setInputUp();
    setParametersUp();
    setProceedUp();
    setPeakTableUp();
    setSolutionTableUp();
    _left_layout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    _right_element->setSizePolicy(*_size_policy_right);

    _main_layout->addWidget(scroll_area);
    _main_layout->addWidget(_right_element);
}

SubframeAutoIndexer::~SubframeAutoIndexer()
{
    delete _size_policy_widgets;
    delete _size_policy_box;
    delete _size_policy_right;
    delete _size_policy_fixed;
}

void SubframeAutoIndexer::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Expanding);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void SubframeAutoIndexer::setInputUp()
{
    _input_box = new Spoiler("Input");

    QGridLayout* _input_grid = new QGridLayout();

    QLabel* exp_label = new QLabel("Experiment");
    exp_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(exp_label, 0, 0, 1, 1);

    QLabel* data_label = new QLabel("Data set");
    data_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(data_label, 1, 0, 1, 1);

    QLabel* list_label = new QLabel("Peak collection");
    list_label->setAlignment(Qt::AlignRight);
    _input_grid->addWidget(list_label, 2, 0, 1, 1);

    _exp_combo = new QComboBox();
    _data_combo = new QComboBox();
    _peak_combo = new QComboBox();

    _exp_combo->setMaximumWidth(1000);
    _data_combo->setMaximumWidth(1000);
    _peak_combo->setMaximumWidth(1000);

    _exp_combo->setSizePolicy(*_size_policy_widgets);
    _data_combo->setSizePolicy(*_size_policy_widgets);
    _peak_combo->setSizePolicy(*_size_policy_widgets);

    _input_grid->addWidget(_exp_combo, 0, 1, 1, 1);
    _input_grid->addWidget(_data_combo, 1, 1, 1, 1);
    _input_grid->addWidget(_peak_combo, 2, 1, 1, 1);

    connect(
        _exp_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeAutoIndexer::updatePeakList);

    connect(
        _peak_combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeAutoIndexer::refreshPeakTable);

    _input_box->setContentLayout(*_input_grid, true);
    _input_box->setSizePolicy(*_size_policy_box);
    _input_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_input_box);
}

void SubframeAutoIndexer::setParametersUp()
{
    _para_box = new Spoiler("Parameters");

    QGridLayout* para_grid = new QGridLayout();

    QLabel* label_ptr;
    QString tooltip;

    label_ptr = new QLabel("Frames:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 0, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = "(frames) - range of frames over which to index";
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("d range:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 1, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = QString::fromUtf8(
        "(\u212B) - only attempt to index peaks over this d (Bragg's law) range ");
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Strength:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 2, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = QString::fromUtf8("(I/\u03C3) - only attempt to index peaks in this strength range");
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Gruber tol.:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 3, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = "Tolerance for Gruber reduction";
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Niggli tol.:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 4, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = "Tolerance for Niggli reduction";
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Max. Cell dim.:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 6, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = QString::fromUtf8("(\u212B) - maximum length of any lattice vector");
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Q Vertices:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 7, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip =
        "Number of points on reciprocal space unit sphere to test against candidate lattice vector";
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Subdivisions:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 8, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = "Number of histogram bins for FFT";
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Unit Cells:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 9, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = "Number of unit cell solutions to find";
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Min. Volume:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 10, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = QString::fromUtf8("(\u212B^3) - discard candidate cells below this volume");
    label_ptr->setToolTip(tooltip);

    label_ptr = new QLabel("Indexing tol.:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 11, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Frequency tol.:");
    label_ptr->setAlignment(Qt::AlignRight);
    para_grid->addWidget(label_ptr, 12, 0, 1, 2);
    label_ptr->setSizePolicy(*_size_policy_widgets);
    tooltip = "Minimum fraction of amplitude of the zeroth Fourier frequency to accept as a candidate lattice vector";
    label_ptr->setToolTip(tooltip);

    QHBoxLayout* frames = new QHBoxLayout();
    QHBoxLayout* drange = new QHBoxLayout();
    QHBoxLayout* strength = new QHBoxLayout();

    _min_frame = new QSpinBox();
    _max_frame = new QSpinBox();
    _d_min = new QDoubleSpinBox();
    _d_max = new QDoubleSpinBox();
    _str_min = new QDoubleSpinBox();
    _str_max = new QDoubleSpinBox();
    _gruber = new QDoubleSpinBox();
    _niggli = new QDoubleSpinBox();
    _only_niggli = new QCheckBox("Find Niggli cell only");
    _max_cell_dimension = new QDoubleSpinBox();
    _number_vertices = new QSpinBox();
    _number_solutions = new QSpinBox();
    _number_subdivisions = new QSpinBox();
    _min_cell_volume = new QDoubleSpinBox();
    _indexing_tolerance = new QDoubleSpinBox();
    _frequency_tolerance = new QDoubleSpinBox();

    _min_frame->setMaximumWidth(1000);
    _min_frame->setMaximum(1000);

    _max_frame->setMaximumWidth(1000);
    _max_frame->setMaximum(1000);

    _d_min->setMaximumWidth(1000);
    _d_min->setMaximum(100);
    _d_min->setDecimals(2);

    _str_min->setMaximumWidth(1000);
    _str_min->setMaximum(100);
    _str_min->setDecimals(2);

    _str_max->setMaximumWidth(1000);
    _str_max->setMaximum(10000000);
    _str_max->setDecimals(2);

    _d_max->setMaximumWidth(1000);
    _d_max->setMaximum(100);
    _d_max->setDecimals(2);

    _gruber->setMaximumWidth(1000);
    _gruber->setMaximum(100000);
    _gruber->setDecimals(6);

    _niggli->setMaximumWidth(1000);
    _niggli->setMaximum(100000);
    _niggli->setDecimals(6);

    _only_niggli->setMaximumWidth(1000);

    _max_cell_dimension->setMaximumWidth(1000);
    _max_cell_dimension->setMaximum(100000);
    _max_cell_dimension->setDecimals(2);

    _number_vertices->setMaximumWidth(10000);
    _number_vertices->setMaximum(10000);

    _number_subdivisions->setMaximumWidth(1000);

    _number_solutions->setMaximumWidth(1000);
    _number_solutions->setMaximum(100000);

    _min_cell_volume->setMaximumWidth(1000);
    _min_cell_volume->setMaximum(100000);
    _min_cell_volume->setDecimals(2);

    _indexing_tolerance->setMaximumWidth(1000);
    _indexing_tolerance->setMaximum(100000);
    _indexing_tolerance->setDecimals(6);

    _frequency_tolerance->setMaximumWidth(1000);
    _frequency_tolerance->setMaximum(1);
    _frequency_tolerance->setDecimals(3);

    _min_frame->setSizePolicy(*_size_policy_widgets);
    _max_frame->setSizePolicy(*_size_policy_widgets);
    _d_min->setSizePolicy(*_size_policy_widgets);
    _d_max->setSizePolicy(*_size_policy_widgets);
    _str_min->setSizePolicy(*_size_policy_widgets);
    _str_max->setSizePolicy(*_size_policy_widgets);
    _gruber->setSizePolicy(*_size_policy_widgets);
    _niggli->setSizePolicy(*_size_policy_widgets);
    _only_niggli->setSizePolicy(*_size_policy_widgets);
    _max_cell_dimension->setSizePolicy(*_size_policy_widgets);
    _number_vertices->setSizePolicy(*_size_policy_widgets);
    _number_solutions->setSizePolicy(*_size_policy_widgets);
    _number_subdivisions->setSizePolicy(*_size_policy_widgets);
    _min_cell_volume->setSizePolicy(*_size_policy_widgets);
    _indexing_tolerance->setSizePolicy(*_size_policy_widgets);
    _frequency_tolerance->setSizePolicy(*_size_policy_widgets);

    frames->addWidget(_min_frame);
    frames->addWidget(_max_frame);
    drange->addWidget(_d_min);
    drange->addWidget(_d_max);
    strength->addWidget(_str_min);
    strength->addWidget(_str_max);

    para_grid->addLayout(frames, 0, 3, 1, 3);
    para_grid->addLayout(drange, 1, 3, 1, 3);
    para_grid->addLayout(strength, 2, 3, 1, 3);
    para_grid->addWidget(_gruber, 3, 3, 1, 3);
    para_grid->addWidget(_niggli, 4, 3, 1, 3);
    para_grid->addWidget(_only_niggli, 5, 3, 1, 3);
    para_grid->addWidget(_max_cell_dimension, 6, 3, 1, 3);
    para_grid->addWidget(_number_vertices, 7, 3, 1, 3);
    para_grid->addWidget(_number_subdivisions, 8, 3, 1, 3);
    para_grid->addWidget(_number_solutions, 9, 3, 1, 3);
    para_grid->addWidget(_min_cell_volume, 10, 3, 1, 3);
    para_grid->addWidget(_indexing_tolerance, 11, 3, 1, 3);
    para_grid->addWidget(_frequency_tolerance, 12, 3, 1, 3);

    _para_box->setContentLayout(*para_grid, true);
    _para_box->setSizePolicy(*_size_policy_box);
    _para_box->contentArea.setSizePolicy(*_size_policy_box);

    _left_layout->addWidget(_para_box);
}

void SubframeAutoIndexer::setProceedUp()
{
    _solve_button = new QPushButton("Find unit cells");
    _solve_button->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_solve_button);

    _save_button = new QPushButton("Assign selected unit cell");
    _save_button->setSizePolicy(*_size_policy_widgets);
    _left_layout->addWidget(_save_button);

    connect(_solve_button, &QPushButton::clicked, this, &SubframeAutoIndexer::runAutoIndexer);

    connect(_save_button, &QPushButton::clicked, this, &SubframeAutoIndexer::acceptSolution);
}

void SubframeAutoIndexer::setPeakTableUp()
{
    QGroupBox* peak_group = new QGroupBox("Peaks");
    QGridLayout* peak_grid = new QGridLayout(peak_group);

    peak_group->setSizePolicy(*_size_policy_right);

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

    solution_group->setSizePolicy(*_size_policy_right);

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

    for (QString exp : gSession->experimentNames())
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
    if (_solutions.size() == 0)
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
        for (std::string name : _selected_unit_cell->compatibleSpaceGroups())
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
