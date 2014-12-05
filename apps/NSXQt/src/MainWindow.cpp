#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <cmath>
#include <functional>
#include <stdexcept>
#include <utility>

#include <Eigen/Dense>

#include <CutterGraphicsItem.h>
#include <QDateTime>
#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QGraphicsEllipseItem>
#include <QShortcut>
#include <QThread>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QtDebug>
#include <QTransform>

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
#include "SXPlot.h"
#include "PeakTableView.h"
#include "CutLineGraphicsItem.h"
#include "CutSliceGraphicsItem.h"
#include "PlottableGraphicsItem.h"
#include "PeakGraphicsItem.h"
#include "PlotFactory.h"
#include <ctime>

using namespace SX::Units;
using namespace SX::Instrument;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
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
    _ui->selectionMode->addItem(QIcon(":/resources/selectionIcon.png"),"");

    _ui->splitter->setStretchFactor(0,10);
    _ui->splitter->setStretchFactor(1,90);

    // signals and slots
    connect(_ui->experimentTree,SIGNAL(plotData(SX::Data::IData*)),_ui->_dview->getScene(),SLOT(setData(SX::Data::IData*)));
    connect(_ui->experimentTree,SIGNAL(plotData(SX::Data::IData*)),this,SLOT(changeData(SX::Data::IData*)));
    connect(_ui->experimentTree,SIGNAL(showPeakList(std::vector<SX::Data::IData*>)),this,SLOT(showPeakList(std::vector<SX::Data::IData*>)));
    connect(_ui->frame,SIGNAL(valueChanged(int)),_ui->_dview->getScene(),SLOT(changeFrame(int)));
    connect(_ui->intensity,SIGNAL(valueChanged(int)),_ui->_dview->getScene(),SLOT(setMaxIntensity(int)));
    connect(_ui->selectionMode,SIGNAL(currentIndexChanged(int)),_ui->_dview->getScene(),SLOT(changeInteractionMode(int)));
    connect(_ui->_dview->getScene(),SIGNAL(updatePlot(PlottableGraphicsItem*)),this,SLOT(updatePlot(PlottableGraphicsItem*)));
    connect(_ui->action_open,SIGNAL(triggered()),_ui->experimentTree,SLOT(createNewExperiment()));

    _ui->plotterDockWidget->show();

}

MainWindow::~MainWindow()
{
    delete _ui;
}

void MainWindow::changeData(IData* data)
{
    _ui->frameFrame->setEnabled(true);
    _ui->intensityFrame->setEnabled(true);

    int frameMax = data->getNFrames()-1;

    int frame = _ui->frame->value();

    if (frame > frameMax)
        frame = frameMax;

    _ui->frame->setValue(frame);

    _ui->frame->setMaximum(frameMax);

    _ui->spinBox_Frame->setMaximum(frameMax);

    _ui->intensity->setValue(10);

}

void MainWindow::showPeakList(std::vector<SX::Data::IData*> data)
{

    if (data.empty())
        return;

    PeakTableView* table=new PeakTableView();
    table->setData(data);
    table->show();
    // Ensure plot1D is updated
    connect(table,SIGNAL(plotPeak(SX::Crystal::Peak3D*)),this,SLOT(plotPeak(SX::Crystal::Peak3D*)));
    connect(table,
            SIGNAL(plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&)),
            this,
            SLOT(plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&)));
}

void MainWindow::plotPeak(SX::Crystal::Peak3D* peak)
{
    auto scenePtr=_ui->_dview->getScene();
    // Ensure that frames
    changeData(peak->getData());
    scenePtr->setData(peak->getData(),std::round(peak->getPeak()->getCenter()[2]));
    auto pgi=scenePtr->findPeakGraphicsItem(peak);
    if (pgi)
    {
        updatePlot(pgi);
    }
}

