#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QModelIndex>
#include <QPushButton>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QString>

#include <nsxlib/CC.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/MillerIndex.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ResolutionShell.h>
#include <nsxlib/RFactor.h>

#include "DialogStatistics.h"
#include "ui_DialogStatistics.h"

DialogStatistics::DialogStatistics(const nsx::PeakList& peaks, const nsx::SpaceGroup& spaceGroup, QWidget *parent)
: QDialog(parent),
  ui(new Ui::DialogStatistics),
  _peaks(peaks),
  _spaceGroup(spaceGroup),
  _merged_data(spaceGroup,true)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);

    double dmin(std::numeric_limits<double>::infinity());
    double dmax(-std::numeric_limits<double>::infinity());
    for (auto peak : peaks) {
        double d = 1.0/peak->q().rowVector().norm();
        dmin = std::min(dmin,d);
        dmax = std::max(dmax,d);
    }

    ui->dmin->setValue(dmin);
    ui->dmax->setValue(dmax);

    QStandardItemModel* shell_model = new QStandardItemModel(0,13,this);
    ui->statistics->setModel(shell_model);
    shell_model->setHorizontalHeaderLabels({"dmax","dmin","nobs","nmerge","redundancy",
                                            "Rmeas","Rmeas(est.)",
                                            "Rmerge/Rsym","Rmerge(est.)",
                                            "Rpim","Rpim(est.)",
                                            "CChalf","CC*"});

    QStandardItemModel* merged_peaks_model = new QStandardItemModel(7,0,this);
    ui->mergedPeaks->setModel(merged_peaks_model);
    merged_peaks_model->setHorizontalHeaderLabels({"h","k","l","I","sigmaI","chi2","p"});

    QStandardItemModel* unmerged_peaks_model = new QStandardItemModel(9,0,this);
    ui->unmergedPeaks->setModel(unmerged_peaks_model);
    unmerged_peaks_model->setHorizontalHeaderLabels({"h","k","l","I","sigmaI","x","y","frame","numor"});

    update();

    connect(ui->dmin,SIGNAL(valueChanged(double)),this,SLOT(update()));
    connect(ui->dmax,SIGNAL(valueChanged(double)),this,SLOT(update()));
    connect(ui->nShells,SIGNAL(valueChanged(int)),this,SLOT(update()));
    connect(ui->friedel,SIGNAL(stateChanged(int)),this,SLOT(update()));

    connect(ui->saveStatistics,SIGNAL(clicked()),this,SLOT(saveStatistics()));
    connect(ui->saveMergedPeaks,SIGNAL(clicked()),this,SLOT(saveMergedPeaks()));
    connect(ui->saveUnmergedPeaks,SIGNAL(clicked()),this,SLOT(saveUnmergedPeaks()));
}

DialogStatistics::~DialogStatistics()
{
    delete ui;
}

void DialogStatistics::saveStatistics()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save statistics"), ".", tr("Text file (*.dat *.txt)"));

    if (filename.isEmpty()) {
        return;
    }

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return;
    }

    std::vector<char> buf(1024, 0); // buffer for snprintf

    std::snprintf(&buf[0], buf.size(),
            " %10s %10s %10s %10s %10s %10s %11s %10s %12s %10s %10s %10s %10s",
            "dmax","dmin","nobs","nmerged","redundancy","Rmeas","Rmeas(est.)","Rmerge/sym","Rmerge(est.)","Rpim","Rpim(est.)","CChalf","CC*");

    file << &buf[0] << std::endl;

    auto model = dynamic_cast<QStandardItemModel*>(ui->statistics->model());

    for (size_t i = 0; i < model->rowCount(); ++i) {
        std::snprintf(&buf[0], buf.size(),
                " %10.2f %10.2f" // dmax, dmin
                " %10zd %10zd %10.3f" // nobs, nmerge, redundancy
                " %10.3f %11.3f" // Rmeas, expected Rmeas
                " %10.3f %12.3f" // Rmerge, expected Rmerge
                " %10.3f %10.3f" // Rpim, expected Rpim
                " %10.3f %10.3f", // CC half, CC*
                model->index(i, 0).data().toDouble(),
                model->index(i, 1).data().toDouble(),
                model->index(i, 2).data().toInt(),
                model->index(i, 3).data().toInt(),
                model->index(i, 4).data().toDouble(),
                model->index(i, 5).data().toDouble(),
                model->index(i, 6).data().toDouble(),
                model->index(i, 7).data().toDouble(),
                model->index(i, 8).data().toDouble(),
                model->index(i, 9).data().toDouble(),
                model->index(i,10).data().toDouble(),
                model->index(i,11).data().toDouble(),
                model->index(i,12).data().toDouble(),
                model->index(i,13).data().toDouble());
        file << &buf[0] << std::endl;
    }

    file.close();
}

