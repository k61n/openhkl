#include "PlotFactory.h"
#include "PeakPlot.h"
#include "SimplePlot.h"

PlotFactory::PlotFactory() {
  registerCallback("simple", &SimplePlot::create);
  registerCallback("peak", &PeakPlot::create);
}

PlotFactory::~PlotFactory() {}
