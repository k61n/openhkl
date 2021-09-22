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
#include "base/utils/StringIO.h" // datetime_str
#include "gui/MainWin.h"
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include <QColor>
#include <QTextEdit>

LogWidget::LogWidget(QWidget* parent):
    QTextEdit(parent)
{
    if (gGui->isDark()) // looks like we have a dark theme
        _infoColor = QColor(Qt::white);
    else
        _infoColor = QColor(Qt::black);

    // Text display
    setReadOnly(true);
    setAcceptRichText(false);
    setLineWrapColumnOrWidth(_wrapColumn);
    setFontFamily(QString::fromStdString(_fontFamily));
    setFontPointSize(_fontPointSize);
    // add a prologue
    setText(
        QString::fromStdString("-*- " + _initText + " ["
                               + nsx::datetime_str() + "] -*-"));

    // set minmimum size to 100 columns, 30 lines (depending on font; assumes monospace)
    auto* fontMetrics = new QFontMetrics(currentFont());
    QSize fontSize = fontMetrics->size(0, "A");
    setMinimumSize(fontSize.width() * 100 + 10, fontSize.height() * 30 + 10);
    resize(fontSize.width() * 100 + 10, fontSize.height() + 10);

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
    if (message.level > _print_level)
        return;

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

void LogWidget::setPrintLevel(const nsx::Level& level)
{
    _print_level = level;
}