void DialogStatistics::saveMergedPeaks()
{
    QString format = ui->mergedPeaksFormat->currentText();

    if (format.compare("ShelX")==0) {
        saveToShelX(ui->mergedPeaks);
    } else if (format.compare("FullProf")==0) {
        saveToFullProf(ui->mergedPeaks);
    }
}

void DialogStatistics::saveUnmergedPeaks()
{
    QString format = ui->unmergedPeaksFormat->currentText();

    if (format.compare("ShelX")==0) {
        saveToShelX(ui->unmergedPeaks);
    } else if (format.compare("FullProf")==0) {
        saveToFullProf(ui->unmergedPeaks);
    }
}

void DialogStatistics::saveToShelX(QTableView* table) {

    QString filename = QFileDialog::getSaveFileName(this, tr("Save peaks to ShelX"), ".", tr("ShelX hkl file (*.hkl)"));

    if (filename.isEmpty()) {
        return;
    }

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        nsx::error() << "Error writing to this file, please check write permisions";
        return;
    }

    auto model = dynamic_cast<QStandardItemModel*>(table->model());

    // buffer for snprintf
    std::vector<char> buf(1024, 0);

    for (size_t i = 0; i < model->rowCount(); ++i) {
        std::snprintf(&buf[0], buf.size(),
                "%4d%4d%4d%8.2f%8.2f",
                model->index(i, 0).data().toInt(),     // h
                model->index(i, 1).data().toInt(),     // k
                model->index(i, 2).data().toInt(),     // l
                model->index(i, 3).data().toDouble(),  // I
                model->index(i, 4).data().toDouble()); // sigmaI
        file << &buf[0] << std::endl;
    }

    file.close();
}

void DialogStatistics::saveToFullProf(QTableView* table) {

    QString filename = QFileDialog::getSaveFileName(this, tr("Save peaks to FullProf"), ".", tr("ShelX hkl file (*.hkl)"));

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

    double wave = _peaks[0]->data()->metadata()->getKey<double>("wavelength");

    std::vector<char> buf(1024, 0); // buffer for snprintf
    std::snprintf(&buf[0], buf.size(),"%8.3f",wave);
    file << &buf[0] << " 0 0" << std::endl;

    auto model = dynamic_cast<QStandardItemModel*>(table->model());

    for (size_t i = 0; i < model->rowCount(); ++i) {
        buf.assign(1024,0);
        std::snprintf(&buf[0], buf.size(),
                "%4d%4d%4d%14.4f%14.4f%5d",
                model->index(i, 0).data().toInt(),    // h
                model->index(i, 1).data().toInt(),    // k
                model->index(i, 2).data().toInt(),    // l
                model->index(i, 3).data().toDouble(), // I
                model->index(i, 4).data().toDouble(), // sigmaI
                1);
        file << &buf[0] << std::endl;
    }

    file.close();
}

void DialogStatistics::update()
{
    bool include_friedel = ui->friedel->isChecked();

    _merged_data = nsx::MergedData(_spaceGroup,include_friedel);

    for (auto peak : _peaks) {
        _merged_data.addPeak(peak);
    }

    updateStatisticsTab();
    updateMergedPeaksTab();
    updateUnmergedPeaksTab();
}

void DialogStatistics::updateStatisticsTab()
{
    double dmin  = ui->dmin->value();
    double dmax  = ui->dmax->value();
    int n_shells = ui->nShells->value();

    bool include_friedel = ui->friedel->isChecked();

    nsx::ResolutionShell resolution_shells(dmin, dmax, n_shells);
    for (auto peak : _peaks) {
        resolution_shells.addPeak(peak);
    }

    auto model = dynamic_cast<QStandardItemModel*>(ui->statistics->model());

    model->removeRows(0,model->rowCount());

    // Write per-shell statistics

    for (int i = n_shells-1; i >= 0; --i) {

        const double d_lower = resolution_shells.shell(i).dmin;
        const double d_upper = resolution_shells.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(_spaceGroup, include_friedel);

        for (auto&& peak: resolution_shells.shell(i).peaks) {
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
        row.append(new QStandardItem(QString::number(merged_data_per_shell.getPeaks().size())));
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
    row.append(new QStandardItem(QString::number(_merged_data.getPeaks().size())));
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

void DialogStatistics::updateMergedPeaksTab()
{
    // Clear the merged peaks model/table
    auto model = dynamic_cast<QStandardItemModel*>(ui->mergedPeaks->model());
    model->removeRows(0,model->rowCount());

    for (auto&& peak : _merged_data.getPeaks()) {

        const auto hkl = peak.getIndex();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        auto I = peak.getIntensity();

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

void DialogStatistics::updateUnmergedPeaksTab()
{
    // Clear the unmerged peaks model/table
    auto model = dynamic_cast<QStandardItemModel*>(ui->unmergedPeaks->model());
    model->removeRows(0,model->rowCount());

    for (auto&& peak : _merged_data.getPeaks()) {

        for (auto unmerged_peak : peak.getPeaks()) {

            const auto& cell = *(unmerged_peak->activeUnitCell());
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
