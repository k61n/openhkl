//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_merge/SubframeMerge.cpp
//! @brief     Implements class SubframeMerge
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_merge/SubframeMerge.h"

#include "core/algo/Refiner.h"
#include "core/data/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeak.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/PeakMerger.h"
#include "gui/MainWin.h" // gGui
#include "gui/dialogs/PeakExportDialog.h"
#include "gui/graphics/SXPlot.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/IntegratedPeakComboBox.h"
#include "gui/utility/SideBar.h"
#include "tables/crystal/UnitCell.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableView>
#include <QSettings>
#include <QStandardItemModel>
#include <QVBoxLayout>


SubframeMerge::SubframeMerge()
{
    _exporter = {};
    setSizePolicies();

    _frame_set = false;

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* drop_layout = new QHBoxLayout();

    _peak_combo_1 = new IntegratedPeakComboBox();
    _peak_combo_2 = new IntegratedPeakComboBox();
    _peak_combo_2->setEmptyFirst();

    _peak_combo_1->setSizePolicy(*_size_policy_right);
    _peak_combo_2->setSizePolicy(*_size_policy_right);

    drop_layout->addWidget(new QLabel("Peak collection 1:"));
    drop_layout->addWidget(_peak_combo_1);
    drop_layout->addWidget(new QLabel("Peak collection 2:"));
    drop_layout->addWidget(_peak_combo_2);
    drop_layout->addStretch();
    layout->addLayout(drop_layout);

    _main_tab_widget = new QTabWidget();

    _shell_tab = new QWidget();
    _merged_tab = new QWidget();
    _unmerged_tab = new QWidget();

    setDShellUp();
    setMergedUp();
    setUnmergedUp();
    toggleUnsafeWidgets();

    _main_tab_widget->addTab(_shell_tab, "Resolution shell statistics");
    _main_tab_widget->addTab(_merged_tab, "Merged peaks");
    _main_tab_widget->addTab(_unmerged_tab, "Unmerged peaks");

    layout->addWidget(_main_tab_widget);

    connect(
        _peak_combo_1, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMerge::processMerge);
    connect(
        _peak_combo_2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMerge::processMerge);
}

void SubframeMerge::grabMergeParameters()
{
    QSignalBlocker blocker1(_d_min);
    QSignalBlocker blocker2(_d_max);
    QSignalBlocker blocker3(_frame_min);
    QSignalBlocker blocker4(_frame_max);
    QSignalBlocker blocker5(_d_shells);
    QSignalBlocker blocker6(_friedel);

    auto params = gSession->currentProject()->experiment()->peakMerger()->parameters();

    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
    _frame_min->setValue(params->frame_min + 1);
    _frame_max->setValue(params->frame_max + 1);
    _d_shells->setValue(params->n_shells);
    _friedel->setChecked(params->friedel);
}

void SubframeMerge::setMergeParameters()
{
    if (!gSession->hasProject())
        return;

    auto params = gSession->currentProject()->experiment()->peakMerger()->parameters();

    params->d_min = _d_min->value();
    params->d_max = _d_max->value();
    params->frame_min = _frame_min->value() - 1;
    params->frame_max = _frame_max->value() - 1;
    params->n_shells = _d_shells->value();
    params->friedel = _friedel->isChecked();
}

void SubframeMerge::setSizePolicies()
{
    _size_policy_widgets = new QSizePolicy();
    _size_policy_widgets->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_widgets->setVerticalPolicy(QSizePolicy::Fixed);

    _size_policy_box = new QSizePolicy();
    _size_policy_box->setHorizontalPolicy(QSizePolicy::Preferred);
    _size_policy_box->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_right = new QSizePolicy();
    _size_policy_right->setHorizontalPolicy(QSizePolicy::Expanding);
    _size_policy_right->setVerticalPolicy(QSizePolicy::Preferred);

    _size_policy_fixed = new QSizePolicy();
    _size_policy_fixed->setHorizontalPolicy(QSizePolicy::Fixed);
    _size_policy_fixed->setVerticalPolicy(QSizePolicy::Fixed);
}

