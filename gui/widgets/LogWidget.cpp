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

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

#include <QColor>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

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
    QWidget(parent)
{
    // Text display
    _textDisplay = new QTextEdit();
    _textDisplay->setReadOnly(true);
    _textDisplay->setAcceptRichText(false);
    _textDisplay->setLineWrapColumnOrWidth(_wrapColumn);
    _textDisplay->setFontFamily(QString::fromStdString(_fontFamily));
    _textDisplay->setFontPointSize(_fontPointSize);
    // add a prologue
    _textDisplay->setText(
        QString::fromStdString("-*- " + _initText + " [" + datetime() + "] -*-"));

    // save button
    _saveButton = new QPushButton("Save");

    // clear button
    _clearButton = new QPushButton("Clear");

    // button layout
    QHBoxLayout* button_hlay = new QHBoxLayout();
    button_hlay->addWidget(_saveButton);
    button_hlay->addWidget(_clearButton);

    // widget layout
    QVBoxLayout* widget_vlay = new QVBoxLayout();
    widget_vlay->addLayout(button_hlay);
    widget_vlay->addWidget(_textDisplay);

    setLayout(widget_vlay);

    _connectUI();
}

void LogWidget::_connectUI()
{
    connect(_saveButton, &QPushButton::clicked, this, &LogWidget::saveText);
    connect(_clearButton, &QPushButton::clicked, this, &LogWidget::clearText);
}

void LogWidget::clearText() const
{
    _textDisplay->clear();
}

void LogWidget::saveText() const
{
    // TODO: Implement this
}

std::string LogWidget::text() const
{
    return (_textDisplay->toPlainText()).toStdString();
}

void LogWidget::show(const nsx::LogMessage& message)
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
    };

    _textDisplay->setTextColor(text_color);
    _textDisplay->append(QString::fromStdString(message.body));
}
