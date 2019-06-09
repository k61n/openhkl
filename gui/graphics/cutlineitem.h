#ifndef GUI_GRAPHICS_CUTLINEITEM_H
#define GUI_GRAPHICS_CUTLINEITEM_H

#include "gui/graphics/cutteritem.h"

class QGraphicsSceneWheelEvent;
class QWidget;
class NSXPlot;

class CutLineItem : public CutterItem {
public:
    explicit CutLineItem(nsx::sptrDataSet data);

    ~CutLineItem();

    void plot(NSXPlot* plot);

    void wheelEvent(QGraphicsSceneWheelEvent* event);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    int getNPoints() const;

    void setNPoints(int nPoints);

    // Getters and setters
    std::string getPlotType() const;

private:
    int _nPoints;
};

#endif //GUI_GRAPHICS_CUTLINEITEM_H
