//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subframe_experiment/LoggerPanel.h
//! @brief     Defines class LoggerPanel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef GUI_SUBFRAME_EXPERIMENT_LOGGER_PANEL_H
#define GUI_SUBFRAME_EXPERIMENT_LOGGER_PANEL_H

#include <QTextEdit>

//! Subframe of the main window that shows the logger lines
class LoggerPanel : public QWidget {
    Q_OBJECT
 public:
    LoggerPanel();

 private slots:
    void slotPrintLog(const QString& line);

 private:
    QTextEdit* logText;
};

#endif // GUI_SUBFRAME_EXPERIMENT_LOGGER_PANEL_H
