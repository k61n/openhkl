//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeLogger.cpp
//! @brief     Implements class SubframeLogger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/panels/SubframeLogger.h"

#include "gui/MainWin.h"
#include "gui/actions/Triggers.h"
#include <QCR/engine/logger.h>
#include <QTreeView>

SubframeLogger::SubframeLogger() : QcrDockWidget {"Logger"}
{
    logText = new QTextEdit;
    setWidget(logText);
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->triggers->viewLogger, SLOT(setChecked(bool)));
    connect(gLogger, SIGNAL(sigLine(QString)), this, SLOT(slotPrintLog(QString)));
}

void SubframeLogger::slotPrintLog(const QString& line)
{
    logText->append(line);
}
