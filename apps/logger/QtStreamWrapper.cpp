//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/logger/QtStreamWrapper.cpp
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <sstream>

#include <QDebug>
#include <QString>
#include <QTextEdit>

#include "core/logger/AggregateStreamWrapper.h"
#include "core/logger/LogFileStreamWrapper.h"
#include "core/logger/Logger.h"

#include "apps/NoteBook.h"
#include "apps/logger/QtStreamWrapper.h"

QtStreamWrapper::QtStreamWrapper(
    NoteBook* notebook, std::function<std::string()> prefix, std::function<std::string()> suffix)
    : nsx::IStreamWrapper(prefix, suffix)
{
    connect(
        this, SIGNAL(sendLogMessage(const std::string&)), notebook,
        SLOT(printLogMessage(const std::string&)));
}

QtStreamWrapper::~QtStreamWrapper() {}

void QtStreamWrapper::print(const std::string& message)
{
    _os << message;
}

void QtStreamWrapper::printPrefix()
{
    if (_prefix) {
        _os << _prefix();
    }
}

void QtStreamWrapper::printSuffix()
{
    if (_suffix) {
        _os << _suffix();
    }
    emit sendLogMessage(_os.str());
    _os.clear();
}
