#include <ctime>
#include <vector>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QPixmap>
#include <QtDebug>
#include <QToolTip>

#include "Detector.h"
#include "IData.h"
#include "Sample.h"
#include "Source.h"
#include "Units.h"

#include "ColorMap.h"
#include "DetectorScene.h"
#include "GraphicsItems/PeakGraphicsItem.h"
#include "GraphicsItems/CutSliceGraphicsItem.h"
#include "GraphicsItems/CutLineGraphicsItem.h"
#include "GraphicsItems/MaskGraphicsItem.h"
#include "Detector.h"
#include "Sample.h"
#include "Gonio.h"

DetectorScene::DetectorScene(QObject *parent)
: QGraphicsScene(parent),
  _currentData(nullptr),
  _currentFrameIndex(0),
  _currentIntensity(10),
  _currentFrame(),
  _cursorMode(PIXEL),
  _mode(ZOOM),
  _zoomstart(0,0),
  _zoomend(0,0),
  _zoomrect(nullptr),
  _zoomStack(),
  _itemSelected(false),
  _image(nullptr),
  _peakGraphicsItems(),
  _masks(),
  _lastClickedGI(nullptr),
  _showPeakCalcs(false),
  _logarithmic(false)
{
    //setBspTreeDepth(4);
    qDebug() << "BSP tree depth = " << bspTreeDepth();
}

void DetectorScene::changeFrame(unsigned int frame)
{
    if (!_currentData)
        return;

    if (!_currentData->isOpened())
        _currentData->open();

    if (frame == _currentFrameIndex)
        return;

    _currentFrameIndex = frame;

//    for (auto& peak : _peaks)
//        (peak.second)->setFrame(_currentFrameIndex);

    updatePeaks();
    showPeakCalcs(_showPeakCalcs);

    loadCurrentImage();

    updateMasks(frame);
}

void DetectorScene::setMaxIntensity(int intensity)
{
    if (_currentIntensity==intensity)
        return;

    _currentIntensity = intensity;

    if (!_currentData)
        return;

    if (!_currentData->isOpened())
        _currentData->open();

    loadCurrentImage(false);
}

void DetectorScene::setData(std::shared_ptr<SX::Data::IData> data)
{
    _currentData = data;
    _currentData->open();

    std::shared_ptr<SX::Instrument::Detector> det=_currentData->getDiffractometer()->getDetector();

    _zoomStack.clear();
    _zoomStack.push_back(QRect(0,0,det->getNCols(),det->getNRows()));


    if (_lastClickedGI) {
        removeItem(_lastClickedGI);
        delete _lastClickedGI;
        _lastClickedGI=nullptr;
    }

    loadCurrentImage();
    updatePeaks();
}

