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
    left->addWidget(properties, 7.5);
    logger = new SubframeLogger;
    left->addWidget(logger, 2.5);
    layout->addLayout(left, 1);

    QVBoxLayout* right = new QVBoxLayout;
    image = new ImageWidget;
    right->addWidget(image, 7);
    plot = new SubframePlot;
    right->addWidget(plot, 3);
    layout->addLayout(right, 2);
}
