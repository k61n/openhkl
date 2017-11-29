#include <ctime>
#include <vector>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QPixmap>
#include <QToolTip>

#include <nsxlib/AABB.h>
#include <nsxlib/BoxMask.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/EllipseMask.h>
#include <nsxlib/Detector.h>
#include <nsxlib/DetectorEvent.h>
#include <nsxlib/Diffractometer.h>
#include <nsxlib/Gonio.h>
#include <nsxlib/InstrumentState.h>
#include <nsxlib/Logger.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/ReciprocalVector.h>
#include <nsxlib/Sample.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/Source.h>
#include <nsxlib/UnitCell.h>
#include <nsxlib/Units.h>

#include "ColorMap.h"
#include "DetectorScene.h"
#include "CutLineGraphicsItem.h"
#include "CutSliceGraphicsItem.h"
#include "EllipseMaskGraphicsItem.h"
#include "MaskGraphicsItem.h"
#include "PeakGraphicsItem.h"

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
  _logarithmic(false),
  _colormap(new ColorMap()),
  _integrationRegion(nullptr),
  _drawIntegrationRegion(false)
{
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

void DetectorScene::setData(std::shared_ptr<SessionModel> session, const nsx::sptrDataSet& data)
{
    setData(session, data, 0);
}

void DetectorScene::setData(std::shared_ptr<SessionModel> session, const nsx::sptrDataSet& data, size_t frame)
{
    _currentData = data;
    _session = session;

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
    changeFrame(frame);
}

void DetectorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
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
    EllipseMaskGraphicsItem* ellipse_mask(nullptr);
    //_masks.emplace_back(new graphicsItem(nullptr));
    
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
            mask = new MaskGraphicsItem(_currentData, new nsx::AABB);
            mask->setFrom(event->lastScenePos());
            mask->setTo(event->lastScenePos());
            addItem(mask);
            _lastClickedGI=mask;
            _masks.emplace_back(mask, nullptr);
            break;
            //case of Ellipse mask mode
        case ELLIPSE_MASK:
            ellipse_mask = new EllipseMaskGraphicsItem(_currentData, new nsx::AABB);
            ellipse_mask->setFrom(event->lastScenePos());
            ellipse_mask->setTo(event->lastScenePos());
            addItem(ellipse_mask);
            _lastClickedGI=ellipse_mask;
            _masks.emplace_back(ellipse_mask, nullptr);
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

    auto peaks = _session->peaks(_currentData.get());

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
                auto it = findMask(p);
                if (it != _masks.end()) {
                    it->second = new nsx::BoxMask(*p->getAABB());
                    _currentData->addMask(it->second);                    
                    _lastClickedGI = nullptr;
                }
                _currentData->maskPeaks(peaks);
                update();
                updateMasks(_currentFrameIndex);
            }else if (auto p=dynamic_cast<EllipseMaskGraphicsItem*>(_lastClickedGI)) {
                auto it = findMask(p);
                if (it != _masks.end()) {
                    it->second = new nsx::EllipseMask(*p->getAABB());
                    _currentData->addMask(it->second);                    
                    _lastClickedGI = nullptr;
                }
                _currentData->maskPeaks(peaks);
                update();
                updateMasks(_currentFrameIndex);    
            }
        }
    }
    
    #if 0
    for(mask == _masks)
    {
        if(item == mask.first)
        data->addMask(BoxMask);
        mask.second = BoxMask; 

    } 
    #endif

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
                _session->removePeak(p->getPeak());
                _peakGraphicsItems.erase(p->getPeak());

            }
            // If the item is a mask graphics item, remove its corresponding mask from the data,
            // update the QList of mask graphics items and update the scene
            else if (auto p = dynamic_cast<MaskGraphicsItem*>(item)) {
                auto it = findMask(p);
                if (it != _masks.end()) {
                    _currentData->removeMask(it->second);
                    _masks.erase(it);
                    auto peaks = _session->peaks(_currentData.get());
                    _currentData->maskPeaks(peaks);
                    update();
                    updateMasks(_currentFrameIndex);    
                }
            }
            else if (auto p = dynamic_cast<EllipseMaskGraphicsItem*>(item)) {
                auto it = findMask(p);
                if (it != _masks.end()) {
                    _currentData->removeMask(it->second);
                    _masks.erase(it);
                    auto peaks =  _session->peaks(_currentData.get());
                    _currentData->maskPeaks(peaks);
                    update();
                    updateMasks(_currentFrameIndex);    
                }
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
            nsx::info() << "Removed "<< nPeaksErased << " peaks";
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
    int intensity = _currentFrame(row,col);

    nsx::InstrumentState state = _currentData->getInterpolatedState(_currentFrameIndex);

    const auto& samplev = state.sample.values();
    const auto& detectorv = state.detector.values();
    auto sample=instr->getSample();
    auto& mono = instr->getSource()->getSelectedMonochromator();
    double wave=mono.getWavelength();

    QString ttip;
    double gamma, nu, th2;
    auto ev = nsx::DetectorEvent(_currentData, col, row, _currentFrameIndex);
    auto si = nsx::ReciprocalVector({0, 1.0, 0});
    ev.getGammaNu(gamma, nu);
    th2 = ev.get2Theta(si);

    switch (_cursorMode) {
    case PIXEL:
        ttip = QString("(%1,%2) I:%3").arg(col).arg(row).arg(intensity);
        break;
    case GAMMA:           
        ttip = QString("(%1,%2) I: %3").arg(gamma/nsx::deg).arg(nu/nsx::deg).arg(intensity);
        break;
    case THETA:      
        ttip = QString("(%1) I: %2").arg(th2/nsx::deg).arg(intensity);
        break;
    case DSPACING:
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
    const unsigned int red =     (128u << 24) | (255u << 16);
    const unsigned int green =   (128u << 24) | (255u << 8);
    const unsigned int yellow =  (128u << 24) | (255u << 16) | (255u << 8);
    const unsigned int purple =  (128u << 24) | (153u << 16) | ( 51u << 8) | (255u);
    const unsigned int pink   =  (128u << 24) | (255u << 16) | (153u << 8) | (204u);
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

        for (auto&& peak: _session->peaks(_currentData.get())) {
            peak->getIntegrationRegion().updateMask(mask, _currentFrameIndex);
        }

        for (auto&& peak: _session->peaks(_currentData.get())) {
            auto&& region = peak->getIntegrationRegion();
            auto aabb = region.getBackground().aabb();
            auto&& lower = aabb.lower();
            auto&& upper = aabb.upper();

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
                for (auto y = ymin; y < ymax; ++y) {
                    Eigen::Vector3d p(x, y, _currentFrameIndex);

                    if (region.inRegion(p)) {
                        region_img.setPixel(x, y, peak->isSelected() ? (peak->isObserved() ? green : purple) : red);
                    }
                    if (region.inBackground(p) && (mask(y,x) == 0)) {
                        region_img.setPixel(x, y, peak->isSelected() ? (peak->isObserved() ? yellow : pink) : red);
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
    auto peaks = _session->peaks(_currentData.get());

    for (auto&& peak: peaks) {
        auto aabb = peak->getIntegrationRegion().getBackground().aabb();
        const Eigen::Vector3d& l = aabb.lower();
        const Eigen::Vector3d& u = aabb.upper();

        if (_currentFrameIndex < l[2] || _currentFrameIndex > u[2]) {
            continue;
        }
        PeakGraphicsItem* pgi = new PeakGraphicsItem(peak);
        pgi->setFrame(_currentFrameIndex);
        addItem(pgi);
        _peakGraphicsItems.insert(std::pair<nsx::sptrPeak3D, PeakGraphicsItem*>(peak,pgi));
    }
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
    Q_UNUSED(frame)
    _lastClickedGI = nullptr;
}

void DetectorScene::showPeakLabels(bool peaklabel)
{
    if (!_peakGraphicsItems.empty()) {
        for (const auto& p : _peakGraphicsItems)
            p.second->setLabelVisible(peaklabel);
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
    updatePeaks();
    _currentData = nullptr;
    _currentFrameIndex = 0;
    _zoomrect = nullptr;
    _zoomStack.clear();
    _image = nullptr;
    _integrationRegion = nullptr;
    _masks.clear();
    _lastClickedGI = nullptr;
}

std::vector<std::pair<QGraphicsItem*, nsx::IMask*>>::iterator DetectorScene::findMask(QGraphicsItem* item)
{
    return std::find_if(_masks.begin(), _masks.end(), [item](const std::pair<QGraphicsItem*, nsx::IMask*>& x) {return x.first == item;});
}
