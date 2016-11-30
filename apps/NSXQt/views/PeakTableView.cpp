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
#include <iomanip>
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

PeakTableView::PeakTableView(QWidget *parent)
: QTableView(parent),
  _columnUp(-1,false),
  _normalized(false)
{
    // Make sure that the user can not edit the content of the table
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Selection of a cell in the table select the whole line.
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    //
    setMinimumSize(800,400);
    setFocusPolicy(Qt::StrongFocus);

    // Set selection model
    setSelectionMode(QAbstractItemView::MultiSelection);

    // Signal sent when sorting by column
    QHeaderView* horizontal=this->horizontalHeader();
    connect(horizontal,SIGNAL(sectionClicked(int)),this,SLOT(sortByColumn(int)));

    // Signal sent when clicking on a row to plot peak
    QHeaderView* vertical = this->verticalHeader();
    connect(vertical, &QHeaderView::sectionClicked, [&](int index)
                                                 {
                                                  sptrPeak3D peak=_peaks[index];
                                                  emit plotPeak(peak);
                                                 });

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(deselectPeak(QModelIndex)));
    // Hide the vertical Header
    //this->verticalHeader()->hide();
}

void PeakTableView::setData(std::vector<std::shared_ptr<SX::Data::IData>> data)
{
    for (auto ptr: data) {
        // Add peaks present in this numor to the LatticeFinder
        for (sptrPeak3D peak : ptr->getPeaks())
            _peaks.push_back(peak);
    }
    constructTable();
}

void PeakTableView::peakChanged(QModelIndex current, QModelIndex last)
{
    if (current.row() != last.row()) {
        sptrPeak3D peak=_peaks[current.row()];
        emit plotPeak(peak);
    }
}

void PeakTableView::sortByColumn(int i)
{
    if (i>5 || i==2 || _peaks.size()==0)
        return;

    int& column=std::get<0>(_columnUp);
    bool& up=std::get<1>(_columnUp);

    // If column already sorted, swith direction
    if (i==column) {
        up = !up;
    }
    column = i;

    switch (i)
    {
    case 0:
        sortByHKL(up);
        break;
    case 1:
        sortByIntensity(up);
        break;
    case 3:
        sortByTransmission(up);
        break;
    case 4:
        sortByNumor(up);
        break;
    case 5:
        sortBySelected(up);
        break;
    }

    constructTable();
    QStandardItemModel* model=dynamic_cast<QStandardItemModel*>(this->model());
    QStandardItem* columni=model->horizontalHeaderItem(i);
    if (up)
        columni->setIcon(QIcon(":/resources/sortUpIcon.png"));
    else
        columni->setIcon(QIcon(":/resources/sortDownIcon.png"));
}

void PeakTableView::constructTable()
{
    // icons for selected and deselected peaks
    QIcon selectedIcon(":/resources/peakSelectedIcon.png");
    QIcon deselectedIcon(":/resources/peakDeselectedIcon.png");

    // set up progress handler and view
    std::shared_ptr<SX::Utils::ProgressHandler> progressHandler(new SX::Utils::ProgressHandler);
    ProgressView progressView(this);

    progressView.watch(progressHandler);
    progressHandler->setStatus("Creating peak tables..");
    progressHandler->setProgress(0);

    // Create table
    QStandardItemModel* model=new QStandardItemModel(_peaks.size(),6,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("h k l"));
    model->setHorizontalHeaderItem(1,new QStandardItem("I"));
    model->setHorizontalHeaderItem(2,new QStandardItem(QString((QChar) 0x03C3)+"I"));
    model->setHorizontalHeaderItem(3,new QStandardItem("Transmission"));
    model->setHorizontalHeaderItem(4,new QStandardItem("Numor"));
    model->setHorizontalHeaderItem(5,new QStandardItem("Selected"));

    int i = 0;

    // Setup content of the table
    for (sptrPeak3D peak: _peaks) {
        const Eigen::RowVector3d& hkl = peak->getMillerIndices();

        QStandardItem* col0 = new QStandardItem(QString::number(hkl[0],'f',2)
                + "  " + QString::number(hkl[1],'f',2)
                + "  " + QString::number(hkl[2],'f',2));

        double l = peak->getLorentzFactor();
        double t = peak->getTransmission();

        QStandardItem* col1 = new QStandardItem(QString::number(peak->getScaledIntensity()/l/t,'f',2));
        QStandardItem* col2 = new QStandardItem(QString::number(peak->getScaledSigma()/l/t,'f',2));
        QStandardItem* col3 = new QStandardItem(QString::number(t,'f',2));
        QStandardItem* col4 = new QStandardItem(QString::number(peak->getData()->getMetadata()->getKey<int>("Numor")));
        QStandardItem* col5;

        if (peak->isSelected())
            col5= new QStandardItem(selectedIcon,"");
        else
            col5= new QStandardItem(deselectedIcon, "");

        model->setItem(i,0,col0);
        model->setItem(i,1,col1);
        model->setItem(i,2,col2);
        model->setItem(i,3,col3);
        model->setItem(i,4,col4);
        model->setItem(i++,5,col5);

        progressHandler->setProgress(i * 100.0 / _peaks.size() );
    }
    setModel(model);

    this->setColumnWidth(0,150);

    // Signal sent when the user navigates the table (e.g. up down arrow )
    connect(this->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(peakChanged(QModelIndex,QModelIndex)));
}

