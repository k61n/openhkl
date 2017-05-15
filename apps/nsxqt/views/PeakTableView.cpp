/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2016- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
    j.fisher[at]fz-juelich.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "PeakTableView.h"

#include <fstream>
#include <iostream>
#include <set>
#include <memory>
#include <cstdio>

#include <QContextMenuEvent>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QtDebug>
#include <QMessageBox>

#include <nsxlib/data/IData.h>
#include <nsxlib/crystal/Peak3D.h>

#include <nsxlib/utils/ProgressHandler.h>
#include "ProgressView.h"
#include "dialogs/LogFileDialog.h"

#include <nsxlib/crystal/ResolutionShell.h>
#include <nsxlib/crystal/MergedPeak.h>

#include "models/CollectedPeaksDelegate.h"
#include "models/CollectedPeaksModel.h"

#include "dialogs/DialogAutoIndexing.h"
#include "dialogs/DialogRefineUnitCell.h"
#include "dialogs/DialogTransformationMatrix.h"
#include "dialogs/DialogUnitCellParameters.h"

PeakTableView::PeakTableView(QWidget *parent)
: QTableView(parent),
  _normalized(false),
  _friedel(false)
{
    setEditTriggers(QAbstractItemView::SelectedClicked);
    // Selection of a cell in the table select the whole line.
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //
    setMinimumSize(800,400);
    setFocusPolicy(Qt::StrongFocus);

    auto delegate = new CollectedPeaksDelegate(this);
    setItemDelegate(delegate);
    QHeaderView* vertical = this->verticalHeader();
    connect(vertical,SIGNAL(sectionClicked(int)),this,SLOT(plotSelectedPeak(int)));

}

void PeakTableView::plotSelectedPeak(int index)
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    auto peaks = peaksModel->getPeaks();
    if (peaks.empty()) {
        return;
    }
    if (index < 0 || index >= peaks.size()) {
        return;
    }
    sptrPeak3D peak=peaks[index];
    emit plotPeak(peak);
}

void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty()) {
        return;
    }
    // Show all peaks as selected when context menu is requested
    auto menu = new QMenu(this);
    //
    QAction* sortbyEquivalence=new QAction("Sort by equivalences",menu);
    menu->addAction(sortbyEquivalence);
    connect(sortbyEquivalence,SIGNAL(triggered()),peaksModel,SLOT(sortEquivalents()));

    auto normalize = new QAction("Normalize to monitor", menu);
    menu->addSeparator();
    menu->addAction(normalize);
    auto writeMenu = menu->addMenu("Write");
    auto writeFullProf = new QAction("FullProf file", writeMenu);
    auto writeShelX = new QAction("SHELX file", writeMenu);
    writeMenu->addAction(writeFullProf);
    writeMenu->addAction(writeShelX);

    // Menu to plot against metadata
    QModelIndexList indexList = selectionModel()->selectedIndexes();

    if (indexList.size()) {
        QMenu* plotasmenu=menu->addMenu("Plot as");
        nsx::Data::MetaData* met=peaks[indexList[0].row()]->getData()->getMetadata();
        const std::set<std::string>& keys=met->getAllKeys();
        for (const auto& key : keys) {
            try {
                //Ensure metadata is a Numeric type
                met->getKey<double>(key);
            } catch(std::exception& e) {
                continue;
            }
            QAction* newparam=new QAction(QString::fromStdString(key),plotasmenu);
            connect(newparam,&QAction::triggered,this,[&](){plotAs(key);});
            plotasmenu->addAction(newparam);
        }
    }
    menu->addSeparator();
    QMenu* selectionMenu=menu->addMenu("Selection");
    auto selectAllPeaks=new QAction("all peaks",menu);
    auto selectValidPeaks=new QAction("valid peaks",menu);
    auto selectUnindexedPeaks=new QAction("unindexed peaks",menu);
    auto clearSelectedPeaks=new QAction("clear selection",menu);
    auto togglePeaksSelection=new QAction("toggle",menu);
    selectionMenu->addAction(selectAllPeaks);
    selectionMenu->addAction(selectValidPeaks);
    selectionMenu->addAction(selectUnindexedPeaks);
    selectionMenu->addSeparator();
    selectionMenu->addAction(clearSelectedPeaks);
    selectionMenu->addSeparator();
    selectionMenu->addAction(togglePeaksSelection);

    menu->addSeparator();

    auto autoIndexing=new QAction("Auto-indexing",menu);
    menu->addAction(autoIndexing);
    auto refineParameters=new QAction("Refine unit cell and instrument parameters",menu);
    menu->addAction(refineParameters);

    connect(normalize,SIGNAL(triggered()),this,SLOT(normalizeToMonitor()));
    connect(writeFullProf,SIGNAL(triggered()),this,SLOT(writeFullProf()));
    connect(writeShelX,SIGNAL(triggered()),this,SLOT(writeShelX()));
    menu->popup(event->globalPos());

    connect(clearSelectedPeaks,SIGNAL(triggered()),this,SLOT(clearSelectedPeaks()));
    connect(selectAllPeaks,SIGNAL(triggered()),this,SLOT(selectAllPeaks()));
    connect(selectValidPeaks,SIGNAL(triggered()),this,SLOT(selectValidPeaks()));
    connect(selectUnindexedPeaks,SIGNAL(triggered()),this,SLOT(selectUnindexedPeaks()));
    connect(togglePeaksSelection,SIGNAL(triggered()),this,SLOT(togglePeaksSelection()));

    connect(autoIndexing,SIGNAL(triggered()),this,SLOT(openAutoIndexingDialog()));
    connect(refineParameters,SIGNAL(triggered()),this,SLOT(openRefiningParametersDialog()));
}

