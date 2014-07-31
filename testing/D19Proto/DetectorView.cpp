#include "DetectorView.h"
#include <QGraphicsView>
#include <QWidget>
#include <QMouseEvent>
#include <QToolTip>
#include "Data.h"
#include "ColorMap.h"
#include <QGraphicsRectItem>
#include <cmath>
#include <QColor>
#include "Plotter1D.h"
#include "slicerect.h"

DetectorView::DetectorView(QWidget* parent): QGraphicsView(parent),
    _ptrData(nullptr),
    _scene(new QGraphicsScene(this)),
    _mode(PIXEL),
    _cutterMode(ZOOM),
    _line(nullptr),_zoom(nullptr),_currentImage(nullptr),
    _cutPloter(nullptr),_maxIntensity(1),_plotter(nullptr),
    _sliceThickness(0),
    _pixmap(nullptr),
    _peakplotter(nullptr),
    _nCutPoints(10),
    _slices(),
    _selectedSlice(0)
{
    this->setScene(_scene);
}

void DetectorView::detectorToScene(double& x, double& y)
{
    x-=_zoomLeft;
    y-=_zoomTop;
    double hrange=_zoomRight-_zoomLeft;
    double vrange=_zoomBottom-_zoomTop;
    x*=(static_cast<double>(this->width())/hrange);
    y*=(static_cast<double>(this->height())/vrange);
    return;
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

void DetectorView::getGammaNu(double x, double y, double &gamma, double &nu)
{
    SX::Data::MetaData* meta=_ptrData->mm->getMetaData();
    double gammacenter=meta->getKey<double>("2theta(gamma)");
    gamma=gammacenter+_gammawidth*(0.5-x/pixels_h);
    double h=(0.5*pixels_v-y-0.5)/pixels_v*_height;
    nu=atan2(h,_distance)*180/M_PI;
}

bool DetectorView::hasData() const
{
    return (_ptrData!=nullptr);
}

void DetectorView::integrateHorizontal(int xmin, int xmax, int ymin, int ymax, QVector<double> &projection, QVector<double> &error)
{

    if (xmin>xmax)
        std::swap(xmin,xmax);
    if (ymin>ymax)
        std::swap(ymin,ymax);
    projection.resize(ymax-ymin);
    error.resize(ymax-ymin);

    int* d=&(_ptrData->_currentFrame[0]);
    for (int i=0;i<pixels_h;++i)
    {
        for (int j=0;j<pixels_v;++j)
        {
            if (i>=xmin && i<xmax && j>=ymin && j<ymax)
                projection[j-ymin]+=*d;
            d++;
        }
    }

    for (int i=0;i<(ymax-ymin);++i)
        error[i]=sqrt(projection[i]);

    return;
}

void DetectorView::integrateVertical(int xmin, int xmax, int ymin, int ymax, QVector<double> &projection, QVector<double> &error)
{
    if (xmin>xmax)
        std::swap(xmin,xmax);
    if (ymin>ymax)
        std::swap(ymin,ymax);
    projection.resize(xmax-xmin);
    error.resize(xmax-xmin);

    int* d=&(_ptrData->_currentFrame[0]);
    for (int i=0;i<pixels_h;++i)
    {
        for (int j=0;j<pixels_v;++j)
        {
            if (i>=xmin && i<xmax && j>=ymin && j<ymax)
                projection[i-xmin]+=*d;
            d++;
        }
    }

    for (int i=0;i<(xmax-xmin);++i)
        error[i]=sqrt(projection[i]);

    return;
}

void DetectorView::keyPressEvent(QKeyEvent* event)
{

    switch (event->key())
    {
    case(Qt::Key_D):
    {
        _mode=D;
        break;
    }

    case(Qt::Key_T):
    {
        _mode=THETA;
        break;
    }

    case(Qt::Key_P):
    {
        _mode=PIXEL;
        break;
    }

    case(Qt::Key_G):
    {
        _mode=GAMMA;
        break;
    }

    case(Qt::Key_Escape):
    {
        QList<QGraphicsItem*> items=_scene->selectedItems();
        for (auto item : items)
        {
            _scene->removeItem(item);
        }
        break;
    }
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
        case(ZOOM):
        {
            if (_zoom)
                updateZoomCutter(pos);
            break;
        }

        case(LINE):
        {
            break;
        }

        case(HORIZONTALSLICE):
        {
            if (!_slices.isEmpty())
            {
                _slices[_selectedSlice]->setRect(0,event->y(),width(),_sliceThickness);
                updateSliceIntegrator();
            }
            break;
        }

        case(VERTICALSLICE):
        {
            if (!_slices.isEmpty())
            {
                _slices[_selectedSlice]->setRect(event->x(),0,_sliceThickness,height());
                updateSliceIntegrator();
            }
            break;
        }
        }
    }

    // If peak is detected
    QGraphicsItem* item=_scene->itemAt(pos.x(),pos.y());
    QGraphicsRectItem* peak=dynamic_cast<QGraphicsRectItem*>(item);

    if (peak)
    {
        peak->setActive(true);
        peak->setCursor(Qt::PointingHandCursor);
        return;
    }

    // If not on peak
    item->setActive(true);
    item->setCursor(Qt::CrossCursor);

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
        }

        int count=0;
        if (_ptrData && pointInScene(pos))
            count=_ptrData->_currentFrame[static_cast<int>(posx)*256+static_cast<int>(posy)];
        os << "I: " << count;

    }

    QToolTip::showText(event->globalPos(),QString::fromStdString(os.str()),this,QRect());
}

