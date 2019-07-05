//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/NSXQtApp.cpp
//! @brief     Implements class NSXQtApp
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "apps/NSXQtApp.h"
#include <QApplication>
#include <QDebug>
#include <QtGlobal>

NSXQtApp::NSXQtApp(int& argc, char* argv[]) : QApplication(argc, argv)
{
    this->setApplicationDisplayName(
        QString::fromStdString("NSXQt Version?")); // TODO restore version
    this->setApplicationName("nsxqt");
}

bool NSXQtApp::notify(QObject* receiver, QEvent* event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception& e) {
        qWarning() << e.what();
        return false;
    }
}