void PeakTableView::normalizeToMonitor()
{
    bool ok;
    double factor = QInputDialog::getDouble(this,"Enter normalization factor","",1,1,100000000,1,&ok);

    if(!ok) {
        return;
    }

    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty()) {
        return;
    }
    peaksModel->normalizeToMonitor(factor);

    // Keep track of the last selected index before rebuilding the table
    QModelIndex index=currentIndex();
    _normalized=true;
    selectRow(index.row());

    // If no row selected do nothing else.
    if (!index.isValid()) {
        return;
    }
    sptrPeak3D peak=peaks[index.row()];
    emit plotPeak(peak);
}

void PeakTableView::writeFullProf()
{
    if (!checkBeforeWritting()) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save FullProf file"),
                                                    QString::fromStdString(getPeaksRange()+".int"),
                                                    tr("FullProf Files (*.int)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);


    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    peaksModel->writeFullProf(filename.toStdString());
}

void PeakTableView::writeShelX()
{
    if (!checkBeforeWritting()) {
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save ShelX file"),
                                                    QString::fromStdString(getPeaksRange()+".hkl"),
                                                    tr("ShelX Files (*.hkl)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);

    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    peaksModel->writeShelX(filename.toStdString());
}

void PeakTableView::plotAs(const std::string& key)
{
    QModelIndexList indexList = selectionModel()->selectedIndexes();
    if (!indexList.size()) {
        return;
    }

    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty()) {
        return;
    }

    int nPoints=indexList.size();

    QVector<double> x(nPoints);
    QVector<double> y(nPoints);
    QVector<double> e(nPoints);

    for (int i=0;i<nPoints;++i) {
        sptrPeak3D p=peaks[indexList[i].row()];
        x[i]=p->getData()->getMetadata()->getKey<double>(key);
        y[i]=p->getScaledIntensity().getValue();
        e[i]=p->getScaledIntensity().getSigma();
    }
    emit plotData(x,y,e);
}

std::string PeakTableView::getPeaksRange() const
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return "";
    }
    auto peaks = peaksModel->getPeaks();

    if (peaks.empty()) {
        return "";
    }
    std::set<std::string> temp;

    for (auto&& p : peaks) {
        temp.insert(std::to_string(p->getData()->getMetadata()->getKey<int>("Numor")));
    }
    std::string range(*(temp.begin()));
    std::string last=*(temp.rbegin());

    if (range.compare(last)!=0) {
        range += "_"+last;
    }
    return range;
}

bool PeakTableView::checkBeforeWritting()
{
    if (!_normalized) {
        int reply=QMessageBox::question(
                    this,
                    "Writing data",
                    "No normalisation (time/monitor) has been found. Are you sure you want to export",
                    (QMessageBox::Yes | QMessageBox::Abort));
        if (reply==QMessageBox::Abort) {
            return false;
        }
    }
    return true;
}

