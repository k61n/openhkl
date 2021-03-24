//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/LoggerPanel.cpp
//! @brief     Implements class LoggerPanel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subframe_experiment/LoggerPanel.h"

#include "gui/MainWin.h"
#include "gui/actions/Actions.h"
#include <QTreeView>
#include <QVBoxLayout>

LoggerPanel::LoggerPanel()
{
    logText = new QTextEdit;

    QVBoxLayout* box = new QVBoxLayout(this);
    box->addWidget(logText);

    // connect(gLogger, SIGNAL(sigLine(QString)), this, SLOT(slotPrintLog(QString)));
}

void LoggerPanel::slotPrintLog(const QString& line)
{
    logText->append(line);
}
