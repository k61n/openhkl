//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SideBar.cpp
//! @brief     Implements class SideBar
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/panels/SideBar.h"

#include "gui/MainWin.h"
#include "gui/models/Session.h"

SideBar::SideBar(QWidget *parent)
    : QToolBar{parent}
{
    setFloatable(false);
    setMovable(false);
    setObjectName("NSXToolSidebar");
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QAction* home = addAction(QIcon(":/images/homeIcon.svg"), "Home");
    QAction* experiment = addAction(QIcon(":/images/experimentIcon.png"), "Experiment");
    QAction* finder = addAction(QIcon(":/images/findPeakIcon.png"), "Find Peaks");
    QAction* filter = addAction(QIcon(":/images/filterIcon.svg"), "Peak Filter");
    QAction* indexer = addAction(QIcon(":/images/findUnitCellIcon.png"), "Indexer");
    QAction* predictor = addAction("Predictor");
    QAction* info = addAction(QIcon(":/images/peakInformationIcon.svg"), "Information");

    connect(home, &QAction::triggered, this, &SideBar::onHome);
    connect(experiment, &QAction::triggered, this, &SideBar::onExperiment);
    connect(finder, &QAction::triggered, this, &SideBar::onFindPeaks);
    connect(filter, &QAction::triggered, this, &SideBar::onFilterPeaks);
    connect(indexer, &QAction::triggered, this, &SideBar::onIndexer);
    connect(predictor, &QAction::triggered, this, &SideBar::onPredictor);
    connect(info, &QAction::triggered, this, &SideBar::onInformation);
}

void SideBar::onHome()
{
    gGui->layoutStack_->setCurrentIndex(0);
}

void SideBar::onExperiment()
{
    gGui->layoutStack_->setCurrentIndex(1);
}

void SideBar::onFindPeaks()
{
    gGui->layoutStack_->setCurrentIndex(2);
    gGui->finder_->refreshAll();
}

void SideBar::onFilterPeaks()
{
    gGui->layoutStack_->setCurrentIndex(3);
    gGui->filter_->refreshAll();
}

void SideBar::onIndexer()
{
    gGui->layoutStack_->setCurrentIndex(4);
    gGui->indexer_->refreshAll();
}

void SideBar::onPredictor()
{
    gGui->layoutStack_->setCurrentIndex(5);
    gGui->predictor_->refreshAll();
}

void SideBar::onInformation()
{
    //gGui->layoutStack_->setCurrentIndex(5);
}
