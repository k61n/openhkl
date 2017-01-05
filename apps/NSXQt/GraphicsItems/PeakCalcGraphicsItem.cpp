#include "GraphicsItems/PeakCalcGraphicsItem.h"
#include <QPainter>
#include "PeakCalc.h"

bool PeakCalcGraphicsItem::_labelVisible=false;

PeakCalcGraphicsItem::PeakCalcGraphicsItem(SX::Crystal::PeakCalc p):SXGraphicsItem(),_x(p._x), _y(p._y), _frame(p._frame)
{
    _label=new QGraphicsTextItem(this);
    //Ensure text is alwyas real size despite zoom
    _label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _label->setParentItem(this);
    QString hkl;
    hkl=QString("%1,%2,%3").arg(p._h).arg(p._k).arg(p._l);
    _label->setPlainText(hkl);
    _label->setDefaultTextColor(QColor("#FF3300"));
    setPos(_x,_y);

    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
    _pen.setColor(QColor("#FF3300"));
}

void PeakCalcGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    if (!isVisible())
        return;

    _label->setVisible(_labelVisible);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(_pen);
    painter->drawEllipse(-2,-2,4,4);
    _label->setPos(2,0);
}

void PeakCalcGraphicsItem::setFrame(unsigned long frame)
{
    if (frame>=_frame-1 && frame<=_frame+1) {
        setVisible(true);
        _label->setVisible(_labelVisible);
    } else {
        setVisible(false);
        _label->setVisible(false);
    }
}

void PeakCalcGraphicsItem::setLabelVisible(bool flag)
{
    _labelVisible = flag;
}

QRectF PeakCalcGraphicsItem::boundingRect() const
{
    return QRectF(-1, -1, 2, 2);
}

