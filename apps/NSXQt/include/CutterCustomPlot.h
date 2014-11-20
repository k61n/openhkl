#ifndef CUTTERCUSTOMPLOT_H
#define CUTTERCUSTOMPLOT_H

#include "SXCustomPlot.h"
#include "CutterGraphicsItem.h"

class PlottableGraphicsItem;
class DetectorScene;

class CutterCustomPlot : public SXCustomPlot
{
    Q_OBJECT
public:
    explicit CutterCustomPlot(QWidget *parent = 0);
    virtual CutterCustomPlot* create(QWidget *parent=0)=0;
    virtual ~CutterCustomPlot()=0;

public slots:
    virtual void update(PlottableGraphicsItem* cutter)=0;

};

#endif // CUTTERCUSTOMPLOT_H
