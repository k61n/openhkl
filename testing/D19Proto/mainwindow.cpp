#include "mainwindow.h"
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

    ui->selectionMode->addItem(QIcon(":/zoomIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/cutlineIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/horizontalsliceIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/verticalsliceIcon.png"),"");

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
    dialog.setDirectory("/home/chapon/Data/D19/July2014/data/DKDP/");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    QStringList fileNames;
    fileNames= dialog.getOpenFileNames(this,"select numors","/home/chapon/Data/D19/July2014/data/DKDP/");
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
        }catch(...)
        {

           qDebug() << "File: " << fileNames[i] << " is not readable as a Numor";

           continue;
        }
        ui->numor_Widget->addItem(litem);
        }
    }
    QListWidgetItem* first=ui->numor_Widget->item(0);

    if (!first)
        return;

    qDebug() << "Read " << fileNames.size() << " file(s)";

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



void MainWindow::ShowContextMenu(const QPoint& pos)
{
    loggerContextMenu ->exec(QCursor::pos());
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
    QProgressDialog progress("Copying files...", "Abort Copy", 0, max, this);
    progress.show();
    progress.setValue(i);
    progress.setWindowModality(Qt::WindowModal);
    progress.setStyleSheet(QLatin1String("QProgressBar {\n"
                                         "border: 1px solid black;\n"
                                         "text-align: top;\n"
                                         "padding: 1px;\n"
                                         "border-bottom-right-radius: 7px;\n"
                                         "border-bottom-left-radius: 7px;\n"
                                         "background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,\n"
                                         "stop: 0 #fff,\n"
                                         "stop: 0.4999 #eee,\n"
                                         "stop: 0.5 #ddd,\n"
                                         "stop: 1 #eee );\n"
                                         "width: 15px;\n"
                                         "}\n"
                                         "\n"
                                         "QProgressBar::chunk {\n"
                                         "background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,\n"
                                         "stop: 0 #78d,\n"
                                         "stop: 1 #238 );\n"
                                         "border-bottom-right-radius: 7px;\n"
                                         "border-bottom-left-radius: 7px;\n"
                                         "border: 1px solid black;\n"
                                         "}"));
    QCoreApplication::processEvents();

    for (auto& numor : numors)
    {
      progress.setValue(i++);
      QCoreApplication::processEvents();
//    ui->textLogger->log(Logger::INFO) << "Start reading " << numor-> << " in memory";
//    ui->textLogger->flush();
      numor->readInMemory();
//    ui->textLogger->log(Logger::INFO) << "Finished reading" << numor << " in memory";
//    ui->textLogger->flush();

    int background_level=2;

    // Get pointers to start of each frame
    std::vector<int*> temp(numor->_nblocks);
    for (int i=0;i<numor->_nblocks;++i)
    {
        vint& v=numor->_data[i];
        temp[i]=&(v[0]);
    }


    // Finding peaks
    SX::Geometry::blob3DCollection blobs;
    ui->textLogger->log(Logger::INFO) << "Peak find starts";
    ui->textLogger->flush();
    try
    {
      blobs=SX::Geometry::findBlobs3D<int>(temp, 256,640, threshold*background_level, 5, 10000, confidence, 0);
    }catch(std::exception& e) // Warning if RAM error
    {


    }
    ui->textLogger->log(Logger::INFO) << "Peak find finished, found " << blobs.size() << "peaks";
    ui->textLogger->flush();

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

    ui->textLogger->log(Logger::INFO) << "Start integrating peaks";
    ui->textLogger->flush();
    for (int i=0;i<numor->_rpeaks.size();++i)
        numor->_rpeaks[i].integrate();
    ui->textLogger->log(Logger::INFO) << "Finished integrating peaks";
    ui->textLogger->flush();
    //
    setCursor(Qt::ArrowCursor);
    numor->releaseMemory();
    }
}

void MainWindow::on_textLogger_customContextMenuRequested(const QPoint &pos)
{
    std::cout  << pos.x() << std::endl;
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
