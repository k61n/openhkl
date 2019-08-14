//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/SubframeLogger.h
//! @brief     Defines class SubframeLogger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_PANELS_SUBFRAMELOGGER_H
#define GUI_PANELS_SUBFRAMELOGGER_H

#include <QCR/widgets/views.h>
#include <QTextEdit>

//! Subframe of the main window that shows the logger lines
class SubframeLogger : public QcrWidget {
    Q_OBJECT
 public:
    SubframeLogger();

 private slots:
    void slotPrintLog(const QString& line);

 private:
    QTextEdit* logText;
};

#endif // GUI_PANELS_SUBFRAMELOGGER_H
