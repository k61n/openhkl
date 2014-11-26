#ifndef SLICEGRAPHICSITEM_H
#define SLICEGRAPHICSITEM_H

#include <string>

#include <CutterGraphicsItem.h>

// Forward declarations
namespace SX
{
namespace Data
{
class IData;
}
}
class SXCustomPlot;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneWheelEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class SliceGraphicsItem : public CutterGraphicsItem
{
public:

    // Constructors and destructor

    //! Construct a data slicer
    SliceGraphicsItem(SX::Data::IData*, bool horizontal=true);
    //! The destructor
    ~SliceGraphicsItem();

    // Events

    //! Handles a mouse wheel event
    void wheelEvent(QGraphicsSceneWheelEvent* event);


    // Getters and setters
    std::string getPlotType() const;

    // Other methods

    void plot(SXCustomPlot*);
//    //! Returns a slice plot
//    SXCustomPlot* createPlot(QWidget* parent=0);
    //! Return true if the slice is horizontal.
    bool isHorizontal() const;
    //! Paint the slice
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    //! Horizontal or vertical integration
    bool _horizontal;
};

#endif // SLICEGRAPHICSITEM_H
