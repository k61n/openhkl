#pragma once

#include <nsxlib/logger/IStreamWrapper.h>

class QTextEdit;

class QtStreamWrapper : public nsx::IStreamWrapper {
public:

    //! Constructor
    QtStreamWrapper(QTextEdit* stream);

    //! Destructor
    virtual ~QtStreamWrapper();

    //! Write a message to the stream
    virtual void print(const std::string& message) override;

private:

    // The widget that will receive string messages
    QTextEdit* _notebook;
};
