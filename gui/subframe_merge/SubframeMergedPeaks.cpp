//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subframe_merge/SubframeMergedPeaks.cpp
//! @brief     Implements class SubframeMergedPeaks
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_merge/SubframeMergedPeaks.h"

#include "core/data/DataSet.h"
#include "core/experiment/DataQuality.h"
#include "core/experiment/Experiment.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/CC.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/MergedPeak.h"
#include "core/statistics/PeakExporter.h"
#include "core/statistics/PeakMerger.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include "gui/MainWin.h" // gGui
#include "gui/graphics/SXPlot.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/utility/IntegratedPeakComboBox.h"
#include "gui/utility/SideBar.h"
#include "tables/crystal/UnitCell.h"

#include "core/experiment/MtzExporter.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QVBoxLayout>

#include <fstream>
#include <iomanip>

#include "gui/dialogs/PeakExportDialog.h"

SubframeMergedPeaks::SubframeMergedPeaks()
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
        &SubframeMergedPeaks::processMerge);
    connect(
        _peak_combo_2, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMergedPeaks::processMerge);
}

void SubframeMergedPeaks::grabMergeParameters()
{
    auto params = gSession->currentProject()->experiment()->peakMerger()->parameters();

    _d_min->setValue(params->d_min);
    _d_max->setValue(params->d_max);
    _frame_min->setValue(params->frame_min + 1);
    _frame_max->setValue(params->frame_max + 1);
    _d_shells->setValue(params->n_shells);
    _friedel->setChecked(params->friedel);
}

void SubframeMergedPeaks::setMergeParameters()
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
        {"Max. d", "Min. d", "Num. peaks observed", "Num. merged peaks", "Redundancy", "Rmeas",
         "Rmeas(est.)", "Rmerge/Rsym", "Rmerge(est.)", "Rpim", "Rpim(est.)", "CChalf", "CC*",
         "Completeness"});
    shell_layout->addWidget(_d_shell_view);
    _d_shell_view->resizeColumnsToContents();

    QHBoxLayout* d_shell_down = new QHBoxLayout;
    QGridLayout* d_shell_down_left = new QGridLayout;

    QLabel* label_ptr;

    label_ptr = new QLabel("Resolution (d) range:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 0, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Image range:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Num. resolution shells:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Space group:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 3, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Plot y axis:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 5, 0, 1, 1);
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
    _merged_model = new QStandardItemModel(0, 8, this);
    _merged_view->setModel(_merged_model);
    QString sigma_header{QString((QChar)0x03C3 + QString{"(I)"})};
    QString chi_header{QString((QChar)0x03C7 + QString{"2"})};
    _merged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "nobs", chi_header, "p"});
    _merged_view->resizeColumnsToContents();
    merged_layout->addWidget(_merged_view);

    QHBoxLayout* merged_row = new QHBoxLayout;

    _merged_save_type = new QComboBox();
    for (int idx = 0; idx < static_cast<int>(ohkl::ExportFormat::Count); ++idx) {
        std::string text =
            _exporter.exportFormatStrings()->at(static_cast<ohkl::ExportFormat>(idx));
        _merged_save_type->addItem(QString::fromStdString(text));
    }

    _save_merged = new QPushButton("Save merged");

    QLabel* label = new QLabel("Intensity scale factor");

    _intensity_rescale_merged = new QDoubleSpinBox();
    _intensity_rescale_merged->setValue(1);
    _intensity_rescale_merged->setMaximum(1000000);
    _intensity_rescale_merged->setMinimum(0.000001);
    _intensity_rescale_merged->setDecimals(6);
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
    _unmerged_model = new QStandardItemModel(0, 8, this);
    _unmerged_view->setModel(_unmerged_model);
    QString sigma_header{QString((QChar)0x03C3 + QString{"(I)"})};
    _unmerged_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", sigma_header, "x", "y", "frame"});
    _unmerged_view->resizeColumnsToContents();
    unmerged_layout->addWidget(_unmerged_view);

    QHBoxLayout* unmerged_row = new QHBoxLayout;
    _unmerged_save_type = new QComboBox();
    for (int idx = 0; idx < static_cast<int>(ohkl::ExportFormat::Count); ++idx) {
        std::string text =
            _exporter.exportFormatStrings()->at(static_cast<ohkl::ExportFormat>(idx));
        _unmerged_save_type->addItem(QString::fromStdString(text));
    }
    _save_unmerged = new QPushButton("Save unmerged");

    QLabel* label = new QLabel("Intensity scale factor");

    _intensity_rescale_unmerged = new QDoubleSpinBox();
    _intensity_rescale_unmerged->setValue(1);
    _intensity_rescale_unmerged->setMaximum(1000000);
    _intensity_rescale_unmerged->setMinimum(0.000001);
    _intensity_rescale_unmerged->setDecimals(6);
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
    if (!gSession->hasProject())
        return;

    refreshSpaceGroupCombo();
    refreshPeakLists();
    grabMergeParameters();
    toggleUnsafeWidgets();
}

