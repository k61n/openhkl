//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/frames/FrameStatistics.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <vector>

#include <QComboBox>
#include <QStandardItem>
#include <QTableView>
#include <QVector>

#include "core/statistics/CC.h"
#include "core/experiment/DataSet.h"
#include "core/data/IDataReader.h"
#include "core/logger/Logger.h"
#include "core/data/MetaData.h"
#include "core/crystal/MillerIndex.h"
#include "core/peak/Peak3D.h"
#include "core/statistics/RFactor.h"
#include "core/crystal/ResolutionShell.h"

#include "apps/models/CollectedPeaksModel.h"
#include "apps/delegates/DoubleItemDelegate.h"
#include "apps/models/ExperimentItem.h"
#include "apps/frames/FrameStatistics.h"
#include "apps/models/PeaksItem.h"
#include "apps/utils/PeaksUtils.h"
#include "apps/plot/SXPlot.h"
#include "apps/widgets/WidgetRefinerFit.h"

#include "ui_FrameStatistics.h"

FrameStatistics* FrameStatistics::_instance = nullptr;

FrameStatistics*
FrameStatistics::create(const nsx::PeakList& peaks, const nsx::SpaceGroup& space_group)
{
    if (!_instance) {
        _instance = new FrameStatistics(peaks, space_group);
    }

    return _instance;
}

FrameStatistics* FrameStatistics::Instance()
{
    return _instance;
}

FrameStatistics::FrameStatistics(const nsx::PeakList& peaks, const nsx::SpaceGroup& space_group)
    : NSXQFrame()
    , _ui(new Ui::FrameStatistics)
    , _peaks(peaks)
    , _space_group(space_group)
    , _merged_data(space_group, true)
{
    _ui->setupUi(this);

    _ui->tabs->setCurrentIndex(0);

    auto&& drange = dRange(peaks);
    _ui->dmin->setValue(drange.first);
    _ui->dmax->setValue(drange.second);

    QStandardItemModel* shell_model = new QStandardItemModel(0, 13, this);
    _ui->statistics->setModel(shell_model);
    shell_model->setHorizontalHeaderLabels({"dmax", "dmin", "nobs", "nmerge", "redundancy", "Rmeas",
                                            "Rmeas(est.)", "Rmerge/Rsym", "Rmerge(est.)", "Rpim",
                                            "Rpim(est.)", "CChalf", "CC*"});

    QStandardItemModel* merged_peaks_model = new QStandardItemModel(7, 0, this);
    _ui->merged_peaks->setModel(merged_peaks_model);
    merged_peaks_model->setHorizontalHeaderLabels({"h", "k", "l", "I", "sigmaI", "chi2", "p"});

    QStandardItemModel* unmerged_peaks_model = new QStandardItemModel(9, 0, this);
    _ui->unmerged_peaks->setModel(unmerged_peaks_model);
    unmerged_peaks_model->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", "sigmaI", "x", "y", "frame", "numor"});

    for (auto i = 0; i < shell_model->columnCount(); ++i) {
        auto header_item = shell_model->horizontalHeaderItem(i);
        _ui->selected_statistics->addItem(header_item->text());
    }

    connect(_ui->dmin, SIGNAL(valueChanged(double)), this, SLOT(update()));
    connect(_ui->dmax, SIGNAL(valueChanged(double)), this, SLOT(update()));
    connect(_ui->n_shells, SIGNAL(valueChanged(int)), this, SLOT(update()));
    connect(_ui->friedel, SIGNAL(stateChanged(int)), this, SLOT(update()));

    connect(_ui->save_statistics, SIGNAL(clicked()), this, SLOT(saveStatistics()));
    connect(_ui->save_merged_peaks, SIGNAL(clicked()), this, SLOT(saveMergedPeaks()));
    connect(_ui->save_unmerged_peaks, SIGNAL(clicked()), this, SLOT(saveUnmergedPeaks()));

    connect(
        _ui->selected_statistics,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [=](int column) { plotStatistics(column); });

    update();
}

FrameStatistics::~FrameStatistics()
{
    delete _ui;

    if (_instance) {
        _instance = nullptr;
    }
}

