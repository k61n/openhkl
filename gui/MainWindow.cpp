#include <cmath>
#include <fstream>
#include <functional>
#include <stdexcept>

#include <Eigen/Dense>

#include <QDateTime>
#include <QDockWidget>
#include <QFileDialog>
#include <QGraphicsBlurEffect>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QProgressDialog>
#include <QStatusBar>
#include <QShortcut>
#include <QThread>
#include <QTransform>

#include <core/AABB.h>
#include <core/AggregateStreamWrapper.h>
#include <core/CrystalTypes.h>
#include <core/Detector.h>
#include <core/Diffractometer.h>
#include <core/Ellipsoid.h>
#include <core/GruberReduction.h>
#include <core/LogFileStreamWrapper.h>
#include <core/Logger.h>
#include <core/MathematicsTypes.h>
#include <core/NiggliReduction.h>
#include <core/Path.h>
#include <core/Peak3D.h>
#include <core/PeakFilter.h>
#include <core/PeakFinder.h>
#include <core/ProgressHandler.h>
#include <core/Sample.h>
#include <core/Source.h>
#include <core/SpaceGroup.h>
#include <core/UnitCell.h>
#include <core/Units.h>
#include <core/Version.h>

#include "AbsorptionWidget.h"
#include "CollectedPeaksModel.h"
#include "CutLineGraphicsItem.h"
#include "CutSliceGraphicsItem.h"
#include "CutterGraphicsItem.h"
#include "DataItem.h"
#include "DetectorScene.h"
#include "DialogExperiment.h"
#include "DialogIntegrate.h"
#include "DialogPeakFilter.h"
#include "ExperimentTree.h"
#include "FramePeakFinder.h"
#include "MainWindow.h"
#include "MouseInteractionModeModel.h"
#include "NoteBook.h"
#include "NSXMenu.h"
#include "PeakGraphicsItem.h"
#include "PlottableGraphicsItem.h"
#include "PeakTableView.h"
#include "PlotFactory.h"
#include "QtStreamWrapper.h"
#include "SessionModel.h"
#include "SXPlot.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
  _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    _status_bar = new QStatusBar(this);
    setStatusBar(_status_bar);

    _menu_bar = new NSXMenu(this);

    // make experiment tree aware of the session
    _session_model = new SessionModel();
    _ui->experimentTree->setModel(_session_model);
    _ui->experimentTree->setMainWindow(this);

    _task_manager_model = new TaskManagerModel(this);
    _ui->task_manager->setModel(_task_manager_model);

    _ui->dview->getScene()->setSession(_session_model);

    _ui->frameLayout->setEnabled(false);
    _ui->intensityLayout->setEnabled(false);

    MouseInteractionModeModel *mouse_interaction_model = new MouseInteractionModeModel(this);
    _ui->selectionMode->setModel(mouse_interaction_model);

    // Vertical splitter between Tree and Inspector Widget
    _ui->splitterVertical->setStretchFactor(0,50);
    _ui->splitterVertical->setStretchFactor(1,50);

    // Horizontal splitter between Tree and DetectorScene
    _ui->splitterHorizontal->setStretchFactor(0,10);
    _ui->splitterHorizontal->setStretchFactor(1,90);

    // signals and slots
    connect(_session_model, SIGNAL(signalSelectedDataChanged(nsx::sptrDataSet,int)), this, SLOT(slotChangeSelectedData(nsx::sptrDataSet,int)));
    connect(_session_model, SIGNAL(signalSelectedPeakChanged(nsx::sptrPeak3D)), this, SLOT(slotChangeSelectedPeak(nsx::sptrPeak3D)));

    connect(_ui->frame,SIGNAL(valueChanged(int)),_ui->dview->getScene(),SLOT(slotChangeSelectedFrame(int)));

    connect(_ui->intensity,SIGNAL(valueChanged(int)),_ui->dview->getScene(),SLOT(setMaxIntensity(int)));
    connect(_ui->selectionMode,SIGNAL(currentIndexChanged(int)),_ui->dview->getScene(),SLOT(changeInteractionMode(int)));
    connect(_ui->dview->getScene(),SIGNAL(updatePlot(PlottableGraphicsItem*)),this,SLOT(updatePlot(PlottableGraphicsItem*)));

    connect(_ui->experimentTree, SIGNAL(resetScene()), _ui->dview->getScene(), SLOT(resetScene()));
    connect(_ui->experimentTree,&ExperimentTree::openPeakFindDialog,[=](DataItem *data_item){onOpenPeakFinderDialog(data_item);});

    connect(_session_model, SIGNAL(updatePeaks()), _ui->dview->getScene(), SLOT(resetPeakGraphicsItems()));

    _ui->monitorDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->plotterDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->dockWidget_Property->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);

    _ui->plotterDockWidget->show();
    _ui->dockWidget_Property->show();

    connect(_ui->experimentTree,SIGNAL(inspectWidget(QWidget*)),this,SLOT(setInspectorWidget(QWidget*)));
}

