#ifndef GUI_GRAPHICS_NSXPLOT_H
#define GUI_GRAPHICS_NSXPLOT_H

#include <string>
#include "3rdparty/QCustomPlot/QCustomPlot.h"

class PlottableItem;

class NSXPlot : public QCustomPlot {
    Q_OBJECT
public:
    static NSXPlot* create(QWidget* parent);

    explicit NSXPlot(QWidget* parent = 0);
    void keyPressEvent(QKeyEvent*);
    virtual ~NSXPlot();

    // Getters and setters

    virtual std::string getType() const;

public slots:
    //
    void mousePress();
    //
    void mouseWheel();
    // Update the plot
    virtual void update(PlottableItem* item);
    void titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title);
    // Double click on the legend
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    // Context menu
    void setmenuRequested(QPoint);
    // Export the graphs into column ASCII format
    void exportToAscii();

private:
    void copyViewToClipboard();
};

#endif //GUI_GRAPHICS_NSXPLOT_H
