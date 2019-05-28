
#ifndef SUBFRAME_LOGGER_H
#define SUBFRAME_LOGGER_H

#include <QCR/widgets/views.h>
#include <QTextEdit>

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframeLogger : public QcrDockWidget {
    Q_OBJECT
public:
    SubframeLogger();

private slots:
    void slotPrintLog(const QString& line);

private:
    QTextEdit* logText;
};

#endif // SUBFRAME_LOGGER_H
