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
#include <nsxlib/Basis.h>
#include <nsxlib/CrystalTypes.h>
#include <nsxlib/Detector.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Ellipsoid.h>
#include <nsxlib/GruberReduction.h>
#include <nsxlib/IFrameIterator.h>
#include <nsxlib/LogFileStreamWrapper.h>
#include <nsxlib/Logger.h>
#include <nsxlib/MathematicsTypes.h>
#include <nsxlib/NiggliReduction.h>
#include <nsxlib/Path.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakFilter.h>
#include <nsxlib/PeakFinder.h>
#include <nsxlib/Gaussian3d.h>
#include <nsxlib/ProgressHandler.h>
#include <nsxlib/Sample.h>
#include <nsxlib/Source.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>
#include <nsxlib/WeakPeakIntegrator.h>
#include <nsxlib/ISigmaIntegrator.h>
#include <nsxlib/Profile1DIntegrator.h>
#include <nsxlib/GaussianIntegrator.h>

#include "AbsorptionWidget.h"
#include "CollectedPeaksModel.h"
#include "ColorMap.h"
#include "CutLineGraphicsItem.h"
#include "CutSliceGraphicsItem.h"
#include "CutterGraphicsItem.h"
#include "DetectorScene.h"
#include "DialogExperiment.h"
#include "DialogIntegrate.h"
#include "DialogPeakFilter.h"
#include "ExperimentTree.h"
#include "IsotopeDatabaseDialog.h"
#include "JobHandler.h"
#include "MainWindow.h"
#include "NoteBook.h"
#include "PeakGraphicsItem.h"
#include "PlottableGraphicsItem.h"
#include "ResolutionCutoffDialog.h"
#include "PeakListPropertyWidget.h"
#include "PeakTableView.h"
#include "PlotFactory.h"
#include "QtStreamWrapper.h"
#include "SessionModel.h"
#include "SXPlot.h"
#include "UnitCellPropertyWidget.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
  _ui(new Ui::MainWindow),
  //_experiments(),
  _currentData(nullptr)
{

    _ui->setupUi(this);

    // make experiment tree aware of the session
    _session = new SessionModel;
    _ui->experimentTree->setModel(_session);

    // Set Date to the application window title
    QDateTime datetime=QDateTime::currentDateTime();
    this->setWindowTitle(QString("NSXTool version:")+ datetime.toString());

    auto debug_log = [this]() -> nsx::Logger
    {
        auto initialize = []() -> std::string {return "[DEBUG] " + nsx::currentTime();};
        auto finalize = []() -> std::string {return "\n";};

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_debug.txt",initialize,finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->noteBook,initialize));

        return nsx::Logger(wrapper);
    };

    auto info_log = [this]() -> nsx::Logger
    {
        auto initialize = []() -> std::string {return "[INFO]  " + nsx::currentTime();};
        auto finalize = []() -> std::string {return "\n";};

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_info.txt",initialize,finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->noteBook,initialize));

        return nsx::Logger(wrapper);
    };

    auto error_log = [this]() -> nsx::Logger
    {
        auto initialize = []() -> std::string {return "[ERROR]" + nsx::currentTime();};
        auto finalize = []() -> std::string {return "\n";};

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_error.txt",initialize,finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->noteBook, initialize));

        return nsx::Logger(wrapper);
    };

    nsx::setDebug(debug_log);
    nsx::setInfo(info_log);
    nsx::setError(error_log);

    //
    _ui->frameFrame->setEnabled(false);
    _ui->intensityFrame->setEnabled(false);

    _ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/selectionIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/ellipseIcon.png"),"");
    _ui->selectionMode->addItem(QIcon(":/resources/slice3D.png"),"");

    // Vertical splitter between Tree and Inspector Widget
    _ui->splitterVertical->setStretchFactor(0,50);
    _ui->splitterVertical->setStretchFactor(1,50);

    // Horizontal splitter between Tree and DetectorScene
    _ui->splitterHorizontal->setStretchFactor(0,10);
    _ui->splitterHorizontal->setStretchFactor(1,90);

    // signals and slots
    connect(_ui->experimentTree, SIGNAL(plotData(nsx::sptrDataSet)),
            this, SLOT(changeData(nsx::sptrDataSet)));


    connect(_ui->frame,&QScrollBar::valueChanged,[=](const int value){_ui->_dview->getScene()->changeFrame(value);});

    connect(_ui->intensity,SIGNAL(valueChanged(int)),_ui->_dview->getScene(),SLOT(setMaxIntensity(int)));
    connect(_ui->selectionMode,SIGNAL(currentIndexChanged(int)),_ui->_dview->getScene(),SLOT(changeInteractionMode(int)));
    connect(_ui->_dview->getScene(),SIGNAL(updatePlot(PlottableGraphicsItem*)),this,SLOT(updatePlot(PlottableGraphicsItem*)));
    connect(_ui->action_open,SIGNAL(triggered()), _session, SLOT(createNewExperiment()));

    connect(_ui->experimentTree, SIGNAL(resetScene()), _ui->_dview->getScene(), SLOT(resetScene()));
   
    connect(_session, SIGNAL(updatePeaks()), _ui->_dview->getScene(), SLOT(updatePeaks()));

    _ui->loggerDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->plotterDockWidget->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
    _ui->dockWidget_Property->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);

    _ui->plotterDockWidget->show();
    _ui->dockWidget_Property->show();

    connect(_ui->experimentTree,SIGNAL(inspectWidget(QWidget*)),this,SLOT(setInspectorWidget(QWidget*)));

    _progressHandler = nsx::sptrProgressHandler(new nsx::ProgressHandler());
    _peakFinder = nsx::sptrPeakFinder(new nsx::PeakFinder());

    for (auto&& action: _ui->menuColor_map->actions()) {
        _ui->menuColor_map->removeAction(action);
    }

    auto names = ColorMap::getColorMapNames();

    for (auto&& name: names) {
        QAction* action = new QAction(name.c_str(), _ui->menuColor_map);

        auto slot_fn = [=] () -> void
        {
            const std::string name_str = action->text().toStdString();
            _session->setColorMap(name_str);
            _ui->_dview->getScene()->setColorMap(name_str);
            _ui->_dview->getScene()->redrawImage();
        };

        connect(action, &QAction::triggered, this, slot_fn);
        _ui->menuColor_map->addAction(action);
    }
}

