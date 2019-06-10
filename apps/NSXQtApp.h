//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/NSXQtApp.h
//! @brief     Defines class NSXQtApp
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QApplication>

class NSXQtApp : public QApplication {
public:
    NSXQtApp(int& argc, char* argv[]);

private:
    bool notify(QObject* receiver, QEvent* event);
};
