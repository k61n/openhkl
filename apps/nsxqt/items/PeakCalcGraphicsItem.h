#ifndef NSXQT_PEAKCALCGRAPHICSITEM_H
#define NSXQT_PEAKCALCGRAPHICSITEM_H

#include "SXGraphicsItem.h"

#include <nsxlib/crystal/PeakCalc.h>

class PeakCalcGraphicsItem : public SXGraphicsItem
{
public:
    PeakCalcGraphicsItem(nsx::PeakCalc p);
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
     void setFrame(unsigned long frame);
     static void setLabelVisible(bool flag=true);
     virtual QRectF boundingRect() const;
private:
     double _x, _y, _frame;
      static bool _labelVisible;
};

#endif // NSXQT_PEAKCALCGRAPHICSITEM_H