void PeakTableView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        return;

    QTableView::mousePressEvent(event);
}

void PeakTableView::keyPressEvent(QKeyEvent *event)
{
    QModelIndexList selected = selectedIndexes();
    if (selected.isEmpty())
        return;

    // take last element
    QModelIndex last = selected.last();
    unsigned int index = last.row();

    if (event->key() == Qt::Key_Up)  {
       --index;
    }
    else if (event->key() == Qt::Key_Down) {
        ++index;
    }
    // Change status of peak from unselected to selected by using spacebar
    else if (event->key() == Qt::Key_Space) {
        sptrPeak3D peak = _peaks[index];
        bool newstatus = !(peak->isSelected());
        peak->setSelected(newstatus);
        QStandardItemModel* model = dynamic_cast<QStandardItemModel*>(this->model());

        if (newstatus)
            model->setItem(index, 5, new QStandardItem(QIcon(":/resources/peakSelectedIcon.png"),""));
        else
            model->setItem(index, 5, new QStandardItem(QIcon(":/resources/peakDeselectedIcon.png"),""));
        emit plotPeak(peak);
    }

    if (index>_peaks.size()-1)
        return;

    if (!isRowHidden(index)) {
        clearSelection();
        selectRow(index);
        sptrPeak3D peak = _peaks[index];
        emit plotPeak(peak);
    }
}

void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
{
    // Show all peaks as selected when contet menu is requested
    QMenu* menu = new QMenu(this);

    QAction* sortbyEquivalence = new QAction("Sort by equivalences", menu);
    menu->addAction(sortbyEquivalence);
    connect(sortbyEquivalence, SIGNAL(triggered()), this, SLOT(sortEquivalents()));

    QAction* normalize = new QAction("Normalize to monitor", menu);
    menu->addSeparator();
    menu->addAction(normalize);
    QMenu* writeMenu = menu->addMenu("Write");
    QAction* writeFullProf = new QAction("FullProf file", writeMenu);
    QAction* writeShelX = new QAction("SHELX file", writeMenu);
    QAction* writeLog = new QAction("Detailed log file", writeMenu);
    writeMenu->addAction(writeFullProf);
    writeMenu->addAction(writeShelX);
    writeMenu->addAction(writeLog);

    // Menu to plot against metadata
    QModelIndexList indexList = selectionModel()->selectedIndexes();
    //at least one peak
    if (indexList.size()) {
        QMenu* plotasmenu = menu->addMenu("Plot as");
        SX::Data::MetaData* met = _peaks[indexList[0].row()]->getData()->getMetadata();
        const std::set<std::string>& keys = met->getAllKeys();

        for (const auto& key: keys) {
            try {
                met->getKey<double>(key); //Ensure metadata is a Numeric tyoe
            }
            catch(std::exception& e) {
                continue;
            }
            QAction* newparam = new QAction(QString::fromStdString(key),plotasmenu);
            connect(newparam,&QAction::triggered, this, [&](){plotAs(key);});
            plotasmenu->addAction(newparam);
        }
    }

    // Connections
    connect(normalize, SIGNAL(triggered()), this, SLOT(normalizeToMonitor()));
    connect(writeFullProf, SIGNAL(triggered()), this, SLOT(writeFullProf()));
    connect(writeShelX, SIGNAL(triggered()), this, SLOT(writeShelX()));
    connect(writeLog, SIGNAL(triggered()), this, SLOT(writeLog()));

    menu->popup(event->globalPos());
}

