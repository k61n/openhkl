#include "QtStreamWrapper.h"

#include <QDebug>

QtStreamWrapper::QtStreamWrapper() : nsx::IStreamWrapper()
{
}

QtStreamWrapper::~QtStreamWrapper()
{
}

void QtStreamWrapper::print(const std::string& message)
{
    qDebug() << message.c_str();
}
