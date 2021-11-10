//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/MainWin.cpp
//! @brief     Implements class MainWin
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/MainWin.h"

#include "gui/actions/Actions.h"
#include "gui/actions/Menus.h"
#include "gui/connect/Sentinel.h"
#include "gui/subwindows/DetectorWindow.h"
#include "gui/subwindows/LogWindow.h"
#include "gui/subwindows/PeakWindow.h"
#include "gui/graphics/DetectorScene.h"
#include "gui/graphics/DetectorView.h"
#include "gui/graphics_items/PlottableItem.h"
#include "gui/subframe_combine/SubframeMergedPeaks.h"
#include "gui/subframe_experiment/PlotPanel.h"
#include "gui/subframe_experiment/PropertyPanel.h"
#include "gui/subframe_experiment/SubframeExperiment.h"
#include "gui/subframe_filter/SubframeFilterPeaks.h"
#include "gui/subframe_find/SubframeFindPeaks.h"
#include "gui/subframe_home/SubframeHome.h"
#include "gui/subframe_index/SubframeAutoIndexer.h"
#include "gui/subframe_integrate/SubframeIntegrate.h"
#include "gui/subframe_predict/SubframePredictPeaks.h"
#include "gui/subframe_refiner/SubframeRefiner.h"
#include "gui/utility/SideBar.h"
#include "gui/widgets/DetectorWidget.h"

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

#include <iostream>

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

    home = new SubframeHome;
    experiment = new SubframeExperiment;
    finder = new SubframeFindPeaks;
    filter = new SubframeFilterPeaks;
    indexer = new SubframeAutoIndexer;
    predictor = new SubframePredictPeaks;
    refiner = new SubframeRefiner;
    integrator = new SubframeIntegrate;
    merger = new SubframeMergedPeaks;

    detector_window = new DetectorWindow();
    log_window = new LogWindow(this);
    peak_window = new PeakWindow(this);

    _layout_stack = new QStackedWidget(main_widget);
    _layout_stack->addWidget(home);
    _layout_stack->addWidget(experiment);
    _layout_stack->addWidget(finder);
    _layout_stack->addWidget(filter);
    _layout_stack->addWidget(indexer);
    _layout_stack->addWidget(predictor);
    _layout_stack->addWidget(refiner);
    _layout_stack->addWidget(integrator);
    _layout_stack->addWidget(merger);
    _layout_stack->setCurrentIndex(0);

    main_layout->addWidget(_layout_stack);
    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);
    readSettings();
    show();

    initStatusBar();
}

void MainWin::onDataChanged() const
{
    experiment->dataChanged();
    experiment->getProperty()->dataChanged();
}

void MainWin::onExperimentChanged() const
{
    experiment->getProperty()->experimentChanged();
}

void MainWin::onPeaksChanged() const
{
    experiment->getProperty()->peaksChanged();
}

void MainWin::onUnitCellChanged() const
{
    experiment->getProperty()->unitCellChanged();
}

void MainWin::changeView(int option) const
{
    experiment->detectorWidget()->changeView(option);
}

void MainWin::updatePlot(PlottableItem* p) const
{
    experiment->getPlot()->updatePlot(p);
}

void MainWin::cursormode(int i) const
{
    experiment->detectorWidget()->scene()->changeCursorMode(i);
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
    QSettings s;
    s.beginGroup("MainWindow");
    s.setValue("geometry", saveGeometry());
    s.setValue("state", saveState());
}

void MainWin::readSettings()
{
    if (initialState_.isEmpty())
        initialState_ = saveState();
    QSettings s;
    s.beginGroup("MainWindow");
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("state").toByteArray());
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

    if (peak_window)
        peak_window->close();

    if (log_window)
        log_window->close();

    QMainWindow::closeEvent(event);
}

void MainWin::initStatusBar()
{
    _status = new QLabel("Ready", statusBar());
    _light = new QLabel();
    _red_circle = QPixmap(
        ":images/statusbar/red-circle.svg").scaledToHeight(statusBar()->height() * 0.5);
    _green_circle = QPixmap(
        ":images/statusbar/green-circle.svg").scaledToHeight(statusBar()->height() * 0.5);
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
    double b  = color.blue();
    double luminance = (0.2126 * r + 0.7152 * g + 0.0722 * b) / 256.0;
    return luminance < 0.5;
}
