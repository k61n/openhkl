#include "gui/graphics/plotfactory.h"
#include "gui/graphics/peakplot.h"
#include "gui/graphics/simpleplot.h"

PlotFactory::PlotFactory()
{
    registerCallback("simple", &SimplePlot::create);
    registerCallback("peak", &PeakPlot::create);
}

PlotFactory::~PlotFactory() {}
