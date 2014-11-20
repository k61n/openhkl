#include <vector>

#include <QPointF>
#include <QLineF>

#include "PlottableGraphicsItem.h"
#include "LineCutterCustomPlot.h"
#include "LineCutGraphicsItem.h"
#include "Detector.h"
#include "DetectorScene.h"
#include "IData.h"

LineCutterCustomPlot::LineCutterCustomPlot(QWidget *parent) : CutterCustomPlot(parent)
{
}

CutterCustomPlot* LineCutterCustomPlot::create(QWidget *parent)
{
    return new LineCutterCustomPlot(parent);
}

void LineCutterCustomPlot::update(PlottableGraphicsItem* cutter)
{

    auto cutterPtr=dynamic_cast<LineCutGraphicsItem*>(cutter);
    if (!cutterPtr)
        return;

    // Set the pointer to the detector scene to the scene that holds the cutter
    auto detPtr=dynamic_cast<DetectorScene*>(cutterPtr->scene());
    if (!detPtr)
        return;

    int nPoints = cutterPtr->getNPoints();

    QVector<double> x(nPoints);
    QVector<double> y(nPoints);
    QVector<double> e(nPoints);

    QLineF line;
    line.setP1(cutterPtr->pos()+cutterPtr->boundingRect().bottomLeft());
    line.setP2(cutterPtr->pos()+cutterPtr->boundingRect().topRight());

    SX::Data::IData* dataPtr=detPtr->getData();
    SX::Instrument::Detector* det=dataPtr->getDiffractometer()->getDetector();

    int nRows=det->getNRows();

    const std::vector<int>& currentFrame=detPtr->getCurrentFrame();

    for (int i=0; i<nPoints; ++i)
    {
        x[i] = i;

        QPointF point=line.pointAt(i/static_cast<double>(nPoints));

        int ipx=static_cast<int>(point.x());

        int ipy=static_cast<int>(point.y());

        QPoint lowestCorner=QPoint(ipx,ipy);

        double sdist2 = 0.0;

        for (int pi=0;pi<2;++pi)
        {
            for (int pj=0;pj<2;++pj)
            {
                QPoint currentCorner = lowestCorner + QPoint(pi,pj);
                QPointF dp = point - currentCorner;
                double dist2 = dp.x()*dp.x() + dp.y()*dp.y();
                int count=currentFrame[currentCorner.x()*nRows+currentCorner.y()];
                y[i] += dist2*count;
                sdist2 += dist2;
            }
        }
        y[i] /= sdist2;
        e[i] = sqrt(y[i]);
    }

    graph(0)->setDataValueError(x, y, e);
    rescaleAxes();

    replot();
}