void DetectorView::mousePressEvent(QMouseEvent* event)
{
    // Act only if data are present
    if (!_ptrData)
        return;

    if (event->button() == Qt::LeftButton)
    {
        QGraphicsItem* item=_scene->itemAt(event->x(),event->y());
        // Any Graphic object other than the base pixmap can be selected and deselected
        if (!dynamic_cast<QGraphicsPixmapItem*>(item))
        {

            if (dynamic_cast<SliceRect*>(item))
            {
                _selectedSlice = _slices.indexOf(dynamic_cast<SliceRect*>(item));
            }
            else
            {
                item->setSelected(!item->isSelected());
                if (QGraphicsRectItem* p=dynamic_cast<QGraphicsRectItem*>(item))
                {
                    QString s=p->toolTip();
                    int peak_number=s.toInt();
                    if (!_peakplotter)
                        _peakplotter=new PeakPlotter(this);
                    _peakplotter->setPeak(_ptrData->_rpeaks[peak_number]);
                    _peakplotter->show();
                }
            }
            return; // Nothing else need to be done
        }

        // Only one cut line allowed
        if (_line)
        {
            _scene->removeItem(_line);
            delete _line;
            _line=nullptr;
        }

        // Only one slice allowed
//        if (_slice)
//        {
//            _scene->removeItem(_slice);
//            delete _slice;
//            _slice=nullptr;
//        }

        switch (_cutterMode)
        {
        case(ZOOM):
        {
            _zoom=_scene->addRect(event->x(),event->y(),0,0);
            _zoom->setVisible(true);
            _zoom->setPen(QPen(QBrush(QColor("gray")),1.0));
            break;
        }

        case(LINE):
        {
            _line=_scene->addLine(event->x(),event->y(),event->x(),event->y());
            _line->setPen(QPen(QBrush(QColor("blue")),2.0));

            break;
        }

        case(HORIZONTALSLICE):
        {
            _scene->addItem(new SliceRect(0,event->y(),width(),_sliceThickness));
            _slices.push_back(dynamic_cast<SliceRect*>(_scene->items().first()));
            _slices.last()->setVisible(true);
            _selectedSlice = _slices.size()-1;
            int cId = (_selectedSlice) % QColor::colorNames().size();
            QColor sliceColor = QColor(QColor::colorNames()[cId]);
            _slices.last()->setPen(QPen(QBrush(QColor(sliceColor)),1.0));
            updateSliceIntegrator();

//            _slice=_scene->addRect(0,event->y(),width(),_sliceThickness);
//            _slice->setVisible(true);
//            _slice->setPen(QPen(QBrush(QColor("gray")),1.0));
//            updateSliceIntegrator();
            break;
        }

        case(VERTICALSLICE):
        {
            _scene->addItem(new SliceRect(event->x(),0,_sliceThickness,height()));
            _slices.push_back(dynamic_cast<SliceRect*>(_scene->items().first()));
            _slices.last()->setVisible(true);
            _selectedSlice = _slices.size()-1;
            int cId = (_selectedSlice) % QColor::colorNames().size();
            QColor sliceColor = QColor(QColor::colorNames()[cId]);
            _slices.last()->setPen(QPen(QBrush(QColor(sliceColor)),1.0));
            updateSliceIntegrator();

//            _slice=_scene->addRect(event->x(),0,_sliceThickness,height());
//            _slice->setVisible(true);
//            _slice->setPen(QPen(QBrush(QColor("gray")),1.0));
//            updateSliceIntegrator();
            break;
        }
        }

    }
    else if (event->button() == Qt::RightButton)
    {
        // Unzoom mode
        if (_cutterMode==ZOOM)
            setPreviousZoomLevel();
    }
}