void PeakTableView::normalizeToMonitor()
{
    bool ok;
    double factor = QInputDialog::getDouble(this,"Enter normalization factor","",1,1,100000000,1,&ok);
    if (ok) {
        for (sptrPeak3D peak : _peaks)
            peak->setScale(factor/peak->getData()->getMetadata()->getKey<double>("monitor"));
        // Keep track of the last selected index before rebuilding the table
        QModelIndex index=currentIndex();
        constructTable();
        _normalized = true;
        selectRow(index.row());
        // If no row selected do nothing else.
        if (!index.isValid())
            return;
        sptrPeak3D peak = _peaks[index.row()];
        emit plotPeak(peak);
    }
}

void PeakTableView::writeFullProf()
{
    if (!_peaks.size())
        qCritical() << "No peaks in the table";

    if (!checkBeforeWriting())
        return;

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save FullProf file"),
                                                    QString::fromStdString(getPeaksRange()+".int"),
                                                    tr("FullProf Files (*.int)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);
    if (filename.isEmpty())
        return;

    std::fstream file(filename.toStdString(), std::ios::out);

    if (!file.is_open()) {
        qCritical()<<"Error writing to this file, please check write permisions";
        return;
    }

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";
    double wave = _peaks[0]->getData()->getMetadata()->getKey<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wave << " 0 0" << std::endl;

    for (sptrPeak3D peak: _peaks) {
        if (peak->isSelected()) {
            const Eigen::RowVector3d& hkl=peak->getMillerIndices();

            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
            double l = peak->getLorentzFactor();
            double t = peak->getTransmission();
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getScaledIntensity()/l/t;
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak->getScaledSigma()/l/t;
            file << std::setprecision(0) << std::setw(5) << 1  << std::endl;
        }
    }
    if (file.is_open())
        file.close();
}


void PeakTableView::writeShelX()
{
    if (!_peaks.size()) {
        qCritical() << "No peaks in the table";
        return;
    }

    if (!checkBeforeWriting())
        return;

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save ShelX file"),
                                                    QString::fromStdString(getPeaksRange()+".hkl"),
                                                    tr("ShelX Files (*.hkl)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);
    if (filename.isEmpty())
        return;

    std::fstream file(filename.toStdString().c_str(), std::ios::out);
    if (!file.is_open()) {
        qCritical() << "Error writing to this file, please check write permisions";
        return;
    }

    auto sptrBasis = _peaks[0]->getUnitCell();
    if (sptrBasis == nullptr) {
        qCritical() << "No unit cell defined the peaks. No index can be defined.";
        return;
    }

    for (sptrPeak3D peak: _peaks) {
        if (peak->isSelected()) {
            const Eigen::RowVector3d& hkl = peak->getMillerIndices();
            auto sptrCurrentBasis = peak->getUnitCell();

            if (sptrCurrentBasis != sptrBasis) {
                qCritical()<<"Not all the peaks have the same unit cell. Multi crystal not implement yet";
                return;
            }

            if (!(peak->hasIntegerHKL(*sptrCurrentBasis,0.2)))
                continue;

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0];

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[1];

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[2];

            double l = peak->getLorentzFactor();
            double t = peak->getTransmission();
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getScaledIntensity()/l/t;
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak->getScaledSigma()/l/t <<std::endl;
        }
    }
    if (file.is_open())
        file.close();
}

void PeakTableView::writeLog()
{
    LogFileDialog dialog;

    if (!_peaks.size()) {
        qCritical() << "No peaks in the table";
        return;
    }

    // return if user cancels dialog
    if (!dialog.exec())
        return;

    if (dialog.writeUnmerged()) {
        if (!writeNewShelX(dialog.unmergedFilename(), _peaks))
            qCritical() << "Could not write unmerged data to " << dialog.unmergedFilename().c_str();
    }

    if (dialog.writeMerged()) {
        qDebug() << "writing of merged data not yet implemented";
    }

    if (dialog.writeStatistics()) {
        if (!writeStatistics(dialog.statisticsFilename(),
                             _peaks,
                             dialog.dmin(), dialog.dmax(), dialog.numShells()))
            qCritical() << "Could not write statistics log to " << dialog.statisticsFilename().c_str();
    }
}

void PeakTableView::sortByHKL(bool up)
{
    auto compare_fn = [](sptrPeak3D p1, sptrPeak3D p2) -> bool
    {
        auto hkl1 = p1->getMillerIndices();
        auto hkl2 = p2->getMillerIndices();

        if (hkl1[0] != hkl2[0])
            return hkl1[0] < hkl2[0];
        else if (hkl1[1] != hkl2[1])
            return hkl1[1] < hkl2[1];
        else
            return hkl1[2] < hkl2[2];
    };

    auto compare_fn_up = [compare_fn](sptrPeak3D p1, sptrPeak3D p2) -> bool
    {
        return compare_fn(p2, p1);
    };

    if (up)
        std::sort(_peaks.begin(), _peaks.end(), compare_fn_up);
    else
        std::sort(_peaks.begin(),_peaks.end(), compare_fn);
}

void PeakTableView::sortBySelected(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1, const sptrPeak3D p2)
              {
                return (p2->isSelected()<p1->isSelected());
              }
              );
    else
        std::sort(_peaks.begin(),_peaks.end(),
              [&](const sptrPeak3D p1, sptrPeak3D p2)
              {
                return (p2->isSelected()>p1->isSelected());
              }
              );
}

