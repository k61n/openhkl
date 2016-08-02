#ifndef CUTLINEGRAPHICSITEM_H
#define CUTLINEGRAPHICSITEM_H

#include <string>
#include <memory>

#include <QGraphicsItem>
#include <GraphicsItems/CutterGraphicsItem.h>

namespace SX
{
namespace Data
{
class IData;
}
}

class QGraphicsSceneWheelEvent;
class QWidget;
class SXPlot;

class CutLineGraphicsItem : public CutterGraphicsItem
{
public:

    explicit CutLineGraphicsItem(std::shared_ptr<SX::Data::IData>);

    ~CutLineGraphicsItem();

    void plot(SXPlot* plot);

    void wheelEvent(QGraphicsSceneWheelEvent* event);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int getNPoints() const;

    void setNPoints(int nPoints);

    // Getters and setters
    std::string getPlotType() const;


private:
    int _nPoints;

};

#endif // CUTLINEGRAPHICSITEM_H
