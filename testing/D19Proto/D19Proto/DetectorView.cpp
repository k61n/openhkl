#include "DetectorView.h"
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"
#include "ColorMap.h"


    DetectorView::DetectorView(QWidget* parent): QGraphicsView(parent), _ptrData(nullptr), _scene(new QGraphicsScene(this))
    {

    }
    void DetectorView::setDimensions(int hor,int vert)
    {
        pixels_h=hor;
        pixels_v=vert;
    }


    void DetectorView::updateView(Data* ptr,int frame, double colormax)
    {
        _ptrData=ptr;
        // Replot the detector image
        if (_ptrData)
        {
            _ptrData->readBlock(frame);
            QImage image=QImage(Mat2QImage(&(_ptrData->_currentFrame[0]),256,640,colormax));
            QPixmap pix=QPixmap::fromImage(image);
            pix=pix.scaled(width(),height(),Qt::IgnoreAspectRatio);
            _scene->clear();
            _scene->addPixmap(pix);
            setScene(_scene);
        }
        else
        {
            _scene->clear();
        }
    }

    void DetectorView::mouseMoveEvent(QMouseEvent* event)
    {
        std::ostringstream os;
        double posx=static_cast<double>(event->x())/this->width()*pixels_h;
        double posy=static_cast<double>(event->y())/this->height()*pixels_v;

        os << "(" << posx << "," << posy << ") \n";
        int count=0;
        if (_ptrData)
            count=_ptrData->_currentFrame[static_cast<int>(posx)*256+static_cast<int>(posy)];
        os << "I: " << count;


        if (_ptrData)
        {
            SX::Data::MetaData* meta=_ptrData->mm->getMetaData();
            double th2center=meta->getKey<double>("2theta(gamma)");
            os << "\n" << th2center+60.0-posx/640.0*120.0 ;
        }

        QToolTip::showText(event->globalPos(),QString::fromStdString(os.str()),this,QRect());
    }
    void DetectorView::mouseDoubleClickEvent(QMouseEvent* event)
    {
        QGraphicsScene* scene=this->scene();
        scene->addRect(event->x()-6,event->y()-6,12,12,QPen(QColor("magenta")));
    }

