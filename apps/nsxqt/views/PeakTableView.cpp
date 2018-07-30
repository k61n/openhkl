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

#include <cstdio>
#include <memory>
#include <set>

#include <QContextMenuEvent>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItemModel>

#include <nsxlib/DataSet.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MergedPeak.h>
#include <nsxlib/MetaData.h>
#include <nsxlib/MillerIndex.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/ResolutionShell.h>
#include <nsxlib/UnitCell.h>

#include "CollectedPeaksDelegate.h"
#include "CollectedPeaksModel.h"
#include "DialogTransformationMatrix.h"
#include "DialogUnitCellParameters.h"
#include "PeakTableView.h"

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
    //setMinimumSize(800,400);
    setFocusPolicy(Qt::StrongFocus);

    auto delegate = new CollectedPeaksDelegate(this);
    setItemDelegate(delegate);

    this->verticalHeader()->setVisible(false);

    connect(this,SIGNAL(clicked(const QModelIndex&)),this,SLOT(plotSelectedPeak(const QModelIndex&)));
}

void PeakTableView::plotSelectedPeak(const QModelIndex& index)
{
    int idx = index.row();
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    auto peaks = peaksModel->peaks();
    if (peaks.empty()) {
        return;
    }
    if (idx < 0 || static_cast<size_t>(idx) >= peaks.size()) {
        return;
    }

    nsx::sptrPeak3D peak=peaks[idx];
    emit plotPeak(peak);
}

void PeakTableView::keyPressEvent(QKeyEvent *event)
{
    QModelIndexList selected=selectedIndexes();
    if (selected.isEmpty())
        return;

    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }
    auto peaks = peaksModel->peaks();
    if (peaks.empty()) {
        return;
    }

    // take last element
    QModelIndex last=selected.last();
    int index=last.row();
    if (event->key() == Qt::Key_Up) {
        --index;
        if (index >= 0 && static_cast<size_t>(index) < peaks.size())
            emit plotPeak(peaks[index]);
    } else if (event->key() == Qt::Key_Down) {
        ++index;
        if (index >= 0 && static_cast<size_t>(index) < peaks.size())
            emit plotPeak(peaks[index]);
    }
    QTableView::keyPressEvent(event);
}

void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return;
    }

    auto peaks = peaksModel->peaks();
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

    // Menu to plot against metadata
    QModelIndexList indexList = selectionModel()->selectedIndexes();

    if (indexList.size()) {
        QMenu* plotasmenu=menu->addMenu("Plot as");
        nsx::MetaData* met=peaks[indexList[0].row()]->data()->metadata();
        const std::set<std::string>& keys=met->keys();
        for (const auto& key : keys) {
            try {
                //Ensure metadata is a Numeric type
                met->key<double>(key);
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

    connect(normalize,SIGNAL(triggered()),this,SLOT(normalizeToMonitor()));
    menu->popup(event->globalPos());

    connect(clearSelectedPeaks,SIGNAL(triggered()),this,SLOT(clearSelectedPeaks()));
    connect(selectAllPeaks,SIGNAL(triggered()),this,SLOT(selectAllPeaks()));
    connect(selectValidPeaks,SIGNAL(triggered()),this,SLOT(selectValidPeaks()));
    connect(selectUnindexedPeaks,SIGNAL(triggered()),this,SLOT(selectUnindexedPeaks()));
    connect(togglePeaksSelection,SIGNAL(triggered()),this,SLOT(togglePeaksSelection()));
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

    auto peaks = peaksModel->peaks();
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
    nsx::sptrPeak3D peak=peaks[index.row()];
    emit plotPeak(peak);
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

    auto peaks = peaksModel->peaks();
    if (peaks.empty()) {
        return;
    }

    int nPoints=indexList.size();

    QVector<double> x(nPoints);
    QVector<double> y(nPoints);
    QVector<double> e(nPoints);

    for (int i=0;i<nPoints;++i) {
        nsx::sptrPeak3D p=peaks[indexList[i].row()];
        x[i]=p->data()->metadata()->key<double>(key);
        y[i]=p->scaledIntensity().value();
        e[i]=p->scaledIntensity().sigma();
    }
    emit plotData(x,y,e);
}

std::string PeakTableView::peaksRange() const
{
    auto peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (peaksModel == nullptr) {
        return "";
    }
    auto peaks = peaksModel->peaks();

    if (peaks.empty()) {
        return "";
    }
    std::set<std::string> temp;

    for (auto&& p : peaks) {
        temp.insert(std::to_string(p->data()->metadata()->key<int>("Numor")));
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
    auto peaks = peaksModel->peaks();
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

    if (!(okh && okk && okl)) {
        unsigned int row=0;
        for (row=0;row<peaks.size();row++) {
            setRowHidden(row,false);
        }
        return;
    }

    unsigned int row=0;
    for (row=0;row<peaks.size();row++) {
        nsx::sptrPeak3D peak = peaks[row];
        auto cell = peak->unitCell();
        if (!cell) {
            continue;
        }
        nsx::MillerIndex hkl(peak->q(), *cell);
        setRowHidden(row,hkl.indexed(cell->indexingTolerance()));
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

void PeakTableView::updateUnitCell(const nsx::sptrUnitCell& unitCell)
{
    QModelIndexList selectedPeaks = selectionModel()->selectedRows();
    if (selectedPeaks.empty()) {
        nsx::error() << "no peaks selected!";
        return;
    }
    nsx::info() << "updating unit cell";
    auto peakModel = dynamic_cast<CollectedPeaksModel*>(model());
    peakModel->setUnitCell(unitCell, selectedPeaks);
}
