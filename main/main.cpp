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
#include "core/experiment/Experiment.h" // needed to instatiate Session()
#include "gui/models/Project.h" // needed to instantiate vector<unique_ptr<Project>> in Session()
#include "gui/models/Session.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char* argv[])
{
    QApplication app {argc, argv};

    app.setApplicationName("NSXTool");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("nsx");

    QLoggingCategory::setFilterRules("*.debug=true\nqt.*.debug=false\nqt.qpa*=false");
    Session session;

    new MainWin {};

    return app.exec();
}
