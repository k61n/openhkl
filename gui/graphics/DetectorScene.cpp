//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/DetectorScene.cpp
//! @brief     Implements class DetectorScene
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/DetectorScene.h"

#include "base/geometry/AABB.h"
#include "base/geometry/ReciprocalVector.h"
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

#include "gui/MainWin.h"
#include "gui/graphics_tools/CutLineItem.h"
#include "gui/graphics_tools/CutSliceItem.h"
#include "gui/graphics_tools/CutterItem.h"

#include "gui/graphics_items/EllipseItem.h"
#include "gui/graphics_items/MaskItem.h"
#include "gui/graphics_items/PeakItemGraphic.h"
#include "gui/graphics_items/PlottableItem.h"
#include "gui/graphics_items/SXGraphicsItem.h"

#include "gui/models/Session.h"

#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

#include <QCR/engine/logger.h>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QPixmap>
#include <QToolTip>
#include <QtGlobal>

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
    , _selected_peak_gi(nullptr)
    , _peak_graphics_items()
    , _selected_peak(nullptr)
{
}

void DetectorScene::linkPeakModel(PeakCollectionModel* source)
{
    if (!(_peak_model == nullptr))
        unlinkPeakModel();
    _peak_model = source;
    connect(
        _peak_model, &PeakCollectionModel::dataChanged,
        this, &DetectorScene::peakModelDataChanged
    );

}

PeakCollectionModel* DetectorScene::peakModel() const
{
    return _peak_model;
}

void DetectorScene::unlinkPeakModel()
{
    _peak_model = nullptr;
}

void DetectorScene::peakModelDataChanged()
{
    drawPeakitems();
    update();
}

void DetectorScene::clearPeakItems()
{
    if (!_currentData)
        return;

    for (PeakItemGraphic* p : _peak_graphics_items) {
        removeItem(p);
    }

    _peak_graphics_items.clear();
}

void DetectorScene::drawPeakitems()
{
    if (_peak_model == nullptr || _peak_model->root() == nullptr)
        return;

    clearPeakItems();

    std::vector<PeakItem*> peak_items = _peak_model->root()->peakItems();

    for (PeakItem* peak_item : peak_items) {
        nsx::Ellipsoid peak_ellipsoid = peak_item->peak()->shape();
        peak_ellipsoid.scale(peak_item->peak()->peakEnd());
        const nsx::AABB& aabb = peak_ellipsoid.aabb();
        Eigen::Vector3d lower = aabb.lower();
        Eigen::Vector3d upper = aabb.upper();

        // If the current frame of the scene is out of the peak bounds do not paint it
        if (_currentFrameIndex < lower[2] || _currentFrameIndex > upper[2])
            continue;

        PeakItemGraphic* peak_graphic = peak_item->peakGraphic();
        peak_graphic->setCenter(_currentFrameIndex);
        _peak_graphics_items.push_back(peak_graphic);
        addItem(peak_graphic);

    }

    // if (_selected_peak_gi) {
    //     removeItem(_selected_peak_gi);
    //     delete _selected_peak_gi;
    //     _selected_peak_gi = nullptr;
    // }

    // auto it = _peak_graphics_items.find(_selected_peak);

    // if (it != _peak_graphics_items.end())
    //     it->second->setVisible(true);

    // if (_selected_peak) {
    //     nsx::Ellipsoid selected_peak_ellipsoid = _selected_peak_item->peak()->shape();
    //     selected_peak_ellipsoid.scale(_selected_peak_item->peak()->peakEnd());
    //     double frame_index = static_cast<double>(_currentFrameIndex);
    //     const nsx::AABB& aabb = selected_peak_ellipsoid.aabb();
    //     const Eigen::Vector3d& lower = aabb.lower();
    //     const Eigen::Vector3d& upper = aabb.upper();

    //     if (frame_index >= lower[2] && frame_index <= upper[2]) {
    //         Eigen::Vector3d center = selected_peak_ellipsoid.intersectionCenter(
    //             {0.0, 0.0, 1.0}, {0.0, 0.0, static_cast<double>(_currentFrameIndex)});

    //         _selected_peak_gi = new QGraphicsRectItem(nullptr);
    //         _selected_peak_gi->setPos(center[0], center[1]);
    //         _selected_peak_gi->setRect(-10, -10, 20, 20);

    //         QPen pen;
    //         pen.setColor(Qt::darkCyan);
    //         pen.setStyle(Qt::DotLine);
    //         _selected_peak_gi->setPen(pen);
    //         _selected_peak_gi->setZValue(-1);
    //         _selected_peak_gi->setAcceptHoverEvents(false);

    //         addItem(_selected_peak_gi);
    //     }
    // }
}