void FrameStatistics::slotActionClicked(QAbstractButton* button)
{
    auto button_role = _ui->actions->standardButton(button);

    switch (button_role) {
    case QDialogButtonBox::StandardButton::Ok: {
        close();
        break;
    }
    default: {
        return;
    }
    }
}

void FrameStatistics::plotStatistics(int column)
{
    auto statistics_table_model = dynamic_cast<QStandardItemModel*>(_ui->statistics->model());

    // The last row is for the overall statistics, skip it
    int n_shells = statistics_table_model->rowCount() - 1;

    std::vector<double> shells(n_shells);
    std::iota(shells.begin(), shells.end(), 0);

    QVector<double> x_values = QVector<double>::fromStdVector(shells);
    QVector<double> y_values;

    for (auto i = 0; i < n_shells; ++i) {
        auto value = statistics_table_model->item(i, column)->data(Qt::DisplayRole).value<double>();
        y_values.append(value);
    }

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    _ui->plot->clearGraphs();
    _ui->plot->addGraph();
    _ui->plot->graph(0)->setPen(pen);

    _ui->plot->graph(0)->addData(x_values, y_values);

    _ui->plot->xAxis->setLabel("shell");
    _ui->plot->yAxis->setLabel(_ui->selected_statistics->itemText(column));

    _ui->plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    _ui->plot->xAxis->setTickLabelFont(font);
    _ui->plot->yAxis->setTickLabelFont(font);

    _ui->plot->setInteractions(
        QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
        | QCP::iSelectPlottables);

    _ui->plot->rescaleAxes();

    _ui->plot->replot();
}

void FrameStatistics::saveStatistics()
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save statistics"), ".", tr("Text file (*.dat *.txt)"));

    if (filename.isEmpty()) {
        return;
    }

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return;
    }

    file << std::setw(10) << "dmax";
    file << std::setw(10) << "dmin";
    file << std::setw(10) << "nobs";
    file << std::setw(10) << "nmerged";
    file << std::setw(11) << "redundancy";
    file << std::setw(10) << "Rmeas";
    file << std::setw(12) << "Rmeas(est.)";
    file << std::setw(11) << "Rmerge/sym";
    file << std::setw(13) << "Rmerge(est.)";
    file << std::setw(10) << "Rpim";
    file << std::setw(11) << "Rpim(est.)";
    file << std::setw(10) << "CChalf";
    file << std::setw(10) << "CC*";
    file << std::endl;

    auto model = dynamic_cast<QStandardItemModel*>(_ui->statistics->model());

    for (size_t i = 0; i < model->rowCount(); ++i) {

        file << std::fixed << std::setw(10) << std::setprecision(2)
             << model->index(i, 0).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(2)
             << model->index(i, 1).data().toDouble();
        file << std::fixed << std::setw(10) << model->index(i, 2).data().toInt();
        file << std::fixed << std::setw(10) << model->index(i, 3).data().toInt();
        file << std::fixed << std::setw(11) << std::setprecision(3)
             << model->index(i, 4).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3)
             << model->index(i, 5).data().toDouble();
        file << std::fixed << std::setw(12) << std::setprecision(3)
             << model->index(i, 6).data().toDouble();
        file << std::fixed << std::setw(11) << std::setprecision(3)
             << model->index(i, 7).data().toDouble();
        file << std::fixed << std::setw(13) << std::setprecision(3)
             << model->index(i, 8).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3)
             << model->index(i, 9).data().toDouble();
        file << std::fixed << std::setw(11) << std::setprecision(3)
             << model->index(i, 10).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3)
             << model->index(i, 11).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3)
             << model->index(i, 12).data().toDouble();
        file << std::endl;
    }

    file.close();
}

void FrameStatistics::saveMergedPeaks()
{
    QString format = _ui->merged_peaks_format->currentText();

    if (format.compare("ShelX") == 0) {
        saveToShelX(_ui->merged_peaks);
    } else if (format.compare("FullProf") == 0) {
        saveToFullProf(_ui->merged_peaks);
    }
}

void FrameStatistics::saveUnmergedPeaks()
{
    QString format = _ui->unmerged_peaks_format->currentText();

    if (format.compare("ShelX") == 0) {
        saveToShelX(_ui->unmerged_peaks);
    } else if (format.compare("FullProf") == 0) {
        saveToFullProf(_ui->unmerged_peaks);
    }
}

