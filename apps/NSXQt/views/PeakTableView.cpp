#include "PeakTableView.h"

#include <fstream>
#include <iostream>
#include <set>
#include <memory>

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
#include "CollectedPeaksDelegate.h"
#include <CollectedPeaksModel.h>

PeakTableView::PeakTableView(QWidget *parent)
: QTableView(parent),
  _columnUp(-1,false),
  _normalized(false)
{
    // Make sure that the user can not edit the content of the table
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Selection of a cell in the table select the whole line.
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //
    setMinimumSize(800,400);
    setFocusPolicy(Qt::StrongFocus);

    CollectedPeaksDelegate *delegate = new CollectedPeaksDelegate(this);
    setItemDelegate(delegate);

    QHeaderView* vertical=this->verticalHeader();
    connect(vertical,SIGNAL(sectionClicked(int)),this,SLOT(plotSelectedPeak(int)));
}

void PeakTableView::plotSelectedPeak(int index)
{
    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    auto peaks = peaksModel->getPeaks();
    if (peaks.empty())
        return;

    if (index < 0 || index >= peaks.size())
        return;

    sptrPeak3D peak=peaks[index];
    emit plotPeak(peak);
}

void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
{
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

    QAction* normalize=new QAction("Normalize to monitor",menu);
    menu->addSeparator();
    menu->addAction(normalize);
    QMenu* writeMenu=menu->addMenu("Write");
    QAction* writeFullProf=new QAction("FullProf file",writeMenu);
    QAction* writeShelX=new QAction("SHELX file",writeMenu);
    writeMenu->addAction(writeFullProf);
    writeMenu->addAction(writeShelX);

    // Menu to plot against metadata
    QModelIndexList indexList = selectionModel()->selectedIndexes();
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
            plotasmenu->addAction(newparam);
        }
    }

    // Connections
    connect(normalize,SIGNAL(triggered()),this,SLOT(normalizeToMonitor()));
    connect(writeFullProf,SIGNAL(triggered()),this,SLOT(writeFullProf()));
    connect(writeShelX,SIGNAL(triggered()),this,SLOT(writeShelX()));
    menu->popup(event->globalPos());

}

void PeakTableView::normalizeToMonitor()
{
    bool ok;
    double factor = QInputDialog::getDouble(this,"Enter normalization factor","",1,1,100000000,1,&ok);
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
        selectRow(index.row());
        // If no row selected do nothing else.
        if (!index.isValid())
            return;
        sptrPeak3D peak=peaks[index.row()];
        emit plotPeak(peak);
    }
}

void PeakTableView::writeFullProf()
{
    if (!checkBeforeWritting())
        return;

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save FullProf file"),
                                                    QString::fromStdString(getPeaksRange()+".int"),
                                                    tr("FullProf Files (*.int)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);

    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    peaksModel->writeFullProf(filename.toStdString(),0.2);

}

void PeakTableView::writeShelX()
{
    if (!checkBeforeWritting())
        return;

    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save ShelX file"),
                                                    QString::fromStdString(getPeaksRange()+".hkl"),
                                                    tr("ShelX Files (*.hkl)"),
                                                    nullptr,
                                                    QFileDialog::DontUseNativeDialog);

    CollectedPeaksModel *peaksModel = dynamic_cast<CollectedPeaksModel*>(model());
    if (!peaksModel)
        return;

    peaksModel->writeShelX(filename.toStdString().c_str(),0.2);
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

    for (int i=0;i<nPoints;++i)
    {
        sptrPeak3D p=peaks[indexList[i].row()];
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

bool PeakTableView::checkBeforeWritting()
{
    if (!_normalized)
    {
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
        Eigen::Vector3d hkl=p->getMillerIndices();
        if (std::fabs(hkl[0]-h)>1e-2 || std::fabs(hkl[1]-k)>1e-2 || std::fabs(hkl[2]-l)>1e-2)
            setRowHidden(row,true);
        else
            setRowHidden(row,false);
    }
}



