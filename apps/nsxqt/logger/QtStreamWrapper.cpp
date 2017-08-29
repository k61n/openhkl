#include "QtStreamWrapper.h"

#include <sstream>

#include <QDebug>
#include <QString>
#include <QTextEdit>

QtStreamWrapper::QtStreamWrapper(QTextEdit* notebook) : nsx::IStreamWrapper(), _notebook(notebook)
{
}

QtStreamWrapper::~QtStreamWrapper()
{
}

void QtStreamWrapper::print(const std::string& message)
{
    std::ostringstream os;
    os << message;
    _notebook->append(QString::fromStdString(os.str()));
}

