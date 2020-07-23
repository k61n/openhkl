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
#include "core/experiment/Experiment.h"
#include "core/statistics/CC.h"
#include "core/statistics/MergedPeak.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <fstream>
#include <iomanip>

SubframeMergedPeaks::SubframeMergedPeaks() : QWidget()
{
    setSizePolicies();

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* drop_layout = new QHBoxLayout();

    _exp_drop = new QComboBox();
    _found_drop = new QComboBox();
    _predicted_drop = new QComboBox();

    _exp_drop->setSizePolicy(*_size_policy_right);
    _found_drop->setSizePolicy(*_size_policy_right);
    _predicted_drop->setSizePolicy(*_size_policy_right);

    drop_layout->addWidget(new QLabel("Experiment:"));
    drop_layout->addWidget(_exp_drop);
    drop_layout->addWidget(new QLabel("Found peaks:"));
    drop_layout->addWidget(_found_drop);
    drop_layout->addWidget(new QLabel("Predicted peaks:"));
    drop_layout->addWidget(_predicted_drop);
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
        _found_drop, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMergedPeaks::processMerge);
    connect(
        _predicted_drop, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &SubframeMergedPeaks::processMerge);

    show();
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
    _shell_model->setHorizontalHeaderLabels({"dmax", "dmin", "nobs", "nmerge", "redundancy",
                                             "Rmeas", "Rmeas(est.)", "Rmerge/Rsym", "Rmerge(est.)",
                                             "Rpim", "Rpim(est.)", "CChalf", "CC*"});
    shell_layout->addWidget(_d_shell_view);

    QHBoxLayout* d_shell_down = new QHBoxLayout;
    QGridLayout* d_shell_down_left = new QGridLayout;

    QLabel* label_ptr;

    label_ptr = new QLabel("Minimum d:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 0, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Maximum d:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 1, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("N. of d-shells:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 2, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    label_ptr = new QLabel("Plot axis:");
    label_ptr->setAlignment(Qt::AlignRight);
    d_shell_down_left->addWidget(label_ptr, 4, 0, 1, 1);
    label_ptr->setSizePolicy(*_size_policy_widgets);

    _d_min = new QDoubleSpinBox();
    _d_max = new QDoubleSpinBox();
    _d_shells = new QSpinBox();
    _friedel = new QCheckBox("Include friedel");
    _plottable_statistics = new QComboBox();
    _save_shell = new QPushButton("Save");

    _d_min->setValue(1.5);
    _d_max->setValue(50);
    _d_shells->setValue(10);
    _friedel->setChecked(true);

    _d_min->setSizePolicy(*_size_policy_widgets);
    _d_max->setSizePolicy(*_size_policy_widgets);
    _d_shells->setSizePolicy(*_size_policy_widgets);
    _friedel->setSizePolicy(*_size_policy_widgets);
    _plottable_statistics->setSizePolicy(*_size_policy_widgets);
    _save_shell->setSizePolicy(*_size_policy_widgets);

    QStringList selection_stats;
    for (int i = 0; i < _shell_model->columnCount(); ++i) {
        QStandardItem* header_item = _shell_model->horizontalHeaderItem(i);
        selection_stats.push_back(header_item->text());
    }
    _plottable_statistics->addItems(selection_stats);

    d_shell_down_left->addWidget(_d_min, 0, 1, 1, 1);
    d_shell_down_left->addWidget(_d_max, 1, 1, 1, 1);
    d_shell_down_left->addWidget(_d_shells, 2, 1, 1, 1);
    d_shell_down_left->addWidget(_friedel, 3, 0, 1, 2);
    d_shell_down_left->addWidget(_plottable_statistics, 4, 1, 1, 1);
    d_shell_down_left->addWidget(_save_shell, 5, 0, 1, 2);
    d_shell_down->addLayout(d_shell_down_left);

    _statistics_plot = new SXPlot;
    _statistics_plot->setSizePolicy(*_size_policy_right);
    d_shell_down->addWidget(_statistics_plot);

    connect(
        _d_min, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeMergedPeaks::refreshDShellTable);

    connect(
        _d_max, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
        &SubframeMergedPeaks::refreshDShellTable);

    connect(
        _d_shells, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &SubframeMergedPeaks::refreshDShellTable);

    connect(_friedel, &QCheckBox::clicked, this, &SubframeMergedPeaks::refreshDShellTable);

    connect(
        _plottable_statistics,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &SubframeMergedPeaks::refreshGraphTable);

    connect(_save_shell, &QPushButton::clicked, this, &SubframeMergedPeaks::saveStatistics);

    shell_layout->addLayout(d_shell_down);
}

