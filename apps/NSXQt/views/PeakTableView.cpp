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

#include "IData.h"
#include "Peak3D.h"

#include "ProgressHandler.h"
#include "ProgressView.h"
#include "LogFileDialog.h"

#include "ResolutionShell.h"
#include "MergedPeak.h"

#include "CollectedPeaksDelegate.h"
#include "CollectedPeaksModel.h"

#include "dialogs/DialogAutoIndexing.h"
#include "dialogs/DialogRefineUnitCell.h"
#include "dialogs/DialogTransformationMatrix.h"
#include "dialogs/DialogUnitCellParameters.h"

PeakTableView::PeakTableView(QWidget *parent)
: QTableView(parent),
  _normalized(false)
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

    CollectedPeaksDelegate *delegate = new CollectedPeaksDelegate(this);
    setItemDelegate(delegate);


//    // Signal sent when sorting by column
//    QHeaderView* horizontal=this->horizontalHeader();
//    connect(horizontal,SIGNAL(sectionClicked(int)),this,SLOT(sortByColumn(int)));

//    // Signal sent when clicking on a row to plot peak
//    QHeaderView* vertical = this->verticalHeader();
//    connect(vertical, &QHeaderView::sectionClicked, [&](int index)
//                                                 {
//                                                  sptrPeak3D peak=_peaks[index];
//                                                  emit plotPeak(peak);
//                                                 });

//    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(deselectPeak(QModelIndex)));
//    // Hide the vertical Header
//    //this->verticalHeader()->hide();

    QHeaderView* vertical=this->verticalHeader();
    connect(vertical,SIGNAL(sectionClicked(int)),this,SLOT(plotSelectedPeak(int)));

}

void PeakTableView::plotSelectedPeak(int index)
{
//<<<<<<< HEAD
//    for (auto ptr: data) {
//        // Add peaks present in this numor to the LatticeFinder
//        for (sptrPeak3D peak : ptr->getPeaks())
//            _peaks.push_back(peak);
//    }
//    constructTable();
//}

//void PeakTableView::peakChanged(QModelIndex current, QModelIndex last)
//{
//    if (current.row() != last.row()) {
//        sptrPeak3D peak=_peaks[current.row()];
//        emit plotPeak(peak);
//    }
//}

//void PeakTableView::sortByColumn(int i)
//{
//    if (i>5 || i==2 || _peaks.size()==0)
//        return;

//    int& column=std::get<0>(_columnUp);
//    bool& up=std::get<1>(_columnUp);

//    // If column already sorted, swith direction
//    if (i==column) {
//        up = !up;
//    }
//    column = i;

//    switch (i)
//    {
//    case 0:
//        sortByHKL(up);
//        break;
//    case 1:
//        sortByIntensity(up);
//        break;
//    case 3:
//        sortByTransmission(up);
//        break;
//    case 4:
//        sortByNumor(up);
//        break;
//    case 5:
//        sortBySelected(up);
//        break;
//    }

//    constructTable();
//    QStandardItemModel* model=dynamic_cast<QStandardItemModel*>(this->model());
//    QStandardItem* columni=model->horizontalHeaderItem(i);
//    if (up)
//        columni->setIcon(QIcon(":/resources/sortUpIcon.png"));
//    else
//        columni->setIcon(QIcon(":/resources/sortDownIcon.png"));
//}

//void PeakTableView::constructTable()
//{
//    // icons for selected and deselected peaks
//    QIcon selectedIcon(":/resources/peakSelectedIcon.png");
//    QIcon deselectedIcon(":/resources/peakDeselectedIcon.png");

//    // set up progress handler and view
//    std::shared_ptr<SX::Utils::ProgressHandler> progressHandler(new SX::Utils::ProgressHandler);
//    ProgressView progressView(this);

//    progressView.watch(progressHandler);
//    progressHandler->setStatus("Creating peak tables..");
//    progressHandler->setProgress(0);

//    // Create table
//    QStandardItemModel* model=new QStandardItemModel(_peaks.size(),6,this);
//    model->setHorizontalHeaderItem(0,new QStandardItem("h k l"));
//    model->setHorizontalHeaderItem(1,new QStandardItem("I"));
//    model->setHorizontalHeaderItem(2,new QStandardItem(QString((QChar) 0x03C3)+"I"));
//    model->setHorizontalHeaderItem(3,new QStandardItem("Transmission"));
//    model->setHorizontalHeaderItem(4,new QStandardItem("Numor"));
//    model->setHorizontalHeaderItem(5,new QStandardItem("Selected"));

//    int i = 0;

//    // Setup content of the table
//    for (sptrPeak3D peak: _peaks) {
//        const Eigen::RowVector3d& hkl = peak->getMillerIndices();

//        QStandardItem* col0 = new QStandardItem(QString::number(hkl[0],'f',2)
//                + "  " + QString::number(hkl[1],'f',2)
//                + "  " + QString::number(hkl[2],'f',2));

//        double l = peak->getLorentzFactor();
//        double t = peak->getTransmission();

//        QStandardItem* col1 = new QStandardItem(QString::number(peak->getScaledIntensity()/l/t,'f',2));
//        QStandardItem* col2 = new QStandardItem(QString::number(peak->getScaledSigma()/l/t,'f',2));
//        QStandardItem* col3 = new QStandardItem(QString::number(t,'f',2));
//        QStandardItem* col4 = new QStandardItem(QString::number(peak->getData()->getMetadata()->getKey<int>("Numor")));
//        QStandardItem* col5;

//        if (peak->isSelected())
//            col5= new QStandardItem(selectedIcon,"");
//        else
//            col5= new QStandardItem(deselectedIcon, "");

//        model->setItem(i,0,col0);
//        model->setItem(i,1,col1);
//        model->setItem(i,2,col2);
//        model->setItem(i,3,col3);
//        model->setItem(i,4,col4);
//        model->setItem(i++,5,col5);

//        progressHandler->setProgress(i * 100.0 / _peaks.size() );
//    }
//    setModel(model);

//    this->setColumnWidth(0,150);

//    // Signal sent when the user navigates the table (e.g. up down arrow )
//    connect(this->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
//            this, SLOT(peakChanged(QModelIndex,QModelIndex)));
//}
//=======
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty())
        return;
