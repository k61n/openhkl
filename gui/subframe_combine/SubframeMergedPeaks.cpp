//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_combine/SubframeMergedPeaks.cpp
//! @brief     Implements class SubframeMergedPeaks
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_combine/SubframeMergedPeaks.h"

#include "core/data/DataSet.h"
#include "core/experiment/DataQuality.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/CC.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/MergedPeak.h"
#include "core/statistics/PeakMerger.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include "gui/MainWin.h" // gGui
#include "gui/graphics/SXPlot.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/LinkedComboBox.h"
#include "gui/utility/SideBar.h"
#include "tables/crystal/UnitCell.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QVBoxLayout>

#include <fstream>
#include <iomanip>
#include <qobject.h>

SubframeMergedPeaks::SubframeMergedPeaks()
{
    setSizePolicies();

    _frame_set = false;

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* drop_layout = new QHBoxLayout();

    _exp_drop = new QComboBox();
    _peaks1_drop = new LinkedComboBox(ComboType::PeakCollection, gGui->sentinel);
    _peaks2_drop = new LinkedComboBox(ComboType::PeakCollection, gGui->sentinel);

    _exp_drop->setSizePolicy(*_size_policy_right);
    _peaks1_drop->setSizePolicy(*_size_policy_right);
    _peaks2_drop->setSizePolicy(*_size_policy_right);

    drop_layout->addWidget(new QLabel("Experiment:"));
    drop_layout->addWidget(_exp_drop);
    drop_layout->addWidget(new QLabel("Peak collection 1:"));
    drop_layout->addWidget(_peaks1_drop);
    drop_layout->addWidget(new QLabel("Peak collection 2:"));
    drop_layout->addWidget(_peaks2_drop);
    drop_layout->addStretch();
    layout->addLayout(drop_layout);

    _main_tab_widget = new QTabWidget();

    _shell_tab = new QWidget();
    _merged_tab = new QWidget();
    _unmerged_tab = new QWidget();

    setDShellUp();
    setMergedUp();
    setUnmergedUp();

    _main_tab_widget->addTab(_shell_tab, "d Shell statistic");
    _main_tab_widget->addTab(_merged_tab, "Merged representation");
    _main_tab_widget->addTab(_unmerged_tab, "Unmerged representation");

    layout->addWidget(_main_tab_widget);

    connect(
        _exp_drop, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMergedPeaks::refreshPeakLists);
    connect(
        _peaks1_drop, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMergedPeaks::processMerge);
    connect(
        _peaks2_drop, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMergedPeaks::processMerge);
}

void SubframeMergedPeaks::grabMergeParameters()
{
    auto params =
        gSession->experimentAt(_exp_drop->currentIndex())->experiment()->peakMerger()->parameters();

    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
    _frame_min->setValue(params->frame_min + 1);
    _frame_max->setValue(params->frame_max + 1);
    _d_shells->setValue(params->n_shells);
    _friedel->setChecked(params->friedel);
}

void SubframeMergedPeaks::setMergeParameters()
{
    if (_exp_drop->count() == 0)
        return;

    auto params =
        gSession->experimentAt(_exp_drop->currentIndex())->experiment()->peakMerger()->parameters();

    params->d_min = _d_min->value();
    params->d_max = _d_max->value();
    params->frame_min = _frame_min->value() - 1;
    params->frame_max = _frame_max->value() - 1;
    params->n_shells = _d_shells->value();
    params->friedel = _friedel->isChecked();
}

void SubframeMergedPeaks::setSizePolicies()
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

