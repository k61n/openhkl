//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/subwindows/LogWindow.h
//! @brief     Defines class LogWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_SUBWINDOWS_LOGWINDOW_H
#define NSX_GUI_SUBWINDOWS_LOGWINDOW_H

#include "base/utils/LogLevel.h"

#include <QDialog>

class LogWidget;
class QComboBox;
class QPushButton;

//! Modeless dialog to display log messages
class LogWindow : public QDialog {
 public:
    LogWindow(QWidget* parent = nullptr);
    // Save the displayed log messages
    void saveLog();

 private:
    LogWidget* _log_widget;
    QPushButton* _saveButton;
    QPushButton* _clearButton;
    QComboBox* _levelCombo;

    void _connectUI();
    void _setPrintLevel();

    const std::map<std::string, nsx::Level> _level_strings{
        {"Off", nsx::Level::Off},
        {"Info", nsx::Level::Info},
        {"Error", nsx::Level::Error},
        {"Warning", nsx::Level::Warning},
        {"Debug", nsx::Level::Debug}};
};

#endif // NSX_GUI_SUBWINDOWS_LOGWINDOW_H