void DetectorView::mouseReleaseEvent(QMouseEvent *event)
{
    // Act only if data are present
    if (!_ptrData)
        return;

    QPointF pos=event->posF();

    if (event->button() != Qt::LeftButton)
        return;

    switch(_cutterMode)
    {

    case(ZOOM):
    {
        if (!_zoom)
            return;
        _zoom->setVisible(false);
        // This is the rectangle in scene coordinates
        QRectF rectf=_zoom->rect();
        double xmind=rectf.left();
        double ymind=rectf.top();
        sceneToDetector(xmind,ymind);
        double xmaxd=rectf.right();
        double ymaxd=rectf.bottom();
        sceneToDetector(xmaxd,ymaxd);
        int xmin=static_cast<int>(xmind);
        int ymin=static_cast<int>(ymind);
        int xmax=static_cast<int>(xmaxd);
        int ymax=static_cast<int>(ymaxd);
        if (xmin!=xmax && ymin!=ymax)
        {
            registerZoomLevel(xmin,xmax,ymin,ymax);
            setZoom(xmin,ymin,xmax,ymax);
            updatePlot();
        }
        break;
    }

    case(LINE):
    {
        if (!pointInScene(pos))
            return;
        _line->setVisible(true);
        QLineF l=_line->line();
        l.setP2(pos);
        _line->setLine(l);
        updateLineCutter();
        break;
    }

    case(HORIZONTALSLICE):
    {
       break;
    }
    case(VERTICALSLICE):
    {
       break;
    }
    }

}

void DetectorView::plotEllipsoids()
{
    if (!_currentPeaks.isEmpty())
    {
        for (int i=0;i<_currentPeaks.size();++i)
        {
            _scene->removeItem(_currentPeaks[i]);
        }
    }
    if (_ptrData->has3DEllipsoid())
    {
        for (auto el : _ptrData->_peaks)
        {
            SX::Geometry::Ellipsoid<double,3>& peak=el.second;
            const Eigen::Vector3d& lower=peak.getLower();
            const Eigen::Vector3d& upper=peak.getUpper();
            // Plot bounding box
            if (_currentFrame > lower[2] && _currentFrame < upper[2])
            {

                double left=lower[0];
                double top=lower[1];
                detectorToScene(left,top);
                double right=upper[0];
                double bottom=upper[1];
                detectorToScene(right,bottom);
                // Plot the bounding box
                QGraphicsRectItem* bb=_scene->addRect(left-1,top-1,right-left+1,bottom-top+1,QPen(QBrush(QColor("yellow")),2.0));
                bb->setToolTip(QString::number(el.first));
                bb->setFlags(QGraphicsItem::ItemIsSelectable);
                _currentPeaks.push_back(bb);
            }
        }
    }
}