void MainWindow::on_action_peak_find_triggered()
{

    _ui->_dview->getScene()->clearPeaks();

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

    for (auto numor : numors)
    {
        numor->clearPeaks();
        clock_t t1=clock();
        numor->readInMemory();
        qDebug() << static_cast<double>(clock()-t1)/CLOCKS_PER_SEC;
        qDebug() << "Read data";
        // Get pointers to start of each frame
        std::vector<int*> temp(numor->getNFrames());
        for (unsigned int i=0;i<numor->getNFrames();++i)
        {
            Eigen::MatrixXi& v=numor->getData(i);
            temp[i]=v.data();
        }

        // Finding peaks
        SX::Geometry::blob3DCollection blobs;
        try
        {
            clock_t t1=clock();
            blobs=SX::Geometry::findBlobs3D<int>(temp, numor->getDiffractometer()->getDetector()->getNRows(),numor->getDiffractometer()->getDetector()->getNCols(), threshold+2, 30, 10000, confidence, 0);
            qDebug() << static_cast<double>(clock()-t1)/CLOCKS_PER_SEC;
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
            eigenvalues[0]*=2.0;
            eigenvalues[1]*=2.0;
            eigenvalues[2]*=3.0;
            p->setBackgroundShape(new SX::Geometry::Ellipsoid3D(center,eigenvalues,eigenvectors));
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
        numor->close();
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
    if (_ui->plotterDockWidget->isHidden())
        _ui->plotterDockWidget->show();
    else
        _ui->plotterDockWidget->hide();
}

void MainWindow::plotData(const QVector<double>& x,const QVector<double>& y,const QVector<double>& e)
{

    if (_ui->plot1D->getType().compare("simple")!=0)
    {
        // Store the old size policy
        QSizePolicy oldSizePolicy = _ui->plot1D->sizePolicy();
        // Remove the current plotter from the ui
        _ui->horizontalLayout_4->removeWidget(_ui->plot1D);
        // Delete the plotter instance
        delete _ui->plot1D;

        PlotFactory* pFactory=PlotFactory::Instance();

        _ui->plot1D = pFactory->create("simple",_ui->dockWidgetContents_4);

        // Restore the size policy
        _ui->plot1D->setSizePolicy(oldSizePolicy);

        // Sets some properties of the plotter
        _ui->plot1D->setObjectName(QStringLiteral("1D plotter"));
        _ui->plot1D->setFocusPolicy(Qt::StrongFocus);
        _ui->plot1D->setStyleSheet(QStringLiteral(""));

        // Add the plot to the ui
        _ui->horizontalLayout_4->addWidget(_ui->plot1D);
    }

    _ui->plot1D->graph(0)->setDataValueError(x,y,e);
    _ui->plot1D->rescaleAxes();

    _ui->plot1D->replot();

}

void MainWindow::updatePlot(PlottableGraphicsItem* item)
{
    if (!item)
        return;

    if (!item->isPlottable(_ui->plot1D))
    {
        // Store the old size policy
        QSizePolicy oldSizePolicy = _ui->plot1D->sizePolicy();
        // Remove the current plotter from the ui
        _ui->horizontalLayout_4->removeWidget(_ui->plot1D);
        // Delete the plotter instance
        delete _ui->plot1D;

        PlotFactory* pFactory=PlotFactory::Instance();

        _ui->plot1D = pFactory->create(item->getPlotType(),_ui->dockWidgetContents_4);

        // Restore the size policy
        _ui->plot1D->setSizePolicy(oldSizePolicy);

        // Sets some properties of the plotter
        _ui->plot1D->setObjectName(QStringLiteral("1D plotter"));
        _ui->plot1D->setFocusPolicy(Qt::StrongFocus);
        _ui->plot1D->setStyleSheet(QStringLiteral(""));

        // Add the plot to the ui
        _ui->horizontalLayout_4->addWidget(_ui->plot1D);

    }

    // Plot the data
    item->plot(_ui->plot1D);

}

void MainWindow::on_actionFrom_Sample_triggered()
{
    QTransform trans;
    trans.scale(1,-1);
    _ui->_dview->setTransform(trans);
    _ui->_dview->fitScene();
}

void MainWindow::on_actionBehind_Detector_triggered()
{
    QTransform trans;
    trans.scale(-1,-1);
    _ui->_dview->setTransform(trans);
    _ui->_dview->fitScene();
}

void MainWindow::on_actionShow_labels_triggered()
{
    _ui->_dview->getScene()->showPeakLabels(true);
}
