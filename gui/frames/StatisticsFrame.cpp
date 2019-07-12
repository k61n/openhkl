//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/StatisticsFrame.cpp
//! @brief     Implements class StatisticsFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/StatisticsFrame.h"

#include "core/analyse/MergedPeak.h"
#include "core/experiment/DataSet.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <fstream>
#include <iomanip>

StatisticsFrame::StatisticsFrame() : QcrFrame{"adhoc_Statistics"}
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    tabs = new QcrTabWidget("adhoc_StatisticTabs");
    layout->addWidget(tabs);
    QDialogButtonBox* button = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal);
    layout->addWidget(button);

    //statistics tab
    stats = new QcrWidget("adhoc_TabStats");
    QVBoxLayout* statsLayout = new QVBoxLayout(stats);
    viewStats = new QTableView;
    QStandardItemModel* statsModel = new QStandardItemModel(0, 13, this);
    viewStats->setModel(statsModel);
    statsModel->setHorizontalHeaderLabels({"dmax", "dmin", "nobs", "nmerge", "redundancy", "Rmeas",
                                            "Rmeas(est.)", "Rmerge/Rsym", "Rmerge(est.)", "Rpim",
                                            "Rpim(est.)", "CChalf", "CC*"});
    statsLayout->addWidget(viewStats);
    QHBoxLayout* statsDown = new QHBoxLayout;
    QFormLayout* downleft = new QFormLayout;
    dmin = new QcrDoubleSpinBox("adhoc_StatsDMin", new QcrCell<double>(1.5), 5, 2);
    dmax = new QcrDoubleSpinBox("adhoc_StatsDMax", new QcrCell<double>(50.0), 5, 2);
    dshells = new QcrSpinBox("adhoc_StatsShells", new QcrCell<int>(10), 5);
    friedel = new QcrCheckBox("adhoc_StatsFriedel", "include Friedel", new QcrCell<bool>(false));
    QStringList selStats;
    for (int i = 0; i < statsModel->columnCount(); ++i) {
        QStandardItem* header_item = statsModel->horizontalHeaderItem(i);
        selStats.append(header_item->text());
    }
    selectedStats = new QcrComboBox("adhoc_StatsPlottypes", new QcrCell<int>(0), selStats);
    downleft->addRow("Min d", dmin);
    downleft->addRow("Max d", dmax);
    downleft->addRow("# d-shells", dshells);
    downleft->addRow(friedel);
    downleft->addRow("Plot", selectedStats);
    statsDown->addLayout(downleft);
    plot = new SXPlot;
    statsDown->addWidget(plot);
    statsLayout->addLayout(statsDown);
    saveStats = new QcrTextTriggerButton("adhoc_SaveStats", "Save");
    statsLayout->addWidget(saveStats);

    //merged tab
    merged = new QcrWidget("adhoc_TabMerged");
    QVBoxLayout* mergedlayout = new QVBoxLayout(merged);
    mergedView = new QTableView;
    QStandardItemModel* mergedModel = new QStandardItemModel(7, 0, this);
    mergedView->setModel(mergedModel);
    mergedModel->setHorizontalHeaderLabels({"h", "k", "l", "I", "sigmaI", "chi2", "p"});
    mergedlayout->addWidget(mergedView);
    QHBoxLayout* mergedrow = new QHBoxLayout;
    typesMerged = new QcrComboBox("adhoc_FileTypesMerged", new QcrCell<int>(0), {""});
    saveMerged = new QcrTextTriggerButton("adhoc_SaveMerged", "Save");
    mergedrow->addWidget(typesMerged);
    mergedrow->addWidget(saveMerged);
    mergedlayout->addLayout(mergedrow);

    //unmerged tab
    unmerged = new QcrWidget("adhoc_TabUnmerged");
    QVBoxLayout* unmergedlayout = new QVBoxLayout(unmerged);
    unmergedView = new QTableView;
    QStandardItemModel* unmergedModel = new QStandardItemModel(9, 0, this);
    unmergedView->setModel(unmergedModel);
    unmergedModel->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", "sigmaI", "x", "y", "frame", "numor"});
    unmergedlayout->addWidget(unmergedView);
    QHBoxLayout* unmergedrow = new QHBoxLayout;
    typesUnmerged = new QcrComboBox("adhoc_FileTypesUnmerged", new QcrCell<int>(0), {""});
    saveUnmerged = new QcrTextTriggerButton("adhoc_SaveUnmerged", "Save");
    unmergedrow->addWidget(typesUnmerged);
    unmergedrow->addWidget(saveUnmerged);
    unmergedlayout->addLayout(unmergedrow);

    tabs->addTab(stats, "Statistics");
    tabs->addTab(merged, "Merged hkls");
    tabs->addTab(unmerged, "Unmerged hkls");

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    connect(button, &QDialogButtonBox::accepted, this, &StatisticsFrame::close);
    connect(dmin, &QcrDoubleSpinBox::valueChanged, this, &StatisticsFrame::update);
    connect(dmax, &QcrDoubleSpinBox::valueChanged, this, &StatisticsFrame::update);
    connect(dshells, &QcrSpinBox::valueChanged, this, &StatisticsFrame::update);
    connect(friedel, &QcrCheckBox::stateChanged, this, &StatisticsFrame::update);
    connect(saveStats, &QcrTextTriggerButton::triggered, this, &StatisticsFrame::saveStatistics);
    connect(saveMerged, &QcrTextTriggerButton::triggered, this, &StatisticsFrame::saveMergedPeaks);
    connect(saveUnmerged, &QcrTextTriggerButton::triggered,
            this, &StatisticsFrame::saveUnmergedPeaks);
    connect(selectedStats, &QcrComboBox::currentIndexChanged,
            this, &StatisticsFrame::plotStatistics);
    update();

    show();
}

