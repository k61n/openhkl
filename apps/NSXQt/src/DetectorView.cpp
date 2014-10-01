#include <cmath>

#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QMenu>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QToolTip>
#include <QWidget>

#include "Data.h"
#include "ColorMap.h"
#include "Plotter1D.h"
#include "DetectorView.h"
#include "SliceRect.h"
#include "Detector.h"
#include "Units.h"

QStringList DetectorView::_lineColors = {"aqua","aquamarine","blue","burlywood","cadetblue","chartreuse","cornflowerblue","cornsilk","cyan","darkcyan","lavender","hotpink"};

DetectorView::DetectorView(QWidget* parent): QGraphicsView(parent),
    _ptrData(nullptr),
    _scene(new QGraphicsScene(this)),
    _mode(PIXEL),
    _cutterMode(ZOOM),
    _zoom(nullptr),
    _currentImage(nullptr),
    _cutPloter(nullptr),
    _maxIntensity(1),
    _plotter(nullptr),
    _sliceThickness(10),
    _pixmap(nullptr),
    _peakplotter(nullptr),
    _nCutPoints(10),
    _hSlices(),
    _vSlices(),
    _selectedSlice(0),
    _selectedLine(0)
{
    this->setScene(_scene);

}

void DetectorView::addCutLine(double xstart, double ystart, double xend, double yend)
{
    int colorId = _lines.size()%_lineColors.size();
    _lines.push_back(_scene->addLine(xstart,ystart,xend,yend));
    _lines.last()->setPen(QPen(QBrush(QColor(_lineColors[colorId])),2.0));
    _lines.last()->setFlag(QGraphicsItem::ItemIsSelectable);
    _selectedLine = _lines.size()-1;
}

void DetectorView::addHorizontalCutSlice(double x, double y, double width, double height)
{
    int colorId = _hSlices.size()%_lineColors.size();
    _scene->addItem(new SliceRect(x,y,width,height));
    _hSlices.push_back(dynamic_cast<SliceRect*>(_scene->items().first()));
    _hSlices.last()->setVisible(true);
    _hSlices.last()->setPen(QPen(QBrush(QColor(_lineColors[colorId])),1.0));
    _selectedSlice = _hSlices.size()-1;
}

void DetectorView::addVerticalCutSlice(double x, double y, double width, double height)
{
    int colorId = _vSlices.size()%_lineColors.size();
    _scene->addItem(new SliceRect(x,y,width,height));
    _vSlices.push_back(dynamic_cast<SliceRect*>(_scene->items().first()));
    _vSlices.last()->setVisible(true);
    _vSlices.last()->setPen(QPen(QBrush(QColor(_lineColors[colorId])),1.0));
    _selectedSlice = _vSlices.size()-1;
}

