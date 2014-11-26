#include "PeakPlot.h"
#include "PlotFactory.h"
#include "SimplePlot.h"

PlotFactory::PlotFactory()
{
    registerCallback("simple" ,&SimplePlot::create);
    registerCallback("peak",&PeakPlot::create);
}

PlotFactory::~PlotFactory()
{
}
