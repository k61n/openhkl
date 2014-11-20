#ifndef PEAKCUSTOMPLOT_H
#define PEAKCUSTOMPLOT_H

#include "SXCustomPlot.h"

namespace SX
{
namespace Crystal
{
class Peak3D;
}
}

class PlottableGraphicsItem;

class PeakCustomPlot : public SXCustomPlot
{
    Q_OBJECT
public:
    explicit PeakCustomPlot(QWidget *parent = 0);

signals:

public slots:
    void update(PlottableGraphicsItem* item);
    void setPeak(SX::Crystal::Peak3D*);
    void mousePress(QMouseEvent *event);
    void mouseWheel(QWheelEvent *event);
private:
    SX::Crystal::Peak3D *_current;
};

#endif // PEAKCUSTOMPLOT_H
