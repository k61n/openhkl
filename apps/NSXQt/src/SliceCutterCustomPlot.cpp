#include <iostream>
#include <vector>

#include <QPointF>

#include "PlottableGraphicsItem.h"
#include "Detector.h"
#include "DetectorScene.h"
#include "IData.h"
#include "SliceCutterCustomPlot.h"
#include "SliceGraphicsItem.h"

SliceCutterCustomPlot::SliceCutterCustomPlot(QWidget *parent) : CutterCustomPlot(parent)
{
}

CutterCustomPlot* SliceCutterCustomPlot::create(QWidget *parent)
{
    return new SliceCutterCustomPlot(parent);
}

void SliceCutterCustomPlot::update(PlottableGraphicsItem* cutter)
{

    auto cutterPtr=dynamic_cast<SliceGraphicsItem*>(cutter);
    if (!cutterPtr)
        return;

    // Set the pointer to the detector scene to the scene that holds the cutter
    auto detPtr=dynamic_cast<DetectorScene*>(cutterPtr->scene());
    if (!detPtr)
        return;

    SX::Data::IData* dataPtr=detPtr->getData();
    SX::Instrument::Detector* det=dataPtr->getDiffractometer()->getDetector();

    QPointF pos=cutterPtr->pos();

    // Define the position on the scene of the cutter
    int xmin = pos.x()+cutterPtr->boundingRect().left();
    int xmax = pos.x()+cutterPtr->boundingRect().right();
    int ymin = pos.y()+cutterPtr->boundingRect().top();
    int ymax = pos.y()+cutterPtr->boundingRect().bottom();
    int nRows=det->getNRows();
    int nCols=det->getNCols();
    xmin = std::max(xmin,0);
    xmax = std::min(xmax,nCols);
    ymin = std::max(ymin,0);
    ymax = std::min(ymax,nRows);

    int length;
    int start;

    bool horizontal=cutterPtr->isHorizontal();

    if (horizontal)
    {
        length=xmax-xmin;
        start=xmin;
    }
    else
    {
        length=ymax-ymin;
        start=ymin;
    }

    QVector<double> x(length);
    QVector<double> y(length);
    QVector<double> e(length);

    for (int i=0;i<length;++i)
        x[i]=start+i;

    const std::vector<int>& currentFrame=detPtr->getCurrentFrame();

    for (int i=xmin;i<xmax;++i)
    {
        for (int j=ymin;j<ymax;++j)
        {
            int idx=horizontal?i:j;
            idx -=start;
            y[idx] += currentFrame[i*nRows+j];
            e[idx]  = sqrt(y[idx]);
        }
    }

    graph(0)->setDataValueError(x, y, e);
    rescaleAxes();

    replot();
}
