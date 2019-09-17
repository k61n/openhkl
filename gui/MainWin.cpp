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

#include "gui/actions/Menus.h"
#include "gui/actions/Triggers.h"
#include "gui/panels/SideBar.h"
#include <QApplication>
#include <QCloseEvent>
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

    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // layout
    setContentsMargins(5, 5, 5, 5);

    addToolBar(Qt::LeftToolBarArea, new SideBar(this));

    homeScreen_ = new SubframeHome;
    experimentScreen_ = new SubframeExperiment;
    finder_ = new PeakFinderFrame;
    filter_ = new SubframeFilterPeaks;
    indexer_ = new SubframeAutoIndexer;
    predictor_ = new SubframePredictPeaks;

    layoutStack_ = new QStackedWidget;
    layoutStack_->addWidget(homeScreen_);
    layoutStack_->addWidget(experimentScreen_);
    layoutStack_->addWidget(finder_);
    layoutStack_->addWidget(filter_);
    layoutStack_->addWidget(indexer_);
    layoutStack_->addWidget(predictor_);
    layoutStack_->setCurrentIndex(0);

    setCentralWidget(layoutStack_);

    readSettings();
    show();
}

MainWin::~MainWin() {}

void MainWin::refresh()
{
    bool hasData = false;
    menus_->export_->setEnabled(hasData);
    menus_->experiment_->setEnabled(hasData);
    menus_->file_->setEnabled(true);
    menus_->help_->setEnabled(true);
    menus_->options_->setEnabled(true);
    menus_->view_->setEnabled(true);
}

void MainWin::onDataChanged()
{
    experimentScreen_->image->dataChanged();
    experimentScreen_->properties->dataChanged();
    // dockPlot_->dataChanged();
}

void MainWin::onExperimentChanged()
{
    experimentScreen_->properties->experimentChanged();
}

void MainWin::onPeaksChanged()
{
    experimentScreen_->properties->peaksChanged();
}

void MainWin::onUnitCellChanged()
{
    experimentScreen_->properties->unitCellChanged();
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
    homeScreen_->saveSettings();
    delete triggers;
    delete menus_;
    gGui = nullptr;
    event->accept();
}