MainWindow::~MainWindow()
{
    qInstallMessageHandler(0);
    delete _ui;
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

Ui::MainWindow* MainWindow::getUI() const
{
    return _ui;
}


void MainWindow::changeData(nsx::sptrDataSet data)
{
    _ui->frameFrame->setEnabled(true);
    _ui->intensityFrame->setEnabled(true);

    int frameMax = int(data->nFrames()-1);
    int frame = _ui->frame->value();

    if (frame > frameMax) {
        frame = frameMax;
    }
    // why do we do this? why is the signal not working properly?
    _ui->_dview->getScene()->setData(_session, data, frame);

    _ui->frame->setValue(frame);
    _ui->frame->setMaximum(frameMax);
    _ui->spinBox_Frame->setMaximum(frameMax);

    //_ui->intensity->setValue(10);
}

void MainWindow::plotPeak(nsx::sptrPeak3D peak)
{
    auto data = peak->data();
    auto scenePtr = _ui->_dview->getScene();
    // Ensure that frames
    changeData(data);
    // Get frame number to adjust the data
    size_t data_frame = size_t(std::lround(peak->getShape().aabb().center()[2]));
    scenePtr->setData(_session, data, data_frame);
    // Update the scrollbar
    _ui->frame->setValue(data_frame);
    auto pgi = scenePtr->findPeakGraphicsItem(peak);

    if (pgi) {
        updatePlot(pgi);
    }
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

void MainWindow::on_actionH_k_l_triggered()
{
     _ui->_dview->getScene()->changeCursorMode(DetectorScene::HKL);
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

void MainWindow::on_action_display_isotopes_database_triggered()
{
    // Opens the dialog that will diplay the isotopes database
    IsotopeDatabaseDialog* dlg=new IsotopeDatabaseDialog();
    dlg->exec();
}

void MainWindow::on_actionShow_labels_triggered(bool checked)
{
    _ui->_dview->getScene()->showPeakLabels(checked);
    _ui->_dview->getScene()->update();
}


void MainWindow::setInspectorWidget(QWidget* w)
{
    // Ensure that previous Property Widget is deleted.
    auto widget=_ui->dockWidget_Property->widget();
    if (widget)
        delete widget;

    // Assign current property Widget
    _ui->dockWidget_Property->setWidget(w);

    if (PeakListPropertyWidget* widget=dynamic_cast<PeakListPropertyWidget*>(w)) {
        // Ensure plot1D is updated
        connect(widget->getPeakTableView(),SIGNAL(plotPeak(nsx::sptrPeak3D)),this,SLOT(plotPeak(nsx::sptrPeak3D)));
        connect(widget->getPeakTableView(),
                SIGNAL(plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&)),
                this,
                SLOT(plotData(const QVector<double>&,const QVector<double>&,const QVector<double>&)));

        CollectedPeaksModel* peakModel = dynamic_cast<CollectedPeaksModel*>(widget->getPeakTableView()->model());
        connect(peakModel,SIGNAL(unitCellUpdated()),_ui->_dview->getScene(),SLOT(updatePeaks()));
    }
}

void MainWindow::on_checkBox_AspectRatio_toggled(bool checked)
{
    _ui->_dview->fixDetectorAspectRatio(checked);
}

void MainWindow::on_actionLogarithmic_Scale_triggered(bool checked)
{
    _ui->_dview->getScene()->setLogarithmic(checked);
}

void MainWindow::on_actionDraw_peak_integration_area_triggered(bool checked)
{
    _ui->_dview->getScene()->drawIntegrationRegion(checked);
}
