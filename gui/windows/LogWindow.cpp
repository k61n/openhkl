//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/windows/LogWindow.cpp
//! @brief     Implements class LogWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/windows/LogWindow.h"

#include "gui/widgets/LogWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

LogWindow::LogWindow(QWidget* parent)
    : QDialog(parent)
{
    // log widget
    _log_widget = new LogWidget(parent);

    // save button
    _saveButton = new QPushButton("Save");

    // clear button
    _clearButton = new QPushButton("Clear");

    // button layout
    QHBoxLayout* button_hlay = new QHBoxLayout();
    button_hlay->addStretch();
    button_hlay->addWidget(_saveButton);
    button_hlay->addWidget(_clearButton);

    // log widget layout
    QHBoxLayout* widget_hlay = new QHBoxLayout();
    widget_hlay->addWidget(_log_widget);

    QVBoxLayout* main_vlay = new QVBoxLayout;
    main_vlay->addLayout(button_hlay);
    main_vlay->addLayout(widget_hlay);
    setLayout(main_vlay);

    _connectUI();
}


void LogWindow::_connectUI()
{
    connect(_saveButton, &QPushButton::clicked, _log_widget, &LogWidget::saveText);
    connect(_clearButton, &QPushButton::clicked, _log_widget, &LogWidget::clearText);
}
