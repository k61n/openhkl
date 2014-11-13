#include <vector>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include "ColorMap.h"
#include "Detector.h"
#include "DetectorScene.h"
#include "IData.h"
#include <QToolTip>
#include "Units.h"
#include <QKeyEvent>
#include "Sample.h"
#include "Source.h"
#include "PeakGraphicsItem.h"
#include "SliceGraphicsItem.h"
#include "LineCutGraphicsItem.h"
#include <QtDebug>

DetectorScene::DetectorScene(QObject *parent) :
    QGraphicsScene(parent),
    _currentData(nullptr),
    _currentFrameIndex(0),
    _currentIntensity(10),
    _currentFrame(),
    _cursorMode(PIXEL),
    _mode(ZOOM),
    _currentCut(nullptr),
    _itemSelected(false),
    _image(nullptr)
{
}

void DetectorScene::changeFrame(int frame)
{

    if (!_currentData)
        return;

    if (frame == _currentFrameIndex)
        return;
    _currentFrameIndex = frame;

    for (auto peak : _peaks)
    {
        peak->setFrame(_currentFrameIndex);
    }
    loadCurrentImage();
}

void DetectorScene::setMaxIntensity(int intensity)
{

    _currentIntensity = intensity;

    if (!_currentData)
        return;

    loadCurrentImage();
}

void DetectorScene::setData(SX::Data::IData* data)
{
    if (_currentData==data)
        return;

    _currentData = data;

    if (!_currentData)
        return ;

    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

     _zoomStack.clear();
     _zoomStack.push_back(QRect(0,0,ncols,nrows));


     if (_image)
     {
         removeItem(_image);
         delete _image;
     }

     if (_currentCut)
     {
         removeItem(_currentCut);
         delete _currentCut;
     }

     loadCurrentImage();
     updatePeaks();

}

void DetectorScene::setData(SX::Data::IData* data, int frame)
{
    setData(data);
    changeFrame(frame);
}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (!_currentData)
        return;

    // Zoom mode starts
    if (event->buttons() & Qt::LeftButton)
    {
        // Select GraphicItem if not detector image
        auto item=itemAt(event->lastScenePos(),QTransform());
        if (!dynamic_cast<QGraphicsPixmapItem*>(item))
        {
            item->setSelected(!item->isSelected());
            _itemSelected=true;
            return;
        }
        _itemSelected=false;

       if (_mode==ZOOM)
       {
           _zoomstart=event->lastScenePos().toPoint();
           _zoomend=_zoomstart;
           QRect zoom(_zoomstart,_zoomend);
           _zoomrect=addRect(zoom);
           QPen pen1(QBrush(QColor("gray")),1.0);
           pen1.setWidth(1);
           pen1.setCosmetic(true);
           _zoomrect->setPen(pen1);
           _zoomrect->setBrush(QBrush(QColor(255,0,0,30)));
           return;
       }

       // Slice starts here
       if (_currentCut)
       {
           if (_currentCut->scene()==this)
           {
               removeItem(_currentCut);
           }
       }

       switch(_mode)
       {
           case(HORIZONTALSLICE):
           {
               _currentCut=new SliceGraphicsItem(_currentData,true);
               _currentCut->from(event->lastScenePos().x(),event->lastScenePos().y());
               break;
           }
           case(VERTICALSLICE):
           {
               _currentCut=new SliceGraphicsItem(_currentData,false);
               _currentCut->from(event->lastScenePos().x(),event->lastScenePos().y());
               break;
           }
           case(LINE):
           {
               _currentCut=new LineCutGraphicsItem(_currentData);
               _currentCut->from(event->lastScenePos().x(),event->lastScenePos().y());
               break;
            }
       }

       addItem(_currentCut);

    }   // Move up in the zoom stack
    else if (event->buttons() & Qt::RightButton)
    {
        if (_zoomStack.size()>1)
        {
            _zoomStack.pop(); // Go the previous zoom mode
            if (!_zoomStack.isEmpty()) // If not root, then assign
            {
                setSceneRect(_zoomStack.top());
                emit dataChanged();
            }
        }
    }
}

void DetectorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if (!_currentData)
        return;

    createToolTipText(event);

    if (_itemSelected)
        return;

    if (event->buttons() & Qt::LeftButton)
    {
        if (_mode==ZOOM)
        {
                QRectF zoom=_zoomrect->rect();
                zoom.setBottomRight(event->lastScenePos());
                _zoomrect->setRect(zoom);
        }
        else
        {
            // Slice mode, ensure that point is in View
           auto& rect=_zoomStack.front();
           QPoint point = event->lastScenePos().toPoint();
           if (point.x()>rect.left() && point.x()<rect.right() && point.y() > rect.top() && point.y() <rect.bottom())
           _currentCut->to(event->lastScenePos().x(),event->lastScenePos().y());
        }
    }
    else
    {
        QGraphicsScene::mouseMoveEvent(event);

        PeakGraphicsItem* item=dynamic_cast<PeakGraphicsItem*>(itemAt(event->lastScenePos().toPoint(),QTransform()));
        if (!item)
               return;
        if (!item->isVisible())
                return;
        emit plotPeak(item->getPeak());
    }
}

