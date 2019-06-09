//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/panels/subframe_logger.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef GUI_PANELS_SUBFRAME_LOGGER_H
#define GUI_PANELS_SUBFRAME_LOGGER_H

#include <QCR/widgets/views.h>
#include <QTextEdit>

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframeLogger : public QcrDockWidget {
    Q_OBJECT
public:
    SubframeLogger();

private slots:
    void slotPrintLog(const QString& line);

private:
    QTextEdit* logText;
};

#endif // GUI_PANELS_SUBFRAME_LOGGER_H
