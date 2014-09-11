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
    switch (type) {
    case QtDebugMsg:
        txt = QString("%1").arg(msg);
        Logger::Instance()->getNoteBook()->setTextColor("black");
        break;
    case QtWarningMsg:
        txt = QString("%1 %2 @line %3").arg(msg,context.file, QString::number(context.line));
        Logger::Instance()->getNoteBook()->setTextColor("orange");
        break;
    case QtCriticalMsg:
        txt = QString("%1").arg(msg);
        Logger::Instance()->getNoteBook()->setTextColor("red");
        break;
    case QtFatalMsg:
        abort();
    }
    Logger::Instance()->getNoteBook()->setFontItalic(true);
    Logger::Instance()->getNoteBook()->append(txt);
    QCoreApplication::processEvents();
}
