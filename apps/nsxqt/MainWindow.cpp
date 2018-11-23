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
#include <QShortcut>
#include <QThread>
#include <QTransform>

#include <nsxlib/AABB.h>
#include <nsxlib/AggregateStreamWrapper.h>
#include <nsxlib/CrystalTypes.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Ellipsoid.h>
#include <nsxlib/GruberReduction.h>
#include <nsxlib/LogFileStreamWrapper.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MathematicsTypes.h>
#include <nsxlib/NiggliReduction.h>
#include <nsxlib/Path.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>
#include <nsxlib/Version.h>

#include "AbsorptionWidget.h"
#include "CollectedPeaksModel.h"
#include "ColorMap.h"
#include "CutLineGraphicsItem.h"
#include "CutSliceGraphicsItem.h"
#include "CutterGraphicsItem.h"
#include "DataItem.h"
#include "DetectorScene.h"
#include "DialogExperiment.h"
#include "DialogIntegrate.h"
#include "DialogPeakFilter.h"
#include "ExperimentTree.h"
#include "DialogIsotopesDatabase.h"
#include "FramePeakFinder.h"
#include "MainWindow.h"
#include "NoteBook.h"
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

    // make experiment tree aware of the session
    _session_model = new SessionModel();
    _ui->experimentTree->setModel(_session_model);

    _task_manager_model = new TaskManagerModel(this);
    _ui->task_manager->setModel(_task_manager_model);

    _ui->dview->getScene()->setSession(_session_model);

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

    //
    _ui->frameLayout->setEnabled(false);
    _ui->intensityLayout->setEnabled(false);

    _ui->selectionMode->addItem(QIcon(":/resources/selectIcon.png"),"selection");
    _ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"),"zoom");
    _ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"),"line plot");
    _ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"),"horizontal slice");
    _ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"),"vertical slice");
    _ui->selectionMode->addItem(QIcon(":/resources/rectangularMaskIcon.png"),"rectangular mask");
    _ui->selectionMode->addItem(QIcon(":/resources/ellipsoidalMaskIcon.png"),"ellipsoidal mask");

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
    connect(_ui->action_open,SIGNAL(triggered()), _session_model, SLOT(createNewExperiment()));

    connect(_ui->experimentTree, SIGNAL(resetScene()), _ui->dview->getScene(), SLOT(resetScene()));
    connect(_ui->experimentTree,&ExperimentTree::openPeakFindDialog,[=](DataItem *data_item){onOpenPeakFinderDialog(data_item);});

    connect(_session_model, SIGNAL(updatePeaks()), _ui->dview->getScene(), SLOT(resetPeakGraphicsItems()));

    _ui->monitorDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->plotterDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->dockWidget_Property->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);

    _ui->plotterDockWidget->show();
    _ui->dockWidget_Property->show();

    connect(_ui->experimentTree,SIGNAL(inspectWidget(QWidget*)),this,SLOT(setInspectorWidget(QWidget*)));

    for (auto&& action: _ui->menuColor_map->actions()) {
        _ui->menuColor_map->removeAction(action);
    }

    auto names = ColorMap::getColorMapNames();

    for (auto&& name: names) {
        QAction* action = new QAction(name.c_str(), _ui->menuColor_map);

        auto slot_fn = [=] () -> void
        {
            const std::string name_str = action->text().toStdString();
            setColorMap(name_str);
            _ui->dview->getScene()->setColorMap(name_str);
            _ui->dview->getScene()->loadCurrentImage();
        };

        connect(action, &QAction::triggered, this, slot_fn);
        _ui->menuColor_map->addAction(action);
    }

    connect(_ui->showPeakLabels,SIGNAL(triggered(bool)),_ui->dview->getScene(),SLOT(showPeakLabels(bool)));
    connect(_ui->showPeakAreas,SIGNAL(triggered(bool)),_ui->dview->getScene(),SLOT(showPeakAreas(bool)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::closeAllWindows();

    QMainWindow::closeEvent(event);
}

void MainWindow::setColorMap(const std::string &name)
{
    _colormap = name;
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

void MainWindow::on_actionAbout_triggered()
{
    // Show splash
    QImage splashScrImage(":/resources/splashScreen.png");
    QPixmap Logo;
    Logo.convertFromImage(splashScrImage);
    QSplashScreen* splashScrWindow = new QSplashScreen(this, Logo, Qt::WindowStaysOnTopHint);
    QSize screenSize = QApplication::desktop()->geometry().size();
    splashScrWindow->move(screenSize.width()/2-300,screenSize.height()/2-150);
    splashScrWindow->show();
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

void MainWindow::on_actionPixel_position_triggered()
{
   _ui->dview->getScene()->changeCursorMode(DetectorScene::PIXEL);
}

void MainWindow::on_action2_Theta_triggered()
{
     _ui->dview->getScene()->changeCursorMode(DetectorScene::THETA);
}

void MainWindow::on_actionGamma_Nu_triggered()
{
    _ui->dview->getScene()->changeCursorMode(DetectorScene::GAMMA_NU);
}

void MainWindow::on_actionD_spacing_triggered()
{
    _ui->dview->getScene()->changeCursorMode(DetectorScene::D_SPACING);
}

void MainWindow::on_actionMiller_indices_triggered()
{
    _ui->dview->getScene()->changeCursorMode(DetectorScene::MILLER_INDICES);
}

void MainWindow::on_actionLogger_triggered()
{
    if (_ui->monitorDockWidget->isHidden()) {
        _ui->monitorDockWidget->show();
    } else {
        _ui->monitorDockWidget->hide();
    }
}

void MainWindow::on_action1D_Peak_Ploter_triggered()
{
    if (_ui->plotterDockWidget->isHidden())
        _ui->plotterDockWidget->show();
    else
        _ui->plotterDockWidget->hide();
}

void MainWindow::on_actionProperty_triggered()
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

void MainWindow::on_actionFrom_Sample_triggered()
{
    QTransform trans;
    trans.scale(1,-1);
    _ui->dview->setTransform(trans);
    _ui->dview->fitScene();
}

void MainWindow::on_actionBehind_Detector_triggered()
{
    QTransform trans;
    trans.scale(-1,-1);
    _ui->dview->setTransform(trans);
    _ui->dview->fitScene();
}

void MainWindow::on_action_display_isotopes_database_triggered()
{
    // Opens the dialog that will diplay the isotopes database
    DialogIsotopesDatabase* dlg=new DialogIsotopesDatabase();
    dlg->exec();
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

void MainWindow::on_actionDraw_peak_integration_area_triggered(bool checked)
{
    _ui->dview->getScene()->drawIntegrationRegion(checked);
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
