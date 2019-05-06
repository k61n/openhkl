
#include <QString>
#include <QApplication>
#include <QProgressBar>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>
#include "gui/mainwindow.h"
#include "gui/actions/menus.h"
#include "gui/actions/triggers.h"
#include "gui/view/toggles.h"

#include <iostream>

MainWindow* gGui; //!< global pointer to the main window

//  ***********************************************************************************************
//! @class MainWindow

MainWindow::MainWindow()
{
    gGui = this;

    triggers = new Triggers;
    toggles = new Toggles;
    menus_ = new Menus(menuBar());

    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setAttribute(Qt::WA_DeleteOnClose, true);

    //layout
    setContentsMargins(5,5,5,5);

    addDockWidget(Qt::RightDockWidgetArea, (dockImage_ = new SubframeImage));
    addDockWidget(Qt::RightDockWidgetArea, (dockPlot_ = new SubframePlot));
    addDockWidget(Qt::LeftDockWidgetArea, (dockExperiments_ = new SubframeExperiments));
    addDockWidget(Qt::LeftDockWidgetArea, (dockProperties_ = new SubframeProperties));
    addDockWidget(Qt::LeftDockWidgetArea, (dockLogger_ = new SubframeLogger));

    show();
}

void MainWindow::refresh()
{
    bool hasData = false;
    menus_->export_->setEnabled(hasData);
    menus_->experiment_->setEnabled(hasData);
    menus_->file_->setEnabled(true);
    menus_->help_->setEnabled(true);
    menus_->options_->setEnabled(true);
    menus_->view_->setEnabled(true);
}

void MainWindow::resetViews()
{
    std::cout << "reset Views triggered :)" << std::endl;
}
