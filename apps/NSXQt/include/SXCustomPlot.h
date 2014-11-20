#ifndef SXCUSTOMPLOT_H
#define SXCUSTOMPLOT_H

#include <qcustomplot.h>

class PlottableGraphicsItem;

class SXCustomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit SXCustomPlot(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *);
    virtual ~SXCustomPlot();

public slots:
    void mousePress(QMouseEvent *event);
    void mouseWheel(QWheelEvent *event);
    virtual void update(PlottableGraphicsItem* item)=0;

private:
    void copyViewToClipboard();

};

#endif // SXCUSTOMPLOT_H