void SubframeMerge::setDShellUp()
{
    QVBoxLayout* shell_layout = new QVBoxLayout();
    _shell_tab->setLayout(shell_layout);
    _statistics_tab_widget = new QTabWidget();

    QWidget* sum_tab = new QWidget();
    QWidget* profile_tab = new QWidget();
    QVBoxLayout* sum_layout = new QVBoxLayout();
    QVBoxLayout* profile_layout = new QVBoxLayout();
    sum_tab->setLayout(sum_layout);
    profile_tab->setLayout(profile_layout);
    _statistics_tab_widget->addTab(sum_tab, "Sum intensities");
    _statistics_tab_widget->addTab(profile_tab, "Profile intensities");


    _sum_shell_view = new QTableView;
    _sum_shell_model = new QStandardItemModel(0, 13, this);
    _sum_shell_view->setModel(_sum_shell_model);
    _sum_shell_model->setHorizontalHeaderLabels(
        {"Max. d", "Min. d", "Num. peaks observed", "Num. merged peaks", "Redundancy", "Rmeas",
         "Rmeas(est.)", "Rmerge/Rsym", "Rmerge(est.)", "Rpim", "Rpim(est.)", "CChalf", "CC*",
         "Completeness"});
    _profile_shell_view = new QTableView;
    _profile_shell_model = new QStandardItemModel(0, 13, this);
    _profile_shell_view->setModel(_profile_shell_model);
    _profile_shell_model->setHorizontalHeaderLabels(
        {"Max. d", "Min. d", "Num. peaks observed", "Num. merged peaks", "Redundancy", "Rmeas",
         "Rmeas(est.)", "Rmerge/Rsym", "Rmerge(est.)", "Rpim", "Rpim(est.)", "CChalf", "CC*",
         "Completeness"});

    sum_layout->addWidget(_sum_shell_view);
    profile_layout->addWidget(_profile_shell_view);
    _sum_shell_view->resizeColumnsToContents();
    _profile_shell_view->resizeColumnsToContents();

    QHBoxLayout* d_shell_down = new QHBoxLayout;
    QGridLayout* d_shell_down_left = new QGridLayout;

    QLabel* label_ptr;

    label_ptr = new QLabel("Integration type:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 0, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Resolution (d) range:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Image range:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Num. resolution shells:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Space group:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Plot y axis:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 6, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _d_min = new QDoubleSpinBox();
    _d_max = new QDoubleSpinBox();
    _frame_min = new QSpinBox();
    _frame_max = new QSpinBox();
    _d_shells = new QSpinBox();
    _friedel = new QCheckBox("Include friedel");
    _space_group = new QComboBox();
    _plottable_statistics = new QComboBox();
    _save_shell = new QPushButton("Save statistics");
    _save_peaks = new QPushButton("Export peaks");

    _d_min->setValue(1.5);
    _d_min->setSingleStep(0.1);
    _d_min->setMaximum(100);
    _d_max->setValue(50);
    _d_max->setSingleStep(0.1);
    _d_max->setMaximum(100);
    _frame_min->setMinimum(1);
    _frame_min->setMaximum(1000);
    _frame_max->setMinimum(1);
    _frame_max->setMaximum(1000);
    _d_shells->setValue(10);
    _friedel->setChecked(true);

    _d_min->setSizePolicy(*_size_policy_widgets);
    _d_max->setSizePolicy(*_size_policy_widgets);
    _frame_min->setSizePolicy(*_size_policy_widgets);
    _frame_max->setSizePolicy(*_size_policy_widgets);
    _d_shells->setSizePolicy(*_size_policy_widgets);
    _friedel->setSizePolicy(*_size_policy_widgets);
    _space_group->setSizePolicy(*_size_policy_widgets);
    _plottable_statistics->setSizePolicy(*_size_policy_widgets);
    _save_shell->setSizePolicy(*_size_policy_widgets);

    QStringList selection_stats;
    for (int i = 0; i < _sum_shell_model->columnCount(); ++i) {
        QStandardItem* header_item = _sum_shell_model->horizontalHeaderItem(i);
        selection_stats.push_back(header_item->text());
    }
    _plottable_statistics->addItems(selection_stats);
    _plottable_statistics->setCurrentIndex(7);

    d_shell_down_left->addWidget(_d_min, 1, 1, 1, 1);
    d_shell_down_left->addWidget(_d_max, 1, 2, 1, 1);
    d_shell_down_left->addWidget(_frame_min, 2, 1, 1, 1);
    d_shell_down_left->addWidget(_frame_max, 2, 2, 1, 1);
    d_shell_down_left->addWidget(_d_shells, 3, 1, 1, 2);
    d_shell_down_left->addWidget(_space_group, 4, 1, 1, 2);
    d_shell_down_left->addWidget(_friedel, 5, 1, 1, 2);
    d_shell_down_left->addWidget(_plottable_statistics, 6, 1, 1, 3);
    d_shell_down_left->addWidget(_save_shell, 7, 0, 1, 3);
    d_shell_down_left->addWidget(_save_peaks, 8, 0, 1, 3);
    d_shell_down->addLayout(d_shell_down_left);

    _statistics_plot = new SXPlot;
    _statistics_plot->setSizePolicy(*_size_policy_right);
    d_shell_down->addWidget(_statistics_plot);

    connect(
        _d_min, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeMerge::processMerge);

    connect(
        _d_max, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeMerge::processMerge);

    connect(
        _frame_min, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeMerge::processMerge);

    connect(
        _frame_max, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeMerge::processMerge);

    connect(
        _d_shells, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeMerge::processMerge);

    connect(_friedel, &QCheckBox::clicked, this, &SubframeMerge::processMerge);

    connect(
        _plottable_statistics,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMerge::refreshGraph);

    connect(_save_shell, &QPushButton::clicked, this, &SubframeMerge::saveStatistics);

    connect(_save_peaks, &QPushButton::clicked, this, &SubframeMerge::saveMergedPeaks);

    QWidget* down_widget = new QWidget();
    down_widget->setLayout(d_shell_down);

    shell_layout->addWidget(_statistics_tab_widget);
    shell_layout->addWidget(down_widget);
}

void SubframeMerge::setMergedUp()
{
    QVBoxLayout* merged_layout = new QVBoxLayout();
    _merged_tab->setLayout(merged_layout);
    _merged_tab_widget = new QTabWidget();

    QWidget* sum_tab = new QWidget();
    QWidget* profile_tab = new QWidget();
    QVBoxLayout* sum_layout = new QVBoxLayout();
    QVBoxLayout* profile_layout = new QVBoxLayout();
    sum_tab->setLayout(sum_layout);
    profile_tab->setLayout(profile_layout);
    _merged_tab_widget->addTab(sum_tab, "Sum intensities");
    _merged_tab_widget->addTab(profile_tab, "Profile intensities");

    _sum_merged_view = new QTableView;
    _sum_merged_model = new QStandardItemModel(0, 8, this);
    _sum_merged_view->setModel(_sum_merged_model);

    _profile_merged_view = new QTableView;
    _profile_merged_model = new QStandardItemModel(0, 8, this);
    _profile_merged_view->setModel(_profile_merged_model);

    QString sigma_header{QString((QChar)0x03C3 + QString{"(I)"})};
    QString chi_header{QString((QChar)0x03C7 + QString{"2"})};
    _sum_merged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "nobs", chi_header, "p"});
    _profile_merged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "nobs", chi_header, "p"});
    _sum_merged_view->resizeColumnsToContents();
    _profile_merged_view->resizeColumnsToContents();
    sum_layout->addWidget(_sum_merged_view);
    profile_layout->addWidget(_profile_merged_view);

    QHBoxLayout* merged_row = new QHBoxLayout;

    _save_merged = new QPushButton("Export merged peaks");

    merged_row->addStretch();
    merged_row->addWidget(_save_merged);

    QWidget* lower_widget = new QWidget();
    lower_widget->setLayout(merged_row);

    merged_layout->addWidget(_merged_tab_widget);
    merged_layout->addWidget(lower_widget);

    connect(_save_merged, &QPushButton::clicked, this, &SubframeMerge::saveMergedPeaks);
}