void SubframeMergedPeaks::setDShellUp()
{
    QVBoxLayout* shell_layout = new QVBoxLayout(_shell_tab);

    _d_shell_view = new QTableView;
    _shell_model = new QStandardItemModel(0, 13, this);
    _d_shell_view->setModel(_shell_model);
    _shell_model->setHorizontalHeaderLabels(
        {"dmax", "dmin", "nobs", "nmerge", "redundancy", "Rmeas", "Rmeas(est.)", "Rmerge/Rsym",
         "Rmerge(est.)", "Rpim", "Rpim(est.)", "CChalf", "CC*", "Completeness"});
    shell_layout->addWidget(_d_shell_view);

    QHBoxLayout* d_shell_down = new QHBoxLayout;
    QGridLayout* d_shell_down_left = new QGridLayout;

    QLabel* label_ptr;

    label_ptr = new QLabel("d range:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 0, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Frame range:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("N. of d-shells:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Space group:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Plot axis:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _d_min = new QDoubleSpinBox();
    _d_max = new QDoubleSpinBox();
    _frame_min = new QSpinBox();
    _frame_max = new QSpinBox();
    _d_shells = new QSpinBox();
    _friedel = new QCheckBox("Include friedel");
    _space_group = new QComboBox();
    _plottable_statistics = new QComboBox();
    _save_shell = new QPushButton("Save");

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
    for (int i = 0; i < _shell_model->columnCount(); ++i) {
        QStandardItem* header_item = _shell_model->horizontalHeaderItem(i);
        selection_stats.push_back(header_item->text());
    }
    _plottable_statistics->addItems(selection_stats);

    d_shell_down_left->addWidget(_d_min, 0, 1, 1, 1);
    d_shell_down_left->addWidget(_d_max, 0, 2, 1, 1);
    d_shell_down_left->addWidget(_frame_min, 1, 1, 1, 1);
    d_shell_down_left->addWidget(_frame_max, 1, 2, 1, 1);
    d_shell_down_left->addWidget(_d_shells, 2, 1, 1, 2);
    d_shell_down_left->addWidget(_space_group, 3, 1, 1, 2);
    d_shell_down_left->addWidget(_friedel, 4, 1, 1, 2);
    d_shell_down_left->addWidget(_plottable_statistics, 5, 1, 1, 3);
    d_shell_down_left->addWidget(_save_shell, 6, 0, 1, 3);
    d_shell_down->addLayout(d_shell_down_left);

    _statistics_plot = new SXPlot;
    _statistics_plot->setSizePolicy(*_size_policy_right);
    d_shell_down->addWidget(_statistics_plot);

    connect(
        _d_min, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeMergedPeaks::processMerge);

    connect(
        _d_max, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeMergedPeaks::processMerge);

    connect(
        _frame_min, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeMergedPeaks::processMerge);

    connect(
        _frame_max, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeMergedPeaks::processMerge);

    connect(
        _d_shells, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeMergedPeaks::processMerge);

    connect(_friedel, &QCheckBox::clicked, this, &SubframeMergedPeaks::processMerge);

    connect(
        _plottable_statistics,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMergedPeaks::refreshGraphTable);

    connect(_save_shell, &QPushButton::clicked, this, &SubframeMergedPeaks::saveStatistics);

    connect(
        gGui->sideBar(), &SideBar::subframeChanged, this, &SubframeMergedPeaks::setMergeParameters);
    shell_layout->addLayout(d_shell_down);
}

void SubframeMergedPeaks::setMergedUp()
{
    QVBoxLayout* merged_layout = new QVBoxLayout(_merged_tab);
    _merged_view = new QTableView;
    _merged_model = new QStandardItemModel(8, 0, this);
    _merged_view->setModel(_merged_model);
    QString sigma_header{QString((QChar)0x03C3 + QString{"(I)"})};
    QString chi_header{QString((QChar)0x03C7 + QString{"2"})};
    _merged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "nobs", chi_header, "p"});
    merged_layout->addWidget(_merged_view);

    QHBoxLayout* merged_row = new QHBoxLayout;

    _merged_save_type = new QComboBox();
    _merged_save_type->addItems({"ShelX", "FullProf", "Phenix"});
    _save_merged = new QPushButton("Save merged");

    QLabel* label = new QLabel("Intensity scale factor");

    _intensity_rescale_merged = new QDoubleSpinBox();
    _intensity_rescale_merged->setValue(1);
    _intensity_rescale_merged->setMaximum(1000000);
    _intensity_rescale_merged->setMinimum(0.000001);
    _intensity_rescale_merged->setToolTip(
        "Rescale intensities in output file by this factor (Phenix only)");

    merged_row->addStretch();
    merged_row->addWidget(label);
    merged_row->addWidget(_intensity_rescale_merged);
    merged_row->addWidget(_merged_save_type);
    merged_row->addWidget(_save_merged);
    merged_layout->addLayout(merged_row);

    connect(_save_merged, &QPushButton::clicked, this, &SubframeMergedPeaks::saveMergedPeaks);
}

