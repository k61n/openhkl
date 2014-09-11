#include "MainWindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <QFileDialog>
#include <QShortcut>
#include <QThread>
#include <QGraphicsEllipseItem>
#include <QGraphicsBlurEffect>
#include <QMessageBox>
#include <QMouseEvent>
#include "BlobFinder.h"
#include <Ellipsoid.h>
#include <Plotter1D.h>
#include <QDebug>
#include "IShape.h"
#include "ILLAsciiDataReader.h"
#include "Units.h"
#include "Detector.h"
#include "Cluster.h"
#include "UnitCell.h"
#include "NiggliReduction.h"
#include "GruberReduction.h"
#include <Eigen/Dense>
#include "Basis.h"
#include <QProgressDialog>
#include <functional>
#include <PeakTableView.h>
#include <DialogUnitCell.h>

using namespace SX::Units;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),scene(new QGraphicsScene)
{
    ui->setupUi(this);

    ui->mainToolBar->setIconSize(QSize(32,32));

    ui->frameFrame->setEnabled(false);
    ui->intensityFrame->setEnabled(false);

    scene->setParent(ui->_dview);
    ui->_dview->setScene(scene);
    ui->_dview->setInteractive(true);
    ui->_dview->setDragMode(QGraphicsView::RubberBandDrag);
    //ui->_dview->setDragMode(ui->_dview->ScrollHandDrag);
    ui->dial->setRange(0,15);

    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->numor_Widget);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteNumors()));

    // Specific to D19 detector
    ui->_dview->setNpixels(640,256);
    ui->_dview->setDimensions(120.0,0.4);
    ui->_dview->setDetectorDistance(0.764);
    //

    ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"),"");

    connect(ui->selectionMode,SIGNAL(currentIndexChanged(int)),ui->_dview,SLOT(setCutterMode(int)));
    connect(ui->dial,SIGNAL(valueChanged(int)),ui->_dview,SLOT(setMaxIntensity(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_action_open_triggered()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList fileNames;
    fileNames= dialog.getOpenFileNames(this,"select numors","");
    // No files selected
    if (fileNames.isEmpty())
        return;
    // Deal with files
    for (int i=0;i<fileNames.size();++i)
    {
        QFileInfo fileinfo(fileNames[i]);
        QString numor = fileinfo.baseName(); // Return only a file name
        std::string index=numor.toStdString();
        auto it=_data.find(index);
        if (it==_data.end()) // Add numor only if not already in the list
        {
        QListWidgetItem* litem=new QListWidgetItem(numor);
        _data.insert(std::unordered_map<std::string,Data>::value_type(index,Data()));
        try
        {
            _data[index].fromFile(fileNames[i].toStdString());
        }catch(std::exception& e)
        {
           ui->textLogger->log(Logger::ERROR) << "Error reading numor: " << fileNames[i].toStdString() << std::endl << e.what();
           ui->textLogger->flush();

           continue;
        }
        ui->numor_Widget->addItem(litem);
        }
    }
    QListWidgetItem* first=ui->numor_Widget->item(0);

    if (!first)
        return;

    ui->frameFrame->setEnabled(true);
    ui->intensityFrame->setEnabled(true);

    ui->numor_Widget->setCurrentItem(first);
    std::string firstNumor = first->text().toStdString();
    Data& d=_data[firstNumor];
    ui->horizontalScrollBar->setMaximum(d._nblocks-1);
    ui->spinBox_Frame->setMaximum(d._nblocks-1);
    ui->dial->setRange(1,3000);
    ui->dial->setValue(20);
    updatePlot();
}

void MainWindow::plotUpdate(int numor,int frame)
{
    std::string number=QString::number(numor).toStdString();
    auto it=_data.find(number);
    if (it!=_data.end())
    {
        Data* ptr=&(it->second);
        ui->_dview->updateView(ptr,frame);
    }
    ui->horizontalScrollBar->setValue(frame);
}

void MainWindow::updatePlot()
{
    // Get the numor currently selected, return if void
    QListWidgetItem* item=ui->numor_Widget->currentItem();
    if (!item)
    {
        ui->_dview->updateView(0,0);
        return;
    }
    // Update the data in the detector view
    std::string numor=item->text().toStdString();
    auto it=_data.find(numor);
    if (it!=_data.end())
    {
        Data* ptr=&(it->second);
        int frame= ui->horizontalScrollBar->value();
        // Make sure that the frame value for previously selected file is valid for current one
        if (frame>ptr->_nblocks-1)
            frame=0;
        // Keep previous color
        int colormax= ui->dial->value();
        ui->_dview->updateView(ptr,frame);
    }

}

void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    updatePlot();
}

void MainWindow::on_dial_valueChanged(int value)
{
    updatePlot();
}

void MainWindow::deleteNumors()
{
    QList<QListWidgetItem*> selNumors = ui->numor_Widget->selectedItems();

    for (auto it=selNumors.begin();it!=selNumors.end();++it)
    {
        auto it1 = _data.find((*it)->text().toStdString());
        if (it1 != _data.end())
            _data.erase(it1);
        delete *it;
    }
    qDebug() <<  selNumors.size() << " file(s) have been deleted";

    updatePlot();

    if (!ui->numor_Widget->count())
    {
        ui->frameFrame->setEnabled(false);
        ui->intensityFrame->setEnabled(false);
    }

}


void MainWindow::on_spinBox_max_valueChanged(int arg1)
{
    ui->dial->setValue(arg1);
    updatePlot();
}


void MainWindow::on_numor_Widget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->numor_Widget->clearSelection();
    ui->numor_Widget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->numor_Widget->setItemSelected(item,true);
    if (!item)
       return;
    std::string numor=item->text().toStdString();
    int nmax=_data[numor]._nblocks-1;
    ui->horizontalScrollBar->setMaximum(nmax);
    ui->spinBox_Frame->setMaximum(nmax);


    updatePlot();
}

