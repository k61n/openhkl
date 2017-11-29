#pragma once

#include <sstream>
#include <string>

#include <QObject>

#include <nsxlib/IStreamWrapper.h>

class NoteBook;

class QtStreamWrapper : public QObject, public nsx::IStreamWrapper {

    Q_OBJECT

public:

    //! Constructor
    QtStreamWrapper(NoteBook* notebook, std::function<std::string()> prefix=nullptr, std::function<std::string()> suffix=nullptr);

    //! Destructor
    virtual ~QtStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

    virtual void printPrefix() override;

    virtual void printSuffix() override;

signals:

    void sendLogMessage(const std::string& message);

private:

    std::ostringstream _os;
};
