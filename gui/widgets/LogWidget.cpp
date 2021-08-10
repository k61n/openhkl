//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/widgets/LogWidget.cpp
//! @brief     Implements class LogWidget
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/widgets/LogWidget.h"

#include "base/utils/Logger.h"

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include <QColor>
#include <QTextEdit>


namespace {

// TODO: Move to utils
std::string datetime()
{
    using clock = std::chrono::system_clock;
    std::stringstream output;
    std::time_t current_time = clock::to_time_t(clock::now());
    output << std::put_time(std::gmtime(&current_time), "%Y-%b-%d %T");
    return output.str();
}

} // namespace

LogWidget::LogWidget(QWidget* parent):
    QTextEdit(parent)
{
    // Text display
    setReadOnly(true);
    setAcceptRichText(false);
    setLineWrapColumnOrWidth(_wrapColumn);
    setFontFamily(QString::fromStdString(_fontFamily));
    setFontPointSize(_fontPointSize);
    // add a prologue
    setText(
        QString::fromStdString("-*- " + _initText + " [" + datetime() + "] -*-"));

    _connectUI();
}

void LogWidget::_connectUI()
{
    // register a method as receiver of log messages
    _receiver_handle = nsx::Logger::instance().Msg.addReceiver
        (&LogWidget::showMessage, this);
}

LogWidget::~LogWidget()
{
    // de-register the log-message receiver
    nsx::Logger::instance().Msg.discardReceiver(_receiver_handle);
}

void LogWidget::clearText()
{
    clear();
}

std::string LogWidget::textStr() const
{
    return toPlainText().toStdString();
}

void LogWidget::showMessage(const nsx::LogMessage& message)
{
    QColor text_color {_infoColor};  // default text color

    switch (message.level) {
    case nsx::Level::Info:
        text_color = _infoColor;
        break;
    case nsx::Level::Error:
        text_color = _errorColor;
        break;
    case nsx::Level::Warning:
        text_color = _warningColor;
        break;
    case nsx::Level::Debug:
        text_color = _debugColor;
        break;
    default:
        text_color = _infoColor;
    };

    setTextColor(text_color);
    append(QString::fromStdString(message.body));
}