void DetectorView::copyViewToClipboard()
{
    // Create the image with the exact size of the shrunk scene
    QImage image(_scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    _scene->render(&painter);
    painter.end();
    QApplication::clipboard()->setImage(image,QClipboard::Clipboard);
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
    SX::Data::MetaData* meta=_ptrData->_mm->getMetaData();
    double gammacenter=meta->getKey<double>("2theta(gamma)");
    double gamma=gammacenter+_gammawidth*(0.5-x/pixels_h);
    double h=(0.5*pixels_v-y-0.5)/pixels_v*_height;
    th2=acos(_distance*cos(gamma*M_PI/180.0)/sqrt(_distance*_distance+h*h))*180.0/M_PI;
}

void DetectorView::getDSpacing(double x, double y, double &dspacing)
{
    SX::Data::MetaData* meta=_ptrData->_mm->getMetaData();
    double wavelength=meta->getKey<double>("wavelength");
    double th2;
    get2Theta(x,y,th2);
    dspacing=0.5*wavelength/sin(0.5*th2*M_PI/180);
}

void DetectorView::getGammaNu(double x, double y, double &gamma, double &nu)
{
    double gammanew,newnu;
    _ptrData->_detector->getGammaNu(x,y,gammanew,newnu);
    gammanew /= SX::Units::deg;
    newnu /= SX::Units::deg;

    SX::Data::MetaData* meta=_ptrData->_mm->getMetaData();
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

    if (event->matches(QKeySequence::Copy))
    {
        copyViewToClipboard();
    }
}

void DetectorView::mouseMoveEvent(QMouseEvent* event)
{

    // Act only if data are present
    if (!_ptrData)
        return;

    QPointF pos=event->localPos();

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
            if (!_hSlices.isEmpty())
            {
                _hSlices[_selectedSlice]->setRect(0,event->y(),width(),_sliceThickness);
                updateHorizontalSliceCutter();
            }
            break;
        }

        case(VERTICALSLICE):
        {
            if (!_vSlices.isEmpty())
            {
                _vSlices[_selectedSlice]->setRect(event->x(),0,_sliceThickness,height());
                updateVerticalSliceCutter();
            }
            break;
        }
        }
    }

    // If peak is detected
    QGraphicsItem* item=_scene->itemAt(pos.x(),pos.y(),QTransform());
    if (item)
    {
        if (dynamic_cast<QGraphicsRectItem*>(item) || dynamic_cast<QGraphicsLineItem*>(item))
        {
            item->setActive(true);
            item->setCursor(Qt::PointingHandCursor);
            return;
        }
        else
        {
            item->setActive(true);
            item->setCursor(Qt::CrossCursor);
        }
    }
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
        QGraphicsItem* item=_scene->itemAt(event->x(),event->y(),QTransform());
        // Any Graphic object other than the base pixmap can be selected and deselected
        if (!dynamic_cast<QGraphicsPixmapItem*>(item))
        {
            if (auto p=dynamic_cast<SliceRect*>(item))
            {
                if (_cutterMode==HORIZONTALSLICE)
                    _selectedSlice = _hSlices.indexOf(p);
                else if (_cutterMode==VERTICALSLICE)
                    _selectedSlice = _vSlices.indexOf(p);
            }
            else if (auto p=dynamic_cast<QGraphicsLineItem*>(item))
                _selectedLine = _lines.indexOf(p);
            else
            {
                item->setSelected(!item->isSelected());
                if (QGraphicsRectItem* p=dynamic_cast<QGraphicsRectItem*>(item))
                {
                    QString s=p->toolTip();
                    int peak_number=s.toInt();
                    if (!_peakplotter)
                        _peakplotter=new PeakPlotter(this);
                    _peakplotter->setPeak(&_ptrData->_rpeaks[peak_number]);
                    _peakplotter->show();
                    return;
                }
            }
            return; // Nothing else need to be done
        }

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
            addCutLine(event->x(),event->y(),event->x(),event->y());
            updateLineCutter();
            break;
        }

        case(HORIZONTALSLICE):
        {
            addHorizontalCutSlice(0,event->y(),width(),_sliceThickness);
            updateHorizontalSliceCutter();
            break;
        }

        case(VERTICALSLICE):
        {
            addVerticalCutSlice(event->x(),0,_sliceThickness,height());
            updateVerticalSliceCutter();
            break;
        }
        }

    }
    else if (event->button() == Qt::RightButton)
    {
        switch(_cutterMode)
        {
        // Unzoom mode
        case(ZOOM):
        {
            setPreviousZoomLevel();
            _zoom=nullptr;
            break;
        }

        case(LINE):
        {
            _selectedLine=_lines.size()-1;
            removeCutLine(_selectedLine);
            break;
        }

        case(HORIZONTALSLICE):
        {
            _selectedSlice=_hSlices.size()-1;
            removeHorizontalCutSlice(_selectedSlice);
            break;
        }

        case(VERTICALSLICE):
        {
            _selectedSlice=_vSlices.size()-1;
            removeVerticalCutSlice(_selectedSlice);
            break;
        }
        }
    }
}

