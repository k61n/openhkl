#include "DetectorView.h"
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"
#include "ColorMap.h"
#include <QGraphicsRectItem>
#include <cmath>

    DetectorView::DetectorView(QWidget* parent): QGraphicsView(parent), _ptrData(nullptr), _scene(new QGraphicsScene(this))
    {

    }
    void DetectorView::setNpixels(int hor,int vert)
    {
        pixels_h=hor;
        pixels_v=vert;
    }
    void DetectorView::setDimensions(double hor,double vert)
    {
        _gammawidth=hor;
        _height=vert;
    }
    void DetectorView::setDetectorDistance(double distance)
    {
        _distance=distance;
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

        // Get the coordinates of the current pixel in detector space
        double posx=event->x();
        double posy=event->y();
        sceneToDetector(posx,posy);

        if (_ptrData)
        {
            switch(_mode)
            {
            case(D):
            {
                double d;
                getDSpacing(posx,posy,d);
                os << "d-Spacing:"  << d << "\n" ;
                break;
            }
            case(GAMMA):
            {
                double gamma,nu;
                getGammaNu(posx,posy,gamma,nu);
                os << "g,n: (" << gamma << "," <<nu << ")\n" ;
                break;
            }
            case(THETA):
            {
                double th2;
                get2Theta(posx,posy,th2);
                os << "th2: (" << th2 << ")\n" ;
                break;
            }
            case(PIXEL):
            {
                os << "x,y: (" << posx << "," << posy << ")\n";
                break;
            }
            };

            int count=0;
            if (_ptrData)
                count=_ptrData->_currentFrame[static_cast<int>(posx)*256+static_cast<int>(posy)];
            os << "I: " << count;

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

    void DetectorView::keyPressEvent(QKeyEvent* event)
    {

        switch (event->key())
        {

        case(Qt::Key_D):
            _mode=D;
            break;
        case(Qt::Key_T):
            _mode=THETA;
            break;
        case(Qt::Key_P):
            _mode=PIXEL;
            break;
        case(Qt::Key_G):
            _mode=GAMMA;
            break;
        };
    }

    void DetectorView::getGammaNu(double x, double y, double &gamma, double &nu)
    {
        SX::Data::MetaData* meta=_ptrData->mm->getMetaData();
        double gammacenter=meta->getKey<double>("2theta(gamma)");
        gamma=gammacenter+_gammawidth*(0.5-x/pixels_h);
        double h=(0.5*pixels_v-y-0.5)/pixels_v*_height;
        nu=atan2(h,_distance)*180/M_PI;
    }


    void DetectorView::get2Theta(double x, double y, double &th2)
    {
        SX::Data::MetaData* meta=_ptrData->mm->getMetaData();
        double gammacenter=meta->getKey<double>("2theta(gamma)");
        double gamma=gammacenter+_gammawidth*(0.5-x/pixels_h);
        double h=(0.5*pixels_v-y-0.5)/pixels_v*_height;
        th2=acos(_distance*cos(gamma*M_PI/180.0)/sqrt(_distance*_distance+h*h))*180.0/M_PI;
    }

    void DetectorView::getDSpacing(double x, double y, double &dspacing)
    {
        SX::Data::MetaData* meta=_ptrData->mm->getMetaData();
        double wavelength=meta->getKey<double>("wavelength");
        double th2;
        get2Theta(x,y,th2);
        dspacing=0.5*wavelength/sin(0.5*th2*M_PI/180);
    }


