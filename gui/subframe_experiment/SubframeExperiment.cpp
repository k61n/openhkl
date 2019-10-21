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

SubframeExperiment::SubframeExperiment()
    : QWidget()
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    QVBoxLayout* left = new QVBoxLayout;
    properties = new PropertyPanel;
    left->addWidget(properties, 7);
    logger = new LoggerPanel;
    left->addWidget(logger, 2);
    layout->addLayout(left, 1);

    QVBoxLayout* right = new QVBoxLayout;
    image = new ImagePanel;
    right->addWidget(image, 7);
    plot = new PlotPanel;
    right->addWidget(plot, 3);
    layout->addLayout(right, 2);
}