void SubframeMerge::setUnmergedUp()
{
    QVBoxLayout* unmerged_layout = new QVBoxLayout(_unmerged_tab);
    _unmerged_tab->setLayout(unmerged_layout);
    _unmerged_tab_widget = new QTabWidget();

    QWidget* sum_tab = new QWidget();
    QWidget* profile_tab = new QWidget();
    QVBoxLayout* sum_layout = new QVBoxLayout();
    QVBoxLayout* profile_layout = new QVBoxLayout();
    sum_tab->setLayout(sum_layout);
    profile_tab->setLayout(profile_layout);
    _unmerged_tab_widget->addTab(sum_tab, "Sum intensities");
    _unmerged_tab_widget->addTab(profile_tab, "Profile intensities");

    _sum_unmerged_view = new QTableView;
    _sum_unmerged_model = new QStandardItemModel(0, 8, this);
    _sum_unmerged_view->setModel(_sum_unmerged_model);

    _profile_unmerged_view = new QTableView;
    _profile_unmerged_model = new QStandardItemModel(0, 8, this);
    _profile_unmerged_view->setModel(_profile_unmerged_model);

    QString sigma_header{QString((QChar)0x03C3 + QString{"(I)"})};
    _sum_unmerged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "x", "y", "frame"});
    _profile_unmerged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "x", "y", "frame"});
    _sum_unmerged_view->resizeColumnsToContents();
    _profile_unmerged_view->resizeColumnsToContents();
    sum_layout->addWidget(_sum_unmerged_view);
    profile_layout->addWidget(_profile_unmerged_view);

    QHBoxLayout* unmerged_row = new QHBoxLayout;
    _save_unmerged = new QPushButton("Export unmerged peaks");

    unmerged_row->addStretch();
    unmerged_row->addWidget(_save_unmerged);

    QWidget* lower_widget = new QWidget();
    lower_widget->setLayout(unmerged_row);

    unmerged_layout->addWidget(_unmerged_tab_widget);
    unmerged_layout->addWidget(lower_widget);

    connect(_save_unmerged, &QPushButton::clicked, this, &SubframeMerge::saveUnmergedPeaks);
}

