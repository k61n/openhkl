#ifndef PEAKCALCGRAPHICSITEM_H
#define PEAKCALCGRAPHICSITEM_H
#include "SXGraphicsItem.h"

namespace nsx{
    namespace Crystal{
        struct PeakCalc;
    }
}

class PeakCalcGraphicsItem : public SXGraphicsItem
{
public:
    PeakCalcGraphicsItem(nsx::Crystal::PeakCalc);
     void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
     void setFrame(unsigned long frame);
     static void setLabelVisible(bool flag=true);
     virtual QRectF boundingRect() const;
private:
     double _x, _y, _frame;
      static bool _labelVisible;
};

#endif // PEAKCALCGRAPHICSITEM_H