void PeakTableView::sortByTransmission(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1,sptrPeak3D p2)
                {
                    return (p1->getTransmission()>p2->getTransmission());
                });
    else
        std::sort(_peaks.begin(),_peaks.end(),
                  [&](sptrPeak3D p1,sptrPeak3D p2)
                    {
                        return (p1->getTransmission()<p2->getTransmission());
                    });
}

void PeakTableView::sortByIntensity(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
                {
                    return ((p1->getScaledIntensity()/p1->getLorentzFactor()/p1->getTransmission())>(p2->getScaledIntensity()/p2->getLorentzFactor()/p2->getTransmission()));
                });
    else
        std::sort(_peaks.begin(),_peaks.end(),
                  [&](sptrPeak3D p1, sptrPeak3D p2)
                    {
            return ((p1->getScaledIntensity()/p1->getLorentzFactor()/p1->getTransmission())<(p2->getScaledIntensity()/p2->getLorentzFactor()/p2->getTransmission()));
                    });
}

void PeakTableView::sortByNumor(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
                {
                    int numor1 = p1->getData()->getMetadata()->getKey<int>("Numor");
                    int numor2 = p2->getData()->getMetadata()->getKey<int>("Numor");
                    return (numor1>numor2);
                });
    else
        std::sort(_peaks.begin(),_peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
                {
                    int numor1 = p1->getData()->getMetadata()->getKey<int>("Numor");
                    int numor2 = p2->getData()->getMetadata()->getKey<int>("Numor");
                    return (numor1<numor2);
                });
}

void PeakTableView::deselectPeak(QModelIndex index)
{
    sptrPeak3D peak = _peaks[index.row()];
    peak->setSelected(!peak->isSelected());
    constructTable();
}

void PeakTableView::plotAs(const std::string& key)
{
     QModelIndexList indexList = selectionModel()->selectedIndexes();
     if (!indexList.size())
         return;

    int nPoints=indexList.size();

    QVector<double> x(nPoints);
    QVector<double> y(nPoints);
    QVector<double> e(nPoints);

    for (int i = 0; i < nPoints; ++i) {
        sptrPeak3D p=_peaks[indexList[i].row()];
        x[i]=p->getData()->getMetadata()->getKey<double>(key);
        y[i]=p->getScaledIntensity();
        e[i]=p->getScaledSigma();
    }

    emit plotData(x,y,e);
}