void SubframeMerge::refreshAll()
{
    if (!gSession->hasProject())
        return;

    refreshSpaceGroupCombo();
    refreshPeakLists();
    grabMergeParameters();
    toggleUnsafeWidgets();
}

void SubframeMerge::refreshPeakLists()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;
    refreshPeakCombos();
    processMerge();
    _sum_shell_view->resizeColumnsToContents();
    _profile_shell_view->resizeColumnsToContents();
    _sum_merged_view->resizeColumnsToContents();
    _profile_merged_view->resizeColumnsToContents();
    _sum_unmerged_view->resizeColumnsToContents();
    _profile_unmerged_view->resizeColumnsToContents();
}

void SubframeMerge::refreshPeakCombos()
{
    QSignalBlocker blocker1(_peak_combo_1);
    QSignalBlocker blocker2(_peak_combo_2);
    QSignalBlocker blocker3(_frame_min);
    QSignalBlocker blocker4(_frame_max);
    if (!gSession->hasProject())
        return;

    gSession->onPeaksChanged();

    _peak_combo_1->refresh();
    _peak_combo_2->refresh();

    // Determine the maximum frame number for the frame spinboxes
    auto* peaks1 = _peak_combo_1->currentPeakCollection();

    int max_frames = peaks1->getPeakList()[0]->dataSet()->nFrames();
    if (_peak_combo_2->currentText() != QString()) {
        auto* peaks2 = _peak_combo_2->currentPeakCollection();
        if (peaks2->getPeakList()[0]->dataSet()->nFrames() > max_frames)
            max_frames = peaks2->getPeakList()[0]->dataSet()->nFrames();
    }

    _frame_min->setMaximum(max_frames);
    _frame_max->setMaximum(max_frames);
    if (!_frame_set) { // only set the values the first time
        _frame_min->setValue(1);
        _frame_max->setValue(max_frames);
        _frame_set = true;
    }
}