void MainWindow::onDisplayPeakLabels(bool flag)
{
    _ui->dview->getScene(),SLOT(showPeakLabels(flag));
}

void MainWindow::onDisplayPeakAreas(bool flag)
{
    _ui->dview->getScene(),SLOT(showPeakAreas(flag));
}

void MainWindow::onDisplayPeakIntegrationAreas(bool flag)
{
    _ui->dview->getScene()->drawIntegrationRegion(flag);
}

void MainWindow::initLoggers()
{
    auto debug_log = [this]() -> nsx::Logger
    {
        auto initialize = []() -> std::string {return "[DEBUG] " + nsx::currentTime();};
        auto finalize = []() -> std::string {return "\n";};

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_debug.txt",initialize,finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->logger,initialize));

        return nsx::Logger(wrapper);
    };

    auto info_log = [this]() -> nsx::Logger
    {
        auto initialize = []() -> std::string {return "[INFO]  " + nsx::currentTime();};
        auto finalize = []() -> std::string {return "\n";};

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_info.txt",initialize,finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->logger,initialize));

        return nsx::Logger(wrapper);
    };

    auto error_log = [this]() -> nsx::Logger
    {
        auto initialize = []() -> std::string {return "[ERROR]" + nsx::currentTime();};
        auto finalize = []() -> std::string {return "\n";};

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_error.txt",initialize,finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->logger, initialize));

        return nsx::Logger(wrapper);
    };

    nsx::setDebug(debug_log);
    nsx::setInfo(info_log);
    nsx::setError(error_log);
}


void MainWindow::onNewExperiment()
{
    std::unique_ptr<DialogExperiment> dlg;

    // DialogExperiment could throw an exception if it fails to read the resource files
    try {
        dlg = std::unique_ptr<DialogExperiment>(new DialogExperiment());

        // The user pressed cancel, return
        if (!dlg->exec()) {
            return;
        }

        // If no experiment name is provided, pop up a warning
        if (dlg->experimentName().isEmpty()) {
            throw std::runtime_error("Empty experiment name");
        }
    }
    catch(std::exception& e) {
        nsx::error() << e.what();
        return;
    }

    nsx::sptrExperiment experiment;

    try {
        auto experiment_name = dlg->experimentName().toStdString();
        auto instrument_name = dlg->instrumentName().toStdString();

        // Create an experiment
        experiment = std::make_shared<nsx::Experiment>(experiment_name,instrument_name);
    }
    catch(const std::runtime_error& e) {
        nsx::error() << e.what();
        return;
    }

    _session_model->addExperiment(experiment);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::closeAllWindows();

    QMainWindow::closeEvent(event);
}

void MainWindow::onSetColorMap(const std::string &color_map)
{
    _color_map = color_map;
    _ui->dview->getScene()->setColorMap(_color_map);
}

MainWindow::~MainWindow()
{    
    qInstallMessageHandler(0);

    delete _ui;
}

TaskManagerModel* MainWindow::taskManagerModel()
{
    return _task_manager_model;
}

