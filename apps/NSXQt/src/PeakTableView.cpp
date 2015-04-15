#include "PeakTableView.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>

#include <QContextMenuEvent>
#include <QHeaderView>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QStandardItemModel>
#include <QtDebug>

#include "IData.h"
#include "Peak3D.h"

PeakTableView::PeakTableView(QWidget *parent)
: QTableView(parent),
  _columnUp(-1,false)
{
    // Make sure that the user can not edit the content of the table
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Selection of a cell in the table select the whole line.
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    //
    setBaseSize(500,500);
    setFocusPolicy(Qt::StrongFocus);

    // Set selection model
    setSelectionMode(QAbstractItemView::MultiSelection);

    // Signal sent when sorting by column
    QHeaderView* horizontal=this->horizontalHeader();
    connect(horizontal,SIGNAL(sectionClicked(int)),this,SLOT(sortByColumn(int)));

    // Signal sent when clicking on a row to plot peak
    QHeaderView* vertical=this->verticalHeader();
    connect(vertical, &QHeaderView::sectionClicked, [&](int index)
                                                 {
                                                  SX::Crystal::Peak3D& peak=_peaks[index].get();
                                                  emit plotPeak(&peak);
                                                 });

    connect(this,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(deselectPeak(QModelIndex)));
}

void PeakTableView::setData(std::vector<SX::Data::IData*> data)
{
    for (auto ptr : data)
    {
        // Add peaks present in this numor to the LatticeFinder
        for (auto& peak : ptr->getPeaks())
            _peaks.push_back(std::ref(*peak));
    }
    constructTable();
}

void PeakTableView::peakChanged(QModelIndex current, QModelIndex last)
{
    if (current.row()!=last.row())
    {
        SX::Crystal::Peak3D& peak=_peaks[current.row()].get();
        emit plotPeak(&peak);
    }
}

void PeakTableView::sortByColumn(int i)
{
    // Only 5 columns || no sorting by sigma || check if peak
    if (i>6 || i==2 || _peaks.size()==0)
        return;

    int& column=std::get<0>(_columnUp);
    bool& up=std::get<1>(_columnUp);
    // If column already sorted, swith direction
    if (i==column)
    {
        up=!up;
    }
    column=i;

    switch (i)
    {
    case 0:
        sortByHKL(up);
        break;
    case 1:
        sortByIntensity(up);
        break;
    case 3:
        sortByNumor(up);
        break;
    case 4:
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
    // Create table with 7 columns
    QStandardItemModel* model=new QStandardItemModel(_peaks.size(),7,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("h k l"));
    model->setHorizontalHeaderItem(1,new QStandardItem("I"));
    model->setHorizontalHeaderItem(2,new QStandardItem(QString((QChar) 0x03C3)+"I"));
    model->setHorizontalHeaderItem(3,new QStandardItem("Corrected I"));
    model->setHorizontalHeaderItem(4,new QStandardItem(QString((QChar) 0x03C3)+"I"));
    model->setHorizontalHeaderItem(5,new QStandardItem("Numor"));
    model->setHorizontalHeaderItem(6,new QStandardItem("Selected"));

    // Setup content of the table
    int i=0;
    for (SX::Crystal::Peak3D& peak : _peaks)
    {
        const Eigen::RowVector3d& hkl=peak.getMillerIndices();
        double l=peak.getLorentzFactor();
        QStandardItem* col0=new QStandardItem(QString::number(hkl[0],'f',2) + "  " + QString::number(hkl[1],'f',2) + "  " + QString::number(hkl[2],'f',2));
        QStandardItem* col1=new QStandardItem(QString::number(peak.getScaledIntensity()/l,'f',2));
        QStandardItem* col2=new QStandardItem(QString::number(peak.getScaledSigma()/l,'f',2));
        double t=peak.getTransmission();
        QStandardItem* col3=new QStandardItem(QString::number(peak.getScaledIntensity()/l/t,'f',2));
        QStandardItem* col4=new QStandardItem(QString::number(peak.getScaledSigma()/l/t,'f',2));
        QStandardItem* col5=new QStandardItem(QString::number(peak.getData()->getMetadata()->getKey<int>("Numor")));
        QStandardItem* col6;
        if (peak.isSelected())
            col6= new QStandardItem(QIcon(":/resources/peakSelectedIcon.png"),"");
        else
            col6= new QStandardItem(QIcon(":/resources/peakDeselectedIcon.png"),"");
        model->setVerticalHeaderItem(i,new QStandardItem(QIcon(":/resources/singlePeakIcon.png"),QString::number(i)));
        model->setItem(i,0,col0);
        model->setItem(i,1,col1);
        model->setItem(i,2,col2);
        model->setItem(i,3,col3);
        model->setItem(i,4,col4);
        model->setItem(i,5,col5);
        model->setItem(i++,6,col6);
    }
    setModel(model);

    this->setColumnWidth(0,150);

    // Signal sent when the user navigates the table (e.g. up down arrow )
    connect(this->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(peakChanged(QModelIndex,QModelIndex)));
}

void PeakTableView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        return;

    QTableView::mousePressEvent(event);
}

