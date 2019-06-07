#pragma once

#include <string>

#include "SXPlot.h"

class QWidget;

class PeakPlot : public SXPlot {
    Q_OBJECT
public:
    static SXPlot* create(QWidget* parent);

    explicit PeakPlot(QWidget* parent = 0);

    std::string getType() const;
};