//>>>>>>> feature/twins

    if (index < 0 || index >= peaks.size())
        return;

    sptrPeak3D peak=peaks[index];
    emit plotPeak(peak);
}

void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
{
//<<<<<<< HEAD
//    QModelIndexList selected = selectedIndexes();
//    if (selected.isEmpty())
//        return;

//    // take last element
//    QModelIndex last = selected.last();
//    unsigned int index = last.row();

//    if (event->key() == Qt::Key_Up)  {
//       --index;
//    }
//    else if (event->key() == Qt::Key_Down) {
//        ++index;
//    }
//    // Change status of peak from unselected to selected by using spacebar
//    else if (event->key() == Qt::Key_Space) {
//        sptrPeak3D peak = _peaks[index];
//        bool newstatus = !(peak->isSelected());
//        peak->setSelected(newstatus);
//        QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(this->model());

//        if (newstatus)
//            model->setItem(index, 5, new QStandardItem(QIcon(":/resources/peakSelectedIcon.png"),""));
//        else
//            model->setItem(index, 5, new QStandardItem(QIcon(":/resources/peakDeselectedIcon.png"),""));
//        emit plotPeak(peak);
//    }

//    if (index>_peaks.size()-1)
//        return;

//    if (!isRowHidden(index)) {
//        clearSelection();
//        selectRow(index);
//        sptrPeak3D peak = _peaks[index];
//        emit plotPeak(peak);
//    }
//}

//void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
//{
//    // Show all peaks as selected when contet menu is requested
//    QMenu* menu = new QMenu(this);

//    QAction* sortbyEquivalence = new QAction("Sort by equivalences", menu);
//    menu->addAction(sortbyEquivalence);
//    connect(sortbyEquivalence, SIGNAL(triggered()), this, SLOT(sortEquivalents()));
//=======
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty())
        return;

    // Show all peaks as selected when context menu is requested
    QMenu* menu=new QMenu(this);
    //
    QAction* sortbyEquivalence=new QAction("Sort by equivalences",menu);
    menu->addAction(sortbyEquivalence);
    connect(sortbyEquivalence,SIGNAL(triggered()),peaksModel,SLOT(sortEquivalents()));
