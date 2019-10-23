//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeExperiment.cpp
//! @brief     Implements class SubframeExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/SubframeExperiment.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QWidget>

SubframeExperiment::SubframeExperiment()
    : QWidget()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QSplitter* spliter = new QSplitter(this);

    QWidget* left_widget = new QWidget();
    QVBoxLayout* left_layout = new QVBoxLayout;

    properties = new PropertyPanel;
    left_layout->addWidget(properties, 7);
    logger = new LoggerPanel;
    left_layout->addWidget(logger, 2);

    left_widget->setLayout(left_layout);
    spliter->addWidget(left_widget);

    QSplitter* right_spliter = new QSplitter();
    right_spliter->setOrientation(Qt::Orientation::Vertical);
    right_spliter->setChildrenCollapsible(false);

    image = new ImagePanel;
    right_spliter->addWidget(image);
    plot = new PlotPanel;
    right_spliter->addWidget(plot);

    spliter->addWidget(right_spliter);

    layout->addWidget(spliter);
}
