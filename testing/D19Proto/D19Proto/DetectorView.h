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

    DetectorView(QWidget* parent): QGraphicsView(parent), _ptrData(nullptr)
    {

    }
    void setDimensions(int hor,int vert)
    {
        pixels_h=hor;
        pixels_v=vert;
    }

    void setCurrentData(Data* ptr)
    {
        _ptrData=ptr;
    }



protected:
    void mouseMoveEvent(QMouseEvent* event)
    {
        std::ostringstream os;
        double posx=static_cast<double>(event->x())/this->width()*pixels_h;
        double posy=static_cast<double>(event->y())/this->height()*pixels_v;

        os << "(" << posx << "," << posy << ") \n";
        int count=0;
        if (_ptrData)
            count=_ptrData->_frames[static_cast<int>(posx)*256+static_cast<int>(posy)];
        os << "I: " << count;
        QToolTip::showText(event->globalPos(),QString::fromStdString(os.str()),this,QRect());
    }
    void mousePressEvent(QMouseEvent* event)
    {
    }

private:
    // Pointer to Data
    Data* _ptrData;
    int pixels_h;
    int pixels_v;
};

#endif // DETECTORVIEW_H
