#include "PeakPlot.h"

SXCustomPlot* PeakPlot::create(QWidget *parent)
{
    return new PeakPlot(parent);
}

PeakPlot::PeakPlot(QWidget *parent) : SXCustomPlot(parent)
{
}

std::string PeakPlot::getType() const
{
    return "peak";
}
