//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/MainWindow.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

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

#include "core/geometry/AABB.h"
#include "core/logger/AggregateStreamWrapper.h"
#include "core/experiment/CrystalTypes.h"
#include "core/detector/Detector.h"
#include "core/instrument/Diffractometer.h"
#include "core/geometry/Ellipsoid.h"
#include "core/crystal/GruberReduction.h"
#include "core/logger/LogFileStreamWrapper.h"
#include "core/logger/Logger.h"
#include "core/mathematics/MathematicsTypes.h"
#include "core/crystal/NiggliReduction.h"
#include "core/utils/Path.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakFilter.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/utils/ProgressHandler.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"
#include "core/utils/Units.h"
#include "Version.h"

#include "apps/views/AbsorptionWidget.h"
#include "apps/models/CollectedPeaksModel.h"
#include "apps/ColorMap.h"
#include "apps/items/CutLineGraphicsItem.h"
#include "apps/items/CutSliceGraphicsItem.h"
#include "apps/items/CutterGraphicsItem.h"
#include "apps/models/DetectorScene.h"
#include "apps/dialogs/DialogExperiment.h"
#include "apps/dialogs/DialogIntegrate.h"
#include "apps/dialogs/DialogIsotopesDatabase.h"
#include "apps/dialogs/DialogPeakFilter.h"
#include "apps/tree/ExperimentTree.h"
#include "apps/MainWindow.h"
#include "apps/NoteBook.h"
#include "apps/items/PeakGraphicsItem.h"
#include "apps/views/PeakTableView.h"
#include "apps/plot/PlotFactory.h"
#include "apps/items/PlottableGraphicsItem.h"
#include "apps/logger/QtStreamWrapper.h"
#include "apps/plot/SXPlot.h"
#include "apps/models/SessionModel.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    // make experiment tree aware of the session
    _session = new SessionModel();
    _ui->experimentTree->setModel(_session);
    _ui->dview->getScene()->setSession(_session);

    auto debug_log = [this]() -> nsx::Logger {
        auto initialize = []() -> std::string { return "[DEBUG] " + nsx::currentTime(); };
        auto finalize = []() -> std::string { return "\n"; };

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_debug.txt", initialize, finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->noteBook, initialize));

        return nsx::Logger(wrapper);
    };

    auto info_log = [this]() -> nsx::Logger {
        auto initialize = []() -> std::string { return "[INFO]  " + nsx::currentTime(); };
        auto finalize = []() -> std::string { return "\n"; };

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_info.txt", initialize, finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->noteBook, initialize));

        return nsx::Logger(wrapper);
    };

    auto error_log = [this]() -> nsx::Logger {
        auto initialize = []() -> std::string { return "[ERROR]" + nsx::currentTime(); };
        auto finalize = []() -> std::string { return "\n"; };

        nsx::AggregateStreamWrapper* wrapper = new nsx::AggregateStreamWrapper();
        wrapper->addWrapper(new nsx::LogFileStreamWrapper("nsx_error.txt", initialize, finalize));
        wrapper->addWrapper(new QtStreamWrapper(this->_ui->noteBook, initialize));

        return nsx::Logger(wrapper);
    };

    nsx::setDebug(debug_log);
    nsx::setInfo(info_log);
    nsx::setError(error_log);

    //
    _ui->frameLayout->setEnabled(false);
    _ui->intensityLayout->setEnabled(false);

    _ui->selectionMode->addItem(QIcon(":/resources/selectIcon.png"), "selection");
    _ui->selectionMode->addItem(QIcon(":/resources/zoomIcon.png"), "zoom");
    _ui->selectionMode->addItem(QIcon(":/resources/cutlineIcon.png"), "line plot");
    _ui->selectionMode->addItem(QIcon(":/resources/horizontalSliceIcon.png"), "horizontal slice");
    _ui->selectionMode->addItem(QIcon(":/resources/verticalSliceIcon.png"), "vertical slice");
    _ui->selectionMode->addItem(QIcon(":/resources/rectangularMaskIcon.png"), "rectangular mask");
    _ui->selectionMode->addItem(QIcon(":/resources/ellipsoidalMaskIcon.png"), "ellipsoidal mask");

    // Vertical splitter between Tree and Inspector Widget
    _ui->splitterVertical->setStretchFactor(0, 50);
    _ui->splitterVertical->setStretchFactor(1, 50);

    // Horizontal splitter between Tree and DetectorScene
    _ui->splitterHorizontal->setStretchFactor(0, 10);
    _ui->splitterHorizontal->setStretchFactor(1, 90);

    // signals and slots
    connect(
        _session, SIGNAL(signalSelectedDataChanged(nsx::sptrDataSet, int)), this,
        SLOT(slotChangeSelectedData(nsx::sptrDataSet, int)));
    connect(
        _session, SIGNAL(signalSelectedPeakChanged(nsx::sptrPeak3D)), this,
        SLOT(slotChangeSelectedPeak(nsx::sptrPeak3D)));

    connect(
        _ui->frame, SIGNAL(valueChanged(int)), _ui->dview->getScene(),
        SLOT(slotChangeSelectedFrame(int)));

    connect(
        _ui->intensity, SIGNAL(valueChanged(int)), _ui->dview->getScene(),
        SLOT(setMaxIntensity(int)));
    connect(
        _ui->selectionMode, SIGNAL(currentIndexChanged(int)), _ui->dview->getScene(),
        SLOT(changeInteractionMode(int)));
    connect(
        _ui->dview->getScene(), SIGNAL(updatePlot(PlottableGraphicsItem*)), this,
        SLOT(updatePlot(PlottableGraphicsItem*)));
    connect(_ui->action_open, SIGNAL(triggered()), _session, SLOT(createNewExperiment()));

    connect(_ui->experimentTree, SIGNAL(resetScene()), _ui->dview->getScene(), SLOT(resetScene()));

    connect(
        _session, SIGNAL(updatePeaks()), _ui->dview->getScene(), SLOT(resetPeakGraphicsItems()));

    _ui->loggerDockWidget->setFeatures(
        QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    _ui->plotterDockWidget->setFeatures(
        QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    _ui->dockWidget_Property->setFeatures(
        QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

    _ui->plotterDockWidget->show();
    _ui->dockWidget_Property->show();

    connect(
        _ui->experimentTree, SIGNAL(inspectWidget(QWidget*)), this,
        SLOT(setInspectorWidget(QWidget*)));

    for (auto&& action : _ui->menuColor_map->actions()) {
        _ui->menuColor_map->removeAction(action);
    }

    auto names = ColorMap::getColorMapNames();

    for (auto&& name : names) {
        QAction* action = new QAction(name.c_str(), _ui->menuColor_map);

        auto slot_fn = [=]() -> void {
            const std::string name_str = action->text().toStdString();
            setColorMap(name_str);
            _ui->dview->getScene()->setColorMap(name_str);
            _ui->dview->getScene()->loadCurrentImage();
        };

        connect(action, &QAction::triggered, this, slot_fn);
        _ui->menuColor_map->addAction(action);
    }

    connect(
        _ui->showPeakLabels, SIGNAL(triggered(bool)), _ui->dview->getScene(),
        SLOT(showPeakLabels(bool)));
    connect(
        _ui->showPeakAreas, SIGNAL(triggered(bool)), _ui->dview->getScene(),
        SLOT(showPeakAreas(bool)));
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QApplication::closeAllWindows();

    QMainWindow::closeEvent(event);
}

void MainWindow::setColorMap(const std::string& name)
{
    _colormap = name;
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
    splashScrWindow->move(screenSize.width() / 2 - 300, screenSize.height() / 2 - 150);
    splashScrWindow->show();
}

void MainWindow::slotChangeSelectedData(nsx::sptrDataSet data, int frame)
{
    _ui->frameLayout->setEnabled(true);
    _ui->intensityLayout->setEnabled(true);

    int nFrames = int(data->nFrames());

    frame = frame % nFrames >= 0 ? frame : frame + nFrames;

    _ui->frame->setValue(frame);

    _ui->frame->setMinimum(0);
    _ui->frame->setMaximum(nFrames - 1);

    _ui->spinBox_Frame->setMinimum(0);
    _ui->spinBox_Frame->setMaximum(nFrames - 1);
}

void MainWindow::slotChangeSelectedPeak(nsx::sptrPeak3D peak)
{
    // Get frame number to adjust the data
    size_t frame = size_t(std::lround(peak->shape().aabb().center()[2]));

    slotChangeSelectedData(peak->data(), frame);
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

void MainWindow::plotData(
    const QVector<double>& x, const QVector<double>& y, const QVector<double>& e)
{
    if (_ui->plot1D->getType().compare("simple") != 0) {
        // Store the old size policy
        QSizePolicy oldSizePolicy = _ui->plot1D->sizePolicy();
        // Remove the current plotter from the ui
        _ui->horizontalLayout_4->removeWidget(_ui->plot1D);
        // Delete the plotter instance
        delete _ui->plot1D;

        PlotFactory* pFactory = PlotFactory::Instance();

        _ui->plot1D = pFactory->create("simple", _ui->dockWidgetContents_4);

        // Restore the size policy
        _ui->plot1D->setSizePolicy(oldSizePolicy);

        // Sets some properties of the plotter
        _ui->plot1D->setObjectName(QStringLiteral("1D plotter"));
        _ui->plot1D->setFocusPolicy(Qt::StrongFocus);
        _ui->plot1D->setStyleSheet(QStringLiteral(""));

        // Add the plot to the ui
        _ui->horizontalLayout_4->addWidget(_ui->plot1D);
    }

    _ui->plot1D->graph(0)->setDataValueError(x, y, e);
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

        PlotFactory* pFactory = PlotFactory::Instance();

        _ui->plot1D = pFactory->create(item->getPlotType(), _ui->dockWidgetContents_4);

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
    trans.scale(1, -1);
    _ui->dview->setTransform(trans);
    _ui->dview->fitScene();
}

void MainWindow::on_actionBehind_Detector_triggered()
{
    QTransform trans;
    trans.scale(-1, -1);
    _ui->dview->setTransform(trans);
    _ui->dview->fitScene();
}

void MainWindow::on_action_display_isotopes_database_triggered()
{
    // Opens the dialog that will diplay the isotopes database
    DialogIsotopesDatabase* dlg = new DialogIsotopesDatabase();
    dlg->exec();
}

void MainWindow::setInspectorWidget(QWidget* w)
{
    // Ensure that previous Property Widget is deleted.
    auto widget = _ui->dockWidget_Property->widget();
    if (widget) {
        delete widget;
    }

    // Assign current property Widget
    _ui->dockWidget_Property->setWidget(w);

    //    if (PeakListPropertyWidget* widget =
    //    dynamic_cast<PeakListPropertyWidget*>(w)) {
    //        connect(widget->model(),
    //                SIGNAL(plotData(const QVector<double>&,const
    //                QVector<double>&,const QVector<double>&)), this,
    //                SLOT(plotData(const QVector<double>&,const
    //                QVector<double>&,const QVector<double>&)));

    //        CollectedPeaksModel* peakModel =
    //        dynamic_cast<CollectedPeaksModel*>(widget->getPeakTableView()->model());
    //        connect(peakModel,SIGNAL(unitCellUpdated()),_ui->_dview->getScene(),SLOT(updatePeaks()));
    //    }
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