void SubframeMergedPeaks::setUnmergedUp()
{
    QVBoxLayout* unmerged_layout = new QVBoxLayout(_unmerged_tab);
    _unmerged_view = new QTableView;
    _unmerged_model = new QStandardItemModel(8, 0, this);
    _unmerged_view->setModel(_unmerged_model);
    QString sigma_header{QString((QChar)0x03C3 + QString{"(I)"})};
    _unmerged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "x", "y", "frame"});
    unmerged_layout->addWidget(_unmerged_view);

    QHBoxLayout* unmerged_row = new QHBoxLayout;
    _unmerged_save_type = new QComboBox();
    _unmerged_save_type->addItems({"ShelX", "FullProf", "Phenix"});
    _save_unmerged = new QPushButton("Save unmerged");

    QLabel* label = new QLabel("Intensity scale factor");

    _intensity_rescale_unmerged = new QDoubleSpinBox();
    _intensity_rescale_unmerged->setValue(1);
    _intensity_rescale_unmerged->setMaximum(1000000);
    _intensity_rescale_unmerged->setMinimum(0.000001);
    _intensity_rescale_unmerged->setToolTip(
        "Rescale intensities in output file by this factor (Phenix only)");

    unmerged_row->addStretch();
    unmerged_row->addWidget(label);
    unmerged_row->addWidget(_intensity_rescale_unmerged);
    unmerged_row->addWidget(_unmerged_save_type);
    unmerged_row->addWidget(_save_unmerged);
    unmerged_layout->addLayout(unmerged_row);

    connect(_save_unmerged, &QPushButton::clicked, this, &SubframeMergedPeaks::saveUnmergedPeaks);
}

void SubframeMergedPeaks::refreshAll()
{
    refreshExperimentList();
    if (_exp_drop->currentIndex() >= 0)
        grabMergeParameters();
    toggleUnsafeWidgets();
}

void SubframeMergedPeaks::refreshExperimentList()
{
    _exp_drop->blockSignals(true);
    QString current_exp = _exp_drop->currentText();
    _exp_drop->clear();

    if (gSession->experimentNames().empty())
        return;

    for (const QString& exp : gSession->experimentNames())
        _exp_drop->addItem(exp);
    _exp_drop->setCurrentText(current_exp);
    _exp_drop->blockSignals(false);
    refreshSpaceGroupCombo();
    refreshPeakLists();
}

void SubframeMergedPeaks::refreshPeakLists()
{
    refreshPeakCombos();
    processMerge();
}


