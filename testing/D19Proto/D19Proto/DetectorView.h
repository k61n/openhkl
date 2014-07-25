#ifndef DETECTORVIEW_H
#define DETECTORVIEW_H
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"



class DetectorView : public QGraphicsView
{
public:

    DetectorView(QWidget* parent);
    void setDimensions(int hor,int vert);

public slots:
    void updateView(Data* ptr,int frame, double colormax);

protected:
    void mouseMoveEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
private:
    // Pointer to Data
    Data* _ptrData;
    int pixels_h;
    int pixels_v;
    QGraphicsScene* _scene;
};

#endif // DETECTORVIEW_H