void SubframeMerge::refreshSpaceGroupCombo()
{
    if (!gSession->hasProject())
        return;

    if (!gSession->currentProject()->hasUnitCell())
        return;

    QSignalBlocker blocker(_space_group);
    auto* expt = gSession->currentProject()->experiment();

    if (!gSession->currentProject()->hasUnitCell())
        return;

    std::vector<ohkl::UnitCell*> cells = expt->getUnitCells();
    std::map<std::string, int> space_groups;
    for (auto* cell : cells) {
        if (space_groups.find(cell->spaceGroup().toString()) != space_groups.end())
            ++space_groups[cell->spaceGroup().toString()];
        else
            space_groups[cell->spaceGroup().toString()] = 0;
    }
    std::vector<std::pair<std::string, int>> vec;
    for (const auto& item : space_groups)
        vec.emplace_back(item);
    std::sort(vec.begin(), vec.end(), [](const auto& x, const auto& y) {
        return x.second > y.second;
    });
    _space_group->clear(); // clear first?
    for (const auto& [key, value] : vec)
        _space_group->addItem(QString::fromStdString(key));
    _space_group->setCurrentIndex(0);
}

void SubframeMerge::processMerge()
{
    if (!gSession->hasProject())
        return;

    gGui->setReady(false);
    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();
    merger->reset();
    setMergeParameters();

    if (!gSession->currentProject()->hasPeakCollection()) {
        _sum_merged_data = nullptr;
        _profile_merged_data = nullptr;
    } else {
        std::vector<ohkl::PeakCollection*> peak_collections;
        QString collection1 = _peak_combo_1->currentText();
        QString collection2 = _peak_combo_2->currentText();
        if (_space_group->currentText().toStdString().empty()) {
            return;
        }
        ohkl::SpaceGroup group = {_space_group->currentText().toStdString()};
        merger->setSpaceGroup(group);

        merger->addPeakCollection(expt->getPeakCollection(collection1.toStdString()));
        if (!collection2.isEmpty())
            merger->addPeakCollection(expt->getPeakCollection(collection2.toStdString()));

        merger->mergePeaks();
        _sum_merged_data = merger->sumMergedPeakCollection();
        _sum_merged_data_per_shell = merger->sumMergedPeakCollectionPerShell();
        _profile_merged_data = merger->profileMergedPeakCollection();
        _profile_merged_data_per_shell = merger->profileMergedPeakCollectionPerShell();
    }
    refreshTables();
    toggleUnsafeWidgets();
    gGui->setReady(true);
}

void SubframeMerge::refreshTables()
{
    refreshDShellTable();
    refreshMergedTable();
    refreshUnmergedTable();
    _sum_shell_view->resizeColumnsToContents();
    _profile_shell_view->resizeColumnsToContents();
    _sum_merged_view->resizeColumnsToContents();
    _profile_merged_view->resizeColumnsToContents();
    _sum_unmerged_view->resizeColumnsToContents();
    _profile_unmerged_view->resizeColumnsToContents();
    refreshGraph(_plottable_statistics->currentIndex());
}

void SubframeMerge::refreshDShellTable()
{
    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();
    auto* merged_data = merger->sumMergedPeakCollection();

    if (merged_data == nullptr)
        return;

    if (merged_data->totalSize() == 0)
        return;

    merger->computeQuality();
    updateShellModel(_sum_shell_model, merger->sumShellQuality(), merger->sumOverallQuality());
    updateShellModel(
        _profile_shell_model, merger->profileShellQuality(), merger->profileOverallQuality());
}