void FrameStatistics::saveToShelX(QTableView* table)
{

    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save peaks to ShelX"), ".", tr("ShelX hkl file (*.hkl)"));

    if (filename.isEmpty()) {
        return;
    }

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return;
    }

    auto model = dynamic_cast<QStandardItemModel*>(table->model());

    for (size_t i = 0; i < model->rowCount(); ++i) {
        // h
        file << std::fixed << std::setw(4) << model->index(i, 0).data().toInt();
        // k
        file << std::fixed << std::setw(4) << model->index(i, 1).data().toInt();
        // l
        file << std::fixed << std::setw(4) << model->index(i, 2).data().toInt();
        // I
        file << std::fixed << std::setw(8) << std::setprecision(2)
             << model->index(i, 3).data().toDouble();
        // sigma I
        file << std::fixed << std::setw(8) << std::setprecision(2)
             << model->index(i, 4).data().toDouble();
        file << std::endl;
    }

    file.close();
}

void FrameStatistics::saveToFullProf(QTableView* table)
{

    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save peaks to FullProf"), ".", tr("ShelX hkl file (*.hkl)"));

    if (filename.isEmpty()) {
        return;
    }

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return;
    }

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    double wavelength = _peaks[0]->data()->reader()->metadata().key<double>("wavelength");

    // wavelength
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    auto model = dynamic_cast<QStandardItemModel*>(table->model());

    for (size_t i = 0; i < model->rowCount(); ++i) {

        // h
        file << std::fixed << std::setw(4) << model->index(i, 0).data().toInt();
        // k
        file << std::fixed << std::setw(4) << model->index(i, 1).data().toInt();
        // l
        file << std::fixed << std::setw(4) << model->index(i, 2).data().toInt();
        // I
        file << std::fixed << std::setw(14) << std::setprecision(4)
             << model->index(i, 3).data().toDouble();
        // sigma I
        file << std::fixed << std::setw(14) << std::setprecision(4)
             << model->index(i, 4).data().toDouble();
        // dummy
        file << std::fixed << std::setw(5) << 1;
        file << std::endl;
    }

    file.close();
}

void FrameStatistics::update()
{
    bool include_friedel = _ui->friedel->isChecked();

    _merged_data = nsx::MergedData(_space_group, include_friedel);

    for (auto peak : _peaks) {
        _merged_data.addPeak(peak);
    }

    updateStatisticsTab();
    updateMergedPeaksTab();
    updateUnmergedPeaksTab();
    plotStatistics(_ui->selected_statistics->currentIndex());
}