//>>>>>>> feature/twins

    QAction* normalize = new QAction("Normalize to monitor", menu);
    menu->addSeparator();
    menu->addAction(normalize);
    QMenu* writeMenu = menu->addMenu("Write");
    QAction* writeFullProf = new QAction("FullProf file", writeMenu);
    QAction* writeShelX = new QAction("SHELX file", writeMenu);
    writeMenu->addAction(writeFullProf);
    writeMenu->addAction(writeShelX);

    // Menu to plot against metadata
    QModelIndexList indexList = selectionModel()->selectedIndexes();
//<<<<<<< HEAD
//    //at least one peak
//    if (indexList.size()) {
//        QMenu* plotasmenu = menu->addMenu("Plot as");
//        SX::Data::MetaData* met = _peaks[indexList[0].row()]->getData()->getMetadata();
//        const std::set<std::string>& keys = met->getAllKeys();

//        for (const auto& key: keys) {
//            try {
//                met->getKey<double>(key); //Ensure metadata is a Numeric tyoe
//            }
//            catch(std::exception& e) {
//                continue;
//            }
//            QAction* newparam = new QAction(QString::fromStdString(key),plotasmenu);
//            connect(newparam,&QAction::triggered, this, [&](){plotAs(key);});
//=======
    if (indexList.size())
    {
        QMenu* plotasmenu=menu->addMenu("Plot as");
        SX::Data::MetaData* met=peaks[indexList[0].row()]->getData()->getMetadata();
        const std::set<std::string>& keys=met->getAllKeys();
        for (const auto& key : keys)
        {
            try
            {
                //Ensure metadata is a Numeric type
                met->getKey<double>(key);
            }
            catch(std::exception& e)
            {
                continue;
            }
            QAction* newparam=new QAction(QString::fromStdString(key),plotasmenu);
            // New way to connect slot using C++ 2011 lambda sicne Qt 5
            connect(newparam,&QAction::triggered,this,[&](){plotAs(key);});
//>>>>>>> feature/twins
            plotasmenu->addAction(newparam);
        }
    }

//<<<<<<< HEAD
//    // Connections
//    connect(normalize, SIGNAL(triggered()), this, SLOT(normalizeToMonitor()));
//    connect(writeFullProf, SIGNAL(triggered()), this, SLOT(writeFullProf()));
//    connect(writeShelX, SIGNAL(triggered()), this, SLOT(writeShelX()));

//    menu->popup(event->globalPos());
//=======
    menu->addSeparator();

    QMenu* selectionMenu=menu->addMenu("Selection");

    QAction* selectAllPeaks=new QAction("all peaks",menu);
    QAction* selectValidPeaks=new QAction("valid peaks",menu);
    QAction* selectUnindexedPeaks=new QAction("unindexed peaks",menu);
    QAction* clearSelectedPeaks=new QAction("clear selection",menu);
    QAction* togglePeaksSelection=new QAction("toggle",menu);
    selectionMenu->addAction(selectAllPeaks);
    selectionMenu->addAction(selectValidPeaks);
    selectionMenu->addAction(selectUnindexedPeaks);
    selectionMenu->addSeparator();
    selectionMenu->addAction(clearSelectedPeaks);
    selectionMenu->addSeparator();
    selectionMenu->addAction(togglePeaksSelection);

    menu->addSeparator();

    QAction* autoIndexing=new QAction("Auto-indexing",menu);
    menu->addAction(autoIndexing);
    QAction* refineParameters=new QAction("Refine unit cell and instrument parameters",menu);
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
//>>>>>>> feature/twins
}

