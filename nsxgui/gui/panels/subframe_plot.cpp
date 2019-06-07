
#include "nsxgui/gui/panels/subframe_plot.h"
#include "nsxgui/gui/mainwin.h"
#include "nsxgui/gui/view/toggles.h"
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! class SubframePlot

SubframePlot::SubframePlot() : QcrDockWidget {"Plotter"}
{
    setWidget(new QTreeView);
    connect(
        this, SIGNAL(visibilityChanged(bool)), &gGui->toggles->viewPlotter, SLOT(setChecked(bool)));
}
