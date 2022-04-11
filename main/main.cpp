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

#include "core/experiment/Experiment.h" // needed to instatiate Session()
#include "gui/MainWin.h"
#include "gui/models/Project.h" // needed to instantiate vector<unique_ptr<Project>> in Session()
#include "gui/models/Session.h"
#include <QApplication>
#include <QLoggingCategory>

int main(int argc, char* argv[])
{
    qRegisterMetaTypeStreamOperators<QList<QStringList>>("Data"); // for QSettings
    QApplication app{argc, argv};

    QApplication::setApplicationName("NSXTool");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("nsx");

    QLoggingCategory::setFilterRules("*.debug=true\nqt.*.debug=false\nqt.qpa*=false\nkf.*=false");
    Session session;

    MainWin mainWindow;

    return QApplication::exec();
}