void DetectorScene::setData(std::shared_ptr<SX::Data::IData> data, int frame)
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
    if (event->buttons() & Qt::LeftButton) {

        if (event->modifiers()==Qt::ControlModifier)
            return;

        // Case of the Zoom mode, update the scene
        if (_mode==ZOOM) {
            QRectF zoom=_zoomrect->rect();
            zoom.setBottomRight(event->lastScenePos());
            _zoomrect->setRect(zoom);
        } else {
            if (!_lastClickedGI)
                return;
            _lastClickedGI->mouseMoveEvent(event);

            if (auto p=dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI))
                emit updatePlot(p);
        }
    }
    // No button was pressed, just a mouse move
    else if (event->button() == Qt::NoButton) {
          //      jmf: testing follows
        QGraphicsItem* gItem=itemAt(event->lastScenePos().toPoint(), QTransform());
        auto p = dynamic_cast<PlottableGraphicsItem*>(gItem);
        if (p)
            emit updatePlot(p);
            QGraphicsScene::mouseMoveEvent(event);
    }

}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // If no data is loaded, do nothing
    if (!_currentData)
        return;

    // The left button was pressed
    if (event->buttons() & Qt::LeftButton) {

        // Get the graphics item on which the user has clicked
        auto item=itemAt(event->lastScenePos(),QTransform());

        if (event->modifiers() == Qt::ControlModifier) {
            item->setSelected(!item->isSelected());
            return;
        }

        PeakGraphicsItem* peak = dynamic_cast<PeakGraphicsItem*>(item);

        if (peak && _mode == INDEXING) {
            QMenu* menu = new QMenu();
            std::vector<Eigen::Vector3d> peaks = _indexer->index(*peak->getPeak());
            for (auto p : peaks) {
                std::ostringstream os;
                os << p;
                QAction* action = menu->addAction(os.str().c_str());
                connect(action, &QAction::triggered, [=] () {setPeakIndex(peak->getPeak(),p);});
            }
            menu->popup(event->screenPos());
        }

        // If the item is a NSXTools GI and is selectedit will become the current active GI
        if (auto p = dynamic_cast<SXGraphicsItem*>(item)) {
            if (p->isSelected()) {
                _lastClickedGI = p;
                return;
            }
        }

        // Case of the Zoom mode
        if (_mode==ZOOM) {
            _zoomstart=event->lastScenePos().toPoint();
            _zoomend=_zoomstart;
            QRect zoom(_zoomstart,_zoomend);
            _zoomrect=addRect(zoom);
            _zoomrect->setParentItem(_image);
            QPen pen1(QBrush(QColor("gray")),1.0);
            pen1.setWidth(1);
            pen1.setCosmetic(true);
            _zoomrect->setPen(pen1);
            _zoomrect->setBrush(QBrush(QColor(255,0,0,30)));
        }
        else {
            // Case of Cuttings mode (horizontal/vertical slices, line cut)
            if (_mode==HORIZONTALSLICE || _mode==VERTICALSLICE || _mode==LINE) {
                // Create the cutter item corresponding to the seleced cutting mode
                CutterGraphicsItem* cutter;
                if (_mode==HORIZONTALSLICE)
                    cutter=new CutSliceGraphicsItem(_currentData,true);
                else if (_mode==VERTICALSLICE)
                    cutter=new CutSliceGraphicsItem(_currentData,false);
                else
                    cutter=new CutLineGraphicsItem(_currentData);
                cutter->setFrom(event->lastScenePos());
                addItem(cutter);
                _lastClickedGI=cutter;
            }
            // Case of Mask mode
            else if (_mode==MASK)
            {
                MaskGraphicsItem* mask = new MaskGraphicsItem(_currentData, new AABB<double, 3>);
                mask->setFrom(event->lastScenePos());
                mask->setTo(event->lastScenePos());
                addItem(mask);
                _lastClickedGI=mask;
            }
        }
    }
    // The right button was pressed
    else if (event->buttons() & Qt::RightButton) {
        if (_zoomStack.size()>1) {
            // Remove the last zoom area stored in the stack
            _zoomStack.pop();
            // If not root, then update the scene
            if (!_zoomStack.isEmpty()) {
                setSceneRect(_zoomStack.top());
                emit dataChanged();
            }
        }
    }
}

void DetectorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // If no data is loaded, do nothing
    if (!_currentData)
        return;

    // The user released the left mouse button
    if (event->button() & Qt::LeftButton) {
        if (event->modifiers()==Qt::ControlModifier)
            return;

        // Case of the Zoom mode, the zoom is updated and added on top of the zoom stack
        if(_mode==ZOOM) {
            qreal top=_zoomrect->rect().top();
            qreal bot=_zoomrect->rect().bottom();
            qreal left=_zoomrect->rect().left();
            qreal right=_zoomrect->rect().right();
            if (top==bot || left==right)
                return;
            if (top > bot)
                std::swap(top,bot);
            if (right < left)
                std::swap(left,right);
            QRect max=_zoomStack.front();
            if (top<max.top())
                top=max.top();
            if (bot>max.bottom())
                bot=max.bottom()+1;
            if (left<max.left())
                left=max.left();
            if (right>max.right())
                right=max.right()+1;

            _zoomrect->setRect(left,top,right-left,bot-top);
            setSceneRect(_zoomrect->rect());
            removeItem(_zoomrect);
            _zoomStack.push_back(_zoomrect->rect().toRect());
            emit dataChanged();
        } else {
            if (auto p=dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI))
                emit updatePlot(p);
            else if (auto p=dynamic_cast<MaskGraphicsItem*>(_lastClickedGI)) {
                // add a new mask
                if (std::find(_masks.begin(), _masks.end(), p) == _masks.end()) {
                    _currentData->addMask(p->getAABB());
                    removeItem(p);
                    delete p;
                    _lastClickedGI = nullptr;
                }
                _currentData->maskPeaks();
                update();
                updateMasks(_currentFrameIndex);
            }
        }
    }
}

