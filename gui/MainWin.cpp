//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/MainWin.cpp
//! @brief     Implements class MainWin
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/MainWin.h"

#include "gui/actions/Actions.h"
#include "gui/actions/Menus.h"
#include "gui/connect/Sentinel.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/graphics_items/PlottableItem.h"
#include "gui/subframe_experiment/SubframeExperiment.h"
#include "gui/subframe_filter/SubframeFilterPeaks.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_home/SubframeHome.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_integrate/SubframeIntegrate.h"
#include "gui/subframe_merge/SubframeMergedPeaks.h"
#include "gui/subframe_predict/SubframePredictPeaks.h"
#include "gui/subframe_refiner/SubframeRefiner.h"
#include "gui/subframe_reject/SubframeReject.h"
#include "gui/subframe_shapes/SubframeShapes.h"
#include "gui/subwindows/DetectorWindow.h"
#include "gui/subwindows/InputFilesWindow.h"
#include "gui/subwindows/InstrumentStateWindow.h"
#include "gui/subwindows/LogWindow.h"
#include "gui/subwindows/PeakWindow.h"
#include "gui/subwindows/PeaklistWindow.h"
#include "gui/utility/SideBar.h"
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PlotPanel.h"

#include <QApplication>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QSettings>
#include <QSplitter>
#include <QStackedLayout>
#include <QStatusBar>
#include <QString>
#include <QTimer>

MainWin* gGui; //!< global pointer to the main window

//  ***********************************************************************************************
//! @class MainWin

MainWin::MainWin()
{
    gGui = this;

    triggers = new Actions;
    menus_ = new Menus(menuBar());

    sentinel = new Sentinel();

    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

    // layout
    setContentsMargins(5, 5, 5, 5);
    QWidget* main_widget = new QWidget(this);
    QHBoxLayout* main_layout = new QHBoxLayout();

    _side_bar = new SideBar(main_widget);
    main_layout->addWidget(_side_bar);

    instrumentstate_window = new InstrumentStateWindow();

    home = new SubframeHome;
    experiment = new SubframeExperiment;
    finder = new SubframeFindPeaks;
    filter = new SubframeFilterPeaks;
    indexer = new SubframeAutoIndexer;
    shapes = new SubframeShapes;
    predictor = new SubframePredictPeaks;
    refiner = new SubframeRefiner;
    integrator = new SubframeIntegrate;
    rejector = new SubframeReject;
    merger = new SubframeMergedPeaks;

    detector_window = new DetectorWindow();
    log_window = new LogWindow(this);
    input_files_window = new InputFilesWindow(this);
    peak_list_window = new PeaklistWindow(this);

    _layout_stack = new QStackedWidget(main_widget);
    _layout_stack->addWidget(home);
    _layout_stack->addWidget(experiment);
    _layout_stack->addWidget(finder);
    _layout_stack->addWidget(filter);
    _layout_stack->addWidget(indexer);
    _layout_stack->addWidget(shapes);
    _layout_stack->addWidget(predictor);
    _layout_stack->addWidget(refiner);
    _layout_stack->addWidget(integrator);
    _layout_stack->addWidget(rejector);
    _layout_stack->addWidget(merger);
    _layout_stack->setCurrentIndex(0);

    main_layout->addWidget(_layout_stack);
    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);
    readSettings();
    show();

    initStatusBar();

    connect(
        indexer, &SubframeAutoIndexer::beamPosChanged, predictor,
        &SubframePredictPeaks::onBeamPosChanged);
    connect(
        predictor, &SubframePredictPeaks::beamPosChanged, indexer,
        &SubframeAutoIndexer::onBeamPosChanged);
}

void MainWin::onDataChanged() const { }

void MainWin::onExperimentChanged() const { }

void MainWin::onPeaksChanged() const { }

void MainWin::onUnitCellChanged() const { }

void MainWin::updatePlot(PlottableItem* p) const
{
    experiment->getPlot()->updatePlot(p);
}

void MainWin::exportPlot() const
{
    experiment->getPlot()->exportPlot();
}

void MainWin::plotData(QVector<double>& x, QVector<double>& y, QVector<double>& e) const
{
    experiment->getPlot()->plotData(x, y, e);
}

void MainWin::resetViews()
{
    restoreState(initialState_);
}

void MainWin::saveSettings() const
{
    QSettings settings = qSettings();
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
}

void MainWin::readSettings()
{
    if (initialState_.isEmpty())
        initialState_ = saveState();
    QSettings settings = qSettings();
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
}

void MainWin::closeEvent(QCloseEvent* event)
{
    saveSettings();
    home->saveSettings();
    delete triggers;
    delete menus_;
    gGui = nullptr;

    if (detector_window)
        detector_window->close();

    for (PeakWindow* window : peak_windows) {
        window->close();
        delete window;
    }
    peak_windows.clear();

    if (log_window)
        log_window->close();

    QMainWindow::closeEvent(event);
}

void MainWin::initStatusBar()
{
    _status = new QLabel("Ready", statusBar());
    _light = new QLabel();
    _red_circle =
        QPixmap(":images/statusbar/red-circle.svg").scaledToHeight(statusBar()->height() * 0.5);
    _green_circle =
        QPixmap(":images/statusbar/green-circle.svg").scaledToHeight(statusBar()->height() * 0.5);
    _light->setPixmap(_green_circle);
    statusBar()->addPermanentWidget(_status);
    statusBar()->addPermanentWidget(_light);
}

void MainWin::setReady(bool ready)
{
    if (ready) {
        _status->setText("Ready");
        _light->setPixmap(_green_circle);
    } else {
        _status->setText("Processing");
        _light->setPixmap(_red_circle);
    }
}

//! Determine whether a color is dark or not using luminance
bool MainWin::isDark()
{
    QColor color = palette().color(QPalette::Window);
    double r = color.red();
    double g = color.green();
    double b = color.blue();
    double luminance = (0.2126 * r + 0.7152 * g + 0.0722 * b) / 256.0;
    return luminance < 0.5;
}

void MainWin::refreshMenu()
{
    menus_->toggleEntries();
}

QSettings MainWin::qSettings() const
{
    return QSettings("OpenHKL", "OpenHKL");
}
