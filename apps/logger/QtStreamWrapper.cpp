//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/logger/QtStreamWrapper.cpp
//! @brief     Implements ###THINGS###
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

#include "base/logger/AggregateStreamWrapper.h"
#include "base/logger/LogFileStreamWrapper.h"
#include "base/logger/Logger.h"

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
    if (_prefix)
        _os << _prefix();
}

void QtStreamWrapper::printSuffix()
{
    if (_suffix)
        _os << _suffix();
    emit sendLogMessage(_os.str());
    _os.clear();
}
