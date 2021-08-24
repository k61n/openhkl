//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/LogWidget.h
//! @brief     Defines class LogWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_GUI_WIDGETS_LOGWIDGET_H
#define NSX_GUI_WIDGETS_LOGWIDGET_H

#include <string>

#include "base/utils/Logger.h" // LogMessage::receiverHandle

#include <QTextEdit>  // derives from
#include <QColor>

// Displays the log messages to the GUI user
class LogWidget : public QTextEdit {

 public:
    LogWidget(QWidget* parent = nullptr);
    ~LogWidget();

    // Clear the displayed text
    void clearText();
    // Get the displayed text as plain text
    std::string textStr() const;
    // Write a message in the log window
    void showMessage(const nsx::LogMessage& message);

 private:
    // TODO: declare as static
    QColor _infoColor {"Black"};
    const QColor _errorColor {"#FF003F"}; // Crimson
    const QColor _warningColor {"#FF7F00"}; // Orange
    const QColor _debugColor {"#8F00FF"}; // Violet
    const std::string _fontFamily {"Courier"};
    const std::size_t _fontPointSize = 12;
    const std::size_t _wrapColumn = 80;
    const std::string _initText = "NSXTool Messages";

    //! Receiver handle (for registering log messages)
    nsx::LogMessenger::receiverHandle _receiver_handle;

    void _connectUI();
};

#endif // NSX_GUI_WIDGETS_LOGWIDGET_H
