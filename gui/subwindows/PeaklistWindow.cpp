//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/actions/Actions.cpp
//! @brief     Implements class Actions
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/PeaklistWindow.h"

#include "gui/widgets/PeakProperties.h"

#include <QVBoxLayout>

PeaklistWindow::PeaklistWindow(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("List of peaks");
    auto layout = new QVBoxLayout(this);
    _peak_properties = new PeakProperties();
    layout->addWidget(_peak_properties);
    resize(1400, 600);
}

void PeaklistWindow::refreshAll()
{
    _peak_properties->refreshInput();
}

void PeaklistWindow::setPeakCollection(const QString& pc_name)
{
    _peak_properties->setPeakCollection(pc_name);
}
