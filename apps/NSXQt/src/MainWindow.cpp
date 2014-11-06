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
    _ui(new Ui::MainWindow),
    _scene(new QGraphicsScene),
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

    _scene->setParent(_ui->_dview);
    _ui->_dview->setScene(_scene);
    _ui->_dview->setInteractive(true);
    _ui->_dview->setDragMode(QGraphicsView::RubberBandDrag);
    _ui->dial->setRange(0,15);

//    ui->numor_Widget->setSelectionMode(QAbstractItemView::MultiSelection);
//    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->numor_Widget);
//    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteNumors()));

    _ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"),"");

    connect(_ui->selectionMode,SIGNAL(currentIndexChanged(int)),_ui->_dview,SLOT(onSetCutterMode(int)));
    connect(_ui->dial,SIGNAL(valueChanged(int)),_ui->_dview,SLOT(onSetMaxIntensity(int)));
    connect(_ui->experimentTree,SIGNAL(plotData(IData*)),this,SLOT(onPlotData(IData*)));
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

void MainWindow::onPlotData(IData* data)
{

    if (data == _currentData)
        return;

    _currentData = data;

    Detector* detector = data->getDiffractometer()->getDetector();
    _ui->_dview->setNpixels(detector->getNCols(),detector->getNRows());
    _ui->_dview->setDimensions(detector->getWidthAngle(),detector->getHeigth());
    _ui->_dview->setDetectorDistance(detector->getDistance());

    _ui->frameFrame->setEnabled(true);
    _ui->intensityFrame->setEnabled(true);

    _ui->horizontalScrollBar->setMaximum(_currentData->getNFrames()-1);
    _ui->spinBox_Frame->setMaximum(_currentData->getNFrames()-1);
    _ui->dial->setRange(1,3000);
    _ui->dial->setValue(20);
    _ui->_dview->updateView(data,0);
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
    if (!_currentData)
    {
        _ui->_dview->updateView(0,0);
        return;
    }

    int frame = _ui->horizontalScrollBar->value();
    // Make sure that the frame value for previously selected file is valid for current one
    if (frame>(_currentData->getNFrames()-1))
        frame=0;
    _ui->_dview->updateView(_currentData,frame);
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
    _ui->dial->setValue(arg1);
    updatePlot();
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
    if (!dialog->exec())
        return;

    // Get Confidence and threshold
    double confidence=dialog->getConfidence();
    double threshold=dialog->getThreshold();   

    std::vector<IData*> numors=selectedNumors();
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

        _ui->progressBar->setValue(++comp);
    }

    qDebug() << "Found " << npeaks << " peaks";
    updatePlot();
    // Reinitialise progress bar
    _ui->progressBar->setValue(0);
    _ui->progressBar->setEnabled(false);

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
//    QModelIndexList selIndexes = _ui->experimentTree->selectedIndexes();



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
