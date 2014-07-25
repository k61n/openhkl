#include "DetectorView.h"
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"
#include "ColorMap.h"
#include <QGraphicsRectItem>

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
            if (_ptrData->has3DEllipsoid())
            {
                for (auto el : _ptrData->_peaks)
                {
                    SX::Geometry::Ellipsoid<double,3>& peak=el.second;
                    const Eigen::Vector3d& lower=peak.getLower();
                    const Eigen::Vector3d& upper=peak.getUpper();
                    // Plot bounding box
                    if (frame > lower[2] && frame < upper[2])
                    {

                        double left=lower[0];
                        double top=lower[1];
                        detectorToScene(left,top);
                        double w=upper[0]-lower[0];
                        double h=upper[1]-lower[1];
                        detectorToScene(w,h);
                        QGraphicsRectItem* bb=_scene->addRect(left,top,w,h,QPen(QColor("cyan")));
                        bb->setToolTip(QString::number(el.first));

                    }
                }
            }
        }
        else
        {
            _scene->clear();
        }
    }

    void DetectorView::mouseMoveEvent(QMouseEvent* event)
    {
        // If peak is detected
        QGraphicsRectItem* peak=dynamic_cast<QGraphicsRectItem*>(_scene->itemAt(event->x(),event->y()));

        if (peak)
        {
            peak->setActive(true);
            peak->setCursor(Qt::PointingHandCursor);
            return;
        }
        // If not on peak
        this->setCursor(Qt::CrossCursor);

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
        scene->addRect(event->x()-6,event->y()-6,12,12,QPen(QColor("green")));
    }

    void DetectorView::sceneToDetector(double& x, double& y)
    {
        x/=(static_cast<double>(this->width())/pixels_h);
        y/=(static_cast<double>(this->height())/pixels_v);
        return;
    }
    void DetectorView::detectorToScene(double& x, double& y)
    {
        x*=(static_cast<double>(this->width())/pixels_h);
        y*=(static_cast<double>(this->height())/pixels_v);
        return;
    }