void DetectorScene::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    // If no data, returns
    if (!_currentData)
        return;

    // Get the graphics item on which the user has performed the wheel event
    auto item=itemAt(event->scenePos(),QTransform());

    auto p=dynamic_cast<SXGraphicsItem*>(item);

    if (!p)
        return;

    if (!(p->isSelected()))
        return;

    if (auto p=dynamic_cast<CutterGraphicsItem*>(item)) {
        p->wheelEvent(event);
        emit updatePlot(p);
    }
}

void DetectorScene::keyPressEvent(QKeyEvent* event)
{
    // If no data, returns
    if (!_currentData)
        return;

    // The user pressed on Delete key
    if (event->key() == Qt::Key_Delete) {
        QList<QGraphicsItem*> items=selectedItems();
        int nPeaksErased = _peakGraphicsItems.size();
        for (auto item: items) {
            auto p=dynamic_cast<SXGraphicsItem*>(item);
            // The item must be deletable ... to be deleted
            if (!p->isDeletable())
                continue;

            // If the item is a peak graphics item, remove its corresponding peak from the data,
            // update the set of peak graphics items and update the scene
            if (auto p=dynamic_cast<PeakGraphicsItem*>(item)) {
                bool remove=_currentData->removePeak(p->getPeak());
                if (remove) {
                    _peakGraphicsItems.erase(p->getPeak());
                }
            }
            // If the item is a mask graphics item, remove its corresponding mask from the data,
            // update the QList of mask graphics items and update the scene
            else if (auto p=dynamic_cast<MaskGraphicsItem*>(item)) {
                _currentData->removeMask(p->getAABB());
                _masks.removeOne(p);
            }
            if (p==_lastClickedGI)
                _lastClickedGI=nullptr;
            // Remove the item from the scene
            removeItem(item);
            // Delete the item
            delete item;
        }
        // Computes the new number of peaks, and if it changes log it
        nPeaksErased -= _peakGraphicsItems.size();
        if (nPeaksErased > 0)
            qDebug() << "Removed "<< nPeaksErased << " peaks";
    }
}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    if (!_currentData)
        return;

    auto instr=_currentData->getDiffractometer();
    std::shared_ptr<SX::Instrument::Detector> det=instr->getDetector();


    int nrows=det->getNRows();
    int ncols=det->getNCols();

    int col=static_cast<int>(event->lastScenePos().x());
    int row=static_cast<int>(event->lastScenePos().y());

    if (col<0 || col>ncols-1 || row<0 || row>nrows-1)
        return;

    int intensity=_currentFrame(row,col);

    const auto& samplev=_currentData->getSampleState(_currentFrameIndex).getValues();
    const auto& detectorv=_currentData->getDetectorState(_currentFrameIndex).getValues();
    std::shared_ptr<SX::Instrument::Sample> sample=instr->getSample();
    double wave=instr->getSource()->getWavelength();

    QString ttip;
    switch (_cursorMode) {
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
            if (_cell) {
                auto detector=_currentData->getDiffractometer()->getDetector();
                auto sample=_currentData->getDiffractometer()->getSample();
                auto source=_currentData->getDiffractometer()->getSource();
                auto Qvec=detector->getQ(col,row,source->getWavelength(),_currentData->getDetectorState(_currentFrameIndex).getValues(),sample->getPosition(_currentData->getSampleState(_currentFrameIndex).getValues()));
                sample->getGonio()->transformInverseInPlace(Qvec,_currentData->getSampleState(_currentFrameIndex).getValues());
                auto hkl=_cell->fromReciprocalStandard(Qvec);
                ttip=QString("(%1,%2,%3) I: %4").arg(hkl[0]).arg(hkl[1]).arg(hkl[2]).arg(intensity);
                 break;
            }

        }

    }
    QToolTip::showText(event->screenPos(),ttip);
}

