#include "nsxgui/gui/panels/subframe_logger.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/view/toggles.h"
#include <QCR/engine/logger.h>
#include <QTreeView>

SubframeLogger::SubframeLogger() : QcrDockWidget {"Logger"}
{
    logText = new QTextEdit;
    setWidget(logText);
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewLogger, SLOT(setChecked(bool)));
    connect(gLogger, SIGNAL(sigLine(QString)), this, SLOT(slotPrintLog(QString)));
}

void SubframeLogger::slotPrintLog(const QString& line)
{
    logText->append(line);
}
