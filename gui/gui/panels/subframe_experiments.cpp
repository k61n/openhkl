
#include "gui/panels/subframe_experiments.h"
#include "gui/view/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QToolButton>

//  ***********************************************************************************************
//! @class SubframeFiles

SubframeExperiments::SubframeExperiments()
    : QDockWidget{"experiments"}
{
    setFeatures(DockWidgetMovable);
    setWindowTitle("Experiments");

    setWidget(new QTreeView);

}