void PeakTableView::contextMenuEvent(QContextMenuEvent* event)
{
    // Show all peaks as selected when contet menu is requested
    QMenu* menu=new QMenu(this);
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
    if (indexList.size()) //at least on peak
    {
        QMenu* plotasmenu=menu->addMenu("Plot as");
        SX::Data::MetaData* met=_peaks[indexList[0].row()].get().getData()->getMetadata();
        const std::set<std::string>& keys=met->getAllKeys();
        for (const auto& key : keys)
        {
             try
             {
                met->getKey<double>(key); //Ensure metadata is a Numeric tyoe
             }catch(std::exception& e)
            {
                continue;
            }
             QAction* newparam=new QAction(QString::fromStdString(key),plotasmenu);
             connect(newparam,&QAction::triggered,this,[&](){plotAs(key);}); // New way to connect slot using C++ 2011 lambda sicne Qt 5
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
        for (SX::Crystal::Peak3D& peak : _peaks)
            peak.setScale(factor/peak.getData()->getMetadata()->getKey<double>("monitor"));
        // Keep track of the last selected index before rebuilding the table
        QModelIndex index=currentIndex();
        constructTable();
        selectRow(index.row());
        // If no row selected do nothing else.
        if (!index.isValid() < 0)
            return;
        SX::Crystal::Peak3D& peak=_peaks[index.row()].get();
        emit plotPeak(&peak);
    }
}

void PeakTableView::writeFullProf()
{
    if (!_peaks.size())
        qCritical()<<"No peaks in the table";

    QFileDialog dialog(this);
    dialog.setDefaultSuffix("int");

    QString filename = dialog.getSaveFileName(this,tr("Save FullProf file"), "", tr("FullProf Files (*.int)"));

    if (filename.isEmpty())
        return;

    std::fstream file(filename.toStdString(),std::ios::out);

    if (!file.is_open())
    {
        qCritical()<<"Error writing to this file, please check write permisions";
        return;
    }

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";
    double wave=_peaks[0].get().getData()->getMetadata()->getKey<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wave << " 0 0" << std::endl;
    for (const SX::Crystal::Peak3D& peak : _peaks)
    {
        if (peak.isSelected())
        {
            const Eigen::RowVector3d& hkl=peak.getMillerIndices();

            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
            double l=peak.getLorentzFactor();
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak.getScaledIntensity()/l;
            file << std::fixed << std::setw(14) << std::setprecision(4) << peak.getScaledSigma()/l;
            file << std::setprecision(0) << std::setw(5) << 1  << std::endl;
        }
    }
    if (file.is_open())
        file.close();
}


void PeakTableView::writeShelX()
{
    if (!_peaks.size())
    {
        qCritical()<<"No peaks in the table";
        return;
    }

    QFileDialog dialog(this);
    dialog.setDefaultSuffix("hkl");
    QString filename = dialog.getSaveFileName(this,tr("Save ShelX file"), "", tr("ShelX Files (*.hkl)"));
    if (filename.isEmpty())
        return;

    std::fstream file(filename.toStdString().c_str(),std::ios::out);
    if (!file.is_open())
    {
        qCritical()<<"Error writing to this file, please check write permisions";
        return;
    }

    for (const SX::Crystal::Peak3D& peak : _peaks)
    {
        if (peak.isSelected())
        {
            const Eigen::RowVector3d& hkl=peak.getMillerIndices();

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[0];

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file <<  hkl[1];

            file << std::fixed;
            file << std::setprecision(0);
            file << std::setw(4);
            file << hkl[2];

            double l=peak.getLorentzFactor();
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak.getScaledIntensity()/l;
            file << std::fixed << std::setw(8) << std::setprecision(2) << peak.getScaledSigma()/l <<std::endl;
            }
    }
    if (file.is_open())
        file.close();
}

void PeakTableView::sortByHKL(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](const SX::Crystal::Peak3D& p1, const SX::Crystal::Peak3D& p2)
              {
                return (p2<p1);
              }
              );
    else
        std::sort(_peaks.begin(),_peaks.end());
}

