//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      main/main.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/MainWin.h"
#include "gui/models/Session.h"
#include <QApplication>
#include <QCR/engine/console.h>
#include <QCR/engine/logger.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("nsxgui");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("nsx");

    Logger logger {"nsxgui.log"};
    Console console;
    Session session;

    new MainWin {};

    return app.exec();
}
