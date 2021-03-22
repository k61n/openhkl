//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/SubframeExperiment.cpp
//! @brief     Implements class SubframeExperiment
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/SubframeExperiment.h"

#include "gui/subframe_experiment/ImagePanel.h"
#include "gui/subframe_experiment/LoggerPanel.h"
#include "gui/subframe_experiment/PlotPanel.h"
#include "gui/subframe_experiment/PropertyPanel.h"
#include "gui/subframe_experiment/properties/NumorProperty.h"

#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

SubframeExperiment::SubframeExperiment()
{
    QSizePolicy left_size_policy;
    left_size_policy.setHorizontalPolicy(QSizePolicy::Minimum);
    left_size_policy.setVerticalPolicy(QSizePolicy::Expanding);

    QSizePolicy right_size_policy;
    right_size_policy.setHorizontalPolicy(QSizePolicy::Expanding);
    right_size_policy.setVerticalPolicy(QSizePolicy::Expanding);

    QHBoxLayout* layout = new QHBoxLayout(this);
    QSplitter* splitter = new QSplitter(this);

    QWidget* left_widget = new QWidget();
    left_widget->setSizePolicy(left_size_policy);
    QVBoxLayout* left_layout = new QVBoxLayout;

    _properties = new PropertyPanel;
    _logger = new LoggerPanel;
    _image = new ImagePanel;
    _plot = new PlotPanel;

    _properties->setSizePolicy(left_size_policy);
    _logger->setSizePolicy(left_size_policy);

    left_layout->addWidget(_properties, 7);
    left_layout->addWidget(_logger, 2);
    left_widget->setLayout(left_layout);

    QSplitter* right_splitter = new QSplitter();
    right_splitter->setSizePolicy(right_size_policy);
    right_splitter->setOrientation(Qt::Orientation::Vertical);
    right_splitter->setChildrenCollapsible(false);
    right_splitter->addWidget(_image);
    right_splitter->addWidget(_plot);

    splitter->addWidget(left_widget);
    splitter->addWidget(right_splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    splitter->setChildrenCollapsible(false);

    layout->addWidget(splitter);

    // ensure that correct numor is plotted
    connect(
        _properties->_data->numorSelector(),
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), _image,
        &ImagePanel::dataChanged);
}