void MainWindow::on_numor_Widget_itemActivated(QListWidgetItem *item)
{
    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
    if (item == nullptr)
        return;
    std::string numor=item->text().toStdString();

    int nmax=_data[numor]._nblocks-1;
    ui->horizontalScrollBar->setMaximum(nmax);
    ui->spinBox_Frame->setMaximum(nmax);
    updatePlot();

}


void MainWindow::on_action_peak_find_triggered()
{

    DialogPeakFind* dialog= new DialogPeakFind();

    dialog->setFixedSize(400,200);

    //
    if (!dialog->exec())
        return;
    // Get Confidence and threshold
    double confidence=dialog->getConfidence();
    double threshold=dialog->getThreshold();   

    std::vector<Data*> numors=selectedNumors();

    int max=numors.size();
    int i=0;

    QCoreApplication::processEvents();
    ui->progressBar->setEnabled(true);
    ui->progressBar->setMaximum(max);

    for (auto& numor : numors)
    {

      ui->progressBar->setValue(i++);
      ui->textLogger->log(Logger::INFO) << "Reading " << numor->_mm->getMetaData()->getKey<int>("Numor") << " in memory";
      ui->textLogger->flush();
      QCoreApplication::processEvents();
      numor->readInMemory();


    // Get pointers to start of each frame
    std::vector<int*> temp(numor->_nblocks);
    for (int i=0;i<numor->_nblocks;++i)
    {
        vint& v=numor->_data[i];
        temp[i]=&(v[0]);
    }


    // Finding peaks
    SX::Geometry::blob3DCollection blobs;
    try
    {
      blobs=SX::Geometry::findBlobs3D<int>(temp, 256,640, threshold+2, 30, 10000, confidence, 0);
    }catch(std::exception& e) // Warning if RAM error
    {


    }
    ui->textLogger->log(Logger::INFO) << "Found " << blobs.size() << " peaks";
    ui->textLogger->flush();
    QCoreApplication::processEvents();

    //
    int i=0;
    numor->_peaks.clear();

    for (auto& blob : blobs)
    {
        Eigen::Vector3d center, eigenvalues;
        Eigen::Matrix3d eigenvectors;
        blob.second.toEllipsoid(confidence, center,eigenvalues,eigenvectors);
        SX::Geometry::Ellipsoid<double,3> a(center,eigenvalues,eigenvectors);
        numor->_peaks[i]=a;
        SX::Geometry::Peak3D p(numor);
        p.setPeak(new SX::Geometry::Ellipsoid3D(center,eigenvalues,eigenvectors));
        p.setBackground(new SX::Geometry::Ellipsoid3D(center,eigenvalues*3,eigenvectors));
        Eigen::Vector3d Q=numor->_detector->getQ(center[0],center[1],numor->_wavelength);
        double gamma,nu;
        numor->_detector->getGammaNu(center[0],center[1],gamma,nu);
        int f=std::floor(center[2]);
        double omega=numor->_omegas[f]+(center[2]-f)*(numor->_omegas[f+1]-numor->_omegas[f]);
        Eigen::Transform<double,3,Eigen::Affine> t=numor->_sample->getInverseHomMatrix({omega,numor->_chi,numor->_phi});
        Eigen::Vector3d realQ=t*Q;
        p.setQ(realQ);
        p.setGammaNu(gamma,nu);
        numor->_rpeaks.insert(Data::maprealPeaks::value_type(i++,p));
    }

    ui->textLogger->log(Logger::INFO) << "Integrating peaks...";
    ui->textLogger->flush();
    QCoreApplication::processEvents();
    for (auto& peak : numor->_rpeaks)
        peak.second.integrate();
    //
    setCursor(Qt::ArrowCursor);
    numor->releaseMemory();
    }
    updatePlot();
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(false);

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    updatePlot();
}





void MainWindow::on_actionUnit_Cell_triggered()
{
    DialogUnitCell* dialog=new DialogUnitCell(this);

    std::vector<std::reference_wrapper<SX::Geometry::Peak3D>> peaks;
    std::vector<Data*> numors=selectedNumors();
    for (auto ptr : numors)
    {
      // Add peaks present in this numor
      for (auto& peak : ptr->_rpeaks)
      {
          peaks.push_back(std::ref(peak.second));

      }
    }
    dialog->setPeaks(peaks);
    dialog->show();
}

std::vector<Data*> MainWindow::selectedNumors()
{
    QList<QListWidgetItem*> selNumors = ui->numor_Widget->selectedItems();
    std::vector<Data*> list;
    for (auto it=selNumors.begin();it!=selNumors.end();++it)
    {
        auto it1 = _data.find((*it)->text().toStdString());
        if (it1!= _data.end())
            list.push_back(&(it1->second));
    }
        return list;
}
void MainWindow::on_actionPeak_List_triggered()
{
    std::vector<Data*> numors=selectedNumors();
    PeakTableView* table=new PeakTableView(this);
    table->setData(numors);
    table->show();
}
