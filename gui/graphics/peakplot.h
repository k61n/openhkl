#ifndef GUI_GRAPHICS_PEAKPLOT_H
#define GUI_GRAPHICS_PEAKPLOT_H

#include "gui/graphics/nsxplot.h"

class QWidget;

class PeakPlot : public NSXPlot {
    Q_OBJECT
public:
    static NSXPlot* create(QWidget* parent);

    explicit PeakPlot(QWidget* parent = 0);

    std::string getType() const;
};


#endif //GUI_GRAPHICS_PEAKPLOT_H