void DetectorScene::slotChangeSelectedData(nsx::sptrDataSet data, int frame)
{
    if (data != _currentData) {
        _currentData = data;

        _currentData->open();

        _currentFrameIndex = -1;

        _zoomStack.clear();
        _zoomStack.push_back(QRect(0, 0, int(_currentData->nCols()), int(_currentData->nRows())));

        if (_lastClickedGI != nullptr) {
            removeItem(_lastClickedGI);
            _lastClickedGI = nullptr;
        }
    }

    slotChangeSelectedFrame(frame);
}

void DetectorScene::slotChangeSelectedPeak(nsx::Peak3D* peak)
{
    // if (peak == _selected_peak)
    //     return;

    // _selected_peak = peak;

    // nsx::sptrDataSet data = peak_item->peak()->data();

    // const nsx::Ellipsoid& peak_ellipsoid = peak_item->peak()->shape();

    // // Get frame number to adjust the data
    // size_t frame = size_t(std::lround(peak_ellipsoid.aabb().center()[2]));

    // slotChangeSelectedData(data, frame);

    // update();
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

    clearPeakItems();
    loadCurrentImage();
    updateMasks();
    drawPeakitems();
    
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

        PlottableItem* p = dynamic_cast<PlottableItem*>(_lastClickedGI);
        if (p != nullptr)
            gGui->updatePlot(p);
    }
    // No button was pressed, just a mouse move
    else if (event->button() == Qt::NoButton) {
        QPointF lastPos = event->lastScenePos();
        QPoint point = lastPos.toPoint();
        QTransform trans;
        QGraphicsItem* gItem = itemAt(point, trans);
        if (!gItem)
            return;

        PeakItemGraphic* p = dynamic_cast<PeakItemGraphic*>(gItem);

        if (p) {
            emit signalSelectedPeakItemChanged(p);
            // gGui->updatePlot(p);
            // QGraphicsScene::mouseMoveEvent(event);
        }
    }
}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    CutterItem* cutter(nullptr);
    MaskItem* mask(nullptr);
    EllipseMaskItem* ellipse_mask(nullptr);
    //_masks.emplace_back(new graphicsItem(nullptr));

    QPen pen1;

    // If no data is loaded, do nothing
    if (!_currentData)
        return;
    // The left button was pressed
    if (event->buttons() & Qt::LeftButton) {
        // Get the graphics item on which the user has clicked
        QGraphicsItem* item = itemAt(event->lastScenePos(), QTransform());

        if (event->modifiers() == Qt::ControlModifier) {
            item->setSelected(!item->isSelected());
            return;
        }
        // If the item is a NSXTools GI and is selected it will become the current
        // active GI
        if (SXGraphicsItem* p = dynamic_cast<SXGraphicsItem*>(item)) {
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
                cutter = new CutSliceItem(_currentData, true);
                break;
            }
            case VERTICALSLICE: {
                cutter = new CutSliceItem(_currentData, false);
                break;
            }
            case LINE: {
                cutter = new CutLineItem(_currentData);
                break;
            }
            case MASK: {
                mask = new MaskItem(_currentData, new nsx::AABB);
                mask->setFrom(event->lastScenePos());
                mask->setTo(event->lastScenePos());
                addItem(mask);
                _lastClickedGI = mask;
                _masks.emplace_back(mask, nullptr);
                break;
            }
            case ELLIPSE_MASK: {
                ellipse_mask = new EllipseMaskItem(_currentData, new nsx::AABB);
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
            QGraphicsItem* item = itemAt(event->lastScenePos(), QTransform());

            PeakItemGraphic* peak_item = dynamic_cast<PeakItemGraphic*>(item);

            if (!peak_item)
                return;

            nsx::Peak3D* peak = peak_item->peak();

            //            gSession->onSelectedPeakChanged(peak);
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
        } //else {
            // nsx::PeakList peaks = gSession->selectedExperiment()->getPeaks(0, 0)->peaks_;

            // if (CutterItem* p = dynamic_cast<CutterItem*>(_lastClickedGI)) {
            //     // delete p....
            //     _lastClickedGI = nullptr;
            //     removeItem(p);
            // } else if (PlottableItem* p = dynamic_cast<PlottableItem*>(_lastClickedGI))
            //     gGui->updatePlot(p);
            // else if (MaskItem* p = dynamic_cast<MaskItem*>(_lastClickedGI)) {
            //     // add a new mask
            //     auto it = findMask(p);
            //     if (it != _masks.end()) {
            //         it->second = new nsx::BoxMask(*p->getAABB());
            //         _currentData->addMask(it->second);
            //         _lastClickedGI = nullptr;
            //     }
            //     _currentData->maskPeaks(peaks);
            //     update();
            //     updateMasks();
            //     //                gSession->onMaskedPeaksChanged(peaks);
            // } else if (EllipseMaskItem* p = dynamic_cast<EllipseMaskItem*>(_lastClickedGI)) {
            //     auto it = findMask(p);
            //     if (it != _masks.end()) {
            //         it->second = new nsx::EllipseMask(*p->getAABB());
            //         _currentData->addMask(it->second);
            //         _lastClickedGI = nullptr;
            //     }
            //     _currentData->maskPeaks(peaks);
            //     update();
            //     updateMasks();
            //     //                gSession->onMaskedPeaksChanged(peaks);
            // }
        // }
    }
}

