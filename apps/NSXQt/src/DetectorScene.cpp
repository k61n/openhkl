#include <vector>

#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

#include "ColorMap.h"
#include "Detector.h"
#include "DetectorScene.h"
#include "IData.h"
#include <QToolTip>

DetectorScene::DetectorScene(QObject *parent) : QGraphicsScene(parent), _currentData(nullptr), _currentFrameIndex(0), _currentIntensity(10), _currentFrame()
{
}

void DetectorScene::setCurrentFrame(int frame)
{

    if (_currentData)
    {
        if (frame > _currentData->getNFrames())
        {

        }
    }
}

void DetectorScene::changeFrame(int frame)
{

    if (!_currentData)
        return;

    if (frame == _currentFrameIndex)
        return;

    _currentFrameIndex = frame;

    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

    QRect& last=_zoomStack.back();
    _currentFrame.clear();
    _currentFrame.shrink_to_fit();
    _currentFrame =_currentData->getFrame(frame);
    QImage image=Mat2QImage(&_currentFrame[0], nrows, ncols, last.left(), last.right(), last.top(), last.bottom(), _currentIntensity);
    if (_image)
    {
        removeItem(_image);
        delete _image;
    }
    _image=addPixmap(QPixmap::fromImage(image));
    setSceneRect(_image->boundingRect());

}

void DetectorScene::setMaxIntensity(int intensity)
{

    _currentIntensity = intensity;

    if (!_currentData)
        return;

    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

    QRect& last=_zoomStack.back();

    QImage image=Mat2QImage(&_currentFrame[0], nrows, ncols, last.left(), last.right(), last.top(), last.bottom(), _currentIntensity);

    if (_image)
    {
        removeItem(_image);
        delete _image;
    }
    _image=addPixmap(QPixmap::fromImage(image));
    setSceneRect(_image->boundingRect());

}

void DetectorScene::setData(SX::Data::IData* data)
{

    if (data != _currentData)
        _currentData = data;

    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

     _zoomStack.clear();
     _zoomStack.push_back(QRect(0,0,ncols,nrows));

    QRect& last=_zoomStack.back();

    _currentFrame =_currentData->getFrame(_currentFrameIndex);
    QImage image=Mat2QImage(&_currentFrame[0], nrows, ncols, last.left(), last.right(), last.top(), last.bottom(), _currentIntensity);
    _image=addPixmap(QPixmap::fromImage(image));
    setSceneRect(_image->boundingRect());

    emit hasReceivedData();

}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // Zoom mode starts
    if (event->buttons() & Qt::LeftButton)
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
    } // Move up in the zoom stack
    else if (event->buttons() & Qt::RightButton)
    {
        if (_zoomStack.size()>1)
        {
            _zoomStack.pop(); // Go the previous zoom mode
            if (!_zoomStack.isEmpty()) // If not root, then assign
            {
                setSceneRect(_zoomStack.top());
                emit zoomChanged();
            }
        }
    }
}

void DetectorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!_currentData)
        return;

    createToolTipText(event);

    if (event->buttons() & Qt::LeftButton)
    {
        QRectF zoom=_zoomrect->rect();
        zoom.setBottomRight(event->lastScenePos());
        _zoomrect->setRect(zoom);
    }
}

void DetectorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
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
        emit zoomChanged();
    }
}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    QString ttip;
    int xpos=static_cast<int>(event->scenePos().x());
    int ypos=static_cast<int>(event->scenePos().y());
    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    int intensity=_currentFrame[xpos*nrows+ypos];
    ttip=QString("(%1,%2) : %3").arg(xpos).arg(ypos).arg(intensity);
    QToolTip::showText(event->screenPos(),ttip);
}