void DetectorView::plotIntensityMap()
{
    QImage image=Mat2QImage(&(_ptrData->_currentFrame[0]),256,640,_zoomLeft,_zoomRight,_zoomTop,_zoomBottom,_maxIntensity);
    QPixmap pix=QPixmap::fromImage(image);
    pix=pix.scaled(width(),height(),Qt::IgnoreAspectRatio);

    // If no pixmap is present, create a new one and add to scene
    if (!_pixmap)
        _pixmap = _scene->addPixmap(pix);
    else // update the pixmap with new image
        _pixmap->setPixmap(pix);
}

bool DetectorView::pointInScene(const QPointF& pos)
{
    return (pos.x()<_scene->width() && pos.y()<_scene->height() && pos.x() >0 && pos.y() >0);
}

void DetectorView::registerZoomLevel(int xmin, int xmax, int ymin, int ymax)
{
    // Swap coordinates for swapped rectangle view.
    if (xmin>xmax)
        std::swap(xmin,xmax);
    if (ymin>ymax)
        std::swap(ymin,ymax);
    _zoomStack.push(QRect(xmin,ymin,xmax-xmin+1,ymax-ymin+1));
}

void DetectorView::sceneToDetector(double& x, double& y)
{
    x/=static_cast<double>(this->width());
    y/=static_cast<double>(this->height());
    double hrange=_zoomRight-_zoomLeft;
    double vrange=_zoomBottom-_zoomTop;
    x*=hrange;y*=vrange;
    x+=_zoomLeft;
    y+=_zoomTop;
    return;
}

void DetectorView::setCutterMode(int i)
{
    _cutterMode=static_cast<CutterMode>(i);
    clearPlotter();
}

void DetectorView::clearPlotter()
{
    for (auto s=_slices.begin();s!=_slices.end();++s)
        _scene->removeItem(*s);
    _slices.clear();
    if (_plotter)
        _plotter->clear();
}

void DetectorView::setDetectorDistance(double distance)
{
    _distance=distance;
}

void DetectorView::setDimensions(double hor,double vert)
{
    _gammawidth=hor;
    _height=vert;
}

void DetectorView::setMaxIntensity(int intensity)
{
    _maxIntensity=intensity;
}

void DetectorView::setNpixels(int hor,int vert)
{
    pixels_h=hor;
    pixels_v=vert;
    setZoom(0,0,hor,vert);
    registerZoomLevel(0,hor,0,vert);
}

void DetectorView::setPreviousZoomLevel()
{
    if (_zoomStack.size()>1)
    {
        _zoomStack.pop(); // Go the previous zoom mode
        if (!_zoomStack.isEmpty()) // If not root, then assign
        {
            QRect rect=_zoomStack.top();
            setZoom(rect.left(),rect.top(),rect.right(),rect.bottom());
            updatePlot();
        }
    }
}

void DetectorView::setZoom(int x1, int y1, int x2, int y2)
{
    // Swap coordinates for swapped rectangle view.
    if (x1>x2)
        std::swap(x1,x2);
    if (y1>y2)
        std::swap(y1,y2);
    _zoomLeft=x1;
    _zoomTop=y1;
    _zoomRight=x2;
    _zoomBottom=y2;
}

void DetectorView::updateLineCutter()
{

    QLineF l=_line->line();

    QVector<double> x,projection,e;
    x.resize(_nCutPoints);
    projection.resize(_nCutPoints);
    e.resize(_nCutPoints);

    double dr = l.length()/_nCutPoints;

    for (int i=0; i<_nCutPoints; ++i)
    {
        x[i] = i*dr;
        QPointF point=l.pointAt(i/static_cast<double>(_nCutPoints));
        sceneToDetector(point.rx(),point.ry());
        int ipx=static_cast<int>(point.x());
        int ipy=static_cast<int>(point.y());
        QPoint lowestCorner=QPoint(ipx,ipy);
        double sdist2 = 0.0;
        for (int pi=0;pi<2;++pi)
        {
            for (int pj=0;pj<2;++pj)
            {
                QPoint currentCorner = lowestCorner + QPoint(pi,pj);
                QPointF dp = point - currentCorner;
                double dist2 = dp.x()*dp.x() + dp.y()*dp.y();
                int count=_ptrData->_currentFrame[currentCorner.x()*256+currentCorner.y()];
                projection[i] += dist2*count;
                sdist2 += dist2;
            }
        }
        projection[i] /= sdist2;
        e[i] = sqrt(projection[i]);
    }

    if (!_plotter)
    {
        _plotter=new Plotter1D(this);
        _plotter->addCurve(x,projection,e);
    }
    else
    {
        _plotter->modifyCurve(0,x,projection,e);
    }
    _plotter->show();

}