void PeakTableView::sortBySelected(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](const SX::Crystal::Peak3D& p1, const SX::Crystal::Peak3D& p2)
              {
                return (p2.isSelected()<p1.isSelected());
              }
              );
    else
        std::sort(_peaks.begin(),_peaks.end(),
              [&](const SX::Crystal::Peak3D& p1, const SX::Crystal::Peak3D& p2)
              {
                return (p2.isSelected()>p1.isSelected());
              }
              );
}

void PeakTableView::sortByIntensity(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](const SX::Crystal::Peak3D& p1, const SX::Crystal::Peak3D& p2)
                {
                    return ((p1.getScaledIntensity()/p1.getLorentzFactor())>(p2.getScaledIntensity()/p2.getLorentzFactor()));
                });
    else
        std::sort(_peaks.begin(),_peaks.end(),
                  [&](const SX::Crystal::Peak3D& p1, const SX::Crystal::Peak3D& p2)
                    {
                        return ((p1.getScaledIntensity()/p1.getLorentzFactor())<(p2.getScaledIntensity()/p2.getLorentzFactor()));
                    });
}

void PeakTableView::sortByNumor(bool up)
{
    if (up)
        std::sort(_peaks.begin(),_peaks.end(),
              [&](const SX::Crystal::Peak3D& p1, const SX::Crystal::Peak3D& p2)
                {
                    int numor1=p1.getData()->getMetadata()->getKey<int>("Numor");
                    int numor2=p2.getData()->getMetadata()->getKey<int>("Numor");
                    return (numor1>numor2);
                });
    else
        std::sort(_peaks.begin(),_peaks.end(),
              [&](const SX::Crystal::Peak3D& p1, const SX::Crystal::Peak3D& p2)
                {
                    int numor1=p1.getData()->getMetadata()->getKey<int>("Numor");
                    int numor2=p2.getData()->getMetadata()->getKey<int>("Numor");
                    return (numor1<numor2);
                });
}

void PeakTableView::deselectPeak(QModelIndex index)
{
    auto& peak=_peaks[index.row()];
    peak.get().setSelected(!peak.get().isSelected());
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

    for (int i=0;i<nPoints;++i)
    {
        SX::Crystal::Peak3D p=_peaks[indexList[i].row()].get();
        x[i]=p.getData()->getMetadata()->getKey<double>(key);
        y[i]=p.getScaledIntensity();
        e[i]=p.getScaledSigma();
    }

    emit plotData(x,y,e);
}