void SubframeMerge::updateShellModel(
    QStandardItemModel* model, ohkl::DataResolution* resolution, ohkl::DataResolution* overall)
{
    model->removeRows(0, model->rowCount());
    for (auto shell : resolution->shells) {
        QList<QStandardItem*> row;
        row.push_back(new QStandardItem(QString::number(shell.dmax)));
        row.push_back(new QStandardItem(QString::number(shell.dmin)));
        row.push_back(new QStandardItem(QString::number(shell.nobserved)));
        row.push_back(new QStandardItem(QString::number(shell.nunique)));
        row.push_back(new QStandardItem(QString::number(shell.redundancy)));
        row.push_back(new QStandardItem(QString::number(shell.Rmeas)));
        row.push_back(new QStandardItem(QString::number(shell.expectedRmeas)));
        row.push_back(new QStandardItem(QString::number(shell.Rmerge)));
        row.push_back(new QStandardItem(QString::number(shell.expectedRmerge)));
        row.push_back(new QStandardItem(QString::number(shell.Rpim)));
        row.push_back(new QStandardItem(QString::number(shell.expectedRpim)));
        row.push_back(new QStandardItem(QString::number(shell.CChalf)));
        row.push_back(new QStandardItem(QString::number(shell.CCstar)));
        row.push_back(new QStandardItem(QString::number(shell.Completeness)));

        model->appendRow(row);
    }

    QList<QStandardItem*> row;
    for (auto shell : overall->shells) {
        row.push_back(new QStandardItem(QString::number(shell.dmax)));
        row.push_back(new QStandardItem(QString::number(shell.dmin)));
        row.push_back(new QStandardItem(QString::number(shell.nobserved)));
        row.push_back(new QStandardItem(QString::number(shell.nunique)));
        row.push_back(new QStandardItem(QString::number(shell.redundancy)));
        row.push_back(new QStandardItem(QString::number(shell.Rmeas)));
        row.push_back(new QStandardItem(QString::number(shell.expectedRmeas)));
        row.push_back(new QStandardItem(QString::number(shell.Rmerge)));
        row.push_back(new QStandardItem(QString::number(shell.expectedRmerge)));
        row.push_back(new QStandardItem(QString::number(shell.Rpim)));
        row.push_back(new QStandardItem(QString::number(shell.expectedRpim)));
        row.push_back(new QStandardItem(QString::number(shell.CChalf)));
        row.push_back(new QStandardItem(QString::number(shell.CCstar)));
        row.push_back(new QStandardItem(QString::number(shell.Completeness)));
    }
    for (auto v : row) {
        QFont font(v->font());
        font.setBold(true);
        v->setFont(font);
    }
    model->appendRow(row);
    _sum_shell_view->resizeColumnsToContents();
    _profile_shell_view->resizeColumnsToContents();
}

void SubframeMerge::refreshMergedTable()
{
    _sum_merged_model->removeRows(0, _sum_merged_model->rowCount());
    _profile_merged_model->removeRows(0, _profile_merged_model->rowCount());

    if (_sum_merged_data == nullptr)
        return;

    updateMergedModel(_sum_merged_model, _sum_merged_data);
    updateMergedModel(_profile_merged_model, _profile_merged_data);
}

void SubframeMerge::updateMergedModel(
    QStandardItemModel* model, ohkl::MergedPeakCollection* merged_data)
{
    for (const ohkl::MergedPeak& peak : merged_data->mergedPeakSet()) {

        const auto hkl = peak.index();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        const ohkl::Intensity I = peak.intensity();
        const double chi2 = peak.chi2();
        const double p = peak.chi2();

        const double intensity = I.value();
        const double sigma = I.sigma();
        const int nobs = peak.redundancy();

        QList<QStandardItem*> row;
        row.push_back(new QStandardItem(QString::number(h)));
        row.push_back(new QStandardItem(QString::number(k)));
        row.push_back(new QStandardItem(QString::number(l)));
        row.push_back(new QStandardItem(QString::number(intensity)));
        row.push_back(new QStandardItem(QString::number(sigma)));
        row.push_back(new QStandardItem(QString::number(nobs)));
        row.push_back(new QStandardItem(QString::number(chi2)));
        row.push_back(new QStandardItem(QString::number(p)));

        model->appendRow(row);
    }
}

