//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/subwindows/LogWindow.cpp
//! @brief     Implements class LogWindow
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/subwindows/LogWindow.h"

#include "base/utils/Logger.h"
#include "gui/widgets/LogWidget.h"

#include <cstdio> // fopen, fclose, fprintf, FILE

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QVBoxLayout>


LogWindow::LogWindow(QWidget* parent) : QDialog(parent)
{
    // log widget
    _log_widget = new LogWidget(parent);

    // save button
    _saveButton = new QPushButton("Save");

    // clear button
    _clearButton = new QPushButton("Clear");

    // print level
    _levelCombo = new QComboBox();

    // button layout
    QHBoxLayout* button_hlay = new QHBoxLayout();
    button_hlay->addStretch();
    button_hlay->addWidget(_levelCombo);
    button_hlay->addWidget(_saveButton);
    button_hlay->addWidget(_clearButton);

    for (const auto& [key, val] : _level_strings)
        _levelCombo->addItem(QString::fromStdString(key));

    // log widget layout
    QHBoxLayout* widget_hlay = new QHBoxLayout();
    widget_hlay->addWidget(_log_widget);

    QVBoxLayout* main_vlay = new QVBoxLayout;
    main_vlay->addLayout(button_hlay);
    main_vlay->addLayout(widget_hlay);
    setLayout(main_vlay);

    _connectUI();

    _levelCombo->setCurrentText("Info");
}


void LogWindow::_connectUI()
{
    connect(
        _levelCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
        &LogWindow::_setPrintLevel);
    connect(_saveButton, &QPushButton::clicked, this, &LogWindow::saveLog);
    connect(_clearButton, &QPushButton::clicked, _log_widget, &LogWidget::clearText);
}

#include <iostream>

void LogWindow::saveLog()
{
    QSettings qset;
    qset.beginGroup("RecentDirectories");
    QString logSaveDirectory = qset.value("logdir", QDir::homePath()).toString();

    QString format_string{"Log files(*.log);;all files (*.* *)"};
    QString filename =
        QFileDialog::getSaveFileName(this, "Save log messages", logSaveDirectory, format_string);

    if (filename.isEmpty())
        return;

    // write the log messages as plain text to the given file
    const std::string txt{_log_widget->textStr()};
    const std::string filenm{filename.toStdString()};
    FILE* file_ptr = nullptr;
    file_ptr = fopen(filenm.c_str(), "w");

    if (file_ptr) {
        fprintf(file_ptr, "%s\n-*- END LOG -*-\n", txt.c_str());
        fclose(file_ptr);

        const std::string msg{"Log messages saved to '" + filenm + "'"};
        ohkl::nsxlog(ohkl::Level::Info, msg);
        ohkl::nsxmsg(ohkl::Level::Info, msg);
    } else {
        const std::string msg{"Unable to save log messages to '" + filenm + "'"};
        ohkl::nsxlog(ohkl::Level::Error, msg);
        ohkl::nsxmsg(ohkl::Level::Error, msg);
    }
}

void LogWindow::_setPrintLevel()
{
    _log_widget->setPrintLevel(
        _level_strings.find(_levelCombo->currentText().toStdString())->second);
}
