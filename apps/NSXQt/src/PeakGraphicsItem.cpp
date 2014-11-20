#include <Eigen/Dense>

#include <QPainter>
#include <QtDebug>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include "PeakGraphicsItem.h"
#include "Peak3D.h"
#include "SXCustomPlot.h"
#include "PeakCustomPlot.h"

PeakGraphicsItem::PeakGraphicsItem(SX::Crystal::Peak3D* p) : PlottableGraphicsItem(nullptr), _peak(p)
{
    if (_peak)
    {
        Eigen::Vector3d c=_peak->getPeak()->getCenter();
        setPos(c[0],c[1]);
    }
    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
    _pen.setColor(QColor(0,0,255,255));
    setFlags(QGraphicsItem::ItemIsSelectable);

    QString hkl;
    _label=new QGraphicsTextItem(this);
    _label->setFlag(QGraphicsItem::ItemIgnoresTransformations); //Ensure text is alwyas real size despite zoom
    _label->setParentItem(this);
    setBoundingRegionGranularity(0.0);

    setMovable(false);
}

PeakGraphicsItem::~PeakGraphicsItem()
{
}

SXCustomPlot* PeakGraphicsItem::createPlot(QWidget *parent)
{
    return new PeakCustomPlot(parent);
}

QRectF PeakGraphicsItem::boundingRect() const
{
    const Eigen::Vector3d& l=_peak->getPeak()->getLower();
    const Eigen::Vector3d& u=_peak->getPeak()->getUpper();
    qreal w=u[0]-l[0];
    qreal h=u[1]-l[1];
    return QRectF(-w/2.0-1,-h/2.0-1,w+2,h+2);
}

void PeakGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (!isVisible())
        return;

    if (option->state & QStyle::State_Selected)
        _pen.setStyle(Qt::DotLine);
    else
        _pen.setStyle(Qt::SolidLine);

    painter->setRenderHint(QPainter::Antialiasing);

    if (_hover)
        painter->setBrush(QBrush(QColor(255,255,0,120)));
    _label->setVisible(_hover);

    if (_peak->isSelected())
        _pen.setColor("green");
    else
        _pen.setColor("red");

    painter->setPen(_pen);
    const Eigen::Vector3d& l=_peak->getPeak()->getLower();
    const Eigen::Vector3d& u=_peak->getPeak()->getUpper();
    qreal w=u[0]-l[0];
    qreal h=u[1]-l[1];
    painter->drawRect(-w/2,-h/2,w,h);
    _label->setPos(w/2,-h/2);
}

void PeakGraphicsItem::setFrame(int frame)
{
    const Eigen::Vector3d& l=_peak->getPeak()->getLower();
    const Eigen::Vector3d& u=_peak->getPeak()->getUpper();
    if (frame>=l[2] && frame<=u[2])
    {
        setVisible(true);
        _label->setVisible(true);
        auto& v=_peak->getMillerIndices();
        QString hkl;
        hkl=QString("%1,%2,%3").arg(v[0]).arg(v[1]).arg(v[2]);
        _label->setPlainText(hkl);
    }
    else
    {
        setVisible(false);
        _label->setVisible(false);
    }
}

SX::Crystal::Peak3D* PeakGraphicsItem::getPeak()
{
    return _peak;
}

void PeakGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    _hover=true;
    setCursor(QCursor(Qt::PointingHandCursor));
    update();

}

void PeakGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    _hover=false;
    setCursor(QCursor(Qt::CrossCursor));
    update();
}
