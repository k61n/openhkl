#include "DetectorView.h"
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"
#include "ColorMap.h"
#include <QGraphicsRectItem>
#include <cmath>

DetectorView::DetectorView(QWidget* parent): QGraphicsView(parent),
    _ptrData(nullptr),
    _scene(new QGraphicsScene(this)),
    _mode(PIXEL),
    _cutterMode(LINE),
    _line(0)
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
                    // Plot the bounding box
                    QGraphicsRectItem* bb=_scene->addRect(left-1,top-1,w+1,h+1,QPen(QBrush(QColor("yellow")),2.0));
                    bb->setToolTip(QString::number(el.first));
                    bb->setFlags(QGraphicsItem::ItemIsSelectable);

                }
            }
        }
    }
    else
    {
        _scene->clear();
    }
}


void DetectorView::mousePressEvent(QMouseEvent* event)
{
    // Act only if data are present
    if (!_ptrData)
        return;

    if (event->button() ==Qt::LeftButton)
    {
        QGraphicsItem* item=_scene->itemAt(event->x(),event->y());
        // Any Graphic object other than the base pixmap can be selected and deselected
        if (!dynamic_cast<QGraphicsPixmapItem*>(item))
        {
            item->setSelected(!item->isSelected());
            return; // Nothing else need to be done
        }

        // Enter different selection modes
        switch (_cutterMode)
        {
        case(LINE):
            _line=_scene->addLine(event->x(),event->y(),event->x(),event->y());
            _line->setVisible(true);
            _line->setPen(QPen(QBrush(QColor("blue")),2.0));
            _line->setFlags(QGraphicsItem::ItemIsSelectable);

            setScene(_scene);
            break;
        };
    }
}

void DetectorView::mouseMoveEvent(QMouseEvent* event)
{

    // Act only if data are present
    if (!_ptrData)
        return;

    QPointF pos=event->posF();

    if (event->buttons() & Qt::LeftButton)
    {
    switch(_cutterMode)
    {
    case(LINE):
        if (_line)
        {
           updateLineCutter(pos);
           setScene(_scene);
           break;
        }
    };
    }

    // If peak is detected
    QGraphicsRectItem* peak=dynamic_cast<QGraphicsRectItem*>(_scene->itemAt(pos.x(),pos.y()));

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
    double posx=pos.x();
    double posy=pos.y();
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
        if (_ptrData && pointInScene(pos))
            count=_ptrData->_currentFrame[static_cast<int>(posx)*256+static_cast<int>(posy)];
        os << "I: " << count;

    }

    QToolTip::showText(event->globalPos(),QString::fromStdString(os.str()),this,QRect());
}

void DetectorView::mouseReleaseEvent(QMouseEvent *event)
{
    // Act only if data are present
    if (!_ptrData)
        return;

    switch(_cutterMode)
    {
    case(LINE):
        if (_line)
        {
           break;
        }
    };
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
    case(Qt::Key_Escape):
        {
            QList<QGraphicsItem*> items=_scene->selectedItems();
            for (auto item : items)
            {
                _scene->removeItem(item);
            }
            break;
        }
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

void DetectorView::setCutterMode(int i)
{
    _cutterMode=static_cast<CutterMode>(i);

}

void DetectorView::updateLineCutter(const QPointF& pos)
{
    if (_line)
    {
        if (!pointInScene(pos))
            return;
        QLineF l=_line->line();
        l.setP2(pos);
        _line->setLine(l);
    }
}

bool DetectorView::pointInScene(const QPointF& pos)
{
    return (pos.x()<_scene->width() && pos.y()<_scene->height() && pos.x() >0 && pos.y() >0);
}


bool DetectorView::hasData() const
{
    return (_ptrData!=nullptr);
}