void PeakTableView::normalizeToMonitor()
{
    bool ok;
    double factor = QInputDialog::getDouble(this,"Enter normalization factor","",1,1,100000000,1,&ok);
//<<<<<<< HEAD
//    if (ok) {
//        for (sptrPeak3D peak : _peaks)
//            peak->setScale(factor/peak->getData()->getMetadata()->getKey<double>("monitor"));
//        // Keep track of the last selected index before rebuilding the table
//        QModelIndex index=currentIndex();
//        constructTable();
//        _normalized = true;
//=======
    if (ok)
    {

        CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
        if (!peaksModel)
            return;

        auto peaks = peaksModel->getPeaks();
        if (peaks.empty())
            return;

        peaksModel->normalizeToMonitor(factor);

        // Keep track of the last selected index before rebuilding the table
        QModelIndex index=currentIndex();
        _normalized=true;
//>>>>>>> feature/twins
        selectRow(index.row());
        // If no row selected do nothing else.
        if (!index.isValid())
            return;
//<<<<<<< HEAD
//        sptrPeak3D peak = _peaks[index.row()];
//=======
        sptrPeak3D peak=peaks[index.row()];
//>>>>>>> feature/twins
        emit plotPeak(peak);
    }
}

void PeakTableView::writeFullProf()
{
//<<<<<<< HEAD
//    if (!_peaks.size())
//        qCritical() << "No peaks in the table";

//    if (!checkBeforeWriting())
//=======
    if (!checkBeforeWritting())
//>>>>>>> feature/twins
        return;

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save FullProf file"),
                                                    QString::fromStdString(getPeaksRange()+".int"),
                                                    tr("FullProf Files (*.int)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);

//<<<<<<< HEAD
//    std::fstream file(filename.toStdString(), std::ios::out);

//    if (!file.is_open()) {
//        qCritical()<<"Error writing to this file, please check write permisions";
//=======
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
//>>>>>>> feature/twins
        return;

//<<<<<<< HEAD
//    file << "TITLE File written by ...\n";
//    file << "(3i4,2F14.4,i5,4f8.2)\n";
//    double wave = _peaks[0]->getData()->getMetadata()->getKey<double>("wavelength");
//    file << std::fixed << std::setw(8) << std::setprecision(3) << wave << " 0 0" << std::endl;

//    for (sptrPeak3D peak: _peaks) {
//        if (peak->isSelected()) {
//            const Eigen::RowVector3d& hkl=peak->getMillerIndices();

//            file << std::setprecision(0);
//            file << std::setw(4);
//            file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
//            double l = peak->getLorentzFactor();
//            double t = peak->getTransmission();
//            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getScaledIntensity()/l/t;
//            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getScaledSigma()/l/t;
//            file << std::setprecision(0) << std::setw(5) << 1  << std::endl;
//        }
//    }
//    if (file.is_open())
//        file.close();
//}
//=======
    peaksModel->writeFullProf(filename.toStdString());
//>>>>>>> feature/twins

}

void PeakTableView::writeShelX()
{
//<<<<<<< HEAD
//    if (!_peaks.size()) {
//        qCritical() << "No peaks in the table";
//        return;
//    }

//    if (!checkBeforeWriting())
//=======
    if (!checkBeforeWritting())
//>>>>>>> feature/twins
        return;

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save ShelX file"),
                                                    QString::fromStdString(getPeaksRange()+".hkl"),
                                                    tr("ShelX Files (*.hkl)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);
//<<<<<<< HEAD
//    if (filename.isEmpty())
//        return;

//    std::fstream file(filename.toStdString().c_str(), std::ios::out);
//    if (!file.is_open()) {
//        qCritical() << "Error writing to this file, please check write permisions";
//        return;
//    }

//    auto sptrBasis = _peaks[0]->getUnitCell();
//    if (sptrBasis == nullptr) {
//        qCritical() << "No unit cell defined the peaks. No index can be defined.";
//=======

    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
//>>>>>>> feature/twins
        return;

//<<<<<<< HEAD
//    for (sptrPeak3D peak: _peaks) {
//        if (peak->isSelected()) {
//            const Eigen::RowVector3d& hkl = peak->getMillerIndices();
//            auto sptrCurrentBasis = peak->getUnitCell();

//            if (sptrCurrentBasis != sptrBasis) {
//                qCritical()<<"Not all the peaks have the same unit cell. Multi crystal not implement yet";
//                return;
//            }

//            if (!(peak->hasIntegerHKL(*sptrCurrentBasis,0.2)))
//                continue;

//            file << std::fixed;
//            file << std::setprecision(0);
//            file << std::setw(4);
//            file << hkl[0];

//            file << std::fixed;
//            file << std::setprecision(0);
//            file << std::setw(4);
//            file << hkl[1];

//            file << std::fixed;
//            file << std::setprecision(0);
//            file << std::setw(4);
//            file << hkl[2];

//            double l = peak->getLorentzFactor();
//            double t = peak->getTransmission();
//            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getScaledIntensity()/l/t;
//            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getScaledSigma()/l/t <<std::endl;
//        }
//    }
//    if (file.is_open())
//        file.close();
//}

//void PeakTableView::sortByHKL(bool up)
//{
//    auto compare_fn = [](sptrPeak3D p1, sptrPeak3D p2) -> bool
//    {
//        auto hkl1 = p1->getMillerIndices();
//        auto hkl2 = p2->getMillerIndices();

//        if (hkl1[0] != hkl2[0])
//            return hkl1[0] < hkl2[0];
//        else if (hkl1[1] != hkl2[1])
//            return hkl1[1] < hkl2[1];
//        else
//            return hkl1[2] < hkl2[2];
//    };

//    auto compare_fn_up = [compare_fn](sptrPeak3D p1, sptrPeak3D p2) -> bool
//    {
//        return compare_fn(p2, p1);
//    };

//    if (up)
//        std::sort(_peaks.begin(), _peaks.end(), compare_fn_up);
//    else
//        std::sort(_peaks.begin(),_peaks.end(), compare_fn);
//}

//void PeakTableView::sortBySelected(bool up)
//{
//    if (up)
//        std::sort(_peaks.begin(),_peaks.end(),
//              [&](sptrPeak3D p1, const sptrPeak3D p2)
//              {
//                return (p2->isSelected()<p1->isSelected());
//              }
//              );
//    else
//        std::sort(_peaks.begin(),_peaks.end(),
//              [&](const sptrPeak3D p1, sptrPeak3D p2)
//              {
//                return (p2->isSelected()>p1->isSelected());
//              }
//              );
//}

//void PeakTableView::sortByTransmission(bool up)
//{
//    if (up)
//        std::sort(_peaks.begin(),_peaks.end(),
//              [&](sptrPeak3D p1,sptrPeak3D p2)
//                {
//                    return (p1->getTransmission()>p2->getTransmission());
//                });
//    else
//        std::sort(_peaks.begin(),_peaks.end(),
//                  [&](sptrPeak3D p1,sptrPeak3D p2)
//                    {
//                        return (p1->getTransmission()<p2->getTransmission());
//                    });
//}

//void PeakTableView::sortByIntensity(bool up)
//{
//    if (up)
//        std::sort(_peaks.begin(),_peaks.end(),
//              [&](sptrPeak3D p1, sptrPeak3D p2)
//                {
//                    return ((p1->getScaledIntensity()/p1->getLorentzFactor()/p1->getTransmission())>(p2->getScaledIntensity()/p2->getLorentzFactor()/p2->getTransmission()));
//                });
//    else
//        std::sort(_peaks.begin(),_peaks.end(),
//                  [&](sptrPeak3D p1, sptrPeak3D p2)
//                    {
//            return ((p1->getScaledIntensity()/p1->getLorentzFactor()/p1->getTransmission())<(p2->getScaledIntensity()/p2->getLorentzFactor()/p2->getTransmission()));
//                    });
//}

//void PeakTableView::sortByNumor(bool up)
//{
//    if (up)
//        std::sort(_peaks.begin(),_peaks.end(),
//              [&](sptrPeak3D p1, sptrPeak3D p2)
//                {
//                    int numor1 = p1->getData()->getMetadata()->getKey<int>("Numor");
//                    int numor2 = p2->getData()->getMetadata()->getKey<int>("Numor");
//                    return (numor1>numor2);
//                });
//    else
//        std::sort(_peaks.begin(),_peaks.end(),
//              [&](sptrPeak3D p1, sptrPeak3D p2)
//                {
//                    int numor1 = p1->getData()->getMetadata()->getKey<int>("Numor");
//                    int numor2 = p2->getData()->getMetadata()->getKey<int>("Numor");
//                    return (numor1<numor2);
//                });
//}

//void PeakTableView::deselectPeak(QModelIndex index)
//{
//    sptrPeak3D peak = _peaks[index.row()];
//    peak->setSelected(!peak->isSelected());
//    constructTable();
//=======
    peaksModel->writeShelX(filename.toStdString().c_str());
//>>>>>>> feature/twins
}

void PeakTableView::plotAs(const std::string& key)
{
    QModelIndexList indexList = selectionModel()->selectedIndexes();
     if (!indexList.size())
         return;

     CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
     if (!peaksModel)
         return;

     auto peaks = peaksModel->getPeaks();
     if (peaks.empty())
         return;

    int nPoints=indexList.size();

    QVector<double> x(nPoints);
    QVector<double> y(nPoints);
    QVector<double> e(nPoints);

//<<<<<<< HEAD
//    for (int i = 0; i < nPoints; ++i) {
//        sptrPeak3D p=_peaks[indexList[i].row()];
//=======
    for (int i=0;i<nPoints;++i)
    {
        sptrPeak3D p=peaks[indexList[i].row()];
//>>>>>>> feature/twins
        x[i]=p->getData()->getMetadata()->getKey<double>(key);
        y[i]=p->getScaledIntensity();
        e[i]=p->getScaledSigma();
    }

    emit plotData(x,y,e);
}

std::string PeakTableView::getPeaksRange() const
{
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return "";

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty())
        return "";

    std::set<std::string> temp;

    for (sptrPeak3D p : peaks)
        temp.insert(std::to_string(p->getData()->getMetadata()->getKey<int>("Numor")));

    std::string range(*(temp.begin()));

    std::string last=*(temp.rbegin());

    if (range.compare(last)!=0)
        range += "_"+last;

    return range;
}

//<<<<<<< HEAD
//void PeakTableView::sortEquivalents()
//{
//    qDebug() << "Sorting";
//    auto grp = SX::Crystal::SpaceGroup(_peaks[0]->getUnitCell()->getSpaceGroup());
//    std::sort(_peaks.begin(),
//              _peaks.end(),
//              [&](sptrPeak3D p1, sptrPeak3D p2)
//                {
//                    Eigen::Vector3d hkl1 = p1->getMillerIndices();
//                    Eigen::Vector3d hkl2 = p2->getMillerIndices();
//                    if (grp.isEquivalent(hkl1, hkl2))
//                        return true;
//                    else
//                        return false;
//                }
//              );
//}

//bool PeakTableView::checkBeforeWriting()
//=======
bool PeakTableView::checkBeforeWritting()
//>>>>>>> feature/twins
{
    if (!_normalized) {
        int reply=QMessageBox::question(this,"Writing data","No normalisation (time/monitor) has been found. Are you sure you want to export",(QMessageBox::Yes | QMessageBox::Abort));
        if (reply==QMessageBox::Abort)
            return false;
    }
    return true;
}

void PeakTableView::showPeaksMatchingText(QString text)
{
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty())
        return;

    QStringList list=text.split(" ");
    int nterms=list.size();

    if (nterms<3) // Don't search if h,k,l not complete
    {
        unsigned int row=0;
        for (row=0;row<peaks.size();row++)
            setRowHidden(row,false);
        return;
    }

    bool okh=false,okk=false,okl=false;
    double h=list[0].toDouble(&okh);
    double k=list[1].toDouble(&okk);
    double l=list[2].toDouble(&okl);

    // If problem parsing h k l into double
//<<<<<<< HEAD
//    if (!(okh && okk && okl)) {
//        unsigned int row=0;
//        for (row = 0; row < _peaks.size(); row++) {
//            setRowHidden(row, false);
//        }
//        return;
//    }

//    unsigned int row = 0;
//    for (row = 0; row < _peaks.size(); row++) {
//        sptrPeak3D p = _peaks[row];
//        Eigen::Vector3d hkl = p->getMillerIndices();

//        if (std::fabs(hkl[0]-h)>1e-2 || std::fabs(hkl[1]-k)>1e-2 || std::fabs(hkl[2]-l)>1e-2) {
//            setRowHidden(row, true);
//        }
//        else
//            setRowHidden(row, false);
//    }
//}
//=======
    if (!(okh && okk && okl))
    {
        unsigned int row=0;
        for (row=0;row<peaks.size();row++)
            setRowHidden(row,false);
        return;
    }

    unsigned int row=0;
    for (row=0;row<peaks.size();row++)
    {
        sptrPeak3D p=peaks[row];
        Eigen::RowVector3d hkl;
        bool success = p->getMillerIndices(hkl,true);
        setRowHidden(row,success);
    }
}

