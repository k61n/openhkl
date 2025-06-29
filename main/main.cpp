//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      main/main.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  https://openhkl.org
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
#include <QStyleFactory>

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    QApplication::setApplicationName("OpenHKL");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("OpenHKL");
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    app.setAttribute(Qt::AA_UseHighDpiPixmaps); // Fixes pixellated .svg icons

    QLoggingCategory::setFilterRules("*.debug=true\nqt.*.debug=false\nqt.qpa*=false\nkf.*=false");
    Session session;

    MainWin mainWindow;

    return QApplication::exec();
}