void MainWindow::slotChangeSelectedData(nsx::sptrDataSet data, int frame)
{
    _ui->frameLayout->setEnabled(true);
    _ui->intensityLayout->setEnabled(true);

    int nFrames = int(data->nFrames());

    frame = frame%nFrames >= 0 ? frame : frame+nFrames;

    _ui->frame->setValue(frame);

    _ui->frame->setMinimum(0);
    _ui->frame->setMaximum(nFrames-1);

    _ui->spinBox_Frame->setMinimum(0);
    _ui->spinBox_Frame->setMaximum(nFrames-1);
}

void MainWindow::slotChangeSelectedPeak(nsx::sptrPeak3D peak)
{
    // Get frame number to adjust the data
    size_t frame = size_t(std::lround(peak->shape().aabb().center()[2]));

    slotChangeSelectedData(peak->data(),frame);
}

void MainWindow::slotChangeSelectedFrame(int selected_frame)
{
    _ui->frame->setValue(selected_frame);
}

void MainWindow::onSelectPixelPositionCursorMode()
{
   _ui->dview->getScene()->changeCursorMode(DetectorScene::PIXEL);
}

void MainWindow::onSelect2ThetaCursorMode()
{
    _ui->dview->getScene()->changeCursorMode(DetectorScene::THETA);
}

void MainWindow::onSelectGammaNuCursorMode()
{
    _ui->dview->getScene()->changeCursorMode(DetectorScene::GAMMA_NU);
}

void MainWindow::onSelectDSpacingCursorMode()
{
    _ui->dview->getScene()->changeCursorMode(DetectorScene::D_SPACING);
}

void MainWindow::onSelectMillerIndicesCursorMode()
{
    _ui->dview->getScene()->changeCursorMode(DetectorScene::MILLER_INDICES);
}

void MainWindow::onToggleMonitorPanel()
{
    if (_ui->monitorDockWidget->isHidden()) {
        _ui->monitorDockWidget->show();
    } else {
        _ui->monitorDockWidget->hide();
    }
}

void MainWindow::onTogglePlotterPanel()
{
    if (_ui->plotterDockWidget->isHidden())
        _ui->plotterDockWidget->show();
    else
        _ui->plotterDockWidget->hide();
}

void MainWindow::onToggleWidgetPropertyPanel()
{
    if (_ui->dockWidget_Property->isHidden())
        _ui->dockWidget_Property->show();
    else
        _ui->dockWidget_Property->hide();
}

void MainWindow::plotData(const QVector<double>& x,const QVector<double>& y,const QVector<double>& e)
{
    if (_ui->plot1D->getType().compare("simple") != 0) {
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

    if (!item->isPlottable(_ui->plot1D)) {
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
    update();

}

void MainWindow::onViewDetectorFromSample()
{
    QTransform trans;
    trans.scale(1,-1);
    _ui->dview->setTransform(trans);
    _ui->dview->fitScene();
}

void MainWindow::onViewDetectorFromBehind()
{
    QTransform trans;
    trans.scale(-1,-1);
    _ui->dview->setTransform(trans);
    _ui->dview->fitScene();
}

void MainWindow::setInspectorWidget(QWidget* w)
{
    // Ensure that previous Property Widget is deleted.
    auto previous_widget = _ui->dockWidget_Property->widget();
    if (previous_widget) {
        delete previous_widget;
    }

    // Assign current property Widget
    _ui->dockWidget_Property->setWidget(w);
}

void MainWindow::on_checkBox_AspectRatio_toggled(bool checked)
{
    _ui->dview->fixDetectorAspectRatio(checked);
}

void MainWindow::on_actionLogarithmic_Scale_triggered(bool checked)
{
    _ui->dview->getScene()->setLogarithmic(checked);
}

void MainWindow::onOpenPeakFinderDialog(DataItem *data_item)
{
    nsx::DataList data = data_item->selectedData();

    if (data.empty()) {
        nsx::error()<<"No numors selected for finding peaks";
        return;
    }

    auto experiment_item = data_item->experimentItem();

    FramePeakFinder* frame = FramePeakFinder::create(this,experiment_item,data);

    frame->show();
}
