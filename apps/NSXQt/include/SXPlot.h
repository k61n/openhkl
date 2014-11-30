#ifndef SXPLOT_H
#define SXPLOT_H

#include <string>

#include <qcustomplot.h>

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
    void mousePress();
    void mouseWheel();
    virtual void update(PlottableGraphicsItem* item);
    void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
private:
    void copyViewToClipboard();

};

#endif // SXCPLOT_H