void DetectorView::updatePlot()
{
    plotIntensityMap();
    updateSliceIntegrator();
    plotEllipsoids();
    setScene(_scene);
}

void DetectorView::updateSliceIntegrator()
{

    if (_slices.isEmpty())
        return;

    QRectF rect=_slices[_selectedSlice]->rect();
    double xmind=rect.left();
    double xmaxd=rect.right();
    double ymind=rect.top();
    double ymaxd=rect.bottom();
    sceneToDetector(xmind,ymind);
    sceneToDetector(xmaxd,ymaxd);
    int xmin=static_cast<int>(xmind);
    int xmax=static_cast<int>(xmaxd);
    int ymin=static_cast<int>(ymind);
    int ymax=static_cast<int>(ymaxd);

    if (xmin < 0  || xmax > pixels_h || ymin <0 || ymax > pixels_v)
        return;

    QVector<double> x,y,e;

    if (_cutterMode==HORIZONTALSLICE)
    {
        x.resize(xmax-xmin);
        for (int i=0;i<xmax-xmin;++i)
            x[i]=xmin+i;
        integrateVertical(xmin,xmax-1,ymin,ymax,y,e);
    }
    else if (_cutterMode==VERTICALSLICE)
    {
        x.resize(ymax-ymin);
        for (int i=0;i<ymax-ymin;++i)
            x[i]=ymin+i;
        integrateHorizontal(xmin,xmax-1,ymin,ymax,y,e);
    }

    if (!_plotter)
    {
        _plotter=new Plotter1D(this);
        _plotter->addCurve(x,y,e, _slices[_selectedSlice]->pen().color());
    }
    else
    {
        if (_selectedSlice >= _plotter->nGraphs())
            _plotter->addCurve(x,y,e, _slices[_selectedSlice]->pen().color());
        else
            _plotter->modifyCurve(_selectedSlice,x,y,e);
    }
    _plotter->show();
}

void DetectorView::updateView(Data* ptr,int frame)
{
    _ptrData=ptr;

    // No data
    if (!_ptrData)
    {
        _scene->clear();
        delete _currentImage;
        return;
    }

    _ptrData->readBlock(frame);
    _currentFrame=frame;

    updatePlot();
}

void DetectorView::updateZoomCutter(const QPointF& pos)
{
    if (_zoom)
    {
        if (!pointInScene(pos))
            return;
       QRectF rect=_zoom->rect();
       rect.setBottomRight(pos);
       _zoom->setRect(rect);
    }
}

void DetectorView::wheelEvent(QWheelEvent *event)
{
    if (_cutterMode==LINE)
    {
        _nCutPoints += event->delta()>0 ? 1 : -1;
        if (_nCutPoints <= 0)
            _nCutPoints = 1;
        updateLineCutter();
    }
    else if (_cutterMode==HORIZONTALSLICE)
    {

        if (_slices.isEmpty())
            return;

        // One mouse delta=120
        _sliceThickness+=event->delta()/120;
        QRectF rect=_slices[_selectedSlice]->rect();
        _slices[_selectedSlice]->setRect(0,rect.top(),width(),_sliceThickness);
        updateSliceIntegrator();
    }
    else if (_cutterMode==VERTICALSLICE)
    {
        if (_slices.isEmpty())
            return;

        // One mouse delta=120
        _sliceThickness+=event->delta()/120;
        QRectF rect=_slices[_selectedSlice]->rect();
        _slices[_selectedSlice]->setRect(rect.left(),0,_sliceThickness,height());
        updateSliceIntegrator();
    }
}
