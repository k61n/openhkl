
#include "gui/panels/subframe_plot.h"
#include <QTreeView>

//-------------------------------------------------------------------------------------------------
//! class SubframePlot

SubframePlot::SubframePlot()
    :QDockWidget{"Plotter"}
{
    setWidget(new QTreeView);
}
