#ifndef GUI_GRAPHICS_PEAKPLOT_H
#define GUI_GRAPHICS_PEAKPLOT_H

#include "gui/graphics/nsxplot.h"

class QWidget;

class XPeakPlot : public NSXPlot {
    Q_OBJECT
public:
    static NSXPlot* create(QWidget* parent);

    explicit XPeakPlot(QWidget* parent = 0);

    std::string getType() const;
};


#endif //GUI_GRAPHICS_PEAKPLOT_H