std::string PeakTableView::getPeaksRange() const
{
    std::set<std::string> temp;

    for (sptrPeak3D p : _peaks)
        temp.insert(std::to_string(p->getData()->getMetadata()->getKey<int>("Numor")));

    std::string range(*(temp.begin()));

    std::string last=*(temp.rbegin());

    if (range.compare(last)!=0)
        range += "_"+last;

    return range;
}

void PeakTableView::sortEquivalents()
{
    qDebug() << "Sorting";
    auto grp = SX::Crystal::SpaceGroup(_peaks[0]->getUnitCell()->getSpaceGroup());
    std::sort(_peaks.begin(),
              _peaks.end(),
              [&](sptrPeak3D p1, sptrPeak3D p2)
                {
                    Eigen::Vector3d hkl1 = p1->getMillerIndices();
                    Eigen::Vector3d hkl2 = p2->getMillerIndices();
                    if (grp.isEquivalent(hkl1, hkl2))
                        return true;
                    else
                        return false;
                }
              );
}

bool PeakTableView::checkBeforeWriting()
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
    QStringList list=text.split(" ");
    int nterms=list.size();

    if (nterms<3) // Don't search if h,k,l not complete
    {
        unsigned int row=0;
        for (row=0;row<_peaks.size();row++)
        {
            setRowHidden(row,false);
        }
        return;
    }

    bool okh=false,okk=false,okl=false;
    double h=list[0].toDouble(&okh);
    double k=list[1].toDouble(&okk);
    double l=list[2].toDouble(&okl);

    // If problem parsing h k l into double
    if (!(okh && okk && okl)) {
        unsigned int row=0;
        for (row = 0; row < _peaks.size(); row++) {
            setRowHidden(row, false);
        }
        return;
    }

    unsigned int row = 0;
    for (row = 0; row < _peaks.size(); row++) {
        sptrPeak3D p = _peaks[row];
        Eigen::Vector3d hkl = p->getMillerIndices();

        if (std::fabs(hkl[0]-h)>1e-2 || std::fabs(hkl[1]-k)>1e-2 || std::fabs(hkl[2]-l)>1e-2) {
            setRowHidden(row, true);
        }
        else
            setRowHidden(row, false);
    }
}

bool PeakTableView::writeNewShelX(std::string filename, const std::vector<sptrPeak3D> &peaks)
{
    std::fstream file(filename, std::ios::out);
    std::vector<char> buf(1024, 0); // buffer for snprintf

    if (!file.is_open()) {
        qCritical() << "Error writing to this file, please check write permisions";
        return false;
    }

    auto sptrBasis = peaks[0]->getUnitCell();

    if (!sptrBasis) {
        qCritical() << "No unit cell defined the peaks. No index can be defined.";
        return false;
    }

    for (sptrPeak3D peak: peaks) {
        if (peak->isMasked() || !peak->isSelected())
            continue;

        const Eigen::RowVector3d& hkl = peak->getMillerIndices();
        auto sptrCurrentBasis = peak->getUnitCell();

        if (sptrCurrentBasis != sptrBasis) {
            qCritical() << "Not all the peaks have the same unit cell. Multi crystal not implement yet";
            return false;
        }

        if (!(peak->hasIntegerHKL(*sptrCurrentBasis,0.2)))
            continue;

        const int h = std::round(hkl[0]);
        const int k = std::round(hkl[1]);
        const int l = std::round(hkl[2]);

        double lorentz = peak->getLorentzFactor();
        double trans = peak->getTransmission();

        double intensity = peak->getScaledIntensity() / lorentz / trans;
        double sigma = peak->getScaledSigma() / lorentz / trans;

        std::snprintf(&buf[0], buf.size(), "  %4d %4d %4d %15.2f %10.2f", h, k, l, intensity, sigma);
        file << &buf[0] << std::endl;
    }

    if (file.is_open())
        file.close();

    return true;
}

