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
#include "CutSliceGraphicsItem.h"
#include "CutLineGraphicsItem.h"
#include <QtDebug>
#include "MaskGraphicsItem.h"

DetectorScene::DetectorScene(QObject *parent)
: QGraphicsScene(parent),
  _currentData(nullptr),
  _currentFrameIndex(0),
  _currentIntensity(10),
  _currentFrame(),
  _cursorMode(PIXEL),
  _mode(ZOOM),
  _currentCutter(nullptr),
  _itemSelected(false),
  _image(nullptr)
{
}

void DetectorScene::changeFrame(int frame)
{

    if (!_currentData)
        return;

    if (!_currentData->isMapped())
        _currentData->map();

    if (frame == _currentFrameIndex)
        return;
    _currentFrameIndex = frame;

    for (auto peak : _peaks)
        peak->setFrame(_currentFrameIndex);

    loadCurrentImage();
}

void DetectorScene::setMaxIntensity(int intensity)
{

    _currentIntensity = intensity;

    if (!_currentData)
        return;

    if (!_currentData->isMapped())
        _currentData->map();

    loadCurrentImage();
}

void DetectorScene::setData(SX::Data::IData* data)
{
    if (_currentData==data)
    {
        updatePeaks();
        return;
    }

    if (_currentData)
        _currentData->unMap();

    _currentData = data;

    if (!_currentData->isMapped())
        _currentData->map();

    SX::Instrument::Detector* det=_currentData->getDiffractometer()->getDetector();

     _zoomStack.clear();
     _zoomStack.push_back(QRect(0,0,det->getNCols(),det->getNRows()));


    if (_currentCutter)
     {
         removeItem(_currentCutter);
         delete _currentCutter;
         _currentCutter=nullptr;
     }

     loadCurrentImage();
     updatePeaks();

}

void DetectorScene::setData(SX::Data::IData* data, int frame)
{
    setData(data);
    changeFrame(frame);
}

void DetectorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    // If no data is loaded, do nothing
    if (!_currentData)
        return;

    createToolTipText(event);

    // The left button was pressed
    if (event->buttons() & Qt::LeftButton)
    {
        // Case of the Zoom mode, update the scene
        if (_mode==ZOOM)
        {
            QRectF zoom=_zoomrect->rect();
            zoom.setBottomRight(event->lastScenePos());
            _zoomrect->setRect(zoom);
        }
        // Case of the cutting modes
        else if (_mode==HORIZONTALSLICE || _mode==VERTICALSLICE || _mode==LINE)
        {
            if (!_currentCutter)
                return;
            _currentCutter->mouseMoveEvent(event);
            emit updatePlot(_currentCutter);
        }
        else if (_mode==MASK)
            _masks.last()->setTo(event->lastScenePos());
    }
    // No button was pressed, just a mouse move
    else if (!event->buttons())
    {
        QGraphicsScene::mouseMoveEvent(event);

        QGraphicsItem* gItem=itemAt(event->lastScenePos().toPoint(),QTransform());
        auto p=dynamic_cast<PlottableGraphicsItem*>(gItem);
        if (p)
            emit updatePlot(p);
    }
}

void DetectorScene::unselectItems()
{
    QList<QGraphicsItem*> items=selectedItems();
    for (auto item : items)
        item->setSelected(false);
    _itemSelected=false;
}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    // If no data is loaded, do nothing
    if (!_currentData)
        return;

    // The left button was pressed
    if (event->buttons() & Qt::LeftButton)
    {

        // Case of the Zoom mode
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
        }
        else
        {

            // Get the graphics item on which the user has clicked
            auto item=itemAt(event->lastScenePos(),QTransform());
            // If the item is not the background pixmap, return
            if (!dynamic_cast<QGraphicsPixmapItem*>(item))
                return;

            // Case of Cuttings mode (horizontal/vertical slices, line cut)
            if (_mode==HORIZONTALSLICE || _mode==VERTICALSLICE || _mode==LINE)
            {

                // If there is already a cutter item on the scene, delete it
                if (_currentCutter)
                {
                    delete _currentCutter;
                    _currentCutter=nullptr;
                }
                // Create the cutter item corresponding to the seleced cutting mode
                if (_mode==HORIZONTALSLICE)
                    _currentCutter=new CutSliceGraphicsItem(_currentData,true);
                else if (_mode==VERTICALSLICE)
                    _currentCutter=new CutSliceGraphicsItem(_currentData,false);
                else if (_mode==LINE)
                    _currentCutter=new CutLineGraphicsItem(_currentData);
                auto p=dynamic_cast<CutterGraphicsItem*>(_currentCutter);
                p->setFrom(event->lastScenePos());
                addItem(_currentCutter);
            }
            // Case of Mask mode
            else if (_mode==MASK)
            {
                MaskGraphicsItem* mask = new MaskGraphicsItem(_currentData);
                mask->setFrom(event->lastScenePos());
                mask->setTo(event->lastScenePos());
                _masks.append(mask);
                addItem(_masks.last());
            }
        }
    }
    // The right button was pressed
    else if (event->buttons() & Qt::RightButton)
    {

        // Get the graphics item on which the user has clicked
        auto item=itemAt(event->lastScenePos(),QTransform());
        if (dynamic_cast<QGraphicsPixmapItem*>(item))
        {
            if (_zoomStack.size()>1)
            {
                // Remove the last zoom area stored in the stack
                _zoomStack.pop();
                // If not root, then update the scene
                if (!_zoomStack.isEmpty())
                {
                    setSceneRect(_zoomStack.top());
                    emit dataChanged();
                }
            }
        }
        else
            item->setSelected(!item->isSelected());
    }
}

void DetectorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    // If no data is loaded, do nothing
    if (!_currentData)
        return;

    // The user released the left mouse button
    if (event->button() & Qt::LeftButton)
    {
        // Case of the Zoom mode, the zoom is updated and added on top of the zoom stack
        if(_mode==ZOOM)
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
        // Case of of the cutting modes, the cut plot are updated
        else if (_mode==HORIZONTALSLICE || _mode==VERTICALSLICE || _mode==LINE)
            emit updatePlot(_currentCutter);
        // Case of the Mask mode
        else if (_mode==MASK)
            updatePeaks();
    }
}

void DetectorScene::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    Q_UNUSED(event);

    if (!_currentData)
        return;

    // Zoo mode, does nothing
    if (_mode==ZOOM)
        return;
    // Cut modes, process the wheel event and update the cut plot
    else if (_mode==HORIZONTALSLICE || _mode==VERTICALSLICE || _mode==LINE)
    {
        if (!_currentCutter)
            return;
        _currentCutter->wheelEvent(event);
        emit updatePlot(_currentCutter);
    }

}

void DetectorScene::keyPressEvent(QKeyEvent* event)
{

    if (!_currentData)
        return;

//    switch (event->key())
//    {
//    case(Qt::Key_Delete):
//    {
//        QList<QGraphicsItem*> items=selectedItems();
//        int i=0;
//        for (auto item : items)
//        {
//            removeItem(item);
//            PeakGraphicsItem* peak=dynamic_cast<PeakGraphicsItem*>(item);
//            if (peak)
//            {
//                bool remove=_currentData->removePeak(peak->getPeak());
//                if (remove)
//                    i++;
//                qDebug() << "Removed "<< i << " peaks";
//                _peaks.erase(peak);
//            }

//            delete item;

//        }
//        break;
//    }
//    }

}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    auto instr=_currentData->getDiffractometer();
    SX::Instrument::Detector* det=instr->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

    std::size_t col=static_cast<std::size_t>(event->lastScenePos().x());
    std::size_t row=static_cast<std::size_t>(event->lastScenePos().y());

    if (col<0 || col>ncols-1 || row<0 || row>nrows-1)
        return;

    int intensity=_currentFrame(row,col);

    const auto& samplev=_currentData->getSampleState(_currentFrameIndex).getValues();
    const auto& detectorv=_currentData->getDetectorState(_currentFrameIndex).getValues();
    SX::Instrument::Sample* sample=instr->getSample();
    double wave=instr->getSource()->getWavelength();

    QString ttip;
    switch (_cursorMode)
    {
        case(PIXEL):
        {
            ttip=QString("(%1,%2) I:%3").arg(col).arg(row).arg(intensity);
            break;
        }
        case(GAMMA):
        {
            double gamma,nu;
            det->getGammaNu(col,row,gamma,nu,detectorv,sample->getPosition(samplev));
            ttip=QString("(%1,%2) I: %3").arg(gamma/SX::Units::deg).arg(nu/SX::Units::deg).arg(intensity);
            break;
        }
        case(THETA):
        {
            double th2=det->get2Theta(col,row,detectorv,Eigen::Vector3d(0,1.0/wave,0));
            ttip=QString("(%1) I: %2").arg(th2/SX::Units::deg).arg(intensity);
            break;
        }
        case(DSPACING):
        {
            double th2=det->get2Theta(col,row,detectorv,Eigen::Vector3d(0,1.0/wave,0));
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
    QImage image=Mat2QImage(_currentFrame.data(), nrows, ncols, full.left(), full.right(), full.top(), full.bottom(), _currentIntensity);
    if (!_image)
    {
        _image=addPixmap(QPixmap::fromImage(image));
        _image->setZValue(-1);
    }
    else
        _image->setPixmap(QPixmap::fromImage(image));

    setSceneRect(_zoomStack.back());
    emit dataChanged();
    emit updatePlot(_currentCutter);

}

SX::Data::IData* DetectorScene::getData()
{
    return _currentData;
}

const Eigen::MatrixXi& DetectorScene::getCurrentFrame() const
{
    return _currentFrame;
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

void DetectorScene::showPeakLabels(bool peaklabel)
{
    if (_peaks.size())
    {
        const auto& it=_peaks.begin();
        (*it)->setLabelVisible(peaklabel);
    }
    return;
}

