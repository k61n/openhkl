#include <fstream>
#include <iomanip>
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
#include "PeaksUtils.h"
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

    auto&& drange = dRange(peaks);
    ui->dmin->setValue(drange.first);
    ui->dmax->setValue(drange.second);

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

    auto model = dynamic_cast<QStandardItemModel*>(ui->statistics->model());

    for (size_t i = 0; i < model->rowCount(); ++i) {

        file << std::fixed << std::setw(10) << std::setprecision(2) << model->index(i, 0).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(2) << model->index(i, 1).data().toDouble();
        file << std::fixed << std::setw(10) << model->index(i, 2).data().toInt();
        file << std::fixed << std::setw(10) << model->index(i, 3).data().toInt();
        file << std::fixed << std::setw(11) << std::setprecision(3) << model->index(i, 4).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3) << model->index(i, 5).data().toDouble();
        file << std::fixed << std::setw(12) << std::setprecision(3) << model->index(i, 6).data().toDouble();
        file << std::fixed << std::setw(11) << std::setprecision(3) << model->index(i, 7).data().toDouble();
        file << std::fixed << std::setw(13) << std::setprecision(3) << model->index(i, 8).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3) << model->index(i, 9).data().toDouble();
        file << std::fixed << std::setw(11) << std::setprecision(3) << model->index(i,10).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3) << model->index(i,11).data().toDouble();
        file << std::fixed << std::setw(10) << std::setprecision(3) << model->index(i,12).data().toDouble();
        file << std::endl;
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

    for (size_t i = 0; i < model->rowCount(); ++i) {
        // h
        file << std::fixed << std::setw(4) << model->index(i, 0).data().toInt();
        // k
        file << std::fixed << std::setw(4) << model->index(i, 1).data().toInt();
        // l
        file << std::fixed << std::setw(4) << model->index(i, 2).data().toInt();
        // I
        file << std::fixed << std::setw(8) << std::setprecision(2) << model->index(i, 3).data().toDouble();
        // sigma I
        file << std::fixed << std::setw(8) << std::setprecision(2) << model->index(i, 4).data().toDouble();
        file << std::endl;
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

    double wavelength = _peaks[0]->data()->metadata()->key<double>("wavelength");

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
        file << std::fixed << std::setw(14) << std::setprecision(4) << model->index(i, 3).data().toDouble();
        // sigma I
        file << std::fixed << std::setw(14) << std::setprecision(4) << model->index(i, 4).data().toDouble();
        // dummy
        file << std::fixed << std::setw(5) << 1;
        file << std::endl;
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

void DialogStatistics::updateMergedPeaksTab()
{
    // Clear the merged peaks model/table
    auto model = dynamic_cast<QStandardItemModel*>(ui->mergedPeaks->model());
    model->removeRows(0,model->rowCount());

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

void DialogStatistics::updateUnmergedPeaksTab()
{
    // Clear the unmerged peaks model/table
    auto model = dynamic_cast<QStandardItemModel*>(ui->unmergedPeaks->model());
    model->removeRows(0,model->rowCount());

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