void DetectorView::mouseReleaseEvent(QMouseEvent *event)
{
    // Act only if data are present
    if (!_ptrData)
        return;

    QPointF pos=event->localPos();

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
        _zoom=nullptr;
    }

    case(LINE):
    {
        if (!_lines.isEmpty())
        {
            QGraphicsItem* item=_scene->itemAt(pos.x(),pos.y(),QTransform());
            if (item->cursor().shape() == Qt::CrossCursor)
            {
                _lines[_selectedLine]->setVisible(true);
                QLineF l=_lines[_selectedLine]->line();
                l.setP2(pos);
                _lines[_selectedLine]->setLine(l);
                updateLineCutter();
            }
        }
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
    if (_ptrData->_rpeaks.size())
    {
        for (auto& el : _ptrData->_rpeaks)
        {
            const SX::Geometry::IShape<double,3>* peak=el.second.getPeak();
            const Eigen::Vector3d& lower=peak->getLower();
            const Eigen::Vector3d& upper=peak->getUpper();
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
                QGraphicsRectItem* bb=_scene->addRect(left-1,top-1,right-left+1,bottom-top+1,QPen(QBrush(QColor("green")),2.0));
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
    _pixmap = _scene->addPixmap(pix);
    setScene(_scene);
    update();
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

void DetectorView::removeCutLine(int idx)
{
    // If no lines have been defined just leave.
    if (_lines.isEmpty())
        return;

    // Check that the index of the line to be removed is valid.
    if (idx<0 || idx>=_lines.size())
        return;

    // Remove the line.
    _scene->removeItem(_lines[idx]);
    _lines.removeAt(idx);

    // If a plotter is opened, remove the curve corresponding to the cut line to be removed.
    if (_plotter)
        _plotter->removeCurve(idx);
}

void DetectorView::removeHorizontalCutSlice(int idx)
{
    // If no slices have been defined just leave.
    if (_hSlices.isEmpty())
        return;

    // Check that the index of the slice to be removed is valid.
    if (idx<0 || idx>=_hSlices.size())
        return;

    // Remove the slice.
    _scene->removeItem(_hSlices[idx]);
    _hSlices.removeAt(idx);

    // If a plotter is opened, remove the curve corresponding to the cut slice to be removed.
    if (_plotter)
        _plotter->removeCurve(idx);
}

void DetectorView::removeVerticalCutSlice(int idx)
{
    // If no slices have been defined just leave.
    if (_vSlices.isEmpty())
        return;

    // Check that the index of the slice to be removed is valid.
    if (idx<0 || idx>=_vSlices.size())
        return;

    // Remove the slice.
    _scene->removeItem(_vSlices[idx]);
    _vSlices.removeAt(idx);

    // If a plotter is opened, remove the curve corresponding to the cut slice to be removed.
    if (_plotter)
        _plotter->removeCurve(idx);
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

void DetectorView::clearCutLines()
{
    if (!_lines.isEmpty())
    {
        for (auto l=_lines.begin();l!=_lines.end();++l)
            _scene->removeItem(*l);
        _lines.clear();
    }
}

void DetectorView::clearHorizontalCutSlices()
{
    if (!_hSlices.isEmpty())
    {
        for (auto hs=_hSlices.begin();hs!=_hSlices.end();++hs)
            _scene->removeItem(*hs);
        _hSlices.clear();
    }
}

void DetectorView::clearVerticalCutSlices()
{
    if (!_vSlices.isEmpty())
    {
        for (auto vs=_vSlices.begin();vs!=_vSlices.end();++vs)
            _scene->removeItem(*vs);
        _vSlices.clear();
    }
}

void DetectorView::clearCutObjects()
{
    clearCutLines();
    clearHorizontalCutSlices();
    clearVerticalCutSlices();
}

void DetectorView::setCutterMode(int i)
{
    _cutterMode=static_cast<CutterMode>(i);

    switch(_cutterMode)
    {
    case(ZOOM):
    {
        if (_plotter)
        {
            delete _plotter;
            _plotter = nullptr;
        }

        clearCutObjects();
        return;
    }
    case(LINE):
    {
        clearHorizontalCutSlices();
        clearVerticalCutSlices();
        break;
    }
    case(HORIZONTALSLICE):
    {
        clearCutLines();
        clearVerticalCutSlices();
        break;
    }
    case(VERTICALSLICE):
    {
        clearCutLines();
        clearHorizontalCutSlices();
        break;
    }
    }

    clearPlotter();
}

void DetectorView::clearPlotter()
{
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

    if (_lines.isEmpty())
        return;

    QLineF l=_lines[_selectedLine]->line();

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
        _plotter->addCurve(x,projection,e, _lines[_selectedLine]->pen().color());
    }
    else
    {
        if (_selectedLine >= _plotter->nGraphs())
            _plotter->addCurve(x,projection,e, _lines[_selectedLine]->pen().color());
        else
            _plotter->modifyCurve(_selectedLine,x,projection,e);
    }
    _plotter->show();

}

void DetectorView::updatePlot()
{
    _scene->clear();
    plotIntensityMap();
    updateLineCutter();
    updateHorizontalSliceCutter();
    updateVerticalSliceCutter();
    plotEllipsoids();
    setScene(_scene);
}

void DetectorView::updateHorizontalSliceCutter()
{

    if (_hSlices.isEmpty())
        return;

    QRectF rect=_hSlices[_selectedSlice]->rect();
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

    x.resize(xmax-xmin);
    for (int i=0;i<xmax-xmin;++i)
        x[i]=xmin+i;
    integrateVertical(xmin,xmax-1,ymin,ymax,y,e);

    if (!_plotter)
    {
        _plotter=new Plotter1D(this);
        _plotter->addCurve(x,y,e, _hSlices[_selectedSlice]->pen().color());
    }
    else
    {
        if (_selectedSlice >= _plotter->nGraphs())
            _plotter->addCurve(x,y,e, _hSlices[_selectedSlice]->pen().color());
        else
            _plotter->modifyCurve(_selectedSlice,x,y,e);
    }
    _plotter->show();
}

void DetectorView::updateVerticalSliceCutter()
{

    if (_vSlices.isEmpty())
        return;

    QRectF rect=_vSlices[_selectedSlice]->rect();
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

    x.resize(ymax-ymin);
    for (int i=0;i<ymax-ymin;++i)
        x[i]=ymin+i;
    integrateHorizontal(xmin,xmax-1,ymin,ymax,y,e);

    if (!_plotter)
    {
        _plotter=new Plotter1D(this);
        _plotter->addCurve(x,y,e, _vSlices[_selectedSlice]->pen().color());
    }
    else
    {
        if (_selectedSlice >= _plotter->nGraphs())
            _plotter->addCurve(x,y,e, _vSlices[_selectedSlice]->pen().color());
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

        if (_hSlices.isEmpty())
            return;

        // One mouse delta=120
        _sliceThickness+=event->delta()/120;
        QRectF rect=_hSlices[_selectedSlice]->rect();
        _hSlices[_selectedSlice]->setRect(0,rect.top(),width(),_sliceThickness);
        updateHorizontalSliceCutter();
    }
    else if (_cutterMode==VERTICALSLICE)
    {
        if (_vSlices.isEmpty())
            return;

        // One mouse delta=120
        _sliceThickness+=event->delta()/120;
        QRectF rect=_vSlices[_selectedSlice]->rect();
        _vSlices[_selectedSlice]->setRect(rect.left(),0,_sliceThickness,height());
        updateVerticalSliceCutter();
    }
}