void PeakTableView::selectUnindexedPeaks()
{
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    QModelIndexList unindexedPeaks = peaksModel->getUnindexedPeaks();

    for (QModelIndex index : unindexedPeaks)
        selectRow(index.row());
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

    for (int i=0;i<model()->rowCount();++i)
        selection->select(model()->index(i,0),QItemSelectionModel::Rows|QItemSelectionModel::Toggle);
}

void PeakTableView::selectValidPeaks()
{
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    QModelIndexList validPeaksIndexes = peaksModel->getValidPeaks();

    for (QModelIndex index : validPeaksIndexes)
        selectRow(index.row());
}

QItemSelectionModel::SelectionFlags PeakTableView::selectionCommand(const QModelIndex &index, const QEvent *event) const
{
    if (event==nullptr)
        return QItemSelectionModel::NoUpdate;
    return QTableView::selectionCommand(index,event);
}

void PeakTableView::openAutoIndexingDialog()
{
    CollectedPeaksModel* peakModel = dynamic_cast<CollectedPeaksModel*>(model());
    sptrExperiment experiment = peakModel->getExperiment();

    std::vector<sptrPeak3D> peaks = peakModel->getPeaks(selectionModel()->selectedRows());

    DialogAutoIndexing* dialog = new DialogAutoIndexing(experiment,peaks);
    connect(dialog,SIGNAL(cellUpdated(sptrUnitCell)),this,SLOT(updateUnitCell(sptrUnitCell)));
    dialog->exec();

    selectionModel()->clear();
}

