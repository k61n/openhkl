#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <cstdlib>
#include <functional>
#include <stdexcept>
#include <utility>

#include <Eigen/Dense>

#include <QDateTime>
#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QGraphicsEllipseItem>
#include <QShortcut>
#include <QThread>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QtDebug>

#include "AABB.h"
#include "Basis.h"
#include "BlobFinder.h"
#include "Cluster.h"
#include "Detector.h"
#include "ComponentState.h"
#include "DialogExperiment.h"
#include "DialogUnitCell.h"
#include "Ellipsoid.h"
#include "ExperimentTree.h"
#include "GruberReduction.h"
#include "ILLAsciiData.h"
#include "IShape.h"
#include "Logger.h"
#include "NiggliReduction.h"
#include "NoteBook.h"
#include "Peak3D.h"
#include "PeakTableView.h"
#include "Plotter1D.h"
#include "Sample.h"
#include "Source.h"
#include "UnitCell.h"
#include "Units.h"

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
    ui->dial->setRange(0,15);

//    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
//    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->numor_Widget);
//    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteNumors()));

    _experiments.insert(std::make_pair<std::string,Experiment>("my_exp",Experiment("my_exp","D19-4c")));    

    ui->_dview->setNpixels(_experiments.at("my_exp").getDiffractometer()->getDetector()->getNCols(),_experiments.at("my_exp").getDiffractometer()->getDetector()->getNRows());
    ui->_dview->setDimensions(_experiments.at("my_exp").getDiffractometer()->getDetector()->getWidthAngle(),_experiments.at("my_exp").getDiffractometer()->getDetector()->getHeigth());
    ui->_dview->setDetectorDistance(_experiments.at("my_exp").getDiffractometer()->getDetector()->getDistance());

    ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"),"");
    ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"),"");

    connect(ui->selectionMode,SIGNAL(currentIndexChanged(int)),ui->_dview,SLOT(setCutterMode(int)));
    connect(ui->dial,SIGNAL(valueChanged(int)),ui->_dview,SLOT(setMaxIntensity(int)));
    connect(ui->experimentTree,SIGNAL(sig_plot_data(IData*)),this,SLOT(plotData(IData*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_open_triggered()
{

    DialogExperiment* dlg = new DialogExperiment();

    // The user pressed cancel, return
    if (!dlg->exec())
        return;

    // If no experiment name is provided, pop up a warning
    if (dlg->getExperimentName().isEmpty())
    {
        qWarning() << "Empty experiment name";
        return;
    }

    // Add the experiment
    try
    {
        ui->experimentTree->addExperiment(dlg->getExperimentName().toStdString(),dlg->getInstrumentName().toStdString());
    }
    catch(const std::runtime_error& e)
    {
        qWarning() << e.what();
        return;
    }
}

void MainWindow::plotData(IData* data)
{
    std::cout<<data->getFilename()<<std::endl;
    ui->frameFrame->setEnabled(true);
    ui->intensityFrame->setEnabled(true);

    ui->horizontalScrollBar->setMaximum(data->getNFrames()-1);
    ui->spinBox_Frame->setMaximum(data->getNFrames()-1);
    ui->dial->setRange(1,3000);
    ui->dial->setValue(20);
    ui->_dview->updateView(data,0);
}

void MainWindow::plotUpdate(int numor,int frame)
{
//    QString number = QString::number(numor).rightJustified(6, '0');;

//    QList<QListWidgetItem*> matches=ui->numor_Widget->findItems(number,Qt::MatchExactly);
//    if (matches.size() == 1)
//    {
//        ui->numor_Widget->setCurrentItem(matches[0]);
//        ui->horizontalScrollBar->setValue(frame);
//    }
}

void MainWindow::updatePlot()
{
//    // Get the numor currently selected, return if void
//    QListWidgetItem* item=ui->numor_Widget->currentItem();
//    if (!item)
//    {
//        ui->_dview->updateView(0,0);
//        return;
//    }
//    // Update the data in the detector view
//    std::string numor=item->text().toStdString();
//    auto it=_experiments.at("my_exp").getData(numor);
//    int frame= ui->horizontalScrollBar->value();
//    // Make sure that the frame value for previously selected file is valid for current one
//    if (frame>(it->getNFrames()-1))
//        frame=0;
//    ui->_dview->updateView(it,frame);
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
//    QList<QListWidgetItem*> selNumors = ui->numor_Widget->selectedItems();

//    for (auto it=selNumors.begin();it!=selNumors.end();++it)
//    {
//        _experiments.at("my_exp").removeData((*it)->text().toStdString());
//        delete *it;
//    }
//    qDebug() <<  selNumors.size() << " file(s) have been deleted";

//    updatePlot();

//    if (!ui->numor_Widget->count())
//    {
//        ui->frameFrame->setEnabled(false);
//        ui->intensityFrame->setEnabled(false);
//    }
}

void MainWindow::on_spinBox_max_valueChanged(int arg1)
{
    ui->dial->setValue(arg1);
    updatePlot();
}


void MainWindow::on_numor_Widget_itemDoubleClicked(QListWidgetItem *item)
{
//    ui->numor_Widget->clearSelection();
//    ui->numor_Widget->setSelectionMode(QAbstractItemView::SingleSelection);
//    ui->numor_Widget->setItemSelected(item,true);
//    if (!item)
//       return;
//    std::string numor=item->text().toStdString();
//    int nmax=_experiments.at("my_exp").getData(numor)->getNFrames()-1;
//    ui->horizontalScrollBar->setMaximum(nmax);
//    ui->spinBox_Frame->setMaximum(nmax);

//    updatePlot();
}

void MainWindow::on_numor_Widget_itemActivated(QListWidgetItem *item)
{
//    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
//    if (item == nullptr)
//        return;
//    std::string numor=item->text().toStdString();

//    int nmax=_experiments.at("my_exp").getData(numor)->getNFrames()-1;
//    ui->horizontalScrollBar->setMaximum(nmax);
//    ui->spinBox_Frame->setMaximum(nmax);

//    updatePlot();
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

    QCoreApplication::processEvents();
    ui->progressBar->setEnabled(true);
    ui->progressBar->setMaximum(max);

    std::size_t npeaks=0;
    int comp = 0;
    for (auto& numor : numors)
    {
        numor->clearPeaks();
        numor->loadAllFrames();

        // Get pointers to start of each frame
        std::vector<int*> temp(numor->getNFrames());
        for (unsigned int i=0;i<numor->getNFrames();++i)
        {
            vint& v=numor->getData(i);
            temp[i]=&(v[0]);
        }

        // Finding peaks
        SX::Geometry::blob3DCollection blobs;
        try
        {
            blobs=SX::Geometry::findBlobs3D<int>(temp, numor->getDiffractometer()->getDetector()->getNRows(),numor->getDiffractometer()->getDetector()->getNCols(), threshold+2, 30, 10000, confidence, 0);
        }
        catch(std::exception& e) // Warning if RAM error
        {
            qCritical() << "Peak finder caused a memory exception" << e.what();
        }

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
            p.setSampleState(new SX::Instrument::ComponentState(numor->getSampleInterpolatedState(f)));
            ComponentState detState=numor->getDetectorInterpolatedState(f);
            p.setDetectorEvent(new SX::Instrument::DetectorEvent(numor->getDiffractometer()->getDetector()->createDetectorEvent(center[0],center[1],detState.getValues())));
            p.setWavelength(numor->getDiffractometer()->getSource()->getWavelength());

            if (!dAABB.contains(*p.getPeak()))
                p.setSelected(false);

            numor->addPeak(p);
            npeaks++;
        }

        for (auto& peak : numor->getPeaks())
            peak.second.integrate();

        numor->releaseMemory();

        ui->progressBar->setValue(++comp);
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
//    QList<QListWidgetItem*> selNumors = ui->numor_Widget->selectedItems();
//    std::vector<IData*> list;
//    for (auto it=selNumors.begin();it!=selNumors.end();++it)
//    {
//        auto it1 = _experiments.at("my_exp").getData((*it)->text().toStdString());
//        list.push_back(it1);
//    }
//    return list;
}

void MainWindow::on_actionPeak_List_triggered()
{
    std::vector<IData*> numors=selectedNumors();
    PeakTableView* table=new PeakTableView(this);
    table->setData(numors);
    table->show();
}
