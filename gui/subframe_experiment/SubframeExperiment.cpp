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

#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include "gui/subframe_experiment/PlotPanel.h"
#include "gui/subframe_experiment/PropertyPanel.h"
#include "gui/subframe_experiment/properties/NumorProperty.h"
#include "gui/widgets/DetectorWidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

SubframeExperiment::SubframeExperiment()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QSplitter* splitter = new QSplitter(this);

    QWidget* left_widget = new QWidget();
    left_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    QVBoxLayout* left_layout = new QVBoxLayout;

    _properties = new PropertyPanel;
    _plot = new PlotPanel;

    QGroupBox* figure_group = new QGroupBox("Preview");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(true, false, true, figure_group);
    _detector_widget->modeCombo()->addItems(QStringList{
        "Zoom", "Selection", "Rectangular mask", "Elliptical mask", "Line plot", "Horizontal slice",
        "Vertical slice"});
    QWidget* right_widget = new QWidget(this);
    right_widget->setLayout(_detector_widget);

    _properties->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    left_layout->addWidget(_properties, 7);
    left_widget->setLayout(left_layout);

    QSplitter* right_splitter = new QSplitter();
    right_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_splitter->setOrientation(Qt::Orientation::Vertical);
    right_splitter->setChildrenCollapsible(false);
    right_splitter->addWidget(right_widget);
    right_splitter->addWidget(_plot);

    splitter->addWidget(left_widget);
    splitter->addWidget(right_splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    splitter->setChildrenCollapsible(false);

    layout->addWidget(splitter);
}


void SubframeExperiment::dataChanged()
{
    _data_list = gSession->currentProject()->allData();
    if (_data_list.empty())
        return;

    _detector_widget->updateDatasetList(_data_list);
}

DetectorWidget* SubframeExperiment::detectorWidget()
{
    return _detector_widget;
}
