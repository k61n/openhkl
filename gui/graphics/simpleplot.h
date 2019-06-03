
#ifndef GUI_GRAPHICS_SIMPLEPLOT_H
#define GUI_GRAPHICS_SIMPLEPLOT_H

#include <string>
#include "gui/graphics/nsxplot.h"

class QWidget;

class SimplePlot : public NSXPlot {
    Q_OBJECT
public:
    static NSXPlot* create(QWidget* parent);

    explicit SimplePlot(QWidget* parent = 0);

    // Getters and setters
    std::string getType() const;
};

#endif //GUI_GRAPHICS_SIMPLEPLOT_H