void SubframeMergedPeaks::refreshPeakCombos()
{
    _peaks1_drop->blockSignals(true);
    QString current_peaks1 = _peaks1_drop->currentText();
    _peaks1_drop->clear();
    _peaks1_list.clear();

    QStringList tmp = gSession->experimentAt(_exp_drop->currentIndex())
                          ->getPeakCollectionNames(nsx::listtype::PREDICTED);
    _peaks1_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_drop->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::FILTERED);
    _peaks1_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_drop->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::FOUND);
    _peaks1_list.append(tmp);
    tmp.clear();

    auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
    if (!_peaks1_list.empty()) {
        for (QString& collection : _peaks1_list) {
            if (expt->getPeakCollection(collection.toStdString())->isIntegrated())
                _peaks1_drop->addItem(collection);
        }
        _peaks1_drop->setCurrentText(current_peaks1);
    }
    _peaks1_drop->blockSignals(false);

    _peaks2_drop->blockSignals(true);
    tmp = gSession->experimentAt(_exp_drop->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::PREDICTED);
    _peaks2_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_drop->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::FILTERED);
    _peaks2_list.append(tmp);
    tmp.clear();
    tmp = gSession->experimentAt(_exp_drop->currentIndex())
              ->getPeakCollectionNames(nsx::listtype::FOUND);
    _peaks2_list.append(tmp);
    QString current_peaks2 = _peaks2_drop->currentText();
    _peaks2_drop->clear();
    _peaks2_list.clear();
    _peaks2_list.append(tmp);

    _peaks2_list.push_front(""); // Second peak collection is not used by default
    if (!_peaks2_list.empty()) {
        for (QString& collection : _peaks2_list) {
            if (!expt->hasPeakCollection(collection.toStdString()) || 
                expt->getPeakCollection(collection.toStdString())->isIntegrated())
                _peaks2_drop->addItem(collection);
        }
        _peaks2_drop->setCurrentText(current_peaks2);
    }
    _peaks2_drop->blockSignals(false);

    // Determine the maximum frame number for the frame spinboxes
    if (!(_exp_drop->count() == 0) && !(_peaks1_drop->count() == 0)) {
        auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
        auto* peaks1 = expt->getPeakCollection(_peaks1_drop->currentText().toStdString());

        int max_frames = peaks1->getPeakList()[0]->dataSet()->nFrames();
        if (!(_peaks2_drop->currentText() == QString())) {
            auto* peaks2 = expt->getPeakCollection(_peaks2_drop->currentText().toStdString());
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
}

void SubframeMergedPeaks::refreshSpaceGroupCombo()
{
    QSignalBlocker blocker(_space_group);
    auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
    if (expt->numUnitCells() == 0)
        return;

    std::vector<nsx::UnitCell*> cells = expt->getUnitCells();
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
    std::sort(vec.begin(), vec.end(), [] (const auto& x, const auto& y) { return x.second > y.second;});
    for (const auto& [key, value] : vec) {
        _space_group->addItem(QString::fromStdString(key));
    }
}

void SubframeMergedPeaks::processMerge()
{
    gGui->setReady(false);
    auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
    auto* merger = expt->peakMerger();
    nsx::SpaceGroup group = {_space_group->currentText().toStdString()};
    merger->reset();
    merger->setSpaceGroup(group);
    setMergeParameters();

    if (_peaks1_list.empty() || _peaks2_list.empty()) {
        _merged_data = nullptr;
    } else {
        std::vector<nsx::PeakCollection*> peak_collections;
        QString collection1 = _peaks1_drop->currentText();
        QString collection2 = _peaks2_drop->currentText();

        merger->addPeakCollection(expt->getPeakCollection(collection1.toStdString()));
        if (!collection2.isEmpty())
            merger->addPeakCollection(expt->getPeakCollection(collection2.toStdString()));

        merger->mergePeaks();
        _merged_data = merger->getMergedData();
        _merged_data_per_shell = merger->getMergedDataPerShell();
    }
    refreshTables();
    gGui->setReady(true);
}

void SubframeMergedPeaks::refreshTables()
{
    refreshDShellTable();
    refreshMergedTable();
    refreshUnmergedTable();
    refreshGraphTable(_plottable_statistics->currentIndex());
}

void SubframeMergedPeaks::refreshDShellTable()
{
    auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
    auto* merger = expt->peakMerger();

    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_d_shell_view->model());
    model->removeRows(0, model->rowCount());
    auto* merged_data = merger->getMergedData();

    if (merged_data == nullptr)
        return;

    if (merged_data->totalSize() == 0)
        return;

    merger->computeQuality();
    const nsx::DataResolution* quality = merger->overallQuality();
    const nsx::DataResolution* resolution = merger->shellQuality();

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
    for (auto shell : quality->shells) {
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
}

void SubframeMergedPeaks::refreshMergedTable()
{
    _merged_model->removeRows(0, _merged_model->rowCount());

    if (_merged_data == nullptr)
        return;

    for (const nsx::MergedPeak& peak : _merged_data->mergedPeakSet()) {
        const auto hkl = peak.index();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        nsx::Intensity I = peak.intensity();

        const double intensity = I.value();
        const double sigma = I.sigma();
        const int nobs = peak.redundancy();

        const double chi2 = peak.chi2();
        const double p = peak.pValue();

        QList<QStandardItem*> row;
        row.push_back(new QStandardItem(QString::number(h)));
        row.push_back(new QStandardItem(QString::number(k)));
        row.push_back(new QStandardItem(QString::number(l)));
        row.push_back(new QStandardItem(QString::number(intensity)));
        row.push_back(new QStandardItem(QString::number(sigma)));
        row.push_back(new QStandardItem(QString::number(nobs)));
        row.push_back(new QStandardItem(QString::number(chi2)));
        row.push_back(new QStandardItem(QString::number(p)));

        _merged_model->appendRow(row);
    }
}

void SubframeMergedPeaks::refreshUnmergedTable()
{
    _unmerged_model->removeRows(0, _unmerged_model->rowCount());

    if (_merged_data == nullptr)
        return;

    for (const nsx::MergedPeak& peak : _merged_data->mergedPeakSet()) {
        for (auto unmerged_peak : peak.peaks()) {
            const nsx::UnitCell& cell = *(unmerged_peak->unitCell());
            const nsx::ReciprocalVector& q = unmerged_peak->q();

            const nsx::MillerIndex hkl(q, cell);

            const int h = hkl[0];
            const int k = hkl[1];
            const int l = hkl[2];

            const Eigen::Vector3d& c = unmerged_peak->shape().center();
            nsx::Intensity I = unmerged_peak->correctedIntensity();

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

            _unmerged_model->appendRow(row);
        }
    }
}

void SubframeMergedPeaks::refreshGraphTable(int column)
{
    _statistics_plot->clearGraphs();

    if (_merged_data == nullptr)
        return;

    if (_merged_data->totalSize() == 0)
        return;

    QStandardItemModel* _shell_model = dynamic_cast<QStandardItemModel*>(_d_shell_view->model());
    int nshells = _shell_model->rowCount() - 1;

    QVector<double> xvals;
    QVector<double> yvals;
    for (int i = 0; i < nshells; ++i) {
        xvals.push_back(double(i));
        double val = _shell_model->item(i, column)->data(Qt::DisplayRole).value<double>();
        yvals.push_back(val);
    }

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    _statistics_plot->addGraph();
    _statistics_plot->graph(0)->setPen(pen);
    _statistics_plot->graph(0)->addData(xvals, yvals);
    _statistics_plot->xAxis->setLabel("shell");
    _statistics_plot->yAxis->setLabel(_plottable_statistics->itemText(column));
    _statistics_plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _statistics_plot->xAxis->setTickLabelFont(font);
    _statistics_plot->yAxis->setTickLabelFont(font);

    _statistics_plot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);
    _statistics_plot->rescaleAxes();
    _statistics_plot->replot();
}

