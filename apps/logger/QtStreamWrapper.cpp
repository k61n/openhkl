#include <sstream>

#include <QDebug>
#include <QString>
#include <QTextEdit>

#include <core/AggregateStreamWrapper.h>
#include <core/LogFileStreamWrapper.h>
#include <core/Logger.h>

#include "NoteBook.h"
#include "QtStreamWrapper.h"

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