void SubframeMergedPeaks::setMergedUp()
{
    QVBoxLayout* merged_layout = new QVBoxLayout(_merged_tab);
    _merged_view = new QTableView;
    _merged_model = new QStandardItemModel(7, 0, this);
    _merged_view->setModel(_merged_model);
    _merged_model->setHorizontalHeaderLabels({"h", "k", "l", "I", "sigmaI", "chi2", "p"});
    merged_layout->addWidget(_merged_view);

    QHBoxLayout* merged_row = new QHBoxLayout;
    _merged_save_type = new QComboBox();
    _merged_save_type->addItems({"ShelX", "FullProf", "Phenix"});
    _save_merged = new QPushButton("Save merged");
    merged_row->addStretch();
    merged_row->addWidget(_merged_save_type);
    merged_row->addWidget(_save_merged);
    merged_layout->addLayout(merged_row);

    connect(_save_merged, &QPushButton::clicked, this, &SubframeMergedPeaks::saveMergedPeaks);
}

void SubframeMergedPeaks::setUnmergedUp()
{
    QVBoxLayout* unmerged_layout = new QVBoxLayout(_unmerged_tab);
    _unmerged_view = new QTableView;
    _unmerged_model = new QStandardItemModel(7, 0, this);
    _unmerged_view->setModel(_unmerged_model);
    _unmerged_model->setHorizontalHeaderLabels({"h", "k", "l", "I", "sigmaI", "chi2", "p"});
    unmerged_layout->addWidget(_unmerged_view);

    QHBoxLayout* unmerged_row = new QHBoxLayout;
    _unmerged_save_type = new QComboBox();
    _unmerged_save_type->addItems({"ShelX", "FullProf", "Phenix"});
    _save_unmerged = new QPushButton("Save unmerged");

    unmerged_row->addStretch();
    unmerged_row->addWidget(_unmerged_save_type);
    unmerged_row->addWidget(_save_unmerged);
    unmerged_layout->addLayout(unmerged_row);

    connect(_save_unmerged, &QPushButton::clicked, this, &SubframeMergedPeaks::saveUnmergedPeaks);
}

void SubframeMergedPeaks::refreshAll()
{
    refreshExperimentList();
}

void SubframeMergedPeaks::refreshExperimentList()
{
    _exp_drop->blockSignals(true);
    _exp_drop->clear();

    if (gSession->experimentNames().empty())
        return;

    for (const QString& exp : gSession->experimentNames())
        _exp_drop->addItem(exp);
    _exp_drop->blockSignals(false);
    refreshPeakLists();
}

void SubframeMergedPeaks::refreshPeakLists()
{
    refreshFoundPeakList();
    refreshPredictedPeakList();
    processMerge();
}

void SubframeMergedPeaks::refreshFoundPeakList()
{
    _found_drop->blockSignals(true);

    _found_drop->clear();
    _found_list = gSession->experimentAt(_exp_drop->currentIndex())->getFoundNames();

    if (!_found_list.empty()) {
        _found_drop->addItems(_found_list);
        _found_drop->setCurrentIndex(0);
    }
    _found_drop->blockSignals(false);
}

void SubframeMergedPeaks::refreshPredictedPeakList()
{
    _predicted_drop->blockSignals(true);

    _predicted_drop->clear();
    _predicted_list = gSession->experimentAt(_exp_drop->currentIndex())->getPredictedNames();

    if (!_predicted_list.empty()) {
        _predicted_drop->addItems(_predicted_list);
        _predicted_drop->setCurrentIndex(0);
    }
    _predicted_drop->blockSignals(false);
}

