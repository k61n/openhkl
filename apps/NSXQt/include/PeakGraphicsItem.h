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
    //! Return the peak Object
    SX::Crystal::Peak3D* getPeak();
private:
    //! Turn _hoverOn true
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    //! Turn _hoverOn false
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    //! Pointer to the Peak3D object
    SX::Crystal::Peak3D* _peak;
    //! Pen
    QPen _pen;
    //! Text child object that contains hkl label
    QGraphicsTextItem* _hklText;
    //! True when label of peak is visible
    static bool _labelVisible;
    //! True when object is hover by MouseEvent
    bool _hoverOn;
};

#endif // PEAKGRAPHICSITEM_H
