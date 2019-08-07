//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/MergedPeakInformationFrame.cpp
//! @brief     Implements class MergedPeakInformationFrame
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/frames/MergedPeakInformationFrame.h"

#include "core/analyse/MergedPeak.h"
#include "core/experiment/DataSet.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include "gui/models/Session.h"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <fstream>
#include <iomanip>

MergedPeakInformationFrame::MergedPeakInformationFrame(nsx::SpaceGroup group, nsx::PeakList list)
    : QcrFrame{"adhoc_Statistics"}
    , peakList{list}
    , spaceGroup{group}
    , mergedData{spaceGroup, false}
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QcrTabWidget* tabs = new QcrTabWidget("adhoc_StatisticTabs");
    layout->addWidget(tabs);
    QDialogButtonBox* button = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal);
    layout->addWidget(button);

    //statistics tab
    QcrWidget* statisticsTab = new QcrWidget("adhoc_TabStats");
    QVBoxLayout* statsLayout = new QVBoxLayout(statisticsTab);
    statisticsView = new QTableView;
    QStandardItemModel* statsModel = new QStandardItemModel(0, 13, this);
    statisticsView->setModel(statsModel);
    statsModel->setHorizontalHeaderLabels({"dmax", "dmin", "nobs", "nmerge", "redundancy", "Rmeas",
                                            "Rmeas(est.)", "Rmerge/Rsym", "Rmerge(est.)", "Rpim",
                                            "Rpim(est.)", "CChalf", "CC*"});
    statsLayout->addWidget(statisticsView);
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
    plottableStatistics = new QcrComboBox("adhoc_StatsPlottypes", new QcrCell<int>(0), selStats);
    downleft->addRow("Min d", dmin);
    downleft->addRow("Max d", dmax);
    downleft->addRow("# d-shells", dshells);
    downleft->addRow(friedel);
    downleft->addRow("Plot", plottableStatistics);
    statsDown->addLayout(downleft);
    statisticsPlot = new SXPlot;
    statsDown->addWidget(statisticsPlot);
    statsLayout->addLayout(statsDown);
    QcrTextTriggerButton* saveStatisticsButton =
            new QcrTextTriggerButton("adhoc_SaveStats", "Save");
    statsLayout->addWidget(saveStatisticsButton);

    //merged tab
    QcrWidget* mergedPeaksTab = new QcrWidget("adhoc_TabMerged");
    QVBoxLayout* mergedlayout = new QVBoxLayout(mergedPeaksTab);
    mergedView = new QTableView;
    QStandardItemModel* mergedModel = new QStandardItemModel(7, 0, this);
    mergedView->setModel(mergedModel);
    mergedModel->setHorizontalHeaderLabels({"h", "k", "l", "I", "sigmaI", "chi2", "p"});
    mergedlayout->addWidget(mergedView);
    QHBoxLayout* mergedrow = new QHBoxLayout;
    filetypesMerged = new QcrComboBox(
                "adhoc_FileTypesMerged", new QcrCell<int>(0), {"ShelX", "FullProf", "Phenix"});
    QcrTextTriggerButton* saveMerged = new QcrTextTriggerButton("adhoc_SaveMerged", "Save");
    mergedrow->addWidget(filetypesMerged);
    mergedrow->addWidget(saveMerged);
    mergedlayout->addLayout(mergedrow);

    //unmerged tab
    QcrWidget* unmergedPeaksTab = new QcrWidget("adhoc_TabUnmerged");
    QVBoxLayout* unmergedlayout = new QVBoxLayout(unmergedPeaksTab);
    unmergedView = new QTableView;
    QStandardItemModel* unmergedModel = new QStandardItemModel(9, 0, this);
    unmergedView->setModel(unmergedModel);
    unmergedModel->setHorizontalHeaderLabels(
        {"h", "k", "l", "I", "sigmaI", "x", "y", "frame", "numor"});
    unmergedlayout->addWidget(unmergedView);
    QHBoxLayout* unmergedrow = new QHBoxLayout;
    filetypesUnmerged = new QcrComboBox(
                "adhoc_FileTypesUnmerged", new QcrCell<int>(0), {"ShelX", "FullProf", "Phenix"});
    QcrTextTriggerButton* saveUnmerged = new QcrTextTriggerButton("adhoc_SaveUnmerged", "Save");
    unmergedrow->addWidget(filetypesUnmerged);
    unmergedrow->addWidget(saveUnmerged);
    unmergedlayout->addLayout(unmergedrow);

    tabs->addTab(statisticsTab, "Statistics");
    tabs->addTab(mergedPeaksTab, "Merged hkls");
    tabs->addTab(unmergedPeaksTab, "Unmerged hkls");

    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(Qt::Window);
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    QObject::connect(button, &QDialogButtonBox::accepted, this, &MergedPeakInformationFrame::close);
    dmin->setHook([=](double) { update(); });
    dmax->setHook([=](double) { update(); });
    dshells->setHook([=](int) { update(); });
    friedel->setHook([=](bool) { update(); });
    saveStatisticsButton->trigger()->setTriggerHook([=]() { saveStatistics(); });
    saveMerged->trigger()->setTriggerHook([=]() { saveMergedPeaks(); });
    saveUnmerged->trigger()->setTriggerHook([=]() { saveUnmergedPeaks(); });
    plottableStatistics->setHook([=](int i) { plotStatistics(i); });
    update();

    show();
}

