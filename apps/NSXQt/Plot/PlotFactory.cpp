#include "Plot/PeakPlot.h"
#include "Plot/PlotFactory.h"
#include "Plot/SimplePlot.h"

PlotFactory::PlotFactory()
{
    registerCallback("simple" ,&SimplePlot::create);
    registerCallback("peak",&PeakPlot::create);
}

PlotFactory::~PlotFactory()
{
}
