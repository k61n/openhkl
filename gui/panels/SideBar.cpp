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
    QAction* info = addAction(QIcon(":/images/peakInformationIcon.svg"), "Merger");

    connect(home, &QAction::triggered, this, &SideBar::onHome);
    connect(experiment, &QAction::triggered, this, &SideBar::onExperiment);
    connect(finder, &QAction::triggered, this, &SideBar::onFindPeaks);
    connect(filter, &QAction::triggered, this, &SideBar::onFilterPeaks);
    connect(indexer, &QAction::triggered, this, &SideBar::onIndexer);
    connect(predictor, &QAction::triggered, this, &SideBar::onPredictor);
    connect(info, &QAction::triggered, this, &SideBar::onMerger);
}

void SideBar::onHome()
{
    gGui->_layout_stack->setCurrentIndex(0);
}

void SideBar::onExperiment()
{
    gGui->_layout_stack->setCurrentIndex(1);
}

void SideBar::onFindPeaks()
{
    gGui->_layout_stack->setCurrentIndex(2);
    gGui->_finder->refreshAll();
}

void SideBar::onFilterPeaks()
{
    gGui->_layout_stack->setCurrentIndex(3);
    gGui->_filter->refreshAll();
}

void SideBar::onIndexer()
{
    gGui->_layout_stack->setCurrentIndex(4);
    gGui->_indexer->refreshAll();
}

void SideBar::onPredictor()
{
    gGui->_layout_stack->setCurrentIndex(5);
    gGui->_predictor->refreshAll();
}

void SideBar::onMerger()
{
    gGui->_layout_stack->setCurrentIndex(6);
    gGui->_merger->refreshAll();
}
