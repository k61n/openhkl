//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/models/DetectorScene.cpp
//! @brief     Implements class DetectorScene
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <ctime>
#include <vector>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QPixmap>
#include <QToolTip>

#include "base/geometry/AABB.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/logger/Logger.h"
#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/utils/Units.h"
#include "core/detector/Detector.h"
#include "core/experiment/DataSet.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

#include "apps/ColorMap.h"
#include "apps/items/CutLineGraphicsItem.h"
#include "apps/items/CutSliceGraphicsItem.h"
#include "apps/items/EllipseMaskGraphicsItem.h"
#include "apps/items/MaskGraphicsItem.h"
#include "apps/items/PeakGraphicsItem.h"
#include "apps/models/DetectorScene.h"
#include "apps/models/ExperimentItem.h"
#include "apps/models/MetaTypes.h"
#include "apps/models/UnitCellItem.h"
#include "apps/models/UnitCellsItem.h"

DetectorScene::DetectorScene(QObject* parent)
    : QGraphicsScene(parent)
    , _currentData(nullptr)
    , _currentFrameIndex(-1)
    , _currentIntensity(10)
    , _currentFrame()
    , _cursorMode(PIXEL)
    , _mode(ZOOM)
    , _zoomstart(0, 0)
    , _zoomend(0, 0)
    , _zoomrect(nullptr)
    , _zoomStack()
    , _itemSelected(false)
    , _image(nullptr)
    , _masks()
    , _lastClickedGI(nullptr)
    , _logarithmic(false)
    , _drawIntegrationRegion(false)
    , _colormap(new ColorMap())
    , _integrationRegion(nullptr)
    , _session(nullptr)
    , _selected_peak_gi(nullptr)
    , _peak_graphics_items()
    , _selected_peak(nullptr)
{
}

SessionModel* DetectorScene::session()
{
    return _session;
}

void DetectorScene::setSession(SessionModel* session)
{
    _session = session;

    connect(
        _session, SIGNAL(signalSelectedDataChanged(nsx::sptrDataSet, int)), this,
        SLOT(slotChangeSelectedData(nsx::sptrDataSet, int)));

    connect(
        _session, SIGNAL(signalSelectedPeakChanged(nsx::sptrPeak3D)), this,
        SLOT(slotChangeSelectedPeak(nsx::sptrPeak3D)));

    connect(
        _session, SIGNAL(signalEnabledPeakChanged(nsx::sptrPeak3D)), this,
        SLOT(slotChangeEnabledPeak(nsx::sptrPeak3D)));

    connect(
        _session, SIGNAL(signalMaskedPeaksChanged(const nsx::PeakList&)), this,
        SLOT(slotChangeMaskedPeaks(const nsx::PeakList&)));
}

void DetectorScene::clearPeakGraphicsItems()
{
    if (!_currentData)
        return;

    for (auto p : _peak_graphics_items) {
        removeItem(p.second);
        delete p.second;
    }

    _peak_graphics_items.clear();
}

