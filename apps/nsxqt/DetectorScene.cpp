#include <ctime>
#include <vector>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QPixmap>
#include <QtDebug>
#include <QToolTip>

#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/crystal/PeakCalc.h>
#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Detector.h>
#include <nsxlib/instrument/DetectorEvent.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Gonio.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/Source.h>
#include <nsxlib/utils/Units.h>

#include "ColorMap.h"
#include "DetectorScene.h"
#include "items/PeakGraphicsItem.h"
#include "items/CutSliceGraphicsItem.h"
#include "items/CutLineGraphicsItem.h"
#include "items/MaskGraphicsItem.h"

// compile-time constant to determine whether to draw the peak masks
static const bool g_drawMask = true;

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
  _logarithmic(false),
  _colormap(new ColorMap()),
  _integrationRegion(nullptr),
  _drawIntegrationRegion(false)
{
    //setBspTreeDepth(4);
    qDebug() << "BSP tree depth = " << bspTreeDepth();
}

void DetectorScene::changeFrame(size_t frame)
{
    if (!_currentData) {
        return;
    }
    if (!_currentData->isOpened()) {
        _currentData->open();
    }
    if (frame == _currentFrameIndex) {
        return;
    }
    _currentFrameIndex = frame;
    updatePeaks();
    showPeakCalcs(_showPeakCalcs);
    loadCurrentImage();
    updateMasks(frame);
}

void DetectorScene::setMaxIntensity(int intensity)
{
    if (_currentIntensity==intensity) {
        return;
    }
    _currentIntensity = intensity;

    if (!_currentData) {
        return;
    }
    if (!_currentData->isOpened()) {
        _currentData->open();
    }
    loadCurrentImage(false);
}

void DetectorScene::setData(const nsx::sptrDataSet& data)
{
    _currentData = data;

    if (!_currentData) {
        return;
    }

    _currentData->open();
    auto det = _currentData->getDiffractometer()->getDetector();
    _zoomStack.clear();
    _zoomStack.push_back(QRect(0,0,int(det->getNCols()),int(det->getNRows())));

    if (_lastClickedGI != nullptr) {
        removeItem(_lastClickedGI);
//        delete _lastClickedGI;
        _lastClickedGI=nullptr;
    }
    loadCurrentImage();
    updatePeaks();
}

void DetectorScene::setData(const nsx::sptrDataSet& data, size_t frame)
{
    setData(data);
    changeFrame(frame);
}

void DetectorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // jmf debugging
//    auto the_items = items();
//    qDebug() << "graphics scene has " << the_items.size() << " items";