void DetectorScene::changeInteractionMode(int mode)
{
    _mode=static_cast<MODE>(mode);
}

void DetectorScene::loadCurrentImage(bool newimage)
{
    // Full image size, front of the stack
    QRect& full=_zoomStack.front();

    std::shared_ptr<SX::Instrument::Detector> det=_currentData->getDiffractometer()->getDetector();
    std::size_t nrows=det->getNRows();
    std::size_t ncols=det->getNCols();

    if (_currentFrameIndex>=_currentData->getNFrames())
        _currentFrameIndex=_currentData->getNFrames()-1;
    if (newimage)
        _currentFrame =_currentData->getFrame(_currentFrameIndex);

    if (!_image) {
        _image=addPixmap(QPixmap::fromImage(Mat2QImage(_currentFrame.data(), nrows, ncols,
                                                       full.left(), full.right(), full.top(), full.bottom(),
                                                       _currentIntensity, _logarithmic)));
        _image->setZValue(-1);
    } else
        _image->setPixmap(QPixmap::fromImage(Mat2QImage(_currentFrame.data(), nrows, ncols,
                                                        full.left(), full.right(), full.top(), full.bottom(),
                                                        _currentIntensity, _logarithmic)));

    setSceneRect(_zoomStack.back());
    emit dataChanged();

    if (auto p=dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI))
        emit updatePlot(p);
}

std::shared_ptr<SX::Data::IData> DetectorScene::getData()
{
    return _currentData;
}

const rowMatrix& DetectorScene::getCurrentFrame() const
{
    return _currentFrame;
}

void DetectorScene::changeCursorMode(int mode)
{
    _cursorMode=static_cast<CURSORMODE>(mode);
}

PeakGraphicsItem* DetectorScene::findPeakGraphicsItem(sptrPeak3D peak)
{
    auto it=_peakGraphicsItems.find(peak);
    if (it!=_peakGraphicsItems.end())
        return it->second;
    else
        return nullptr;

}

void DetectorScene::updatePeaks()
{
    if (!_currentData)
        return;

    clearPeaks();

    auto& peaks=_currentData->getPeaks();

    for (auto peak : peaks) {
        const Eigen::Vector3d& l = peak->getPeak()->getLower();
        const Eigen::Vector3d& u = peak->getPeak()->getUpper();

        if (_currentFrameIndex < l[2] || _currentFrameIndex > u[2])
            continue;

        PeakGraphicsItem* pgi = new PeakGraphicsItem(peak);
        pgi->setFrame(_currentFrameIndex);
        addItem(pgi);
        _peakGraphicsItems.insert(std::pair<sptrPeak3D, PeakGraphicsItem*>(peak,pgi));
    }
}

