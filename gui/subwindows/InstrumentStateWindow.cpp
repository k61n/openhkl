//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subwindows/InstrumentStateWindow.cpp
//! @brief     Implemets class InstrumentStateWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/InstrumentStateWindow.h"

#include <QHBoxLayout>

InstrumentStateWindow::InstrumentStateWindow(QWidget* parent)
    : QDialog(parent)
{
    auto main_layout = new QHBoxLayout(this);
}

void InstrumentStateWindow::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    // setGeometry(gGui->x() + 40, gGui->y() + 80, gGui->width(), gGui->height());
}

void InstrumentStateWindow::refreshAll()
{
}