void SubframeMergedPeaks::processMerge()
{
    if (_found_list.empty() || _predicted_list.empty()) {
        gSession->experimentAt(_exp_drop->currentIndex())->experiment()->resetMergedPeaks();
        _merged_data = nullptr;
    } else {
        std::vector<nsx::PeakCollection*> peak_collections;
        peak_collections.push_back(
            gSession->experimentAt(_exp_drop->currentIndex())
                ->experiment()
                ->getPeakCollection(_found_drop->currentText().toStdString()));
        peak_collections.push_back(
            gSession->experimentAt(_exp_drop->currentIndex())
                ->experiment()
                ->getPeakCollection(_predicted_drop->currentText().toStdString()));

        gSession->experimentAt(_exp_drop->currentIndex())
            ->experiment()
            ->setMergedPeaks(peak_collections, _friedel->isChecked());
        _merged_data =
            gSession->experimentAt(_exp_drop->currentIndex())->experiment()->getMergedPeaks();
    }
    refreshTables();
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
    double min = _d_min->value();
    double max = _d_max->value();
    int shells = _d_shells->value();
    bool inclFriedel = _friedel->isChecked();

    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(_d_shell_view->model());
    model->removeRows(0, model->rowCount());

    if (_merged_data == nullptr)
        return;

    if (_merged_data->totalSize() == 0)
        return;

    nsx::PeakCollection* found = gSession->experimentAt(_exp_drop->currentIndex())
                                     ->experiment()
                                     ->getPeakCollection(_found_drop->currentText().toStdString());
    nsx::PeakCollection* predicted =
        gSession->experimentAt(_exp_drop->currentIndex())
            ->experiment()
            ->getPeakCollection(_predicted_drop->currentText().toStdString());

    nsx::ResolutionShell resolutionShell(min, max, shells);
    for (nsx::Peak3D* peak : found->getPeakList())
        resolutionShell.addPeak(peak);
    for (nsx::Peak3D* peak : predicted->getPeakList())
        resolutionShell.addPeak(peak);

    for (int i = shells - 1; i >= 0; --i) {
        const double d_lower = resolutionShell.shell(i).dmin;
        const double d_upper = resolutionShell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(_merged_data->spaceGroup(), inclFriedel);

        for (nsx::Peak3D* peak : resolutionShell.shell(i).peaks)
            merged_data_per_shell.addPeak(peak);

        nsx::CC cc;
        cc.calculate(&merged_data_per_shell);
        nsx::RFactor rfactor;
        rfactor.calculate(&merged_data_per_shell);

        QList<QStandardItem*> row;
        row.push_back(new QStandardItem(QString::number(d_upper)));
        row.push_back(new QStandardItem(QString::number(d_lower)));
        row.push_back(new QStandardItem(QString::number(merged_data_per_shell.totalSize())));
        row.push_back(
            new QStandardItem(QString::number(merged_data_per_shell.mergedPeakSet().size())));
        row.push_back(new QStandardItem(QString::number(merged_data_per_shell.redundancy())));
        row.push_back(new QStandardItem(QString::number(rfactor.Rmeas())));
        row.push_back(new QStandardItem(QString::number(rfactor.expectedRmeas())));
        row.push_back(new QStandardItem(QString::number(rfactor.Rmerge())));
        row.push_back(new QStandardItem(QString::number(rfactor.expectedRmerge())));
        row.push_back(new QStandardItem(QString::number(rfactor.Rpim())));
        row.push_back(new QStandardItem(QString::number(rfactor.expectedRpim())));
        row.push_back(new QStandardItem(QString::number(cc.CChalf())));
        row.push_back(new QStandardItem(QString::number(cc.CCstar())));

        model->appendRow(row);
    }

    nsx::RFactor rfactor;
    rfactor.calculate(_merged_data);

    nsx::CC cc;
    cc.calculate(_merged_data);

    QList<QStandardItem*> row;
    row.push_back(new QStandardItem(QString::number(max)));
    row.push_back(new QStandardItem(QString::number(min)));
    row.push_back(new QStandardItem(QString::number(_merged_data->totalSize())));
    row.push_back(new QStandardItem(QString::number(_merged_data->mergedPeakSet().size())));
    row.push_back(new QStandardItem(QString::number(_merged_data->redundancy())));
    row.push_back(new QStandardItem(QString::number(rfactor.Rmeas())));
    row.push_back(new QStandardItem(QString::number(rfactor.expectedRmeas())));
    row.push_back(new QStandardItem(QString::number(rfactor.Rmerge())));
    row.push_back(new QStandardItem(QString::number(rfactor.expectedRmerge())));
    row.push_back(new QStandardItem(QString::number(rfactor.Rpim())));
    row.push_back(new QStandardItem(QString::number(rfactor.expectedRpim())));
    row.push_back(new QStandardItem(QString::number(cc.CChalf())));
    row.push_back(new QStandardItem(QString::number(cc.CCstar())));
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
            std::string numor = unmerged_peak->dataSet()->filename();
            nsx::Intensity I = unmerged_peak->correctedIntensity();

            const double intensity = I.value();
            const double sigma = I.sigma();

            const QFileInfo fileinfo(QString::fromStdString(numor));

            QList<QStandardItem*> row;
            row.push_back(new QStandardItem(QString::number(h)));
            row.push_back(new QStandardItem(QString::number(k)));
            row.push_back(new QStandardItem(QString::number(l)));
            row.push_back(new QStandardItem(QString::number(intensity)));
            row.push_back(new QStandardItem(QString::number(sigma)));
            row.push_back(new QStandardItem(QString::number(c[0])));
            row.push_back(new QStandardItem(QString::number(c[1])));
            row.push_back(new QStandardItem(QString::number(c[2])));
            row.push_back(new QStandardItem(fileinfo.baseName()));

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
    std::vector<double> shells(nshells);
    std::iota(shells.begin(), shells.end(), 0);

    QVector<double> xvals {shells.begin(), shells.end()};
    QVector<double> yvals;
    for (int i = 0; i < nshells; ++i) {
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
    QString filename = QFileDialog::getSaveFileName(this, "Save the shell info", ".", "(*.txt)");

    if (filename.isEmpty())
        return;

    double min = _d_min->value();
    double max = _d_max->value();
    int shells = _d_shells->value();
    bool inclFriedel = _friedel->isChecked();

    if (_merged_data == nullptr)
        return;

    nsx::PeakCollection* found = gSession->experimentAt(_exp_drop->currentIndex())
                                     ->experiment()
                                     ->getPeakCollection(_found_drop->currentText().toStdString());
    nsx::PeakCollection* predicted =
        gSession->experimentAt(_exp_drop->currentIndex())
            ->experiment()
            ->getPeakCollection(_predicted_drop->currentText().toStdString());

    nsx::ResolutionShell resolutionShell(min, max, shells);
    for (nsx::Peak3D* peak : found->getPeakList())
        resolutionShell.addPeak(peak);
    for (nsx::Peak3D* peak : predicted->getPeakList())
        resolutionShell.addPeak(peak);

    exporter.saveStatistics(
        filename.toStdString(), resolutionShell, _merged_data->spaceGroup(), inclFriedel);
}

void SubframeMergedPeaks::saveMergedPeaks()
{
    QString format = _merged_save_type->currentText();

    if (format.compare("ShelX") == 0) {
        QString filename = QFileDialog::getSaveFileName(
            this, "Save peaks to ShelX", ".", "ShelX hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToShelXMerged(filename.toStdString(), _merged_data);
    } else if (format.compare("FullProf") == 0) {
        QString filename = QFileDialog::getSaveFileName(
            this, "Save peaks to FullProf", ".", "FullProf hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToFullProfMerged(filename.toStdString(), _merged_data);
    } else if (format.compare("Phenix") == 0) {
        QString filename = QFileDialog::getSaveFileName(
            this, "Save peaks to Phenix sca", ".", "Phenix sca file (*.sca)");

        if (filename.isEmpty())
            return;

        exporter.saveToSCAMerged(filename.toStdString(), _merged_data);
    }
}

void SubframeMergedPeaks::saveUnmergedPeaks()
{
    QString format = _merged_save_type->currentText();

    if (format.compare("ShelX") == 0) {
        QString filename = QFileDialog::getSaveFileName(
            this, "Save peaks to ShelX", ".", "ShelX hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToShelXUnmerged(filename.toStdString(), _merged_data);
    } else if (format.compare("FullProf") == 0) {
        QString filename = QFileDialog::getSaveFileName(
            this, "Save peaks to FullProf", ".", "ShelX hkl file (*.hkl)");

        if (filename.isEmpty())
            return;

        exporter.saveToFullProfUnmerged(filename.toStdString(), _merged_data);
    } else if (format.compare("Phenix") == 0) {
        QString filename = QFileDialog::getSaveFileName(
            this, "Save peaks to Phenix sca", ".", "Phenix sca file (*.sca)");

        if (filename.isEmpty())
            return;

        exporter.saveToSCAUnmerged(filename.toStdString(), _merged_data);
    }
}
