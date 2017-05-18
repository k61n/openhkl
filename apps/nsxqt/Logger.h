#ifndef NSXQT_LOGGER_H
#define NSXQT_LOGGER_H

#include <QtMessageHandler>
#include <QString>
#include <sstream>

#include <nsxlib/kernel/Singleton.h>

// Forwards
class QTextEdit;

// Function to handle calls from qDebug(), qWarning(), qCritical() and qFatal()
void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);


// Hack to ensure that any object for which operator<< to a stream is defined, can also be directed to a QDebug
template<class T>
QDebug& operator<<(QDebug& dbg, const T& object)
{
    std::ostringstream os;
    os << object;
    dbg << QString::fromStdString(os.str());
    return dbg;
}

// A singleton Logger class to log all applications output
class Logger : public nsx::Singleton<Logger,nsx::Constructor,nsx::Destructor>
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

#endif // NSXQT_LOGGER_H