void SubframeMergedPeaks::refreshPeakLists()
{
    if (!gSession->currentProject()->hasPeakCollection())
        return;
    refreshPeakCombos();
    processMerge();
    _d_shell_view->resizeColumnsToContents();
    _merged_view->resizeColumnsToContents();
    _unmerged_view->resizeColumnsToContents();
}

void SubframeMergedPeaks::refreshPeakCombos()
{
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

void SubframeMergedPeaks::refreshSpaceGroupCombo()
{
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

void SubframeMergedPeaks::processMerge()
{
    gGui->setReady(false);
    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();
    merger->reset();
    setMergeParameters();

    if (!gSession->currentProject()->hasPeakCollection()) {
        _merged_data = nullptr;
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
    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();

    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_d_shell_view->model());
    model->removeRows(0, model->rowCount());
    auto* merged_data = merger->getMergedData();

    if (merged_data == nullptr)
        return;

    if (merged_data->totalSize() == 0)
        return;

    merger->computeQuality();
    const ohkl::DataResolution* quality = merger->overallQuality();
    const ohkl::DataResolution* resolution = merger->shellQuality();

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

    for (const ohkl::MergedPeak& peak : _merged_data->mergedPeakSet()) {
        const auto hkl = peak.index();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        ohkl::Intensity I = peak.intensity();

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

    for (const ohkl::MergedPeak& peak : _merged_data->mergedPeakSet()) {
        for (auto unmerged_peak : peak.peaks()) {
            const ohkl::UnitCell& cell = *(unmerged_peak->unitCell());
            const ohkl::ReciprocalVector& q = unmerged_peak->q();

            const ohkl::MillerIndex hkl(q, cell);

            const int h = hkl[0];
            const int k = hkl[1];
            const int l = hkl[2];

            const Eigen::Vector3d& c = unmerged_peak->shape().center();
            ohkl::Intensity I = unmerged_peak->correctedIntensity();

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

void SubframeMergedPeaks::savePeaks(bool merged)
{
    QSettings settings = gGui->qSettings();
    settings.beginGroup("RecentDirectories");
    QString loadDirectory = settings.value("merged", QDir::homePath()).toString();
    QString filename;
    auto* formats = _exporter.exportFormatStrings();

    ohkl::ExportFormat fmt;
    if (merged)
        fmt = static_cast<ohkl::ExportFormat>(_merged_save_type->currentIndex());
    else
        fmt = static_cast<ohkl::ExportFormat>(_unmerged_save_type->currentIndex());

    switch (fmt) {
    case ohkl::ExportFormat::Mtz: {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to CCP4 mtz", loadDirectory, QString::fromStdString(formats->at(fmt)));
        break;
    }
    case ohkl::ExportFormat::Phenix: {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to Phenix sca", loadDirectory, QString::fromStdString(formats->at(fmt)));
        break;
    }
    case ohkl::ExportFormat::ShelX: {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to ShelX", loadDirectory, QString::fromStdString(formats->at(fmt)));
        break;
    }
    case ohkl::ExportFormat::FullProf: {
        filename = QFileDialog::getSaveFileName(
            this, "Save peaks to FullProf", loadDirectory, QString::fromStdString(formats->at(fmt)));
        break;
    }
    default: return;
    }
    if (filename.isEmpty())
        return;

    auto* expt = gSession->currentProject()->experiment();
    auto* merger = expt->peakMerger();
    auto cell = singleBatchRefine();
    auto data = _peak_combo_1->currentPeakCollection()->data();

    double scale;
    if (merged)
        scale = _intensity_rescale_merged->value();
    else
        scale = _intensity_rescale_unmerged->value();

    std::string comment = "";
    bool success = _exporter.exportPeaks(
        fmt, filename.toStdString(), merger->getMergedData(), data, cell, merged, scale, comment);
    if (!success)
        QMessageBox::critical(this, "Error", "Peak export unsuccessful");

    QFileInfo info(filename);
    settings.setValue("merged", info.absolutePath());
}

void SubframeMergedPeaks::saveMergedPeaks()
{
    savePeaks(true);
}

void SubframeMergedPeaks::saveUnmergedPeaks()
{
    savePeaks(false);
}

void SubframeMergedPeaks::toggleUnsafeWidgets()
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

ohkl::sptrUnitCell SubframeMergedPeaks::singleBatchRefine()
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
