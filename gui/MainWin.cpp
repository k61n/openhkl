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
#include "gui/utility/SideBar.h"
#include <QApplication>
#include <QCloseEvent>
#include <QProgressBar>
#include <QSettings>
#include <QSplitter>
#include <QStackedLayout>
#include <QHBoxLayout>
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
    QWidget* main_widget = new QWidget(this);
    QHBoxLayout* main_layout = new QHBoxLayout();

    _side_bar = new SideBar(main_widget);
    main_layout->addWidget(_side_bar);

    _home = new SubframeHome;
    _experiment = new SubframeExperiment;
    _finder = new PeakFinderFrame;
    _filter = new SubframeFilterPeaks;
    _indexer = new SubframeAutoIndexer;
    _predictor = new SubframePredictPeaks;
    _merger = new SubframeMergedPeaks;

    _layout_stack = new QStackedWidget(main_widget);
    _layout_stack->addWidget(_home);
    _layout_stack->addWidget(_experiment);
    _layout_stack->addWidget(_finder);
    _layout_stack->addWidget(_filter);
    _layout_stack->addWidget(_indexer);
    _layout_stack->addWidget(_predictor);
    _layout_stack->addWidget(_merger);
    _layout_stack->setCurrentIndex(0);

    main_layout->addWidget(_layout_stack);
    main_widget->setLayout(main_layout);
    setCentralWidget(main_widget);
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
    _experiment->getImage()->dataChanged();
    _experiment->getProperty()->dataChanged();
}

void MainWin::onExperimentChanged()
{
    _experiment->getProperty()->experimentChanged();
}

void MainWin::onPeaksChanged()
{
    _experiment->getProperty()->peaksChanged();
}

void MainWin::onUnitCellChanged()
{
    _experiment->getProperty()->unitCellChanged();
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
    _home->saveSettings();
    delete triggers;
    delete menus_;
    gGui = nullptr;
    event->accept();
}
