#ifndef LOGGER_H
#define LOGGER_H
#include <QtMessageHandler>
#include "Singleton.h"

// Forwards
class QTextEdit;

// Function to handle calls from qDebug(), qWarning(), qCritical() and qFatal()
void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

// A singleton Logger class to log all applications output
class Logger : public SX::Kernel::Singleton<Logger,SX::Kernel::Constructor,SX::Kernel::Destructor>
{
public:
    // Attach a QTextEdit to the logger
    void setNoteBook(QTextEdit* editor);
    // Return the current text editor
    QTextEdit* getNoteBook();
private:
    // The notebook that will receive string messages
    QTextEdit* _notebook;

};


#endif // LOGGER_H
