#ifndef NSXQT_CUTLINEGRAPHICSITEM_H
#define NSXQT_CUTLINEGRAPHICSITEM_H

#include <string>

#include <QGraphicsItem>

#include <nsxlib/DataTypes.h>

#include "CutterGraphicsItem.h"

class QGraphicsSceneWheelEvent;
class QWidget;
class SXPlot;

class CutLineGraphicsItem : public CutterGraphicsItem
{
public:

    explicit CutLineGraphicsItem(nsx::sptrDataSet data);

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

#endif // NSXQT_CUTLINEGRAPHICSITEM_H