void FrameStatistics::updateStatisticsTab()
{
    double dmin = _ui->dmin->value();
    double dmax = _ui->dmax->value();
    int n_shells = _ui->n_shells->value();

    bool include_friedel = _ui->friedel->isChecked();

    nsx::ResolutionShell resolution_shells(dmin, dmax, n_shells);
    for (auto peak : _peaks) {
        resolution_shells.addPeak(peak);
    }

    auto model = dynamic_cast<QStandardItemModel*>(_ui->statistics->model());

    model->removeRows(0, model->rowCount());

    // Write per-shell statistics

    for (int i = n_shells - 1; i >= 0; --i) {

        const double d_lower = resolution_shells.shell(i).dmin;
        const double d_upper = resolution_shells.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(_space_group, include_friedel);

        for (auto&& peak : resolution_shells.shell(i).peaks) {
            merged_data_per_shell.addPeak(peak);
        }

        nsx::CC cc;
        cc.calculate(merged_data_per_shell);
        nsx::RFactor rfactor;
        rfactor.calculate(merged_data_per_shell);

        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(d_upper)));
        row.append(new QStandardItem(QString::number(d_lower)));
        row.append(new QStandardItem(QString::number(merged_data_per_shell.totalSize())));
        row.append(new QStandardItem(QString::number(merged_data_per_shell.peaks().size())));
        row.append(new QStandardItem(QString::number(merged_data_per_shell.redundancy())));
        row.append(new QStandardItem(QString::number(rfactor.Rmeas())));
        row.append(new QStandardItem(QString::number(rfactor.expectedRmeas())));
        row.append(new QStandardItem(QString::number(rfactor.Rmerge())));
        row.append(new QStandardItem(QString::number(rfactor.expectedRmerge())));
        row.append(new QStandardItem(QString::number(rfactor.Rpim())));
        row.append(new QStandardItem(QString::number(rfactor.expectedRpim())));
        row.append(new QStandardItem(QString::number(cc.CChalf())));
        row.append(new QStandardItem(QString::number(cc.CCstar())));

        model->appendRow(row);
    }

    // Write overall statistics

    nsx::RFactor rfactor;
    rfactor.calculate(_merged_data);

    nsx::CC cc;
    cc.calculate(_merged_data);

    QList<QStandardItem*> row;
    row.append(new QStandardItem(QString::number(dmax)));
    row.append(new QStandardItem(QString::number(dmin)));
    row.append(new QStandardItem(QString::number(_merged_data.totalSize())));
    row.append(new QStandardItem(QString::number(_merged_data.peaks().size())));
    row.append(new QStandardItem(QString::number(_merged_data.redundancy())));
    row.append(new QStandardItem(QString::number(rfactor.Rmeas())));
    row.append(new QStandardItem(QString::number(rfactor.expectedRmeas())));
    row.append(new QStandardItem(QString::number(rfactor.Rmerge())));
    row.append(new QStandardItem(QString::number(rfactor.expectedRmerge())));
    row.append(new QStandardItem(QString::number(rfactor.Rpim())));
    row.append(new QStandardItem(QString::number(rfactor.expectedRpim())));
    row.append(new QStandardItem(QString::number(cc.CChalf())));
    row.append(new QStandardItem(QString::number(cc.CCstar())));
    for (auto v : row) {
        QFont font(v->font());
        font.setBold(true);
        v->setFont(font);
    }
    model->appendRow(row);
}

void FrameStatistics::updateMergedPeaksTab()
{
    // Clear the merged peaks model/table
    auto model = dynamic_cast<QStandardItemModel*>(_ui->merged_peaks->model());
    model->removeRows(0, model->rowCount());

    for (auto&& peak : _merged_data.peaks()) {

        const auto hkl = peak.index();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        auto I = peak.intensity();

        const double intensity = I.value();
        const double sigma = I.sigma();
        const int nobs = peak.redundancy();

        const double chi2 = peak.chi2();
        const double p = peak.pValue();

        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(h)));
        row.append(new QStandardItem(QString::number(k)));
        row.append(new QStandardItem(QString::number(l)));
        row.append(new QStandardItem(QString::number(intensity)));
        row.append(new QStandardItem(QString::number(sigma)));
        row.append(new QStandardItem(QString::number(nobs)));
        row.append(new QStandardItem(QString::number(chi2)));
        row.append(new QStandardItem(QString::number(p)));

        model->appendRow(row);
    }
}

void FrameStatistics::updateUnmergedPeaksTab()
{
    // Clear the unmerged peaks model/table
    auto model = dynamic_cast<QStandardItemModel*>(_ui->unmerged_peaks->model());
    model->removeRows(0, model->rowCount());

    for (auto&& peak : _merged_data.peaks()) {

        for (auto unmerged_peak : peak.peaks()) {

            const auto& cell = *(unmerged_peak->unitCell());
            const auto& q = unmerged_peak->q();

            const nsx::MillerIndex hkl(q, cell);

            const int h = hkl[0];
            const int k = hkl[1];
            const int l = hkl[2];

            auto c = unmerged_peak->shape().center();
            auto numor = unmerged_peak->data()->filename();
            auto I = unmerged_peak->correctedIntensity();

            const double intensity = I.value();
            const double sigma = I.sigma();

            const QFileInfo fileinfo(QString::fromStdString(numor));

            QList<QStandardItem*> row;
            row.append(new QStandardItem(QString::number(h)));
            row.append(new QStandardItem(QString::number(k)));
            row.append(new QStandardItem(QString::number(l)));
            row.append(new QStandardItem(QString::number(intensity)));
            row.append(new QStandardItem(QString::number(sigma)));
            row.append(new QStandardItem(QString::number(c[0])));
            row.append(new QStandardItem(QString::number(c[1])));
            row.append(new QStandardItem(QString::number(c[2])));
            row.append(new QStandardItem(fileinfo.baseName()));

            model->appendRow(row);
        }
    }
}