void SubframeMerge::refreshUnmergedTable()
{
    _sum_unmerged_model->removeRows(0, _sum_unmerged_model->rowCount());
    _profile_unmerged_model->removeRows(0, _profile_unmerged_model->rowCount());

    if (_sum_merged_data == nullptr)
        return;

    updateUnmergedModel(_sum_unmerged_model, _sum_merged_data, true);
    updateUnmergedModel(_profile_unmerged_model, _profile_merged_data, false);
}

void SubframeMerge::updateUnmergedModel(
    QStandardItemModel* model, ohkl::MergedPeakCollection* merged_data, bool sum_intensity)
{
    for (const ohkl::MergedPeak& peak : merged_data->mergedPeakSet()) {
        for (auto unmerged_peak : peak.peaks()) {
            const ohkl::UnitCell& cell = *(unmerged_peak->unitCell());
            const ohkl::ReciprocalVector& q = unmerged_peak->q();

            const ohkl::MillerIndex hkl(q, cell);

            const int h = hkl[0];
            const int k = hkl[1];
            const int l = hkl[2];

            const Eigen::Vector3d& c = unmerged_peak->shape().center();
            ohkl::Intensity I;
            if (sum_intensity)
                I = unmerged_peak->correctedSumIntensity();
            else
                I = unmerged_peak->correctedProfileIntensity();

            const double intensity = I.value();
            const double sigma = I.sigma();

            QList<QStandardItem*> row;
            row.push_back(new QStandardItem(QString::number(h)));
            row.push_back(new QStandardItem(QString::number(k)));
            row.push_back(new QStandardItem(QString::number(l)));
            row.push_back(new QStandardItem(QString::number(intensity)));
            row.push_back(new QStandardItem(QString::number(sigma)));
            row.push_back(new QStandardItem(QString::number(c[0])));
            row.push_back(new QStandardItem(QString::number(c[1])));
            row.push_back(new QStandardItem(QString::number(c[2])));

            model->appendRow(row);
        }
    }
}

void SubframeMerge::refreshGraph(int column)
{
    _statistics_plot->clearGraphs();

    if (_sum_merged_data == nullptr)
        return;

    if (_sum_merged_data->totalSize() == 0)
        return;

    int nshells = _sum_shell_model->rowCount() - 1;

    QVector<double> xvals;
    QVector<double> sum_yvals;
    QVector<double> profile_yvals;
    for (int i = 0; i < nshells; ++i) {
        xvals.push_back(double(i));
        double sum_val = _sum_shell_model->item(i, column)->data(Qt::DisplayRole).value<double>();
        double profile_val =
            _profile_shell_model->item(i, column)->data(Qt::DisplayRole).value<double>();
        sum_yvals.push_back(sum_val);
        profile_yvals.push_back(profile_val);
    }

    QPen pen1, pen2, pen3;
    pen1.setColor(QColor("black"));
    pen1.setWidth(2.0);
    pen2.setColor(QColor("red"));
    pen2.setWidth(2.0);
    pen3.setColor(QColor("blue"));
    pen3.setWidth(2.0);

    _statistics_plot->addGraph();
    _statistics_plot->graph(0)->setPen(pen1);
    _statistics_plot->graph(0)->addData(xvals, sum_yvals);
    _statistics_plot->graph(0)->setName("Sum");
    _statistics_plot->addGraph();
    _statistics_plot->graph(1)->setPen(pen2);
    _statistics_plot->graph(1)->addData(xvals, profile_yvals);
    _statistics_plot->graph(1)->setName("Profile");
    _statistics_plot->legend->setVisible(true);

    _statistics_plot->xAxis->setLabel("shell");
    _statistics_plot->yAxis->setLabel(_plottable_statistics->itemText(column));
    _statistics_plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _statistics_plot->xAxis->setTickLabelFont(font);
    _statistics_plot->yAxis->setTickLabelFont(font);

    // Overall and maximum completeness
    if (column == 13) {
        auto* merger = gSession->currentProject()->experiment()->peakMerger();
        QVector<double> sum_overall_compl;
        QVector<double> profile_overall_compl;
        QVector<double> max_compl;
        for (int i = 0; i < nshells; ++i) {
            sum_overall_compl.push_back(merger->sumOverallQuality()->shells[0].Completeness);
            profile_overall_compl.push_back(
                merger->profileOverallQuality()->shells[0].Completeness);
            max_compl.push_back(merger->maxCompleteness());
        }
        std::ostringstream oss;
        oss << "Maximum completeness (" << merger->maxCompleteness() << ")";
        _statistics_plot->addGraph();
        _statistics_plot->graph(2)->setPen(pen1);
        _statistics_plot->graph(2)->addData(xvals, sum_overall_compl);
        _statistics_plot->graph(2)->setName("Sum overall completeness");
        _statistics_plot->addGraph();
        _statistics_plot->graph(3)->setPen(pen2);
        _statistics_plot->graph(3)->addData(xvals, profile_overall_compl);
        _statistics_plot->graph(3)->setName("Profile overall completeness");
        _statistics_plot->addGraph();
        _statistics_plot->graph(4)->setPen(pen3);
        _statistics_plot->graph(4)->addData(xvals, max_compl);
        _statistics_plot->graph(4)->setName(QString::fromStdString(oss.str()));
    }

    _statistics_plot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);
    _statistics_plot->rescaleAxes();
    _statistics_plot->replot();
}

