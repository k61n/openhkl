#ifndef DETECTORVIEW_H
#define DETECTORVIEW_H
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>

class DetectorView : public QGraphicsView
{
public:

    DetectorView(QWidget* parent): QGraphicsView(parent)
    {

    }
    void setDimensions(int hor,int vert)
    {
        pixels_h=hor;
        pixels_v=vert;
    }


protected:
    void mouseMoveEvent(QMouseEvent* event)
    {
        std::ostringstream os;
        os << "(" << static_cast<double>(event->x())/this->width()*pixels_h << "," << static_cast<double>(event->y())/this->height()*pixels_v << ")";
        QToolTip::showText(event->globalPos(),QString::fromStdString(os.str()),this,QRect());
    }
    void mousePressEvent(QMouseEvent* event)
    {
    }

private:
    int pixels_h;
    int pixels_v;
};

#endif // DETECTORVIEW_H