void DetectorScene::updatePeakCalcs()
{
    clock_t start = clock();

    if (!_currentData)
        return;

    _precalculatedPeaks.clear();

    //qDebug() << "BSP tree depth = " << bspTreeDepth();

    //setBspTreeDepth(8);

    auto sample=_currentData->getDiffractometer()->getSample();
    int ncrystals=sample->getNCrystals();
    if (ncrystals) {
        for (int i = 0; i < ncrystals; ++i) {
            SX::Crystal::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());

            auto ub=sample->getUnitCell(i)->getReciprocalStandardM();
            auto hkls=sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
            std::vector<SX::Crystal::PeakCalc> peaks=_currentData->hasPeaks(hkls,ub);
            _precalculatedPeaks.reserve(_precalculatedPeaks.size() + peaks.size());

            for(auto&& p: peaks)
                _precalculatedPeaks.push_back(p);
        }
    }

    showPeakCalcs(_showPeakCalcs);

    qDebug() << "number of calculated peaks " << _peakCalcs.size();
    clock_t end = clock();
    qDebug() << "ELAPSED TIME = "<<static_cast<double>((end-start))/CLOCKS_PER_SEC;
    qDebug() << "BSP tree depth = " << bspTreeDepth();

}

void DetectorScene::clearPeaks()
{
    if (!_currentData)
        return;

    for (auto& peak : _peakGraphicsItems) {
        removeItem(peak.second);
        delete peak.second;
    }

    _peakGraphicsItems.clear();
}

void DetectorScene::updateMasks(unsigned int frame)
{
    _lastClickedGI = nullptr;

    for (auto&& mask: _masks) {
        removeItem(mask);
        delete mask;
    }

    _masks.clear();

    for (auto&& mask: _currentData->getMasks()) {
        const Eigen::Vector3d lower(mask->getLower());
        const Eigen::Vector3d upper(mask->getUpper());

        // mask is out of the frame
        if (frame > upper[2] || frame < lower[2])
            continue;

        MaskGraphicsItem* maskItem = new MaskGraphicsItem(_currentData, mask);
        maskItem->setFrom(QPoint(lower(0), lower(1)));
        maskItem->setTo(QPoint(upper(0), upper(1)));
        addItem(maskItem);
        _masks.push_back(maskItem);
    }
}

void DetectorScene::showPeakLabels(bool peaklabel)
{
    if (!_peakGraphicsItems.empty()) {
        const auto& it=_peakGraphicsItems.begin();
        it->second->setLabelVisible(peaklabel);
    }

    if (!_peakCalcs.empty()) {
        const auto& it = _peakCalcs.begin();
        (*it)->setLabelVisible(peaklabel);
    }
}

void DetectorScene::drawPeakBackground(bool flag)
{
    if (!_peakGraphicsItems.empty()) {
        const auto& it=_peakGraphicsItems.begin();
        it->second->drawBackground(flag);
    }
}

void DetectorScene::activateIndexingMode(std::shared_ptr<SX::Crystal::UnitCell> cell)
{
    _mode = INDEXING;
    _cell = cell;
    _indexer = std::unique_ptr<SX::Crystal::Indexer>(new SX::Crystal::Indexer(_cell));
}

void DetectorScene::setPeakIndex(sptrPeak3D peak, const Eigen::Vector3d &index)
{
    peak->setMillerIndices(index[0],index[1],index[2]);
}

void DetectorScene::showPeakCalcs(bool flag)
{
    _showPeakCalcs = flag;

    if (!flag) {
        for (auto&& p: _peakCalcs)
            delete p;

        _peakCalcs.clear();
        _precalculatedPeaks.clear();
        return;
    }

    if (_precalculatedPeaks.empty())
        updatePeakCalcs();

    for (auto& peak : _peakCalcs) {
        removeItem(peak);
        delete peak;
    }

    _peakCalcs.resize(0);

    for (auto&& p : _precalculatedPeaks) {
        if ( std::abs(_currentFrameIndex-p._frame) > 1.0)
            continue;

            PeakCalcGraphicsItem* peak = new PeakCalcGraphicsItem(p);
            peak->setVisible(flag);
            peak->setFrame(_currentFrameIndex);
            peak->setEnabled(false); // testing
            addItem(peak);
            _peakCalcs.push_back(peak);
    }
}


void DetectorScene::setLogarithmic(bool checked)
{
    _logarithmic = checked;
}
