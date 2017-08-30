#include "QtStreamWrapper.h"

#include <sstream>

#include <QDebug>
#include <QString>
#include <QTextEdit>

#include <nsxlib/logger/AggregateStreamWrapper.h>
#include <nsxlib/logger/Logger.h>
#include <nsxlib/logger/LogFileStreamWrapper.h>

#include "NoteBook.h"

QtStreamWrapper::QtStreamWrapper(NoteBook* notebook) : nsx::IStreamWrapper()
{
    connect(this,SIGNAL(sendLogMessage(const std::string&)),notebook,SLOT(printLogMessage(const std::string&)));
}

QtStreamWrapper::~QtStreamWrapper()
{
}

void QtStreamWrapper::print(const std::string& message)
{
    std::ostringstream os;
    os << message;

    emit sendLogMessage(os.str());
}

