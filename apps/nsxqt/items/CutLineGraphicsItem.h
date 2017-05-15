#ifndef CUTLINEGRAPHICSITEM_H
#define CUTLINEGRAPHICSITEM_H

#include <string>
#include <memory>

#include <QGraphicsItem>
#include "items/CutterGraphicsItem.h"

namespace nsx
{
namespace Data
{
class DataSet;
}
}

class QGraphicsSceneWheelEvent;
class QWidget;
class SXPlot;

class CutLineGraphicsItem : public CutterGraphicsItem
{
public:

    explicit CutLineGraphicsItem(std::shared_ptr<nsx::Data::DataSet>);

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
