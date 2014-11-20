#ifndef LINECUTTERCUSTOMPLOT_H
#define LINECUTTERCUSTOMPLOT_H

#include "CutterCustomPlot.h"

class PlottableGraphicsItem;

class LineCutterCustomPlot : public CutterCustomPlot
{
    Q_OBJECT

public:
    explicit LineCutterCustomPlot(QWidget *parent=0);
    virtual CutterCustomPlot* create(QWidget *parent=0);

public slots:
    void update(PlottableGraphicsItem* cutter);

private:
    int _nPoints;

};

#endif // LINECUTTERCUSTOMPLOT_H
