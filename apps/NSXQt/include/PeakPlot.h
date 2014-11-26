#ifndef PEAKPLOT_H
#define PEAKPLOT_H

#include <string>

#include "SXCustomPlot.h"

class QWidget;

class PeakPlot : public SXCustomPlot
{
    Q_OBJECT
public:

    static SXCustomPlot* create(QWidget* parent);

    explicit PeakPlot(QWidget* parent = 0);

    std::string getType() const;


};

#endif // PEAKCUSTOMPLOT_H
