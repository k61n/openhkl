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
#include "PeakPlotter.h"


DetectorScene::DetectorScene(QObject *parent) :
    QGraphicsScene(parent),
    _image(nullptr),
    _currentData(nullptr),
    _currentFrameIndex(0),
    _currentIntensity(10),
    _currentFrame(),
    _cursorMode(PIXEL),
    _plotter(nullptr)
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
    _currentData = data;

    if (!_currentData)
        return ;

    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

     _zoomStack.clear();
     _zoomStack.push_back(QRect(0,0,ncols,nrows));
     loadCurrentImage();
     // Make sure that new bounding region is sent to the view
     setSceneRect(_image->boundingRect());

     for (auto p : _peaks)
     {
         removeItem(p);
     }
     _peaks.clear();
     _peaks.shrink_to_fit();
     auto& peaks=_currentData->getPeaks();
     _peaks.reserve(peaks.size());
     for (auto& peak : peaks)
     {
         PeakGraphicsItem* p=new PeakGraphicsItem(&peak);
         p->setFrame(_currentFrameIndex);
         addItem(p);
         _peaks.push_back(p);
     }
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

    if (event->buttons() & Qt::LeftButton)
    {
        QRectF zoom=_zoomrect->rect();
        zoom.setBottomRight(event->lastScenePos());
        _zoomrect->setRect(zoom);
    }
    QGraphicsScene::mouseMoveEvent(event);
    PeakGraphicsItem* item=dynamic_cast<PeakGraphicsItem*>(itemAt(event->lastScenePos().toPoint(),QTransform()));
    if (!item)
           return;
    if (!item->isVisible())
            return;
    if (!_plotter)
        _plotter=new PeakPlotter();
    _plotter->setPeak(item->getPeak());
    _plotter->show();
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
        emit dataChanged();
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
        _image=addPixmap(QPixmap::fromImage(image));
    else
        _image->setPixmap(QPixmap::fromImage(image));

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