void SubframeMerge::saveStatistics()
{
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("merged", QDir::homePath()).toString();

    QString filename =
        QFileDialog::getSaveFileName(this, "Save the shell info", loadDirectory, "(*.txt)");

    if (filename.isEmpty())
        return;

    QFileInfo info(filename);
    settings.setValue("merged", info.absolutePath());

    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();

    merger->saveStatistics(filename.toStdString());
}

void SubframeMerge::exportPeaks(bool merged)
{
    PeakExportDialog dlg;
    QString collection1 = _peak_combo_1->currentText();
    QString collection2 = _peak_combo_2->currentText();
    dlg.initialise(collection1, collection2, _d_min->value(), _d_max->value(), merged);
    dlg.exec();
}

void SubframeMerge::saveMergedPeaks()
{
    exportPeaks(true);
}

void SubframeMerge::saveUnmergedPeaks()
{
    exportPeaks(false);
}

void SubframeMerge::toggleUnsafeWidgets()
{
    _save_shell->setEnabled(false);
    _save_merged->setEnabled(false);
    _save_unmerged->setEnabled(false);

    if (!gSession->hasProject())
        return;

    if (_peak_combo_1->count() > 0) {
        _save_shell->setEnabled(true);
        _save_merged->setEnabled(true);
        _save_unmerged->setEnabled(true);
    }
}

ohkl::sptrUnitCell SubframeMerge::singleBatchRefine()
{
    auto expt = gSession->currentProject()->experiment();
    auto* peaks = _peak_combo_1->currentPeakCollection();
    const auto data = peaks->getPeakList()[0]->dataSet();
    auto states = data->instrumentStates();
    auto* refiner = expt->refiner();
    auto* params = refiner->parameters();

    params->nbatches = 1;
    params->max_iter = 1000;
    params->refine_ub = true;
    params->refine_sample_position = false;
    params->refine_sample_orientation = false;
    params->refine_detector_offset = false;
    params->refine_ki = false;
    params->use_batch_cells = true;
    params->set_unit_cell = false;

    try {
        expt->refine(peaks, data.get());
        gSession->onUnitCellChanged();
        toggleUnsafeWidgets();
    } catch (const std::exception& ex) {
        gGui->statusBar()->showMessage("Error: " + QString(ex.what()));
    }

    return refiner->batches()[0].sptrCell();
}
