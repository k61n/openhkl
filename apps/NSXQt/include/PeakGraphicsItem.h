#ifndef PEAKGRAPHICSITEM_H
#define PEAKGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPen>

namespace SX
{
    namespace Crystal
    {
            class Peak3D;
    }
}
class PeakGraphicsItem : public QGraphicsItem
{
public:
    PeakGraphicsItem(SX::Crystal::Peak3D* p);
    ~PeakGraphicsItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setFrame(int);
    static void showLabel(bool);
    SX::Crystal::Peak3D* getPeak();
private:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    SX::Crystal::Peak3D* _peak;
    QPen _pen;
    //! Whether this peak is visible or not depends on frame
    bool _isVisible;
    QGraphicsTextItem* _hklText;
    //!
    static bool _labelVisible;
    bool _hoverOn;
};

#endif // PEAKGRAPHICSITEM_H