//    for (auto&& item: the_items) {
//        auto rect = item->boundingRect();
//    }


    // If no data is loaded, do nothing
    if (!_currentData) {
        return;
    }
    createToolTipText(event);
    auto button = event->button();

    // The left button was pressed
    if (event->buttons() & Qt::LeftButton) {

        if (event->modifiers()==Qt::ControlModifier) {
            return;
        }
        // Case of the Zoom mode, update the scene
        if (_mode==ZOOM) {
            QRectF zoom=_zoomrect->rect();
            zoom.setBottomRight(event->lastScenePos());
            _zoomrect->setRect(zoom);
            return;
        }
        if (_lastClickedGI == nullptr) {
            return;
        }
        _lastClickedGI->mouseMoveEvent(event);
        auto p = dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI);

        if (p != nullptr) {
            emit updatePlot(p);
        }
    }
    // No button was pressed, just a mouse move
    else if (event->button() == Qt::NoButton) {
          //      jmf: testing follows
        auto lastPos = event->lastScenePos();
        auto point = lastPos.toPoint();
        QTransform trans;
        QGraphicsItem* gItem = itemAt(point, trans);
        auto p = dynamic_cast<PlottableGraphicsItem*>(gItem);
        if (p != nullptr) {
            emit updatePlot(p);
            QGraphicsScene::mouseMoveEvent(event);
        }
    }
}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    CutterGraphicsItem* cutter(nullptr);
    MaskGraphicsItem* mask(nullptr);
    QPen pen1;

    // If no data is loaded, do nothing
    if (!_currentData) {
        return;
    }
    // The left button was pressed
    if (event->buttons() & Qt::LeftButton) {
        // Get the graphics item on which the user has clicked
        auto item=itemAt(event->lastScenePos(),QTransform());

        if (event->modifiers() == Qt::ControlModifier) {
            item->setSelected(!item->isSelected());
            return;
        }
        // If the item is a NSXTools GI and is selectedit will become the current active GI
        if (auto p = dynamic_cast<SXGraphicsItem*>(item)) {
            if (p->isSelected()) {
                _lastClickedGI = p;
                return;
            }
        }
        switch(_mode) {
        // Case of the Zoom mode
        case ZOOM:
            _zoomstart = event->lastScenePos().toPoint();
            _zoomend = _zoomstart;
            _zoomrect = addRect(QRect(_zoomstart,_zoomend));
            // _zoomrect->setParentItem(_image);
            pen1 = QPen(QBrush(QColor("gray")),1.0);
            pen1.setWidth(1);
            pen1.setCosmetic(true);
            _zoomrect->setPen(pen1);
            _zoomrect->setBrush(QBrush(QColor(255,0,0,30)));
            break;
            // Case of Cuttings mode (horizontal/vertical slices, line cut)
        case HORIZONTALSLICE:
            cutter=new CutSliceGraphicsItem(_currentData,true);
            break;
        case VERTICALSLICE:
            cutter=new CutSliceGraphicsItem(_currentData,false);
            break;
        case LINE:
            cutter=new CutLineGraphicsItem(_currentData);
            break;
            // Case of Mask mode
        case MASK:
            mask = new MaskGraphicsItem(_currentData, new nsx::AABB<double, 3>);
            mask->setFrom(event->lastScenePos());
            mask->setTo(event->lastScenePos());
            addItem(mask);
            _lastClickedGI=mask;
            break;
        case INDEXING:
#pragma warning "todo: implement this case"
            break;
        }
        if (cutter != nullptr) {
            cutter->setFrom(event->lastScenePos());
            addItem(cutter);
            _lastClickedGI=cutter;
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
    if (!_currentData) {
        return;
    }
    // The user released the left mouse button
    if (event->button() & Qt::LeftButton) {
        if (event->modifiers()==Qt::ControlModifier) {
            return;
        }
        // Case of the Zoom mode, the zoom is updated and added on top of the zoom stack
        if(_mode==ZOOM) {
            qreal top = _zoomrect->rect().top();
            qreal bot = _zoomrect->rect().bottom();
            qreal left = _zoomrect->rect().left();
            qreal right = _zoomrect->rect().right();

            if (qAbs(top-bot)<1e-10 || qAbs(left-right)<1e-10) {
                return;
            }
            if (top > bot) {
                std::swap(top,bot);
            }
            if (right < left) {
                std::swap(left,right);
            }
            QRect max=_zoomStack.front();
            if (top<max.top()) {
                top=max.top();
            }
            if (bot>max.bottom()) {
                bot=max.bottom()+1;
            }
            if (left<max.left()) {
                left=max.left();
            }
            if (right>max.right()) {
                right=max.right()+1;
            }
            _zoomrect->setRect(left,top,right-left,bot-top);
            setSceneRect(_zoomrect->rect());
            removeItem(_zoomrect);
            _zoomStack.push_back(_zoomrect->rect().toRect());
            emit dataChanged();
        } else {
            if (auto p=dynamic_cast<CutterGraphicsItem*>(_lastClickedGI)) {
                // check with tolerance
                //if (p->to() == p->from()) {
                if (true) {
                    // delete p....
                     _lastClickedGI = nullptr;
                    removeItem(p);
//                    delete p;
                }
                else {
                    emit updatePlot(p);
                }
            }
            else if (auto p=dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI)) {
                emit updatePlot(p);
            }
            else if (auto p=dynamic_cast<MaskGraphicsItem*>(_lastClickedGI)) {
                // add a new mask
                if (std::find(_masks.begin(), _masks.end(), p) == _masks.end()) {
                    _currentData->addMask(p->getAABB());
                    _lastClickedGI = nullptr;
                    removeItem(p);
//                    delete p;
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
    if (!_currentData) {
        return;
    }
    // Get the graphics item on which the user has performed the wheel event
    auto item=itemAt(event->scenePos(),QTransform());
    auto p = dynamic_cast<SXGraphicsItem*>(item);

    if (p == nullptr) {
        return;
    }
    if (!(p->isSelected())) {
        return;
    }
    auto q = dynamic_cast<CutterGraphicsItem*>(item);
    if (q != nullptr) {
        q->wheelEvent(event);
        emit updatePlot(q);
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
        int nPeaksErased = int(_peakGraphicsItems.size());
        for (auto item: items) {
            auto p = dynamic_cast<SXGraphicsItem*>(item);

            if (p == nullptr) {
                continue;
            }

            // The item must be deletable ... to be deleted
            if (!p->isDeletable()) {
                continue;
            }
            // If the item is a peak graphics item, remove its corresponding peak from the data,
            // update the set of peak graphics items and update the scene
            if (auto p = dynamic_cast<PeakGraphicsItem*>(item)) {
                bool remove=_currentData->removePeak(p->getPeak());
                if (remove) {
                    _peakGraphicsItems.erase(p->getPeak());
                }
            }
            // If the item is a mask graphics item, remove its corresponding mask from the data,
            // update the QList of mask graphics items and update the scene
            else if (auto p = dynamic_cast<MaskGraphicsItem*>(item)) {
                _currentData->removeMask(p->getAABB());
                _masks.removeOne(p);
            }
            if (p == _lastClickedGI) {
                _lastClickedGI=nullptr;
            }
            // Remove the item from the scene
            removeItem(item);
            // Delete the item
//            delete item;
        }
        // Computes the new number of peaks, and if it changes log it
        nPeaksErased -= _peakGraphicsItems.size();
        if (nPeaksErased > 0) {
            qDebug() << "Removed "<< nPeaksErased << " peaks";
        }
    }
}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    if (!_currentData) {
        return;
    }
    auto instr=_currentData->getDiffractometer();
    auto det=instr->getDetector();

    int nrows = int(det->getNRows());
    int ncols = int(det->getNCols());

    int col = static_cast<int>(event->lastScenePos().x());
    int row = static_cast<int>(event->lastScenePos().y());

    if (col<0 || col>ncols-1 || row<0 || row>nrows-1) {
        return;
    }
    int intensity=_currentFrame(row,col);

    const auto& samplev=_currentData->getSampleState(_currentFrameIndex).getValues();
    const auto& detectorv=_currentData->getDetectorState(_currentFrameIndex).getValues();
    auto sample=instr->getSample();
    auto& mono = instr->getSource()->getSelectedMonochromator();
    double wave=mono.getWavelength();

    QString ttip;
    double gamma, nu, th2;
    switch (_cursorMode) {
    case PIXEL:
        ttip = QString("(%1,%2) I:%3").arg(col).arg(row).arg(intensity);
        break;
    case GAMMA:
        nsx::DetectorEvent(*det, col, row, detectorv).getGammaNu(gamma, nu, sample->getPosition(samplev));
        ttip = QString("(%1,%2) I: %3").arg(gamma/nsx::deg).arg(nu/nsx::deg).arg(intensity);
        break;
    case THETA:
        th2 = nsx::DetectorEvent(*det, col, row, detectorv).get2Theta(Eigen::Vector3d(0, 1.0/wave, 0));
        ttip = QString("(%1) I: %2").arg(th2/nsx::deg).arg(intensity);
        break;
    case DSPACING:
        // th2 = det->get2Theta(col, row, detectorv, Eigen::Vector3d(0, 1.0/wave, 0));
        th2 = nsx::DetectorEvent(*det, col, row, detectorv).get2Theta(Eigen::Vector3d(0, 1.0/wave, 0));
        ttip = QString("(%1) I: %2").arg(wave/(2*sin(0.5*th2))).arg(intensity);
        break;
    case HKL:
#pragma warning "todo: implement this case"
        break;
    }
    QToolTip::showText(event->screenPos(),ttip);
}

void DetectorScene::changeInteractionMode(int mode)
{
    _mode = static_cast<MODE>(mode);
}

void DetectorScene::loadCurrentImage(bool newimage)
{
    const unsigned int red = (128u << 24) | (255u << 16);
    const unsigned int green = (128u << 24) | (255u << 8);
    const unsigned int yellow = (128u << 24) | (255u << 16) | (255u << 8);
    const unsigned int transparent = 0;

    if (_currentData == nullptr) {
        return;
    }

    // Full image size, front of the stack
    QRect& full = _zoomStack.front();

    auto det = _currentData->getDiffractometer()->getDetector();

    if (_currentFrameIndex >= _currentData->getNFrames()) {
        _currentFrameIndex = _currentData->getNFrames()-1;
    }
    if (newimage) {
        _currentFrame =_currentData->getFrame(_currentFrameIndex);
    }
    if (_image == nullptr) {
        _image = addPixmap(QPixmap::fromImage(_colormap->matToImage(_currentFrame.cast<double>(), full, _currentIntensity, _logarithmic)));
        _image->setZValue(-2);
    } else {
        _image->setPixmap(QPixmap::fromImage(_colormap->matToImage(_currentFrame.cast<double>(), full, _currentIntensity, _logarithmic)));
    }

    // update the integration region pixmap
    if (_drawIntegrationRegion && g_drawMask) {
        const int ncols = _currentData->getNCols();
        const int nrows = _currentData->getNRows();

        QImage region_img(ncols, nrows, QImage::Format_ARGB32);

        for (auto c = 0; c < ncols; ++c) {
            for (auto r = 0; r < nrows; ++r) {
                region_img.setPixel(c, r, transparent);
            }
        }

        Eigen::MatrixXi mask(nrows, ncols);
        mask.setZero();

        for (auto&& peak: _currentData->getPeaks()) {
            peak->getIntegrationRegion().updateMask(mask, _currentFrameIndex);
        }

        for (auto&& peak: _currentData->getPeaks()) {
            auto&& region = peak->getIntegrationRegion();
            auto&& lower = region.getBackground().getLower();
            auto&& upper = region.getBackground().getUpper();

            if (_currentFrameIndex < std::floor(lower[2])) {
                continue;
            }

            if (_currentFrameIndex > std::ceil(upper[2])) {
                continue;
            }

            long xmin = std::max(0l, std::lround(std::floor(lower[0])));
            long ymin = std::max(0l, std::lround(std::floor(lower[1])));


            long xmax = std::min(long(_currentData->getNCols()), std::lround(std::ceil(upper[0]))+1);
            long ymax = std::min(long(_currentData->getNRows()), std::lround(std::ceil(upper[1]))+1);

            for (auto x = xmin; x < xmax; ++x) {
                for (auto y = 0; y < ymax; ++y) {
                    Eigen::Vector4d p(x, y, _currentFrameIndex, 1);

                    if (region.inRegion(p)) {
                        region_img.setPixel(x, y, green);
                    }
                    if (region.inBackground(p) && (mask(y,x) == 0)) {
                        region_img.setPixel(x, y, yellow);
                    }
                }
            }
        }

        if (_integrationRegion == nullptr) {
            _integrationRegion = addPixmap(QPixmap::fromImage(region_img));
            _integrationRegion->setZValue(-1);
        } else {
            _integrationRegion->setPixmap(QPixmap::fromImage(region_img));
        }
    }

    setSceneRect(_zoomStack.back());
    emit dataChanged();

    if (auto p = dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI)) {
        emit updatePlot(p);
    }
}

nsx::sptrDataSet DetectorScene::getData()
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

PeakGraphicsItem* DetectorScene::findPeakGraphicsItem(const nsx::sptrPeak3D& peak)
{
    auto it = _peakGraphicsItems.find(peak);

    if (it != _peakGraphicsItems.end()) {
        return it->second;
    }
    return nullptr;
}

void DetectorScene::updatePeaks()
{
    if (!_currentData) {
        return;
    }
    clearPeaks();
    auto& peaks = _currentData->getPeaks();

    for (auto&& peak : peaks) {
        const Eigen::Vector3d& l = peak->getShape().getLower();
        const Eigen::Vector3d& u = peak->getShape().getUpper();

        if (_currentFrameIndex < l[2] || _currentFrameIndex > u[2]) {
            continue;
        }
        PeakGraphicsItem* pgi = new PeakGraphicsItem(peak);
        pgi->setFrame(_currentFrameIndex);
        addItem(pgi);
        _peakGraphicsItems.insert(std::pair<nsx::sptrPeak3D, PeakGraphicsItem*>(peak,pgi));
    }
}

void DetectorScene::updatePeakCalcs()
{
    clock_t start = clock();

    if (!_currentData) {
        return;
    }
    _precalculatedPeaks.clear();
    auto sample=_currentData->getDiffractometer()->getSample();
    size_t ncrystals = sample->getNCrystals();

    if (ncrystals) {
        for (unsigned int i = 0; i < ncrystals; ++i) {
            nsx::SpaceGroup group(sample->getUnitCell(i)->getSpaceGroup());
            auto ub=sample->getUnitCell(i)->getReciprocalStandardM();
            auto hkls=sample->getUnitCell(i)->generateReflectionsInSphere(1.5);
            std::vector<nsx::PeakCalc> peaks=_currentData->hasPeaks(hkls,ub);
            _precalculatedPeaks.reserve(_precalculatedPeaks.size() + peaks.size());

            for(auto&& p: peaks) {
                _precalculatedPeaks.push_back(p);
            }
        }
    }
    qDebug() << "number of calculated peaks " << _peakCalcs.size();
    clock_t end = clock();
    qDebug() << "ELAPSED TIME = " << static_cast<double>((end-start))/CLOCKS_PER_SEC;
    qDebug() << "BSP tree depth = " << bspTreeDepth();
}

void DetectorScene::redrawImage()
{
    loadCurrentImage(false);
}

void DetectorScene::clearPeaks()
{
    if (!_currentData) {
        return;
    }

    for (auto& peak : _peakGraphicsItems) {
        removeItem(peak.second);
//        delete peak.second;
    }
    _peakGraphicsItems.clear();
}

void DetectorScene::updateMasks(unsigned long frame)
{
    _lastClickedGI = nullptr;

    for (auto&& mask: _masks) {
        removeItem(mask);
//        delete mask;
    }
    _masks.clear();

    for (auto&& mask: _currentData->getMasks()) {
        const Eigen::Vector3d lower(mask->getLower());
        const Eigen::Vector3d upper(mask->getUpper());

        // mask is out of the frame
        if (frame > upper[2] || frame < lower[2]) {
            continue;
        }
        auto maskItem = new MaskGraphicsItem(_currentData, mask);
        maskItem->setFrom(QPointF(lower(0), lower(1)));
        maskItem->setTo(QPointF(upper(0), upper(1)));
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

void DetectorScene::drawIntegrationRegion(bool flag)
{
    if (!_peakGraphicsItems.empty()) {
        const auto& it=_peakGraphicsItems.begin();
        it->second->drawBackground(flag);
    }

    // clear the background if necessary
    if (_integrationRegion && flag == false) {
        removeItem(_integrationRegion);
        _integrationRegion = nullptr;
    }
    _drawIntegrationRegion = flag;
    redrawImage();
}

void DetectorScene::showPeakCalcs(bool flag)
{
    _showPeakCalcs = flag;

    if (!flag) {
//        for (auto&& p: _peakCalcs) {
//            delete p;
//        }
        _peakCalcs.clear();
        _precalculatedPeaks.clear();
        return;
    }
    if (_precalculatedPeaks.empty()) {
        updatePeakCalcs();
    }
    for (auto& peak: _peakCalcs) {
        removeItem(peak);
        //delete peak;
    }
    _peakCalcs.resize(0);

    for (auto&& p: _precalculatedPeaks) {
        if ( std::abs(_currentFrameIndex-p._frame) > 1.0) {
            continue;
        }
        auto peak = new PeakCalcGraphicsItem(p);
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

void DetectorScene::setColorMap(const std::string &name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

void DetectorScene::resetScene()
{
    clearPeaks();
    clear();
    updatePeakCalcs();
    updatePeaks();
    _currentData = nullptr;
    _currentFrameIndex = 0;
    _zoomrect = nullptr;
    _zoomStack.clear();
    _image = nullptr;
    _integrationRegion = nullptr;
    _masks.clear();
    _lastClickedGI = nullptr;
    _precalculatedPeaks.clear();
}