void DetectorScene::keyPressEvent(QKeyEvent *event)
{

    if (!_currentData)
        return;

    switch (event->key())
    {
    case(Qt::Key_Delete):
    {
        QList<QGraphicsItem*> items=selectedItems();
        int i=0;
        for (auto item : items)
        {
            removeItem(item);
            PeakGraphicsItem* peak=dynamic_cast<PeakGraphicsItem*>(item);
            if (peak)
            {
                bool remove=_currentData->removePeak(peak->getPeak());
                if (remove)
                    i++;
                qDebug() << "Removed "<< i << " peaks";
                _peaks.erase(peak);
            }

            delete item;

        }
        break;
    }
    }
}

void DetectorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    if (!_currentData)
        return;

    if (_itemSelected)
    {
        _itemSelected=false;
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        switch(_mode)
        {
            case(ZOOM):
            {
                qreal top=_zoomrect->rect().top();
                qreal bot=_zoomrect->rect().bottom();
                qreal left=_zoomrect->rect().left();
                qreal right=_zoomrect->rect().right();
                if (top > bot)
                    std::swap(top,bot);
                if (right < left)
                    std::swap(left,right);
                QRect max=_zoomStack.front();
                if (top<max.top())
                    top=max.top();
                if (bot>max.bottom())
                    bot=max.bottom();
                if (left<max.left())
                    left=max.left();
                if (right>max.right())
                    right=max.right();

                _zoomrect->setRect(left,top,right-left,bot-top);
                setSceneRect(_zoomrect->rect());
                removeItem(_zoomrect);
                _zoomStack.push_back(_zoomrect->rect().toRect());
                emit dataChanged();
            }
        }


    }
}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    QString ttip;
    std::size_t xpos=static_cast<std::size_t>(event->scenePos().x());
    std::size_t ypos=static_cast<std::size_t>(event->scenePos().y());
    auto instr=_currentData->getDiffractometer();

    SX::Instrument::Detector* det=instr->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

    int intensity=0;
    if (xpos<ncols-1 && ypos<nrows-1)
        intensity=_currentFrame[xpos*nrows+ypos];


    const auto& samplev=_currentData->getSampleState(_currentFrameIndex).getValues();
    const auto& detectorv=_currentData->getDetectorState(_currentFrameIndex).getValues();
    SX::Instrument::Sample* sample=instr->getSample();
    double wave=instr->getSource()->getWavelength();

    switch (_cursorMode)
    {
        case(PIXEL):
        {
            ttip=QString("(%1,%2) I:%3").arg(xpos).arg(ypos).arg(intensity);
            break;
        }
        case(GAMMA):
        {
            double gamma,nu;
            det->getGammaNu(xpos,ypos,gamma,nu,detectorv,sample->getPosition(samplev));
            ttip=QString("(%1,%2) I: %3").arg(gamma/SX::Units::deg).arg(nu/SX::Units::deg).arg(intensity);
            break;
        }
        case(THETA):
        {
            double th2=det->get2Theta(xpos,ypos,detectorv,Eigen::Vector3d(0,1.0/wave,0));
            ttip=QString("(%1) I: %2").arg(th2/SX::Units::deg).arg(intensity);
            break;
        }
        case(DSPACING):
        {
            double th2=det->get2Theta(xpos,ypos,detectorv,Eigen::Vector3d(0,1.0/wave,0));
            ttip=QString("(%1) I: %2").arg(wave/(2*sin(0.5*th2))).arg(intensity);
            break;
        }
        case(HKL):
        {

        }

    }
    QToolTip::showText(event->screenPos(),ttip);
}

void DetectorScene::changeInteractionMode(int mode)
{
    _mode=static_cast<MODE>(mode);
}

void DetectorScene::loadCurrentImage()
{
    // Full image size, front of the stack
    QRect& full=_zoomStack.front();

    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

    _currentFrame =_currentData->getFrame(_currentFrameIndex);
    QImage image=Mat2QImage(&_currentFrame[0], nrows, ncols, full.left(), full.right(), full.top(), full.bottom(), _currentIntensity);
    if (!_image)
    {
        _image=addPixmap(QPixmap::fromImage(image));
        _image->setZValue(-1);
    }
    else
        _image->setPixmap(QPixmap::fromImage(image));

    setSceneRect(_zoomStack.back());
    emit dataChanged();

}

SX::Data::IData* DetectorScene::getData()
{
    return _currentData;
}

void DetectorScene::changeCursorMode(int mode)
{
    _cursorMode=static_cast<CURSORMODE>(mode);
}

void DetectorScene::updatePeaks()
{

    if (!_currentData)
        return;

    for (auto peak : _peaks)
    {
        removeItem(peak);
        delete peak;
    }

    _peaks.clear();

    auto& peaks=_currentData->getPeaks();

    for (auto peak : peaks)
    {
        PeakGraphicsItem* p=new PeakGraphicsItem(peak);
        p->setFrame(_currentFrameIndex);
        addItem(p);
        _peaks.insert(p);
    }
}

