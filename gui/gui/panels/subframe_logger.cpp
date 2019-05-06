#include "gui/panels/subframe_logger.h"
#include <QTreeView>

SubframeLogger::SubframeLogger()
    : QDockWidget{"Logger"}
{
    setWidget(new QTreeView);
}