void SubframeMergedPeaks::saveStatistics()
{
    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("merged", QDir::homePath()).toString();

    QString filename =
        QFileDialog::getSaveFileName(this, "Save the shell info", loadDirectory, "(*.txt)");

    if (filename.isEmpty())
        return;

    QFileInfo info(filename);
    s.setValue("merged", info.absolutePath());

    auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
    auto* merger = expt->peakMerger();

    exporter.saveStatistics(
        filename.toStdString(), merger->shellQuality(), merger->overallQuality());
}

void SubframeMergedPeaks::saveMergedPeaks()
{
    QString format = _merged_save_type->currentText();

    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("merged", QDir::homePath()).toString();

    QString filename;

    auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
    auto* merger = expt->peakMerger();
    auto* merged_data = merger->getMergedData();

    if (format.compare("ShelX") == 0) {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to ShelX", loadDirectory, "ShelX hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToShelXMerged(filename.toStdString(), merged_data);
    } else if (format.compare("FullProf") == 0) {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to FullProf", loadDirectory, "FullProf hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToFullProfMerged(filename.toStdString(), merged_data);
    } else if (format.compare("Phenix") == 0) {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to Phenix sca", loadDirectory, "Phenix sca file (*.sca)");

        if (filename.isEmpty())
            return;

        exporter.saveToSCAMerged(
            filename.toStdString(), merged_data, _intensity_rescale_merged->value());
    }

    QFileInfo info(filename);
    s.setValue("merged", info.absolutePath());
}

void SubframeMergedPeaks::saveUnmergedPeaks()
{
    QString format = _merged_save_type->currentText();

    QSettings s;
    s.beginGroup("RecentDirectories");
    QString loadDirectory = s.value("merged", QDir::homePath()).toString();

    QString filename;

    auto* expt = gSession->experimentAt(_exp_drop->currentIndex())->experiment();
    auto* merger = expt->peakMerger();
    auto* merged_data = merger->getMergedData();

    if (format.compare("ShelX") == 0) {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to ShelX", loadDirectory, "ShelX hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToShelXUnmerged(filename.toStdString(), merged_data);
    } else if (format.compare("FullProf") == 0) {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to FullProf", loadDirectory, "ShelX hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToFullProfUnmerged(filename.toStdString(), merged_data);
    } else if (format.compare("Phenix") == 0) {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to Phenix sca", loadDirectory, "Phenix sca file (*.sca)");

        if (filename.isEmpty())
            return;

        exporter.saveToSCAUnmerged(
            filename.toStdString(), merged_data, _intensity_rescale_unmerged->value());
    }

    QFileInfo info(filename);
    s.setValue("merged", info.absolutePath());
}

void SubframeMergedPeaks::toggleUnsafeWidgets()
{
    _save_shell->setEnabled(true);
    _save_merged->setEnabled(true);
    _save_unmerged->setEnabled(true);
    if (_exp_drop->count() == 0 || _peaks1_drop->count() == 0) {
        _save_shell->setEnabled(false);
        _save_merged->setEnabled(false);
        _save_unmerged->setEnabled(false);
    }
}