void PeakTableView::updateUnitCell(sptrUnitCell unitCell)
{
    QModelIndexList selectedPeaks = selectionModel()->selectedRows();
    if (selectedPeaks.empty())
        return;

    CollectedPeaksModel* peakModel = dynamic_cast<CollectedPeaksModel*>(model());
    peakModel->setUnitCell(unitCell,selectedPeaks);
}

void PeakTableView::openRefiningParametersDialog()
{
    CollectedPeaksModel* peakModel = dynamic_cast<CollectedPeaksModel*>(model());
    sptrExperiment experiment = peakModel->getExperiment();

    std::vector<sptrPeak3D> peaks = peakModel->getPeaks(selectionModel()->selectedRows());

    int nPeaks = peaks.size();
    // Check that a minimum number of peaks have been selected for indexing
    if (nPeaks < 10)
    {
        QMessageBox::warning(this, tr("NSXTool"),tr("Need at least 10 peaks for refining"));
        return;
    }

    sptrUnitCell uc(peaks[0]->getActiveUnitCell());
    for (auto peak : peaks)
    {
        if (peak->getActiveUnitCell() != uc)
        {
            uc = nullptr;
            break;
        }
    }

    if (uc == nullptr)
    {
        QMessageBox::warning(this, tr("NSXTool"),tr("The selected peaks must have the same active unit cell for refining"));
        return;
    }

    DialogRefineUnitCell* dialog= new DialogRefineUnitCell(experiment,uc,peaks,this);
    dialog->exec();
//    getLatticeParams();
//    emit cellUpdated();
}
//>>>>>>> feature/twins
