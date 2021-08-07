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
#include "gui/detector_window/DetectorWindow.h"
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

    // set initial max. intensities
    int initial_intensity = experiment->detectorWidget()->slider()->value();
    finder->detectorWidget()->scene()->setMaxIntensity(initial_intensity);
    filter->detectorWidget()->scene()->setMaxIntensity(initial_intensity);
    predictor->detectorWidget()->scene()->setMaxIntensity(initial_intensity);
    integrator->detectorWidget()->scene()->setMaxIntensity(initial_intensity);
    detector_window->detectorWidget()->scene()->setMaxIntensity(initial_intensity);

    std::vector<DetectorScene*> scenes = {
        experiment->detectorWidget()->scene(),
        detector_window->detectorWidget()->scene(),
        finder->detectorWidget()->scene(),
        filter->detectorWidget()->scene(),
        predictor->detectorWidget()->scene(),
        integrator->detectorWidget()->scene()};
    std::vector<QSlider*> sliders = {
        experiment->detectorWidget()->slider(),
        detector_window->detectorWidget()->slider()};

    connect (
        experiment->detectorWidget()->slider(), &QSlider::valueChanged,
        detector_window->detectorWidget()->slider(), &QSlider::setValue);
    connect (
        detector_window->detectorWidget()->slider(), &QSlider::valueChanged,
        experiment->detectorWidget()->slider(), &QSlider::setValue);

    // // sync the max. intensity slider with all other image plots
    for (auto slider1 : sliders) {
        for (auto slider2 : sliders) {
            if (!(slider1 == slider2))
                connect(slider1, &QSlider::valueChanged, slider2, &QSlider::setValue);
        }
    }
    for (auto* slider : sliders) {
        for (auto* scene : scenes)
            connect(slider, &QSlider::valueChanged, scene, &DetectorScene::setMaxIntensity);
    }

    main_layout->addWidget(_layout_stack);
    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);
    readSettings();
    show();
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

    QMainWindow::closeEvent(event);
}