void StatisticsFrame::plotStatistics(int column)
{
    QStandardItemModel* statsModel = dynamic_cast<QStandardItemModel*>(viewStats->model());
    int nshells = statsModel->rowCount() - 1;
    std::vector<double> shells(nshells);
    std::iota(shells.begin(), shells.end(), 0);

    QVector<double> xvals = QVector<double>::fromStdVector(shells);
    QVector<double> yvals;

    for (int i = 0; i < nshells; ++i) {
        double val = statsModel->item(i, column)->data(Qt::DisplayRole).value<double>();
        yvals.append(val);
    }

    QPen pen;
    pen.setColor(QColor("black"));
    pen.setWidth(2.0);

    plot->clearGraphs();
    plot->addGraph();
    plot->graph(0)->setPen(pen);
    plot->graph(0)->addData(xvals, yvals);
    plot->xAxis->setLabel("shell");
    plot->yAxis->setLabel(selectedStats->itemText(column));
    plot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    plot->xAxis->setTickLabelFont(font);
    plot->yAxis->setTickLabelFont(font);

    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend
                          | QCP::iSelectPlottables);
    plot->rescaleAxes();
    plot->replot();
}

void StatisticsFrame::saveStatistics()
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save statistics"), ".", tr("Text file (*.dat *.txt)"));

    if (filename.isEmpty())
        return;

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        qWarning() << "Error writing to this file, please check write permisions";
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

    auto model = dynamic_cast<QStandardItemModel*>(viewStats->model());

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

void StatisticsFrame::saveMergedPeaks()
{
    QString format = typesMerged->currentText();

    if (format.compare("ShelX") == 0)
        saveToShelX(mergedView);
    else
        saveToFullProf(mergedView);
}

void StatisticsFrame::saveUnmergedPeaks()
{
    QString format = typesUnmerged->currentText();

    if (format.compare("ShelX") == 0)
        saveToShelX(unmergedView);
    else
        saveToFullProf(unmergedView);
}

