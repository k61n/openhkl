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
#include "IShape.h"
#include "ILLAsciiData.h"
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
#include "Logger.h"
#include "NoteBook.h"
#include <QtDebug>
#include <QDateTime>
#include "Peak3D.h"
#include "Sample.h"
#include "Data.h"
#include "AABB.h"
#include "Source.h"
#include "ComponentState.h"
#include <utility>

using namespace SX::Units;
using namespace SX::Instrument;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),scene(new QGraphicsScene)
{
    ui->setupUi(this);

    // Starting the logger of the main application
    Logger::Instance()->setNoteBook(ui->noteBook);
    qInstallMessageHandler(customMessageHandler);
    qDebug() << "[NSXTool log]" << QDateTime::currentDateTime().toString();

    //
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

    // LAURENT TEMPORARY PATCH
    _experiments.insert(std::make_pair<std::string,Experiment>("my-exp",Experiment("my-exp","D19-4c")));

    ui->_dview->setNpixels(_experiments["my_exp"].getDiffractometer()->getDetector()->getNRows(),_experiments["my_exp"].getDiffractometer()->getDetector()->getNCols());
    ui->_dview->setDimensions(_experiments["my_exp"].getDiffractometer()->getDetector()->getWidthAngle(),_experiments["my_exp"].getDiffractometer()->getDetector()->getHeigth());
    ui->_dview->setDetectorDistance(_experiments["my_exp"].getDiffractometer()->getDetector()->getDistance());
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
    //
    // No files selected
    if (fileNames.isEmpty())
        return;
    // Deal with files
    for (int i=0;i<fileNames.size();++i)
    {
        QFileInfo fileinfo(fileNames[i]);
        QString numor = fileinfo.baseName(); // Return only a file name
        std::string index=numor.toStdString();
        if (!_experiments["my-exp"].hasData(index)) // Add numor only if not already in the list
        {
            QListWidgetItem* litem=new QListWidgetItem(numor);
//            _data.insert(std::unordered_map<std::string,Data>::value_type(index,Data()));
            try
            {
                ILLAsciiData* d = new ILLAsciiData(fileNames[i].toStdString(),_experiments["my-exp"].getDiffractometer(),false);
                _experiments["my-exp"].addData(d);
//                _data[index].fromFile(fileNames[i].toStdString());
            }catch(std::exception& e)
            {
               qWarning() << "Error reading numor: " + fileNames[i] + " " + QString(e.what());
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
//    std::string firstNumor = first->text().toStdString();
//    Data& d=_data[firstNumor];
    IData* it=_experiments["my-exp"].getData().begin()->second;
    ui->horizontalScrollBar->setMaximum(it->getNFrames()-1);
    ui->spinBox_Frame->setMaximum(it->getNFrames()-1);
    ui->dial->setRange(1,3000);
    ui->dial->setValue(20);
    updatePlot();
}

void MainWindow::plotUpdate(int numor,int frame)
{

    QString number = QString::number(numor).rightJustified(6, '0');;

    QList<QListWidgetItem*> matches=ui->numor_Widget->findItems(number,Qt::MatchExactly);
    if (matches.size() == 1)
    {
        ui->numor_Widget->setCurrentItem(matches[0]);
        ui->horizontalScrollBar->setValue(frame);
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
    auto it=_experiments["my-exp"].getData(numor);
    int frame= ui->horizontalScrollBar->value();
    // Make sure that the frame value for previously selected file is valid for current one
    if (frame>(it->getNFrames()-1))
        frame=0;
    ui->_dview->updateView(it,frame);

}

void MainWindow::on_horizontalScrollBar_valueChanged()
{
    updatePlot();
}

void MainWindow::on_dial_valueChanged()
{
    updatePlot();
}

void MainWindow::deleteNumors()
{
    QList<QListWidgetItem*> selNumors = ui->numor_Widget->selectedItems();

    for (auto it=selNumors.begin();it!=selNumors.end();++it)
    {
        _experiments["my-exp"].removeData((*it)->text().toStdString());
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
    int nmax=_experiments["my-exp"].getData(numor)->getNFrames()-1;
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

    int nmax=_experiments["my-exp"].getData(numor)->getNFrames()-1;
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

    std::vector<IData*> numors=selectedNumors();
    qWarning() << "Peak find algorithm: Searching peaks in " << numors.size() << " files";
    int max=numors.size();
    int i=0;

    QCoreApplication::processEvents();
    ui->progressBar->setEnabled(true);
    ui->progressBar->setMaximum(max);

    std::size_t npeaks=0;
    for (auto& numor : numors)
    {
      numor->clearPeaks();
      ui->progressBar->setValue(i++);
      numor->readInMemory();


    // Get pointers to start of each frame
    std::vector<int*> temp(numor->getNFrames());
    for (int i=0;i<numor->getNFrames();++i)
    {
        vint& v=numor->getData(i);
        temp[i]=&(v[0]);
    }


    // Finding peaks
    SX::Geometry::blob3DCollection blobs;
    try
    {
      blobs=SX::Geometry::findBlobs3D<int>(temp, numor->getDiffractometer()->getDetector()->getNRows(),numor->getDiffractometer()->getDetector()->getNCols(), threshold+2, 30, 10000, confidence, 0);
    }catch(std::exception& e) // Warning if RAM error
    {
        qCritical() << "Peak finder caused a memory exception" << e.what();

    }


    //
    int i=0;
    SX::Geometry::AABB<double,3> dAABB(Eigen::Vector3d(0,0,0),Eigen::Vector3d(numor->getDiffractometer()->getDetector()->getNCols(),numor->getDiffractometer()->getDetector()->getNRows(),numor->getNFrames()-1));
    for (auto& blob : blobs)
    {
        Eigen::Vector3d center, eigenvalues;
        Eigen::Matrix3d eigenvectors;
        blob.second.toEllipsoid(confidence, center,eigenvalues,eigenvectors);
        SX::Crystal::Peak3D p(numor);
        p.setPeakShape(new SX::Geometry::Ellipsoid3D(center,eigenvalues,eigenvectors));
        p.setBackgroundShape(new SX::Geometry::Ellipsoid3D(center,eigenvalues.cwiseProduct(Eigen::Vector3d(1.5,1.5,3)),eigenvectors));
        //
        int f=std::floor(center[2]);
//        double omega=numor->_omegas[f]+(center[2]-f)*(numor->_omegas[f+1]-numor->_omegas[f]);
        p.setSampleState(new SX::Instrument::ComponentState(numor->getSampleInterpolatedState(f)));
        ComponentState detState=numor->getDetectorInterpolatedState(f);
        p.setDetectorEvent(new SX::Instrument::DetectorEvent(numor->getDiffractometer()->getDetector()->createDetectorEvent(center[0],center[1],detState.getValues())));
        p.setWavelength(numor->getDiffractometer()->getSource()->getWavelength());

        if (!dAABB.contains(*p.getPeak()))
        {
            p.setSelected(false);
        }
        numor->addPeak(p);
        npeaks++;
    }


    for (auto& peak : numor->getPeaks())
        peak.second.integrate();
    numor->releaseMemory();
    }
    qDebug() << "Found " << npeaks << " peaks";
    updatePlot();
    // Reinitialise progress bar
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(false);

}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    updatePlot();
}

void MainWindow::on_actionUnit_Cell_triggered()
{
    DialogUnitCell* dialog=new DialogUnitCell(this);

    std::vector<std::reference_wrapper<SX::Crystal::Peak3D>> peaks;
    std::vector<IData*> numors=selectedNumors();
    for (auto ptr : numors)
    {
      // Add peaks present in this numor
      for (auto& peak : ptr->getPeaks())
      {
          peaks.push_back(std::ref(peak.second));

      }
    }
    dialog->setPeaks(peaks);
    dialog->show();
}

std::vector<IData*> MainWindow::selectedNumors()
{
    QList<QListWidgetItem*> selNumors = ui->numor_Widget->selectedItems();
    std::vector<IData*> list;
    for (auto it=selNumors.begin();it!=selNumors.end();++it)
    {
        auto it1 = _experiments["my-exp"].getData((*it)->text().toStdString());
        list.push_back(it1);
    }
        return list;
}
void MainWindow::on_actionPeak_List_triggered()
{
    std::vector<IData*> numors=selectedNumors();
    PeakTableView* table=new PeakTableView(this);
    table->setData(numors);
    table->show();
}
