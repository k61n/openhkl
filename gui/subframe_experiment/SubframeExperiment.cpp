//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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
#include "gui/widgets/DetectorWidget.h"
#include "gui/widgets/PlotPanel.h"

#include <QComboBox>
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

    _plot = new PlotPanel;

    QGroupBox* figure_group = new QGroupBox("Detector image");
    figure_group->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _detector_widget = new DetectorWidget(true, false, true, figure_group);
    _detector_widget->modeCombo()->addItems(QStringList{
        "Zoom", "Selection box", "Rectangular mask", "Elliptical mask", "Line plot",
        "Horizontal slice", "Vertical slice"});

    left_widget->setLayout(left_layout);

    QSplitter* right_splitter = new QSplitter();
    right_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_splitter->setOrientation(Qt::Orientation::Vertical);
    right_splitter->setChildrenCollapsible(false);
    right_splitter->addWidget(figure_group);
    right_splitter->addWidget(_plot);

    splitter->addWidget(left_widget);
    splitter->addWidget(right_splitter);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 4);
    splitter->setChildrenCollapsible(false);

    layout->addWidget(splitter);
}

void SubframeExperiment::refreshAll()
{
    if (!gSession->hasProject())
        return;

    _detector_widget->refresh();
}

DetectorWidget* SubframeExperiment::detectorWidget()
{
    return _detector_widget;
}
