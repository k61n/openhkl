//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/Main.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <fstream>

#include <QDesktopWidget>
#include <QPainter>
#include <QSplashScreen>
#include <QTimer>

#include "base/logger/AggregateStreamWrapper.h"
#include "base/logger/Logger.h"
#include "base/logger/StdStreamWrapper.h"

#include "apps/MainWindow.h"
#include "apps/NSXQtApp.h"

int main(int argc, char* argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QGuiApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#endif
    NSXQtApp a(argc, argv);

    // Ensure . is used rather than , for float and double boxes
    QLocale::setDefault(QLocale::c());

    MainWindow main_window(nullptr);

    nsx::info() << "NSXQt session started";

    main_window.showMaximized();
    main_window.show();

    return a.exec();
}