void DetectorScene::resetPeakGraphicsItems()
{
    if (!_currentData)
        return;

    clearPeakGraphicsItems();

    auto peaks = _session->peaks(_currentData);

    for (auto&& peak : peaks) {
        auto peak_ellipsoid = peak->shape();

        peak_ellipsoid.scale(peak->peakEnd());

        auto& aabb = peak_ellipsoid.aabb();

        auto lower = aabb.lower();

        auto upper = aabb.upper();

        // If the current frame of the scene is out of the peak bounds do not paint
        // it
        if (_currentFrameIndex < lower[2] || _currentFrameIndex > upper[2])
            continue;

        PeakGraphicsItem* peak_gi = new PeakGraphicsItem(peak, _currentFrameIndex);

        addItem(peak_gi);

        _peak_graphics_items.insert(std::make_pair(peak, peak_gi));
    }

    if (_selected_peak_gi) {
        removeItem(_selected_peak_gi);
        delete _selected_peak_gi;
        _selected_peak_gi = nullptr;
    }

    auto it = _peak_graphics_items.find(_selected_peak);

    if (it != _peak_graphics_items.end())
        it->second->setVisible(true);

    if (_selected_peak) {
        auto selected_peak_ellipsoid = _selected_peak->shape();

        selected_peak_ellipsoid.scale(_selected_peak->peakEnd());

        double frame_index = static_cast<double>(_currentFrameIndex);

        auto& aabb = selected_peak_ellipsoid.aabb();

        auto&& lower = aabb.lower();
        auto&& upper = aabb.upper();

        if (frame_index >= lower[2] && frame_index <= upper[2]) {
            auto center = selected_peak_ellipsoid.intersectionCenter(
                {0.0, 0.0, 1.0}, {0.0, 0.0, static_cast<double>(_currentFrameIndex)});

            _selected_peak_gi = new QGraphicsRectItem(nullptr);
            _selected_peak_gi->setPos(center[0], center[1]);
            _selected_peak_gi->setRect(-10, -10, 20, 20);

            QPen pen;
            pen.setColor(Qt::darkCyan);
            pen.setStyle(Qt::DotLine);
            _selected_peak_gi->setPen(pen);
            _selected_peak_gi->setZValue(-1);
            _selected_peak_gi->setAcceptHoverEvents(false);

            addItem(_selected_peak_gi);
        }
    }
}

void DetectorScene::slotChangeEnabledPeak(nsx::sptrPeak3D peak)
{
    Q_UNUSED(peak)

    loadCurrentImage();
}

void DetectorScene::slotChangeMaskedPeaks(const nsx::PeakList& peaks)
{
    Q_UNUSED(peaks)

    loadCurrentImage();
}

void DetectorScene::slotChangeSelectedData(nsx::sptrDataSet data, int frame)
{
    if (data != _currentData) {
        _currentData = data;

        _currentData->open();

        auto det = _currentData->reader()->diffractometer()->detector();

        _currentFrameIndex = -1;

        _zoomStack.clear();
        _zoomStack.push_back(QRect(0, 0, int(det->nCols()), int(det->nRows())));

        if (_lastClickedGI != nullptr) {
            removeItem(_lastClickedGI);
            _lastClickedGI = nullptr;
        }
    }

    slotChangeSelectedFrame(frame);
}

void DetectorScene::slotChangeSelectedPeak(nsx::sptrPeak3D peak)
{
    if (peak == _selected_peak)
        return;

    _selected_peak = peak;

    auto data = peak->data();

    auto peak_ellipsoid = peak->shape();

    // Get frame number to adjust the data
    size_t frame = size_t(std::lround(peak_ellipsoid.aabb().center()[2]));

    slotChangeSelectedData(data, frame);

    update();
}

void DetectorScene::slotChangeSelectedFrame(int frame)
{
    if (!_currentData)
        return;

    if (!_currentData->isOpened())
        _currentData->open();

    if (frame == _currentFrameIndex)
        return;

    _currentFrameIndex = frame;

    clearPeakGraphicsItems();

    resetPeakGraphicsItems();

    loadCurrentImage();

    updateMasks();
}

void DetectorScene::setMaxIntensity(int intensity)
{
    if (_currentIntensity == intensity)
        return;
    _currentIntensity = intensity;

    if (!_currentData)
        return;
    if (!_currentData->isOpened())
        _currentData->open();

    loadCurrentImage();
}

void DetectorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // If no data is loaded, do nothing
    if (!_currentData)
        return;
    createToolTipText(event);

    // The left button was pressed
    if (event->buttons() & Qt::LeftButton) {
        if (event->modifiers() == Qt::ControlModifier)
            return;

        // Case of the Zoom mode, update the scene
        if (_mode == ZOOM) {
            QRectF zoom = _zoomrect->rect();
            zoom.setBottomRight(event->lastScenePos());
            _zoomrect->setRect(zoom);
            return;
        }

        if (!_lastClickedGI)
            return;

        _lastClickedGI->mouseMoveEvent(event);

        auto p = dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI);
        if (p != nullptr)
            emit updatePlot(p);
    }
    // No button was pressed, just a mouse move
    else if (event->button() == Qt::NoButton) {
        auto lastPos = event->lastScenePos();
        auto point = lastPos.toPoint();
        QTransform trans;
        QGraphicsItem* gItem = itemAt(point, trans);
        if (!gItem)
            return;
        auto p = dynamic_cast<PlottableGraphicsItem*>(gItem);
        if (p) {
            emit updatePlot(p);
            QGraphicsScene::mouseMoveEvent(event);
        }
    }
}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    CutterGraphicsItem* cutter(nullptr);
    MaskGraphicsItem* mask(nullptr);
    EllipseMaskGraphicsItem* ellipse_mask(nullptr);
    //_masks.emplace_back(new graphicsItem(nullptr));

    QPen pen1;

    // If no data is loaded, do nothing
    if (!_currentData)
        return;
    // The left button was pressed
    if (event->buttons() & Qt::LeftButton) {
        // Get the graphics item on which the user has clicked
        auto item = itemAt(event->lastScenePos(), QTransform());

        if (event->modifiers() == Qt::ControlModifier) {
            item->setSelected(!item->isSelected());
            return;
        }
        // If the item is a NSXTools GI and is selected it will become the current
        // active GI
        if (auto p = dynamic_cast<SXGraphicsItem*>(item)) {
            if (p->isSelected()) {
                _lastClickedGI = p;
                return;
            }
        }
        switch (_mode) {
            case SELECT: {
                break;
            }
            case ZOOM: {
                _zoomstart = event->lastScenePos().toPoint();
                _zoomend = _zoomstart;
                _zoomrect = addRect(QRect(_zoomstart, _zoomend));

                pen1 = QPen(QBrush(QColor("gray")), 1.0);
                pen1.setWidth(1);
                pen1.setCosmetic(true);
                _zoomrect->setPen(pen1);
                _zoomrect->setBrush(QBrush(QColor(255, 0, 0, 30)));
                break;
            }
            case HORIZONTALSLICE: {
                cutter = new CutSliceGraphicsItem(_currentData, true);
                break;
            }
            case VERTICALSLICE: {
                cutter = new CutSliceGraphicsItem(_currentData, false);
                break;
            }
            case LINE: {
                cutter = new CutLineGraphicsItem(_currentData);
                break;
            }
            case MASK: {
                mask = new MaskGraphicsItem(_currentData, new nsx::AABB);
                mask->setFrom(event->lastScenePos());
                mask->setTo(event->lastScenePos());
                addItem(mask);
                _lastClickedGI = mask;
                _masks.emplace_back(mask, nullptr);
                break;
            }
            case ELLIPSE_MASK: {
                ellipse_mask = new EllipseMaskGraphicsItem(_currentData, new nsx::AABB);
                ellipse_mask->setFrom(event->lastScenePos());
                ellipse_mask->setTo(event->lastScenePos());
                addItem(ellipse_mask);
                _lastClickedGI = ellipse_mask;
                _masks.emplace_back(ellipse_mask, nullptr);
                break;
            }
        }
        if (cutter != nullptr) {
            cutter->setFrom(event->lastScenePos());
            addItem(cutter);
            _lastClickedGI = cutter;
        }
    }
    // The right button was pressed
    else if (event->buttons() & Qt::RightButton) {
        if (_zoomStack.size() > 1) {
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

void DetectorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // If no data is loaded, do nothing
    if (!_currentData)
        return;

    // The user released the left mouse button
    if (event->button() & Qt::LeftButton) {
        if (event->modifiers() == Qt::ControlModifier)
            return;

        if (_mode == SELECT) {
            auto item = itemAt(event->lastScenePos(), QTransform());

            auto peak_item = dynamic_cast<PeakGraphicsItem*>(item);

            if (!peak_item)
                return;

            auto peak = peak_item->peak();

            emit _session->signalSelectedPeakChanged(peak);
        } else if (_mode == ZOOM) {
            qreal top = _zoomrect->rect().top();
            qreal bot = _zoomrect->rect().bottom();
            qreal left = _zoomrect->rect().left();
            qreal right = _zoomrect->rect().right();

            // If the user just clicked on the left mouse button with holding it, skip
            // the event
            if (qAbs(top - bot) <= 1 || qAbs(left - right) <= 1) {
                removeItem(_zoomrect);
                delete _zoomrect;
                return;
            }

            if (top > bot)
                std::swap(top, bot);

            if (right < left)
                std::swap(left, right);

            QRect max = _zoomStack.front();

            if (top < max.top())
                top = max.top();

            if (bot > max.bottom())
                bot = max.bottom() + 1;

            if (left < max.left())
                left = max.left();

            if (right > max.right())
                right = max.right() + 1;

            _zoomrect->setRect(left, top, right - left, bot - top);
            _zoomStack.push_back(_zoomrect->rect().toRect());
            setSceneRect(_zoomrect->rect());
            removeItem(_zoomrect);
            delete _zoomrect;
            emit dataChanged();
        } else {
            auto peaks = _session->peaks(_currentData);

            if (auto p = dynamic_cast<CutterGraphicsItem*>(_lastClickedGI)) {
                if (true) {
                    // delete p....
                    _lastClickedGI = nullptr;
                    removeItem(p);
                } else {
                    emit updatePlot(p);
                }
            } else if (auto p = dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI))
                emit updatePlot(p);
            else if (auto p = dynamic_cast<MaskGraphicsItem*>(_lastClickedGI)) {
                // add a new mask
                auto it = findMask(p);
                if (it != _masks.end()) {
                    it->second = new nsx::BoxMask(*p->getAABB());
                    _currentData->addMask(it->second);
                    _lastClickedGI = nullptr;
                }
                _currentData->maskPeaks(peaks);
                update();
                updateMasks();
                emit _session->signalMaskedPeaksChanged(peaks);
            } else if (auto p = dynamic_cast<EllipseMaskGraphicsItem*>(_lastClickedGI)) {
                auto it = findMask(p);
                if (it != _masks.end()) {
                    it->second = new nsx::EllipseMask(*p->getAABB());
                    _currentData->addMask(it->second);
                    _lastClickedGI = nullptr;
                }
                _currentData->maskPeaks(peaks);
                update();
                updateMasks();
                emit _session->signalMaskedPeaksChanged(peaks);
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
    auto item = itemAt(event->scenePos(), QTransform());
    auto p = dynamic_cast<SXGraphicsItem*>(item);

    if (p == nullptr)
        return;
    if (!(p->isSelected()))
        return;
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
        QList<QGraphicsItem*> items = selectedItems();
        int nPeaksUnselected = int(_peak_graphics_items.size());
        for (auto item : items) {
            auto p = dynamic_cast<SXGraphicsItem*>(item);

            if (p == nullptr)
                continue;

            // The item must be deletable ... to be deleted
            if (!p->isDeletable())
                continue;
            // If the item is a peak graphics item, remove its corresponding peak from
            // the data, update the set of peak graphics items and update the scene
            if (auto p = dynamic_cast<PeakGraphicsItem*>(item))
                p->peak()->setSelected(false);
            // If the item is a mask graphics item, remove its corresponding mask from
            // the data, update the QList of mask graphics items and update the scene
            else if (auto p = dynamic_cast<MaskGraphicsItem*>(item)) {
                auto it = findMask(p);
                if (it != _masks.end()) {
                    _currentData->removeMask(it->second);
                    _masks.erase(it);
                    auto peaks = _session->peaks(_currentData);
                    _currentData->maskPeaks(peaks);
                    update();
                    updateMasks();
                    emit _session->signalMaskedPeaksChanged(peaks);
                }
            } else if (auto p = dynamic_cast<EllipseMaskGraphicsItem*>(item)) {
                auto it = findMask(p);
                if (it != _masks.end()) {
                    _currentData->removeMask(it->second);
                    _masks.erase(it);
                    auto peaks = _session->peaks(_currentData);
                    _currentData->maskPeaks(peaks);
                    update();
                    updateMasks();
                    emit _session->signalMaskedPeaksChanged(peaks);
                }
            }
            if (p == _lastClickedGI)
                _lastClickedGI = nullptr;
            // Remove the item from the scene
            removeItem(item);
        }
        // Computes the new number of peaks, and if it changes log it
        nPeaksUnselected -= _peak_graphics_items.size();
        if (nPeaksUnselected > 0)
            nsx::info() << "Unselected " << nPeaksUnselected << " peaks";
    }
}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    if (!_currentData)
        return;
    auto instr = _currentData->reader()->diffractometer();
    auto det = instr->detector();

    int nrows = int(det->nRows());
    int ncols = int(det->nCols());

    int col = static_cast<int>(event->lastScenePos().x());
    int row = static_cast<int>(event->lastScenePos().y());

    if (col < 0 || col > ncols - 1 || row < 0 || row > nrows - 1)
        return;
    int intensity = _currentFrame(row, col);

    nsx::InstrumentState state = _currentData->interpolatedState(_currentFrameIndex);

    const auto& mono = instr->source().selectedMonochromator();
    double wave = mono.wavelength();

    QString ttip;

    auto pos = _currentData->reader()->diffractometer()->detector()->pixelPosition(col, row);

    double gamma = state.gamma(pos);
    double nu = state.nu(pos);
    double th2 = state.twoTheta(pos);

    switch (_cursorMode) {
        case PIXEL: {
            ttip = QString("(%1,%2) I:%3").arg(col).arg(row).arg(intensity);
            break;
        }
        case GAMMA_NU: {
            ttip = QString("(%1,%2) I: %3")
                       .arg(gamma / nsx::deg, 0, 'f', 3)
                       .arg(nu / nsx::deg, 0, 'f', 3)
                       .arg(intensity);
            break;
        }
        case THETA: {
            ttip = QString("(%1) I: %2").arg(th2 / nsx::deg, 0, 'f', 3).arg(intensity);
            break;
        }
        case D_SPACING: {
            ttip = QString("(%1) I: %2").arg(wave / (2 * sin(0.5 * th2)), 0, 'f', 3).arg(intensity);
            break;
        }
        case MILLER_INDICES: {
            auto experiment_item = _session->selectExperiment(_currentData);
            if (!experiment_item)
                ttip = QString("No experiment found");
            else {
                auto selected_unit_cell_item =
                    experiment_item->unitCellsItem()->selectedUnitCellItem();
                if (selected_unit_cell_item) {
                    auto q = state.sampleQ(pos);
                    auto miller_indices = nsx::MillerIndex(
                        q,
                        *(selected_unit_cell_item->data(Qt::UserRole).value<nsx::sptrUnitCell>()));

                    Eigen::RowVector3d hkl =
                        miller_indices.rowVector().cast<double>() + miller_indices.error();
                    ttip = QString("(%1,%2,%3) I: %4")
                               .arg(hkl[0], 0, 'f', 2)
                               .arg(hkl[1], 0, 'f', 2)
                               .arg(hkl[2], 0, 'f', 2)
                               .arg(intensity);
                } else {
                    ttip = QString("No unit cell selected");
                }
            }
            break;
        }
    }
    QToolTip::showText(event->screenPos(), ttip);
}

void DetectorScene::changeInteractionMode(int mode)
{
    _mode = static_cast<MODE>(mode);
}

// TODO: fix this whole method, it should be using
// IntegrationRegion::updateMask()
void DetectorScene::loadCurrentImage()
{
    if (!_currentData)
        return;

    const unsigned int green = (128u << 24) | (255u << 8);
    const unsigned int yellow = (128u << 24) | (255u << 16) | (255u << 8);
    const unsigned int transparent = 0;

    using EventType = nsx::IntegrationRegion::EventType;

    // Full image size, front of the stack
    QRect& full = _zoomStack.front();

    if (_currentFrameIndex >= _currentData->nFrames())
        _currentFrameIndex = _currentData->nFrames() - 1;

    _currentFrame = _currentData->frame(_currentFrameIndex);

    if (_image == nullptr) {
        _image = addPixmap(QPixmap::fromImage(_colormap->matToImage(
            _currentFrame.cast<double>(), full, _currentIntensity, _logarithmic)));
        _image->setZValue(-2);
    } else {
        _image->setPixmap(QPixmap::fromImage(_colormap->matToImage(
            _currentFrame.cast<double>(), full, _currentIntensity, _logarithmic)));
    }

    // update the integration region pixmap
    if (_drawIntegrationRegion) {
        const int ncols = _currentData->nCols();
        const int nrows = _currentData->nRows();
        Eigen::MatrixXi mask(nrows, ncols);
        mask.setConstant(int(EventType::EXCLUDED));

        auto peaks = _session->peaks(_currentData);
        for (auto peak : peaks) {
            if (peak->enabled()) {
                // IntegrationRegion constructor can throw if the region is invalid
                try {
                    auto region = nsx::IntegrationRegion(
                        peak, peak->peakEnd(), peak->bkgBegin(), peak->bkgEnd());
                    region.updateMask(mask, _currentFrameIndex);
                } catch (...) {
                    peak->setSelected(false);
                }
            }
        }

        QImage region_img(ncols, nrows, QImage::Format_ARGB32);

        for (auto c = 0; c < ncols; ++c) {
            for (auto r = 0; r < nrows; ++r) {
                EventType ev = EventType(mask(r, c));
                unsigned int color;

                switch (ev) {
                    case EventType::PEAK: color = green; break;
                    case EventType::BACKGROUND: color = yellow; break;
                    default: color = transparent; break;
                }

                // todo: what about unselected peaks?
                region_img.setPixel(c, r, color);
            }
        }

        if (!_integrationRegion) {
            _integrationRegion = addPixmap(QPixmap::fromImage(region_img));
            _integrationRegion->setZValue(-1);
        } else {
            _integrationRegion->setPixmap(QPixmap::fromImage(region_img));
        }
    }

    setSceneRect(_zoomStack.back());
    emit dataChanged();

    if (auto p = dynamic_cast<PlottableGraphicsItem*>(_lastClickedGI))
        emit updatePlot(p);
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
    _cursorMode = static_cast<CURSORMODE>(mode);
}

int DetectorScene::currentFrame() const
{
    return _currentFrameIndex;
}

void DetectorScene::updateMasks()
{
    _lastClickedGI = nullptr;
}

void DetectorScene::showPeakLabels(bool flag)
{
    for (auto p : _peak_graphics_items)
        p.second->showLabel(flag);
    update();
}

void DetectorScene::showPeakAreas(bool flag)
{
    for (auto p : _peak_graphics_items)
        p.second->showArea(flag);
    update();
}

void DetectorScene::drawIntegrationRegion(bool flag)
{
    // clear the background if necessary
    if (_integrationRegion && flag == false) {
        removeItem(_integrationRegion);
        delete _integrationRegion;
        _integrationRegion = nullptr;
    }

    _drawIntegrationRegion = flag;

    loadCurrentImage();
}

void DetectorScene::setLogarithmic(bool checked)
{
    _logarithmic = checked;
}

void DetectorScene::setColorMap(const std::string& name)
{
    _colormap = std::unique_ptr<ColorMap>(new ColorMap(name));
}

void DetectorScene::resetScene()
{
    clearPeakGraphicsItems();
    clear();
    _currentData = nullptr;
    _currentFrameIndex = 0;
    _zoomrect = nullptr;
    _zoomStack.clear();
    _image = nullptr;
    _integrationRegion = nullptr;
    _masks.clear();
    _lastClickedGI = nullptr;
}

std::vector<std::pair<QGraphicsItem*, nsx::IMask*>>::iterator
DetectorScene::findMask(QGraphicsItem* item)
{
    return std::find_if(
        _masks.begin(), _masks.end(),
        [item](const std::pair<QGraphicsItem*, nsx::IMask*>& x) { return x.first == item; });
}
