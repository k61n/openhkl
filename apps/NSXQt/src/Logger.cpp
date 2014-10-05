#include "Logger.h"
#include <QCoreApplication>
#include <QTextEdit>
#include <QScrollBar>

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
    Q_UNUSED(context);
    QTextEdit* log=Logger::Instance()->getNoteBook();
    switch (type) {
    case QtDebugMsg:
        txt = QString("%1").arg(msg);
        log->setTextColor("darkBlue");
        log->setFontItalic(true);
        break;
    case QtWarningMsg:
        txt = QString("%1").arg(msg);
        log->setTextColor("orange");
        log->setFontUnderline(true);
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
    QCoreApplication::processEvents();

    // Reinitialze the text color and font
    log->setTextColor(QColor("black"));
    log->setFontItalic(false);
    log->setFontUnderline(false);
    // Ensure that textEdit is at the bottom
    log->verticalScrollBar()->setValue(log->verticalScrollBar()->maximum());

}
