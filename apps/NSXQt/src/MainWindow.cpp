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
#include "DetectorScene.h"
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
#include "Sample.h"
#include "Source.h"
#include "UnitCell.h"
#include "Units.h"
#include "SXCustomPlot.h"
#include "PeakCustomPlot.h"
#include "PeakTableView.h"

using namespace SX::Units;
using namespace SX::Instrument;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _experiments(),
    _currentData(nullptr)
{
    _ui->setupUi(this);

    // Starting the logger of the main application
    Logger::Instance()->setNoteBook(_ui->noteBook);
    qInstallMessageHandler(customMessageHandler);
    qDebug() << "[NSXTool log]" << QDateTime::currentDateTime().toString();

    //
    _ui->frameFrame->setEnabled(false);
    _ui->intensityFrame->setEnabled(false);

    _ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"),"");

    _ui->splitter->setStretchFactor(0,10);
    _ui->splitter->setStretchFactor(1,90);


    connect(_ui->experimentTree,SIGNAL(plotData(SX::Data::IData*)),_ui->_dview->getScene(),SLOT(setData(SX::Data::IData*)));
    connect(_ui->experimentTree,SIGNAL(plotData(SX::Data::IData*)),this,SLOT(changeData(SX::Data::IData*)));
    connect(_ui->experimentTree,SIGNAL(showPeakList(std::vector<SX::Data::IData*>)),this,SLOT(showPeakList(std::vector<SX::Data::IData*>)));

    connect(_ui->horizontalScrollBar,SIGNAL(valueChanged(int)),_ui->_dview->getScene(),SLOT(changeFrame(int)));
    connect(_ui->dial,SIGNAL(valueChanged(int)),_ui->_dview->getScene(),SLOT(setMaxIntensity(int)));

    // Pass interaction mode to Detector Scene
    connect(_ui->selectionMode,SIGNAL(currentIndexChanged(int)),_ui->_dview->getScene(),SLOT(changeInteractionMode(int)));

    _ui->plotterDockWidget_2->hide();

    connect(_ui->_dview->getScene(),SIGNAL(plotPeak(SX::Crystal::Peak3D*)),this,SLOT(plotPeak(SX::Crystal::Peak3D*)));

}

MainWindow::~MainWindow()
{
    delete _ui;
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
        _ui->experimentTree->addExperiment(dlg->getExperimentName().toStdString(),dlg->getInstrumentName().toStdString());
    }
    catch(const std::runtime_error& e)
    {
        qWarning() << e.what();
        return;
    }
}

void MainWindow::changeData(IData* data)
{
    _ui->frameFrame->setEnabled(true);
    _ui->intensityFrame->setEnabled(true);

    int frameMax = data->getNFrames()-1;

    int frame = _ui->horizontalScrollBar->value();

    if (frame > frameMax)
        frame = frameMax;

    _ui->horizontalScrollBar->setValue(frame);

    _ui->horizontalScrollBar->setMaximum(frameMax);

    _ui->spinBox_Frame->setMaximum(frameMax);

    _ui->dial->setRange(1,3000);

}

void MainWindow::showPeakList(std::vector<SX::Data::IData*> data)
{
    PeakTableView* table=new PeakTableView();
    table->setData(data);
    table->show();
    // Ensure plot1D is updated
    connect(table,SIGNAL(plotPeak(SX::Crystal::Peak3D*)),this,SLOT(plotPeak(SX::Crystal::Peak3D*)));
    // Ensure DetectorScene is updated
}


