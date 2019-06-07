//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/NSXQtApp.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <core/Logger.h>
#include <core/Version.h>

#include <QApplication>

#include "NSXQtApp.h"

NSXQtApp::NSXQtApp(int& argc, char* argv[]) : QApplication(argc, argv)
{
    this->setApplicationDisplayName(QString::fromStdString("NSXQt " + nsx::__version__));
    this->setApplicationName("nsxqt");
}

bool NSXQtApp::notify(QObject* receiver, QEvent* event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception& e) {
        nsx::error() << e.what();
        return false;
    }
}
