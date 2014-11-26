#include "SliceGraphicsItem.h"
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <iostream>
#include <QDrag>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QWidget>
#include <QGraphicsSceneWheelEvent>

#include "Detector.h"
#include "IData.h"
#include "DetectorScene.h"
#include "SliceCutterCustomPlot.h"
#include "SXCustomPlot.h"
#include "SimplePlot.h"

SliceGraphicsItem::SliceGraphicsItem(SX::Data::IData* data, bool horizontal) : CutterGraphicsItem(data), _horizontal(horizontal)
{
}

SliceGraphicsItem::~SliceGraphicsItem()
{
}

void SliceGraphicsItem::plot(SXCustomPlot* plot)
{
    auto p=dynamic_cast<SimplePlot*>(plot);
    if (!p)
        return;


    // Set the pointer to the detector scene to the scene that holds the cutter
    auto detPtr=dynamic_cast<DetectorScene*>(scene());
    if (!detPtr)
        return;

    SX::Data::IData* dataPtr=detPtr->getData();
    SX::Instrument::Detector* det=dataPtr->getDiffractometer()->getDetector();

    // Define the position on the scene of the cutter
    int xmin = pos().x()+boundingRect().left();
    int xmax = pos().x()+boundingRect().right();
    int ymin = pos().y()+boundingRect().top();
    int ymax = pos().y()+boundingRect().bottom();
    int nRows=det->getNRows();
    int nCols=det->getNCols();
    xmin = std::max(xmin,0);
    xmax = std::min(xmax,nCols);
    ymin = std::max(ymin,0);
    ymax = std::min(ymax,nRows);

    int length;
    int start;

    bool horizontal=isHorizontal();

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

    int z=0;
    std::generate(x.begin(),x.end(),[&z](){return z++;});
//    for (int i=0;i<length;++i)
//        x[i]=start+i;

//    const std::vector<int>& currentFrame=detPtr->getCurrentFrame();

//    for (int i=xmin;i<xmax;++i)
//    {
//        for (int j=ymin;j<ymax;++j)
//        {
//            int idx=horizontal?i:j;
//            idx -=start;
//            y[idx] += currentFrame[i*nRows+j];
//        }
//    }
//    std::transform(y.begin(),y.end(),e.begin(),[](double p){ return sqrt(p);});

    p->graph(0)->setDataValueError(x, y, e);
    p->rescaleAxes();

    p->replot(QCustomPlot::rpImmediate);

}


//SXCustomPlot* SliceGraphicsItem::createPlot(QWidget *parent)
//{
//    return new SliceCutterCustomPlot(parent);
//}

bool SliceGraphicsItem::isHorizontal() const
{
    return _horizontal;
}

void SliceGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget)
{
    Q_UNUSED(widget);

    // Color depending on selection
    if (option->state & QStyle::State_Selected)
    {
       _pen.setStyle(Qt::DashLine);
       painter->setBrush(QBrush(QColor(0,255,0,5)));
    }
    else
    {
       _pen.setStyle(Qt::SolidLine);
       _pen.setColor(QColor("black"));
    }

   painter->setRenderHint(QPainter::HighQualityAntialiasing);
   painter->setPen(_pen);
   qreal w=_to.x()-_from.x();
   qreal h=_to.y()-_from.y();
   painter->drawRect(-w/2.0,-h/2.0,w,h);

   if (_horizontal)
   {
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0+5,-h/2.0+2);
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0+5,-h/2.0-2);
       painter->drawLine(w/2.0,-h/2.0,w/2.0-5,-h/2.0+2);
       painter->drawLine(w/2.0,-h/2.0,w/2.0-5,-h/2.0-2);
   }
   else
   {
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0+2,-h/2.0+5);
       painter->drawLine(-w/2.0,-h/2.0,-w/2.0-2,-h/2.0+5);
       painter->drawLine(-w/2.0, h/2.0,-w/2.0+2, h/2.0-5);
       painter->drawLine(-w/2.0, h/2.0,-w/2.0-2, h/2.0-5);
   }
}

std::string SliceGraphicsItem::getPlotType() const
{
    return "simple";
}

void SliceGraphicsItem::wheelEvent(QGraphicsSceneWheelEvent *event)
{

    if (!isVisible())
        return;

    if (!isSelected())
        return;

    int step=event->delta()/120;

    if (_horizontal)
    {
        _from += QPointF(0,-step);
        _to += QPointF(0,step);
    }
    else
    {
        _from += QPointF(-step,0);
        _to += QPointF(step,0);
    }
    update();
}



