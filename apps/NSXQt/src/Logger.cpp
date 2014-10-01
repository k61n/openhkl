#include "Logger.h"
#include <QCoreApplication>
#include <QTextEdit>

void Logger::setNoteBook(QTextEdit *nb)
{
    _notebook = nb;
}

QTextEdit* Logger::getNoteBook()
{
    return _notebook;
}


void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString txt;

    QTextEdit* log=Logger::Instance()->getNoteBook();
    switch (type) {
    case QtDebugMsg:
        txt = QString("%1").arg(msg);
        log->setTextColor("darkBlue");
        log->setFontItalic(true);
        break;
    case QtWarningMsg:
        txt = QString("%1 %2 @line %3").arg(msg,context.file, QString::number(context.line));
        log->setTextColor("orange");
        break;
    case QtCriticalMsg:
        txt = QString("%1").arg(msg);
        log->setTextColor("red");
        break;
    case QtFatalMsg:
        abort();
    }
    log->append(txt);
    // Return to black color for user input in the notebook
    log->setTextColor(QColor("black"));
    log->setFontItalic(false);
    QCoreApplication::processEvents();

}
