#include "PeakTableView.h"
#include <QStandardItemModel>
#include <QHeaderView>
#include "IData.h"
#include <fstream>
#include <QMessageBox>
#include <iomanip>

PeakTableView::PeakTableView(MainWindow* main,QWidget *parent)
    :_main(main),QTableView(parent),_plotter(nullptr)
{
    setBaseSize(400,300);
    setFocusPolicy(Qt::StrongFocus);
    QHeaderView* horizontal=this->horizontalHeader();
    connect(horizontal,SIGNAL(sectionClicked(int)),this,SLOT(sortByColumn(int)));
    QHeaderView* vertical=this->verticalHeader();
    connect(vertical,SIGNAL(sectionClicked(int)),this,SLOT(plotPeak(int)));
    connect(vertical,SIGNAL(sectionEntered(int)),this,SLOT(plotPeak(int)));
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
    SLOT(customMenuRequested(QPoint)));
    connect(this,SIGNAL(plot2DUpdate(int,int)),_main,SLOT(plotUpdate(int,int)));   
}

void PeakTableView::setData(const std::vector<Data *> numors)
{
    for (Data* ptr : numors)
    {
        // Add peaks present in this numor to the LatticeFinder
        for (const auto& peak : ptr->_rpeaks)
        {
            _peaks.push_back(std::cref(peak.second));
        }
    }
    constructTable();

}

void PeakTableView::plotPeak(int i)
{
    const SX::Geometry::Peak3D& peak=_peaks[i].get();
    if (!_plotter)
        _plotter=new PeakPlotter(this);
    _plotter->setPeak(peak);
    _plotter->show();
    emit plot2DUpdate(peak.getData()->_mm->getMetaData()->getKey<int>("Numor"),std::round(peak.getPeak()->getCenter()[2]));
}

void PeakTableView::sortByColumn(int i)
{
    if (i==0) // Sort by HKL
        std::sort(_peaks.begin(),_peaks.end());
    if (i==1) //Sort by Intensity
        std::sort(_peaks.begin(),_peaks.end(),
                  [&](const SX::Geometry::Peak3D& p1, const SX::Geometry::Peak3D& p2)
        {
            return (p1.peakTotalCounts()<p2.peakTotalCounts());
        });
    constructTable();
}

void PeakTableView::constructTable()
{
    QStandardItemModel* model=new QStandardItemModel(_peaks.size(),5,this);
    model->setHorizontalHeaderItem(0,new QStandardItem("h,k,l"));
    model->setHorizontalHeaderItem(1,new QStandardItem("I"));
    model->setHorizontalHeaderItem(2,new QStandardItem(QString((QChar) 0x03C3)+"I"));
    model->setHorizontalHeaderItem(3,new QStandardItem("Numor"));
    model->setHorizontalHeaderItem(4,new QStandardItem("Selected"));
    int i=0;
    for (const SX::Geometry::Peak3D& peak : _peaks)
    {
        const Eigen::RowVector3d& hkl=peak.getMillerIndices();
        double l=peak.getLorentzFactor();
        QStandardItem* col1=new QStandardItem(QString::number(hkl[0])+","+QString::number(hkl[1])+","+QString::number(hkl[2]));
        QStandardItem* col2=new QStandardItem(QString::number(peak.peakTotalCounts()/l));
        QStandardItem* col3=new QStandardItem(QString::number(sqrt(peak.peakTotalCounts()/l)));
        QStandardItem* col4=new QStandardItem(QString::number(peak.getData()->_mm->getMetaData()->getKey<int>("Numor")));
        model->setVerticalHeaderItem(i,new QStandardItem(QIcon(":/resources/singlePeakIcon.png"),QString::number(i)));
        model->setItem(i,0,col1);
        model->setItem(i,1,col2);
        model->setItem(i,2,col3);
        model->setItem(i++,3,col4);
    }
    setModel(model);
}


void PeakTableView::customMenuRequested(QPoint pos)
{
    QMenu* menu=new QMenu(this);
    QMenu* writeMenu=menu->addMenu("Write");
    QAction* writeFullProf=new QAction("FullProf file",writeMenu);
    QAction* writeShelX=new QAction("SHELX file",writeMenu);
    writeMenu->addAction(writeFullProf);
    writeMenu->addAction(writeShelX);
    connect(writeFullProf,SIGNAL(triggered()),this,SLOT(writeFullProf()));
    connect(writeShelX,SIGNAL(triggered()),this,SLOT(writeShelX()));
    menu->popup(viewport()->mapToGlobal(pos));
}

void PeakTableView::writeFullProf()
{
    if (!_peaks.size())
        QMessageBox::critical(this,"Error writing","No peaks in the table");
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save FullProf file"), "", tr("FullProd Files (*.int)"));
    std::fstream file(fileName.toStdString().c_str(),std::ios::out);
    if (!file.is_open())
        QMessageBox::critical(this,"Error writing","Error writing to this file, please check write permisions");
    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";
    double wave=_peaks[0].get().getData()->_mm->getMetaData()->getKey<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wave << " 0 0" << std::endl;
    for (const SX::Geometry::Peak3D& peak : _peaks)
    {
        const Eigen::RowVector3d& hkl=peak.getMillerIndices();

        file << std::setprecision(0);
        file << std::setw(4);
        file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
         double l=peak.getLorentzFactor();
        file << std::fixed << std::setw(14) << std::setprecision(4) << peak.peakTotalCounts()/l;
        file << std::fixed << std::setw(14) << std::setprecision(4) << sqrt(peak.peakTotalCounts())/l;
        file << std::setprecision(0) << std::setw(5) << 1  << std::endl;
    }
    file.close();
}


void PeakTableView::writeShelX()
{
    if (!_peaks.size())
        QMessageBox::critical(this,"Error writing","No peaks in the table");
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save ShelX file"), "", tr("ShelX Files (*.hkl)"));

    std::fstream file(fileName.toStdString().c_str(),std::ios::out);
    if (!file.is_open())
        QMessageBox::critical(this,"Error writing","Error writing to this file, please check write permisions");

    for (const SX::Geometry::Peak3D& peak : _peaks)
    {
        const Eigen::RowVector3d& hkl=peak.getMillerIndices();

        file << std::setprecision(0);
        file << std::setw(4);
        file << hkl[0] << std::setw(4) <<  hkl[1] << std::setw(4) << hkl[2];
         double l=peak.getLorentzFactor();
        file << std::fixed << std::setw(8) << std::setprecision(2) << peak.peakTotalCounts()/l;
        file << std::fixed << std::setw(8) << std::setprecision(2) << sqrt(peak.peakTotalCounts())/l <<std::endl;
    }
    file.close();
}
