//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/LogWindow.h
//! @brief     Defines class LogWindow
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_GUI_SUBWINDOWS_LOGWINDOW_H
#define OHKL_GUI_SUBWINDOWS_LOGWINDOW_H

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
    //! Return the log widget
    LogWidget* logWidget() { return _log_widget; };

 private:
    LogWidget* _log_widget;
    QPushButton* _saveButton;
    QPushButton* _clearButton;
    QComboBox* _levelCombo;

    void _connectUI();
    void _setPrintLevel();

    const std::map<std::string, ohkl::Level> _level_strings{
        {"Off", ohkl::Level::Off},
        {"Info", ohkl::Level::Info},
        {"Error", ohkl::Level::Error},
        {"Warning", ohkl::Level::Warning},
        {"Debug", ohkl::Level::Debug}};
};

#endif // OHKL_GUI_SUBWINDOWS_LOGWINDOW_H