void MergedPeakInformationFrame::plotStatistics(int column)
{
    QStandardItemModel* statsModel = dynamic_cast<QStandardItemModel*>(statisticsView->model());
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

    statisticsPlot->clearGraphs();
    statisticsPlot->addGraph();
    statisticsPlot->graph(0)->setPen(pen);
    statisticsPlot->graph(0)->addData(xvals, yvals);
    statisticsPlot->xAxis->setLabel("shell");
    statisticsPlot->yAxis->setLabel(plottableStatistics->itemText(column));
    statisticsPlot->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    statisticsPlot->xAxis->setTickLabelFont(font);
    statisticsPlot->yAxis->setTickLabelFont(font);

    statisticsPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes
                                    | QCP::iSelectLegend | QCP::iSelectPlottables);
    statisticsPlot->rescaleAxes();
    statisticsPlot->replot();
}

void MergedPeakInformationFrame::saveStatistics()
{
    QString filename = QFileDialog::getSaveFileName(
        this, 
        tr("Save the shell info"), 
        ".", 
        tr("(*.txt)"));

    if (filename.isEmpty())
        return;

    double min = dmin->value();
    double max = dmax->value();
    int shells = dshells->value();
    bool inclFriedel = friedel->isChecked();
    nsx::ResolutionShell resolutionShell(min, max, shells);
    for (nsx::sptrPeak3D peak : peakList)
        resolutionShell.addPeak(peak);
        
    exporter.saveStatistics(
        filename.toStdString(),
        resolutionShell,
        spaceGroup,
        inclFriedel);
}

void MergedPeakInformationFrame::saveMergedPeaks()
{
    QString format = filetypesMerged->currentText();

    if (format.compare("ShelX") == 0){
        QString filename = QFileDialog::getSaveFileName(
            this, 
            tr("Save peaks to ShelX"), 
            ".", 
            tr("ShelX hkl file (*.hkl)"));

        if (filename.isEmpty())
            return;

        exporter.saveToShelX(
            filename.toStdString(),
            &mergedData);

    } else if (format.compare("FullProf") == 0){
        QString filename = QFileDialog::getSaveFileName(
            this, 
            tr("Save peaks to FullProf"), 
            ".", 
            tr("FullProf hkl file (*.hkl)"));

        if (filename.isEmpty())
            return;

        exporter.saveToFullProf(
            filename.toStdString(),
            &mergedData,
            &peakList);

    } else if (format.compare("Phenix") == 0){
        QString filename = QFileDialog::getSaveFileName(
            this, 
            tr("Save peaks to Phenix sca"), 
            ".", 
            tr("Phenix sca file (*.sca)"));

        if (filename.isEmpty())
            return;

        exporter.saveToSCA(
            filename.toStdString(),
            &mergedData,
            &peakList);
    }
}