void StatisticsFrame::saveToShelX(QTableView* table)
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save peaks to ShelX"), ".", tr("ShelX hkl file (*.hkl)"));

    if (filename.isEmpty())
        return;

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        qWarning() << "Error writing to this file, please check write permisions";
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

void StatisticsFrame::saveToFullProf(QTableView* table)
{
    QString filename = QFileDialog::getSaveFileName(
        this, tr("Save peaks to FullProf"), ".", tr("ShelX hkl file (*.hkl)"));

    if (filename.isEmpty())
        return;

    std::fstream file(filename.toStdString(), std::ios::out);
    if (!file.is_open()) {
        qWarning() << "Error writing to this file, please check write permisions";
        return;
    }

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    double wavelength = peaks[0]->data()->reader()->metadata().key<double>("wavelength");

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

void StatisticsFrame::update()
{
    bool inclFriedel = friedel->isChecked();
    mergedData = nsx::MergedData(space, inclFriedel);
    for (nsx::sptrPeak3D peak : peaks)
        mergedData.addPeak(peak);

    updateStatisticsTab();
    updateMergedPeaksTab();
    updateUnmergedPeaksTab();
    plotStatistics(selectedStats->currentIndex());
}

void StatisticsFrame::updateStatisticsTab()
{
    double min = dmin->value();
    double max = dmax->value();
    int shells = dshells->value();
    bool inclFriedel = friedel->isChecked();
    nsx::ResolutionShell resolutionShell(min, max, shells);
    for (nsx::sptrPeak3D peak : peaks)
        resolutionShell.addPeak(peak);
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(viewStats->model());
    model->removeRows(0, model->rowCount());

    for (int i = shells - 1; i >= 0; --i) {
        const double d_lower = resolutionShell.shell(i).dmin;
        const double d_upper = resolutionShell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(space, inclFriedel);

        for (const nsx::sptrPeak3D& peak : resolutionShell.shell(i).peaks)
            merged_data_per_shell.addPeak(peak);

        nsx::CC cc;
        cc.calculate(merged_data_per_shell);
        nsx::RFactor rfactor;
        rfactor.calculate(merged_data_per_shell);

        QList<QStandardItem*> row;
        row.append(new QStandardItem(QString::number(d_upper)));
        row.append(new QStandardItem(QString::number(d_lower)));
        row.append(new QStandardItem(QString::number(merged_data_per_shell.totalSize())));
        row.append(
            new QStandardItem(QString::number(merged_data_per_shell.mergedPeakSet().size())));
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

    nsx::RFactor rfactor;
    rfactor.calculate(mergedData);

    nsx::CC cc;
    cc.calculate(mergedData);

    QList<QStandardItem*> row;
    row.append(new QStandardItem(QString::number(dmax)));
    row.append(new QStandardItem(QString::number(dmin)));
    row.append(new QStandardItem(QString::number(mergedData.totalSize())));
    row.append(new QStandardItem(QString::number(mergedData.mergedPeakSet().size())));
    row.append(new QStandardItem(QString::number(mergedData.redundancy())));
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

void StatisticsFrame::updateMergedPeaksTab()
{
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(mergedView->model());
    model->removeRows(0, model->rowCount());

    for (const nsx::MergedPeak& peak : mergedData.mergedPeakSet()) {
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

void StatisticsFrame::updateUnmergedPeaksTab()
{
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(unmergedView->model());
    model->removeRows(0, model->rowCount());

    for (const nsx::MergedPeak& peak : mergedData.mergedPeakSet()) {
        for (auto unmerged_peak : peak.peaks()) {
            const nsx::UnitCell& cell = *(unmerged_peak->unitCell());
            const nsx::ReciprocalVector& q = unmerged_peak->q();

            const nsx::MillerIndex hkl(q, cell);

            const int h = hkl[0];
            const int k = hkl[1];
            const int l = hkl[2];

            const Eigen::Vector3d& c = unmerged_peak->shape().center();
            std::string numor = unmerged_peak->data()->filename();
            nsx::Intensity I = unmerged_peak->correctedIntensity();

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