void DetectorScene::wheelEvent(QGraphicsSceneWheelEvent* event)
{
    // If no data, returns
    if (!_currentData)
        return;
    // Get the graphics item on which the user has performed the wheel event
    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    SXGraphicsItem* p = dynamic_cast<SXGraphicsItem*>(item);

    if (p == nullptr)
        return;
    if (!(p->isSelected()))
        return;
    CutterItem* q = dynamic_cast<CutterItem*>(item);
    if (q != nullptr) {
        q->wheelEvent(event);
        gGui->updatePlot(q);
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
        for (QGraphicsItem* item : items) {
            SXGraphicsItem* p = dynamic_cast<SXGraphicsItem*>(item);

            if (p == nullptr)
                continue;

            // The item must be deletable ... to be deleted
            if (!p->isDeletable())
                continue;
            // If the item is a peak graphics item, remove its corresponding peak from
            // the data, update the set of peak graphics items and update the scene
            // if (PeakItemGraphic* p = dynamic_cast<PeakItemGraphic*>(item))
            //     p->peak()->setSelected(false);
            // // If the item is a mask graphics item, remove its corresponding mask from
            // // the data, update the QList of mask graphics items and update the scene
            // else if (MaskItem* p = dynamic_cast<MaskItem*>(item)) {
            //     auto it = findMask(p);
            //     // if (it != _masks.end()) {
            //     //     _currentData->removeMask(it->second);
            //     //     _masks.erase(it);
            //     //     nsx::PeakList peaks = gSession->selectedExperiment()->getPeaks(0, 0)->peaks_;
            //     //     _currentData->maskPeaks(peaks);
            //     //     update();
            //     //     updateMasks();
            //     //     //                    gSession->onMaskedPeaksChanged(peaks);
            //     }
            // } else if (EllipseMaskItem* p = dynamic_cast<EllipseMaskItem*>(item)) {
            //     auto it = findMask(p);
            //     // if (it != _masks.end()) {
            //     //     _currentData->removeMask(it->second);
            //     //     _masks.erase(it);
            //     //     nsx::PeakList peaks = gSession->selectedExperiment()->getPeaks(0, 0)->peaks_;
            //     //     _currentData->maskPeaks(peaks);
            //     //     update();
            //     //     updateMasks();
            //         //                    gSession->onMaskedPeaksChanged(peaks);
            //     }
            // }
            if (p == _lastClickedGI)
                _lastClickedGI = nullptr;
            // Remove the item from the scene
            removeItem(item);
        }
        // Computes the new number of peaks, and if it changes log it
        nPeaksUnselected -= _peak_graphics_items.size();
        if (nPeaksUnselected > 0)
            qDebug() << "Unselected " << nPeaksUnselected << " peaks";
    }
}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    if (!_currentData)
        return;
    nsx::Diffractometer* instr = _currentData->reader()->diffractometer();
    nsx::Detector* det = instr->detector();

    int nrows = int(det->nRows());
    int ncols = int(det->nCols());

    int col = static_cast<int>(event->lastScenePos().x());
    int row = static_cast<int>(event->lastScenePos().y());

    if (col < 0 || col > ncols - 1 || row < 0 || row > nrows - 1)
        return;
    int intensity = _currentFrame(row, col);

    nsx::InstrumentState state = _currentData->interpolatedState(_currentFrameIndex);

    const nsx::Monochromator& mono = instr->source().selectedMonochromator();
    double wave = mono.wavelength();

    QString ttip;

    nsx::DirectVector pos =
        _currentData->reader()->diffractometer()->detector()->pixelPosition(col, row);

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
            SessionExperiment* experiment_item = gSession->selectedExperiment();
            if (!experiment_item)
                ttip = QString("No experiment found");
            else {
                nsx::sptrUnitCell selectedUnitCell = experiment_item->getUnitCell(0);
                if (selectedUnitCell) {
                    nsx::ReciprocalVector q = state.sampleQ(pos);
                    nsx::MillerIndex miller_indices(q, *selectedUnitCell);

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
    // if (_drawIntegrationRegion) {
    //     const int ncols = _currentData->nCols();
    //     const int nrows = _currentData->nRows();
    //     Eigen::MatrixXi mask(nrows, ncols);
    //     mask.setConstant(int(EventType::EXCLUDED));
    //     nsx::PeakList peaks = gSession->selectedExperiment()->getPeaks(0, 0)->peaks_;
    //     for (nsx::Peak3D* peak : peaks) {
    //         if (peak_item->peak()->enabled()) {
    //             // IntegrationRegion constructor can throw if the region is invalid
    //             try {
    //                 nsx::IntegrationRegion region(
    //                     peak, peak_item->peak()->peakEnd(), peak_item->peak()->bkgBegin(), peak_item->peak()->bkgEnd());
    //                 region.updateMask(mask, _currentFrameIndex);
    //             } catch (...) {
    //                 peak_item->peak()->setSelected(false);
    //             }
    //         }
    //     }
    //     QImage region_img(ncols, nrows, QImage::Format_ARGB32);

    //     for (int c = 0; c < ncols; ++c) {
    //         for (int r = 0; r < nrows; ++r) {
    //             EventType ev = EventType(mask(r, c));
    //             unsigned int color;

    //             switch (ev) {
    //                 case EventType::PEAK: color = green; break;
    //                 case EventType::BACKGROUND: color = yellow; break;
    //                 default: color = transparent; break;
    //             }

    //             // todo: what about unselected peaks?
    //             region_img.setPixel(c, r, color);
    //         }
    //     }
    //     if (!_integrationRegion) {
    //         _integrationRegion = addPixmap(QPixmap::fromImage(region_img));
    //         _integrationRegion->setZValue(-1);
    //     } else {
    //         _integrationRegion->setPixmap(QPixmap::fromImage(region_img));
    //     }
    // }
    // setSceneRect(_zoomStack.back());
    // emit dataChanged();

    // if (PlottableItem* p = dynamic_cast<PlottableItem*>(_lastClickedGI))
    //     gGui->updatePlot(p);
}

void DetectorScene::showPeakLabels(bool flag)
{
    for (auto p : _peak_graphics_items)
        p->showLabel(flag);
    update();
}

void DetectorScene::showPeakAreas(bool flag)
{
    for (auto p : _peak_graphics_items)
        p->showArea(flag);
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

void DetectorScene::resetScene()
{
    clearPeakItems();
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
