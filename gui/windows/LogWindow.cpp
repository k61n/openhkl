//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/windows/LogWindow.cpp
//! @brief     Implements class LogWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/windows/LogWindow.h"

#include "gui/widgets/LogWidget.h"

#include <QVBoxLayout>

LogWindow::LogWindow(QWidget* parent)
    : QDialog(parent)
{
    _log_widget = new LogWidget(parent);
    QVBoxLayout* v_lay = new QVBoxLayout;
    v_lay->addWidget(_log_widget);
    setLayout(v_lay);
}