void MergedPeakInformationFrame::saveUnmergedPeaks()
{
    QString format = filetypesUnmerged->currentText();

    if (format.compare("ShelX") == 0){
        QString filename = QFileDialog::getSaveFileName(
            this, 
            tr("Save peaks to ShelX"), 
            ".", 
            tr("ShelX hkl file (*.hkl)"));

        if (filename.isEmpty())
            return;

        exporter.saveToShelX(
            filename.toStdString(),
            &peakList);

    } else if (format.compare("FullProf") == 0){
        QString filename = QFileDialog::getSaveFileName(
            this, 
            tr("Save peaks to FullProf"), 
            ".", 
            tr("ShelX hkl file (*.hkl)"));

        if (filename.isEmpty())
            return;

        exporter.saveToFullProf(
            filename.toStdString(),
            &peakList);

    } else if (format.compare("Phenix") == 0){
        QString filename = QFileDialog::getSaveFileName(
            this, 
            tr("Save peaks to Phenix sca"), 
            ".", 
            tr("Phenix sca file (*.sca)"));

        if (filename.isEmpty())
            return;

        exporter.saveToSCA(
            filename.toStdString(),
            &peakList);
    }
}

void MergedPeakInformationFrame::update()
{
    bool inclFriedel = friedel->isChecked();
    mergedData = nsx::MergedData(spaceGroup, inclFriedel);
    for (nsx::sptrPeak3D peak : peakList)
        mergedData.addPeak(peak);

    updateStatisticsTab();
    updateMergedPeaksTab();
    updateUnmergedPeaksTab();
    plotStatistics(plottableStatistics->currentIndex());
}

void MergedPeakInformationFrame::updateStatisticsTab()
{
    double min = dmin->value();
    double max = dmax->value();
    int shells = dshells->value();
    bool inclFriedel = friedel->isChecked();
    nsx::ResolutionShell resolutionShell(min, max, shells);
    for (nsx::sptrPeak3D peak : peakList)
        resolutionShell.addPeak(peak);
    QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(statisticsView->model());
    model->removeRows(0, model->rowCount());

    for (int i = shells - 1; i >= 0; --i) {
        const double d_lower = resolutionShell.shell(i).dmin;
        const double d_upper = resolutionShell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(spaceGroup, inclFriedel);

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

    // nsx::RFactor rfactor;
    // rfactor.calculate(mergedData);

    // nsx::CC cc;
    // cc.calculate(mergedData);

    // QList<QStandardItem*> row;
    // row.append(new QStandardItem(QString::number(max)));
    // row.append(new QStandardItem(QString::number(min)));
    // row.append(new QStandardItem(QString::number(mergedData.totalSize())));
    // row.append(new QStandardItem(QString::number(mergedData.mergedPeakSet().size())));
    // row.append(new QStandardItem(QString::number(mergedData.redundancy())));
    // row.append(new QStandardItem(QString::number(rfactor.Rmeas())));
    // row.append(new QStandardItem(QString::number(rfactor.expectedRmeas())));
    // row.append(new QStandardItem(QString::number(rfactor.Rmerge())));
    // row.append(new QStandardItem(QString::number(rfactor.expectedRmerge())));
    // row.append(new QStandardItem(QString::number(rfactor.Rpim())));
    // row.append(new QStandardItem(QString::number(rfactor.expectedRpim())));
    // row.append(new QStandardItem(QString::number(cc.CChalf())));
    // row.append(new QStandardItem(QString::number(cc.CCstar())));
    // for (auto v : row) {
    //     QFont font(v->font());
    //     font.setBold(true);
    //     v->setFont(font);
    // }
    // model->appendRow(row);
}

void MergedPeakInformationFrame::updateMergedPeaksTab()
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

void MergedPeakInformationFrame::updateUnmergedPeaksTab()
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