void PeakTableView::showPeaksMatchingText(const QString& text)
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    auto peaks = peaksModel->getPeaks();
    if (peaks.empty()) {
        return;
    }

    QStringList list=text.split(" ");
    int nterms=list.size();

    // Don't search if h,k,l not complete
    if (nterms<3) {
        unsigned int row=0;
        for (row=0;row<peaks.size();row++) {
            setRowHidden(row,false);
        }
        return;
    }

    bool okh=false,okk=false,okl=false;
    double h=list[0].toDouble(&okh);
    double k=list[1].toDouble(&okk);
    double l=list[2].toDouble(&okl);

    if (!(okh && okk && okl)) {
        unsigned int row=0;
        for (row=0;row<peaks.size();row++) {
            setRowHidden(row,false);
        }
        return;
    }

    unsigned int row=0;
    for (row=0;row<peaks.size();row++) {
        sptrPeak3D p=peaks[row];
        Eigen::RowVector3d hkl;
        bool success = p->getMillerIndices(hkl,true);
        setRowHidden(row,success);
    }
}

void PeakTableView::selectUnindexedPeaks()
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    QModelIndexList unindexedPeaks = peaksModel->getUnindexedPeaks();

    for (QModelIndex index : unindexedPeaks) {
        selectRow(index.row());
    }
}

void PeakTableView::selectAllPeaks()
{
    selectAll();
}

void PeakTableView::clearSelectedPeaks()
{
    clearSelection();
}

void PeakTableView::togglePeaksSelection()
{
    QItemSelectionModel *selection = selectionModel();

    for (int i=0;i<model()->rowCount();++i) {
        selection->select(model()->index(i,0),QItemSelectionModel::Rows|QItemSelectionModel::Toggle);
    }
}

void PeakTableView::selectValidPeaks()
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    QModelIndexList validPeaksIndexes = peaksModel->getValidPeaks();

    for (QModelIndex index : validPeaksIndexes) {
        selectRow(index.row());
    }
}

QItemSelectionModel::SelectionFlags PeakTableView::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if (event==nullptr) {
        return QItemSelectionModel::NoUpdate;
    }
    return QTableView::selectionCommand(index,event);
}

void PeakTableView::openAutoIndexingDialog()
{
    auto peakModel = dynamic_cast<CollectedPeaksModel*>(model());
    sptrExperiment experiment = peakModel->getExperiment();
    std::vector<sptrPeak3D> peaks = peakModel->getPeaks(selectionModel()->selectedRows());
    auto dialog = new DialogAutoIndexing(experiment,peaks);
    connect(dialog,SIGNAL(cellUpdated(sptrUnitCell)),this,SLOT(updateUnitCell(sptrUnitCell)));
    dialog->exec();
    selectionModel()->clear();
}

void PeakTableView::updateUnitCell(const sptrUnitCell& unitCell)
{
    QModelIndexList selectedPeaks = selectionModel()->selectedRows();
    if (selectedPeaks.empty()) {
        return;
    }
    auto peakModel = dynamic_cast<CollectedPeaksModel*>(model());
    peakModel->setUnitCell(unitCell, selectedPeaks);
}

void PeakTableView::openRefiningParametersDialog()
{
    auto peakModel = dynamic_cast<CollectedPeaksModel*>(model());
    sptrExperiment experiment = peakModel->getExperiment();
    std::vector<sptrPeak3D> peaks = peakModel->getPeaks(selectionModel()->selectedRows());

    int nPeaks = peaks.size();
    // Check that a minimum number of peaks have been selected for indexing
    if (nPeaks < 10) {
        QMessageBox::warning(this, tr("NSXTool"),tr("Need at least 10 peaks for refining"));
        return;
    }

    sptrUnitCell uc(peaks[0]->getActiveUnitCell());
    for (auto&& peak : peaks) {
        if (peak->getActiveUnitCell() != uc) {
            uc = nullptr;
            break;
        }
    }

    if (uc == nullptr) {
        QMessageBox::warning(this, tr("NSXTool"),tr("The selected peaks must have the same active unit cell for refining"));
        return;
    }
    DialogRefineUnitCell* dialog= new DialogRefineUnitCell(experiment,uc,peaks,this);
    dialog->exec();
}