bool PeakTableView::writeStatistics(std::string filename,
                                    const std::vector<SX::Crystal::sptrPeak3D> &peaks,
                                    double dmin, double dmax, int num_shells)
{
    std::fstream file(filename, std::ios::out);
    SX::Crystal::ResolutionShell res = {dmin, dmax, num_shells};
    std::vector<char> buf(1024, 0); // buffer for snprintf
    std::vector<SX::Crystal::MergedPeak> merged_peaks;

    if (!file.is_open()) {
        qCritical() << "Error writing to this file, please check write permisions";
        return false;
    }

    if (peaks.size() == 0) {
        qCritical() << "No peaks to write to log!";
        return false;
    }

    // jmf testing
//    for (int i = 0; i < peaks.size(); ++i) {
//        const auto peak = peaks[i];

//        if (peak->isMasked() || !peak->isSelected())
//            continue;

//        if (!peak->hasIntegerHKL(*peak->getUnitCell(), 1e-2))
//            continue;

//        for (int j =i+1; j < peaks.size(); ++j) {
//            const auto other_peak = peaks[j];

//            if (other_peak->isMasked() || !other_peak->isSelected())
//                continue;

//            if (peak->getUnitCell() != other_peak->getUnitCell())
//                continue;

//            if (!other_peak->hasIntegerHKL(*peak->getUnitCell(), 1e-2))
//                continue;

//            const auto hkl1 = peak->getIntegerMillerIndices();
//            const auto hkl2 = other_peak->getIntegerMillerIndices();

//            if (hkl1 == hkl2) {
//                std::cout << "duplicate peak found!" << std::endl;
//                std::cout << hkl1(0) << " " << hkl1(1) << " " << hkl1(2) << std::endl;

//                const auto c1 = peak->getPeak()->getAABBCenter();
//                const auto c2 = other_peak->getPeak()->getAABBCenter();

//                std::cout << c1(0) << " " << c1(1) << " " << c1(2) << std::endl;
//                std::cout << c2(0) << " " << c2(1) << " " << c2(2) << std::endl;
//            }
//        }
//    }

    auto cell = peaks[0]->getUnitCell();
    auto grp = SX::Crystal::SpaceGroup(cell->getSpaceGroup());

    for (auto&& peak: peaks) {
        if (cell != peak->getUnitCell()) {
            qCritical() << "Only one unit cell is supported at this time!!";
            return false;
        }
        res.addPeak(peak);
    }

    auto&& ds = res.getD();
    auto&& shells = res.getShells();

    for (int i = 0; i < num_shells; ++i) {
        const double d_lower = ds[i];
        const double d_upper = ds[i+1];

        std::snprintf(&buf[0], buf.size(), " %6.2f  %6.2f  %6d", d_lower, d_upper, shells[i].size());
        file << &buf[0] << std::endl;
    }

    int shell_count = 0;

    for (auto&& shell: res.getShells()) {
        for (auto&& peak: shell) {
            bool peak_added = false;

            // skip bad/masked peaks
            if (peak->isMasked() || !peak->isSelected())
                continue;

            // skip misindexed peaks
            if (!peak->hasIntegerHKL(*cell))
                continue;

            for (auto&& merged_peak: merged_peaks) {
                if (merged_peak.addPeak(peak)) {
                    peak_added = true;
                    break;
                }
            }

            if (peak_added)
                continue;

            // peak was not equivalent to any of the merged peaks
            SX::Crystal::MergedPeak new_peak(grp);
            new_peak.addPeak(peak);
            merged_peaks.push_back(new_peak);
        }

        qDebug() << "Finished logging shell " << ++shell_count;
    }

    auto compare_fn = [](const SX::Crystal::MergedPeak& p, const SX::Crystal::MergedPeak& q) -> bool
    {
        const auto a = p.getIndex();
        const auto b = q.getIndex();

        if (a(0) != b(0))
            return a(0) < b(0);
        else if (a(1) != b(1))
            return a(1) < b(1);
        else
            return a(2) < b(2);
    };

    std::sort(merged_peaks.begin(), merged_peaks.end(), compare_fn);

    for (auto&& peak: merged_peaks) {

        const auto hkl = peak.getIndex();

        const int h = hkl[0];
        const int k = hkl[1];
        const int l = hkl[2];

        const double intensity = peak.intensity();
        const double sigma = peak.sigma();
        const double chi2 = peak.chiSquared();
        const int nobs = peak.redundancy();

        std::snprintf(&buf[0], buf.size(), "  %4d %4d %4d %15.2f %10.2f %3d %10.5f",
                h, k, l, intensity, sigma, nobs, chi2);

        file << &buf[0] << std::endl;
    }

    qDebug() << "Done writing log file.";

    file.close();
    return true;
}
