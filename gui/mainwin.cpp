//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/mainwin.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "gui/mainwin.h"
#include "gui/actions/menus.h"
#include "gui/actions/triggers.h"
#include "gui/view/toggles.h"
#include <QApplication>
#include <QProgressBar>
#include <QSettings>
#include <QSplitter>
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

    triggers = new Triggers;
    toggles = new Toggles;
    menus_ = new Menus(menuBar());

    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // layout
    setContentsMargins(5, 5, 5, 5);

    addDockWidget(Qt::RightDockWidgetArea, (dockImage_ = new SubframeImage));
    addDockWidget(Qt::RightDockWidgetArea, (dockPlot_ = new SubframePlot));
    addDockWidget(Qt::LeftDockWidgetArea, (dockExperiments_ = new SubframeExperiments));
    addDockWidget(Qt::LeftDockWidgetArea, (dockProperties_ = new SubframeProperties));
    addDockWidget(Qt::LeftDockWidgetArea, (dockLogger_ = new SubframeLogger));

    readSettings();
    show();
}

MainWin::~MainWin()
{
    saveSettings();
    delete triggers;
    delete toggles;
    delete menus_;
    gGui = nullptr;
}

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
