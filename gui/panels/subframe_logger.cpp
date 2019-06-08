//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_logger.cpp
//! @brief     Implements ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/panels/subframe_logger.h"
#include "gui/mainwin.h"
#include "gui/view/toggles.h"
#include <QCR/engine/logger.h>
#include <QTreeView>

SubframeLogger::SubframeLogger() : QcrDockWidget {"Logger"}
{
    logText = new QTextEdit;
    setWidget(logText);
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewLogger, SLOT(setChecked(bool)));
    connect(gLogger, SIGNAL(sigLine(QString)), this, SLOT(slotPrintLog(QString)));
}

void SubframeLogger::slotPrintLog(const QString& line)
{
    logText->append(line);
}
