#ifndef NSXQT_SXPLOT_H
#define NSXQT_SXPLOT_H

#include <string>

#include "externals/qcustomplot.h"

class PlottableGraphicsItem;

class SXPlot : public QCustomPlot
{
    Q_OBJECT
public:

    static SXPlot* create(QWidget* parent);

    explicit SXPlot(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *);
    virtual ~SXPlot();

    // Getters and setters

    virtual std::string getType() const;

public slots:
    //
    void mousePress();
    //
    void mouseWheel();
    // Update the plot
    virtual void update(PlottableGraphicsItem* item);
    void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
    // Double click on the legend
    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
    // Context menu
    void setmenuRequested(QPoint);
    // Export the graphs into column ASCII format
    void exportToAscii();
private:
    void copyViewToClipboard();

};

#endif // NSXQT_SXPLOT_H