void MainWindow::on_action_peak_find_triggered()
{

    std::vector<IData*> numors = _ui->experimentTree->getSelectedNumors();
    if (numors.empty())
    {
        qWarning() << "No numors selected for finding peaks";
        return;
    }

    DialogPeakFind* dialog= new DialogPeakFind();

    dialog->setFixedSize(400,200);
    if (!dialog->exec())
        return;

    // Get Confidence and threshold
    double confidence=dialog->getConfidence();
    double threshold=dialog->getThreshold();   

    qWarning() << "Peak find algorithm: Searching peaks in " << numors.size() << " files";
    int max=numors.size();

    QCoreApplication::processEvents();
    _ui->progressBar->setEnabled(true);
    _ui->progressBar->setMaximum(max);

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
            SX::Crystal::Peak3D* p = new Peak3D(numor);
            p->setPeakShape(new SX::Geometry::Ellipsoid3D(center,eigenvalues,eigenvectors));
            p->setBackgroundShape(new SX::Geometry::Ellipsoid3D(center,eigenvalues.cwiseProduct(Eigen::Vector3d(1.5,1.5,3)),eigenvectors));
            //
            int f=std::floor(center[2]);
            p->setSampleState(new SX::Instrument::ComponentState(numor->getSampleInterpolatedState(f)));
            ComponentState detState=numor->getDetectorInterpolatedState(f);
            p->setDetectorEvent(new SX::Instrument::DetectorEvent(numor->getDiffractometer()->getDetector()->createDetectorEvent(center[0],center[1],detState.getValues())));
            p->setWavelength(numor->getDiffractometer()->getSource()->getWavelength());

            if (!dAABB.contains(*(p->getPeak())))
                p->setSelected(false);

            numor->addPeak(p);
            npeaks++;
        }

        for (auto& peak : numor->getPeaks())
            peak->integrate();

        numor->releaseMemory();

        _ui->progressBar->setValue(++comp);
    }

    qDebug() << "Found " << npeaks << " peaks";
    // Reinitialise progress bar
    _ui->progressBar->setValue(0);
    _ui->progressBar->setEnabled(false);

    _ui->_dview->getScene()->updatePeaks();

}

void MainWindow::on_actionUnit_Cell_triggered()
{

    std::vector<IData*> numors = _ui->experimentTree->getSelectedNumors();
    if (numors.empty())
    {
        qWarning() << "No numors selected for finding peaks";
        return;
    }

    DialogUnitCell* dialog=new DialogUnitCell(this);

    std::vector<std::reference_wrapper<SX::Crystal::Peak3D>> peaks;
    for (auto ptr : numors)
    {
      // Add peaks present in this numor
      for (auto& peak : ptr->getPeaks())
          peaks.push_back(std::ref(*peak));
    }
    dialog->setPeaks(peaks);
    dialog->show();
}

void MainWindow::on_actionPixel_position_triggered()
{
   _ui->_dview->getScene()->changeCursorMode(DetectorScene::PIXEL);
}

void MainWindow::on_actionGamma_Nu_triggered()
{
    _ui->_dview->getScene()->changeCursorMode(DetectorScene::GAMMA);
}

void MainWindow::on_action2_Theta_triggered()
{
     _ui->_dview->getScene()->changeCursorMode(DetectorScene::THETA);
}


void MainWindow::on_actionD_spacing_triggered()
{
  _ui->_dview->getScene()->changeCursorMode(DetectorScene::DSPACING);
}

void MainWindow::on_actionLogger_triggered()
{
    if (_ui->loggerDockWidget->isHidden())
        _ui->loggerDockWidget->show();
    else
        _ui->loggerDockWidget->hide();
}

void MainWindow::on_action1D_Peak_Ploter_triggered()
{
    if (_ui->plotterDockWidget_2->isHidden())
        _ui->plotterDockWidget_2->show();
    else
        _ui->plotterDockWidget_2->hide();
}

void MainWindow::plotPeak(SX::Crystal::Peak3D* peak)
{
//    emit ExperimentTree::plotData(peak);
//    _ui->_dview->getScene()->setData(peak->getData());
//    changeData(peak->getData());
//    int frame = peak->getPeak()->getCenter()[2];
//    _ui->_dview->getScene()->changeFrame(frame);
//    _ui->horizontalScrollBar->setValue(frame);

    _ui->plot1D->setPeak(peak);
}
