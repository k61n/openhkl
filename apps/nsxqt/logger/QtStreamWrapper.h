#pragma once

#include <string>

#include <QObject>

#include <nsxlib/logger/IStreamWrapper.h>

class NoteBook;

class QtStreamWrapper : public QObject, public nsx::IStreamWrapper {

    Q_OBJECT

public:

    //! Constructor
    QtStreamWrapper(NoteBook* notebook);

    //! Destructor
    virtual ~QtStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

signals:

    void sendLogMessage(const std::string& message);
};

