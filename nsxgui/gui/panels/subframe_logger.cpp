#include "nsxgui/gui/panels/subframe_logger.h"
#include "nsxgui/qcr/engine/logger.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/view/toggles.h"
#include <QTreeView>

SubframeLogger::SubframeLogger()
    : QcrDockWidget{"Logger"}
{
    setWidget(gLoggerText);
    connect(this, SIGNAL( visibilityChanged(bool) ), &gGui->toggles->viewLogger ,
            SLOT( setChecked(bool)) );
}
