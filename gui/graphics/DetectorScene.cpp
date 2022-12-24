//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/DetectorScene.cpp
//! @brief     Implements class DetectorScene
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/DetectorScene.h"

#include "base/geometry/AABB.h"
#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "core/loader/XFileHandler.h"
#include "core/peak/Peak3D.h"
#include "gui/MainWin.h"
#include "gui/graphics/PeakCollectionGraphics.h"
#include "gui/graphics_items/CrosshairGraphic.h"
#include "gui/graphics_items/EllipseMaskItem.h"
#include "gui/graphics_items/MaskItem.h"
#include "gui/graphics_items/PeakItemGraphic.h"
#include "gui/graphics_items/PlottableItem.h"
#include "gui/graphics_items/SXGraphicsItem.h"
#include "gui/graphics_tools/CutLineItem.h"
#include "gui/graphics_tools/CutSliceItem.h"
#include "gui/graphics_tools/CutterItem.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/models/MaskHandler.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/models/Session.h"
#include "gui/subwindows/PeakWindow.h"
#include "gui/widgets/PeakViewWidget.h"

#include <exception>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPainterPath>
#include <QPixmap>
#include <QToolTip>
#include <opencv2/core/types.hpp>

QPointF DetectorScene::_current_beam_position = {0, 0};

DetectorScene::DetectorScene(std::size_t npeakcollections, QObject* parent)
    : QGraphicsScene(parent)
    , _params()
    , _currentData(nullptr)
    , _currentFrameIndex(-1)
    , _mode(ZOOM)
    , _zoomstart(0, 0)
    , _zoomend(0, 0)
    , _zoomrect(nullptr)
    , _selectionRect(nullptr)
    , _zoomStack()
    , _itemSelected(false)
    , _image(nullptr)
    , _lastClickedGI(nullptr)
    , _selected_peak_gi(nullptr)
    , _max_peak_collections(npeakcollections)
    , _beam_color(Qt::black)
    , _old_beam_color(Qt::gray)
    , _beam_size(20)
    , _beam_pos_setter(nullptr)
    , _cutter(nullptr)
    , _selected_peak(nullptr)
    , _peak(nullptr)
    , _mask_handler(std::make_shared<MaskHandler>())
{
    _dataset_graphics = std::make_unique<DataSetGraphics>(&_params);
    for (std::size_t idx = 0; idx < _max_peak_collections; ++idx)
        _peak_graphics.push_back(std::make_unique<PeakCollectionGraphics>());
}

void DetectorScene::onGradientSetting(int kernel, bool fft)
{
    try {
        _params.gradientKernel = static_cast<ohkl::GradientKernel>(kernel);
        _params.fftGradient = fft;
        loadCurrentImage();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Error", QString(e.what()));
    }
}

void DetectorScene::addBeamSetter(int size, int linewidth)
{
    if (_beam_pos_setter) {
        removeBeamSetter();
        delete _beam_pos_setter;
    }

    _beam_pos_setter = new CrosshairGraphic(_current_beam_position);
    _beam_pos_setter->setSize(size);
    _beam_pos_setter->setLinewidth(linewidth);
    addItem(_beam_pos_setter);
}

void DetectorScene::removeBeamSetter()
{
    for (auto item : items())
        if (dynamic_cast<CrosshairGraphic*>(item) != nullptr)
            removeItem(item);
}

void DetectorScene::showBeamSetter(bool show)
{
    _beam_pos_setter->setVisible(show);
    update();
}

void DetectorScene::linkPeakModel(
    PeakCollectionModel* source, PeakViewWidget* widget, std::size_t idx /* = 0 */)
{
    // unless specified, link model to first element of _peak_graphics
    if (idx >= _max_peak_collections)
        throw std::range_error(
            "DetectorScene::linkPeakModel: _peak_graphics index out of range");

    _peak_graphics.at(idx)->setPeakModel(source);
    _peak_graphics.at(idx)->setPeakViewWidget(widget);
    connect(
        _peak_graphics.at(idx)->peakModel(), &PeakCollectionModel::dataChanged, this,
        &DetectorScene::peakModelDataChanged);
}

void DetectorScene::setVisualisationMode(VisualisationType vtype)
{
    for (const auto& graphic : _peak_graphics)
        graphic->setVisualisationType(vtype);
}

PeakCollectionGraphics* DetectorScene::peakCollectionGraphics(std::size_t idx) const
{
    if (idx >= _max_peak_collections)
        throw std::range_error(
            "DetectorScene::peakCollectionGraphics: _peak_graphics index out of range");
    return _peak_graphics.at(idx).get();
}

void DetectorScene::link3rdPartyPeaks(ohkl::PeakCenterDataSet* pcd, std::size_t idx)
{
    if (idx >= _max_peak_collections)
        throw std::range_error(
            "DetectorScene::link3rdPartyPeaks: _peak_graphics index out of range");

    _peak_graphics.at(idx)->setExtPeakData(pcd);
    drawPeakItems();
}

void DetectorScene::linkPerFrameSpots(
    std::vector<std::vector<cv::KeyPoint>>* points, std::size_t idx)
{
    if (idx >= _max_peak_collections)
        throw std::range_error(
            "DetectorScene::linkPerFrameSpots: _peak_graphics index out of range");

    _peak_graphics.at(idx)->setPerFrameSpots(points);
    drawPeakItems();
}

void DetectorScene::linkDirectBeamPositions(std::vector<ohkl::DetectorEvent>* events)
{
    _direct_beam_events = events;
}

void DetectorScene::linkOldDirectBeamPositions(std::vector<ohkl::DetectorEvent>* events)
{
    _old_direct_beam_events = events;
}

void DetectorScene::peakModelDataChanged()
{
    loadCurrentImage();
    drawPeakItems();
    update();
}

void DetectorScene::clearPeakItems()
{
    if (!_currentData)
        return;

    for (auto item : items()) {
        if (dynamic_cast<PeakItemGraphic*>(item) != nullptr)
            removeItem(item);
        if (dynamic_cast<PeakCenterGraphic*>(item) != nullptr) // Remove 3rd party centers
            removeItem(item);
        if (dynamic_cast<DirectBeamGraphic*>(item) != nullptr) // Remove direct beam position``
            removeItem(item);
    }
}

void DetectorScene::drawPeakItems()
{
    clearPeakItems();
    for (const auto& graphic : _peak_graphics) {
        if (graphic->peaksEnabled()) {
            for (auto* peak_graphic : graphic->peakItemGraphics(_currentFrameIndex))
                addItem(peak_graphic);
        }
        if (graphic->extPeaksEnabled()) {
            for (auto* peak_graphic : graphic->extPeakGraphics(_currentFrameIndex))
                addItem(peak_graphic);
        }
        if (graphic->detectorSpotsEnabled()) {
            for (auto* peak_graphic : graphic->detectorSpots(_currentFrameIndex))
              addItem(peak_graphic);
        }
    }
    if (_params.directBeam)
        drawDirectBeamPositions();
    loadCurrentImage();
}

void DetectorScene::drawDirectBeamPositions()
{
    for (auto&& event : *_direct_beam_events) {
        double upper = double(_currentFrameIndex) + 0.01;
        double lower = double(_currentFrameIndex) - 0.01;
        if (event.frame < upper && event.frame > lower) {
            DirectBeamGraphic* beam = new DirectBeamGraphic();
            beam->setPos(event.px, event.py);
            beam->setZValue(10);
            beam->setAcceptHoverEvents(false);
            beam->setRect(-_beam_size / 2, -_beam_size / 2, _beam_size, _beam_size);
            QPen pen;
            pen.setCosmetic(true);
            pen.setColor(_beam_color);
            pen.setStyle(Qt::SolidLine);
            beam->setPen(pen);
            addItem(beam);
        }
    }

    for (auto&& event : *_old_direct_beam_events) {
        double upper = double(_currentFrameIndex) + 0.01;
        double lower = double(_currentFrameIndex) - 0.01;
        if (event.frame < upper && event.frame > lower) {
            DirectBeamGraphic* beam = new DirectBeamGraphic();
            beam->setPos(event.px, event.py);
            beam->setZValue(10);
            beam->setAcceptHoverEvents(false);
            beam->setRect(-_beam_size / 2, -_beam_size / 2, _beam_size, _beam_size);
            QPen pen;
            pen.setCosmetic(true);
            pen.setColor(_old_beam_color);
            pen.setStyle(Qt::SolidLine);
            beam->setPen(pen);
            addItem(beam);
        }
    }
}

void DetectorScene::slotChangeSelectedData(ohkl::sptrDataSet data, int frame_1based)
{
    if (data != _currentData) {
        _currentData = data;
        _currentData->open();
        _currentFrameIndex = -1;
        _dataset_graphics->setData(_currentData);

        _zoomStack.clear();
        _zoomStack.push_back(QRect(0, 0, int(_currentData->nCols()), int(_currentData->nRows())));

        loadMasksFromData();

        if (_lastClickedGI != nullptr) {
            removeItem(_lastClickedGI);
            _lastClickedGI = nullptr;
        }

        _current_beam_position = {_currentData->nCols() / 2.0, _currentData->nRows() / 2.0};
    }

    slotChangeSelectedFrame(frame_1based);
}

void DetectorScene::slotChangeSelectedFrame(int frame_1based)
{
    if (!_currentData)
        return;

    _currentData->open();

    if (frame_1based - 1 == _currentFrameIndex)
        return;

    _currentFrameIndex = frame_1based - 1;

    clearPeakItems();
    loadCurrentImage();
    updateMasks();
    drawPeakItems();
}

void DetectorScene::setMaxIntensity(int intensity)
{
    if (_params.intensity == intensity)
        return;
    _params.intensity = intensity;

    if (!_currentData)
        return;

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
            if (!_zoomrect)
                return;
            QRectF zoom = _zoomrect->rect();
            zoom.setBottomRight(event->lastScenePos());
            _zoomrect->setRect(zoom);
            return;
        } else if (_mode == SELECT) {
            if (!_selectionRect)
                return;
            QRectF select = _selectionRect->rect();
            select.setBottomRight(event->lastScenePos());
            _selectionRect->setRect(select);
            return;
        } else if (_mode == DRAG_DROP) {
            _current_dragged_item->setPos(event->scenePos());
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

        QList<QGraphicsItem*> gItemList =
            items(point, Qt::IntersectsItemShape, Qt::DescendingOrder, trans);
        for (auto gItem : gItemList) {
            if (!gItem)
                continue;

            PeakItemGraphic* p = dynamic_cast<PeakItemGraphic*>(gItem);

            if (p)
                emit signalSelectedPeakItemChanged(p);
        }
    }
}

void DetectorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (_cutter) {
        removeItem(_cutter);
        delete _cutter;
        _cutter = nullptr;
    }

    if (_selectionRect) {
        removeItem(_selectionRect);
        delete _selectionRect;
        _selectionRect = nullptr;
    }

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
        // If the item is a OpenHKLs GI and is selected it will become the current
        // active GI
        if (SXGraphicsItem* p = dynamic_cast<SXGraphicsItem*>(item)) {
            if (p->isSelected()) {
                _lastClickedGI = p;
                return;
            }
        }
        for (auto* item : items(event->scenePos())) {
            PeakItemGraphic* peak_item = dynamic_cast<PeakItemGraphic*>(item);
            if (peak_item) {
                _selected_peak = peak_item;
                break;
            }
        }
        switch (_mode) {
            case SELECT: {
                QPoint select_start = event->lastScenePos().toPoint();
                QPoint select_end = select_start;
                _selectionRect = addRect(QRect(select_start, select_end));

                pen1 = QPen(QBrush(QColor("gray")), 1.0);
                pen1.setWidth(1);
                pen1.setCosmetic(true);
                _selectionRect->setBrush(QBrush(Qt::transparent));
                _selectionRect->setPen(pen1);
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
                _cutter = new CutSliceItem(_currentData, true);
                break;
            }
            case VERTICALSLICE: {
                _cutter = new CutSliceItem(_currentData, false);
                break;
            }
            case LINE: {
                _cutter = new CutLineItem(_currentData);
                break;
            }
            case MASK: {
                auto* mask = new MaskItem(_currentData, new ohkl::AABB);
                mask->setFrom(event->lastScenePos());
                mask->setTo(event->lastScenePos());

                addItem(mask);
                _lastClickedGI = mask;
                break;
            }
            case ELLIPSE_MASK: {
                auto* ellipse_mask = new EllipseMaskItem(_currentData, new ohkl::AABB);
                ellipse_mask->setFrom(event->lastScenePos());
                ellipse_mask->setTo(event->lastScenePos());
                addItem(ellipse_mask);
                _lastClickedGI = ellipse_mask;
                break;
            }
            case DRAG_DROP: {
                _current_dragged_item = _beam_pos_setter;
                break;
            }
            default: break;
        }
        if (_cutter != nullptr) {
            _cutter->setFrom(event->lastScenePos());
            addItem(_cutter);
            _lastClickedGI = _cutter;
        }
    }
    // The right button was pressed
    else if (event->buttons() & Qt::RightButton) {
        if (_zoomStack.size() > 1) {
            // Remove the last zoom area stored in the stack
            _zoomStack.pop();
            // If not root, then update the scene
            if (!_zoomStack.empty()) {
                setSceneRect(_zoomStack.top());
                emit dataChanged();
            }
        }
    }
}

void DetectorScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    // If no data is loaded, do nothing
    if (!_currentData)
        return;

    if (event->button() == Qt::LeftButton) {
        for (auto* item : items(event->scenePos())) {
            PeakItemGraphic* peak_item = dynamic_cast<PeakItemGraphic*>(item);
            if (peak_item) {
                auto* peak = peak_item->peak();
                try {
                    PeakWindow* window = new PeakWindow(peak);
                    window->show();
                } catch (std::runtime_error& e) {
                    gGui->statusBar()->showMessage(
                        "Invalid integration region; could not open peak window");
                    continue;
                }

                if (peak_item == _lastClickedGI)
                    _lastClickedGI = nullptr;
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

        if (_selected_peak) {
            _peak = _selected_peak->peak();
            emit signalPeakSelected(_selected_peak->peak());
        }

        if (_mode == SELECT) {
            if (_selectionRect) {
                qreal top = _selectionRect->rect().top();
                qreal bot = _selectionRect->rect().bottom();
                qreal left = _selectionRect->rect().left();
                qreal right = _selectionRect->rect().right();

                // If the user just clicked on the left mouse button with holding it, skip
                // the event
                if (qAbs(top - bot) <= 1 || qAbs(left - right) <= 1) {
                    if (_selectionRect) {
                        removeItem(_selectionRect);
                        delete _selectionRect;
                        _selectionRect = nullptr;
                    }
                    return;
                }

                if (top > bot)
                    std::swap(top, bot);

                if (right < left)
                    std::swap(left, right);

                clearSelection();
                _selectionRect->setRect(left, top, right - left, bot - top);
                QPainterPath path;
                path.addRect(_selectionRect->rect());
                setSelectionArea(path);
                // now mark associated detector masks in DataSet as selected
                //_mask_handler->updateSelectionFlag(_currentData);
                // updateMaskObjects();
                // tell the gui table to update
                emit signalMasksSelected();
            }
        } else if (_mode == ZOOM) {
            if (_zoomrect) {
                qreal top = _zoomrect->rect().top();
                qreal bot = _zoomrect->rect().bottom();
                qreal left = _zoomrect->rect().left();
                qreal right = _zoomrect->rect().right();

                // If the user just clicked on the left mouse button with holding it, skip
                // the event
                if (qAbs(top - bot) <= 1 || qAbs(left - right) <= 1) {
                    // _zoomrect->setVisible(false);
                    if (_zoomrect) {
                        removeItem(_zoomrect);
                        delete _zoomrect;
                        _zoomrect = nullptr;
                    }
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
                // _zoomrect->setVisible(false);
                if (_zoomrect) {
                    removeItem(_zoomrect);
                    delete _zoomrect;
                    _zoomrect = nullptr;
                }
                emit dataChanged();
            }
        } else if (_mode == DRAG_DROP) {
            _current_dragged_item->setPos(event->scenePos());
            int size = _current_dragged_item->size();
            int linewidth = _current_dragged_item->linewidth();
            emit beamPosChanged(event->scenePos());
            _current_beam_position = event->scenePos();
            addBeamSetter(size, linewidth);
        } else {
            for (const auto& graphic : _peak_graphics) {
                PeakCollectionModel* model = graphic->peakModel();
                std::vector<ohkl::Peak3D*> peaks;
                std::map<ohkl::Peak3D*, ohkl::RejectionFlag> tmp_map;
                if (model)
                    peaks = model->root()->peakCollection()->getPeakList();
                if (CutterItem* p = dynamic_cast<CutterItem*>(_lastClickedGI)) {
                    _lastClickedGI = nullptr;
                    removeItem(p);
                } else if (PlottableItem* p = dynamic_cast<PlottableItem*>(_lastClickedGI))
                    gGui->updatePlot(p);
                else if (MaskItem* p = dynamic_cast<MaskItem*>(_lastClickedGI)) {
                    // add a new mask
                    if (!_mask_handler->findIMask(_currentData, p)) {
                        // no IMask has been found
                        _mask_handler->addIMask(_currentData, p, new ohkl::BoxMask(*p->getAABB()));
                        emit signalMaskChanged();
                        _lastClickedGI = nullptr;
                    }
                    std::map<ohkl::Peak3D*, ohkl::RejectionFlag> tmp_map;
                    _currentData->maskPeaks(peaks, tmp_map);
                    gGui->statusBar()->showMessage(
                        QString::number(tmp_map.size()) + " peaks masked");
                    update();
                    updateMasks();

                } else if (EllipseMaskItem* p = dynamic_cast<EllipseMaskItem*>(_lastClickedGI)) {
                    if (!_mask_handler->findIMask(_currentData, p)) {
                        // no IMask has been found
                        _mask_handler->addIMask(
                            _currentData, p, new ohkl::EllipseMask(*p->getAABB()));
                        emit signalMaskChanged();
                        _lastClickedGI = nullptr;
                    }
                    _currentData->maskPeaks(peaks, tmp_map);
                    gGui->statusBar()->showMessage(
                        QString::number(tmp_map.size()) + " peaks masked");
                    update();
                    updateMasks();
                }

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

    // The user pressed on Backspace key
    if (event->key() == Qt::Key_Backspace) {
        for (QGraphicsItem* item : selectedItems()) {
            SXGraphicsItem* p = dynamic_cast<SXGraphicsItem*>(item);
            if (p == nullptr)
                continue;
            // The item must be deletable
            if (!p->isDeletable())
                continue;

            // If the item is a peak graphics item, remove its corresponding peak from
            // the data, update the set of peak graphics items and update the scene
            if (PeakItemGraphic* peak_item = dynamic_cast<PeakItemGraphic*>(item)) {
                peak_item->peak()->setRejectionFlag(ohkl::RejectionFlag::ManuallyRejected, true);
                peak_item->peak()->setSelected(false);
                peak_item->setCenterColor(Qt::red);
                // If the item is a mask graphics item, remove its corresponding mask from
                // the data, update the std::vector of mask graphics items and update the scene
            } else if (MaskItem* mask_item = dynamic_cast<MaskItem*>(item)) {
                if (_mask_handler->removeMask(_currentData, mask_item)) {
                    emit signalMaskChanged();
                    update();
                    updateMasks();
                }

            } else if (EllipseMaskItem* ellipse_item = dynamic_cast<EllipseMaskItem*>(item)) {
                if (_mask_handler->removeMask(_currentData, ellipse_item)) {
                    emit signalMaskChanged();
                    update();
                    updateMasks();
                }
            }
            if (p == _lastClickedGI)
                _lastClickedGI = nullptr;
        }
    }
    if (event->key() == Qt::Key_U) {
        for (QGraphicsItem* item : selectedItems()) {
            SXGraphicsItem* p = dynamic_cast<SXGraphicsItem*>(item);
            if (p == nullptr)
                continue;
            // The item must be deletable
            if (!p->isDeletable())
                continue;

            // If the item is a peak graphics item, remove its corresponding peak from
            // the data, update the set of peak graphics items and update the scene
            if (PeakItemGraphic* peak_item = dynamic_cast<PeakItemGraphic*>(item)) {
                peak_item->peak()->setRejectionFlag(ohkl::RejectionFlag::NotRejected, true);
                peak_item->peak()->setSelected(true);
                peak_item->setCenterColor(Qt::green);
            }
            if (p == _lastClickedGI)
                _lastClickedGI = nullptr;
        }
    }
}

void DetectorScene::createToolTipText(QGraphicsSceneMouseEvent* event)
{
    const int col = static_cast<int>(event->lastScenePos().x());
    const int row = static_cast<int>(event->lastScenePos().y());

    auto tooltip = _dataset_graphics->tooltip(col, row);
    if (!tooltip)
        return;

    QToolTip::showText(event->screenPos(), tooltip.value());
}

void DetectorScene::loadCurrentImage()
{
    if (!_currentData)
        return;

    // Full image size, front of the stack
    QRect full = _zoomStack.front();
    if (_currentFrameIndex >= _currentData->nFrames())
        _currentFrameIndex = _currentData->nFrames() - 1;
    std::optional<QImage> base_image = _dataset_graphics->baseImage(_currentFrameIndex, full);


    if (base_image) {
        if (!_image)
            _image = addPixmap(QPixmap::fromImage(base_image.value()));
        else
            _image->setPixmap(QPixmap::fromImage(base_image.value()));
        _image->setZValue(-2);
    }

    // update the integration region pixmap
    drawIntegrationRegion();

    // let's recreate QGraphicItems from masks in DataSet since life cycle of this entitys seems
    // unpreditable at best
    _mask_handler->rebuildMasks(_currentData);
    _mask_handler->setVisibleFlags(_currentData, _params.masks);
    addMasks();

    setSceneRect(_zoomStack.back());
    emit dataChanged();

    if (PlottableItem* p = dynamic_cast<PlottableItem*>(_lastClickedGI))
        gGui->updatePlot(p);
}

void DetectorScene::drawIntegrationRegion()
{
    if (_peak_graphics.empty())
        return;

    clearIntegrationRegion();

    ohkl::Peak3D* peak = nullptr;
    if (_params.singlePeakIntRegion)
        peak = _peak;
    for (const auto& graphic : _peak_graphics) {
        graphic->initIntRegionFromPeakWidget();
        QImage* region_img = graphic->getIntegrationRegionImage(_currentFrameIndex, peak);
        if (region_img) {
            QGraphicsPixmapItem* overlay = addPixmap(QPixmap::fromImage(*region_img));
            overlay->setZValue(-1);
            _integration_regions.push_back(overlay);
        }
    }
}

void DetectorScene::clearIntegrationRegion()
{
    // clear existing integration regions
    for (auto* region : _integration_regions) {
        removeItem(region);
        delete region;
    }
    _integration_regions.clear();
}

void DetectorScene::clearMasks()
{
    if (!_currentData)
        return;
    for (auto item : items()) {
        if (dynamic_cast<MaskItem*>(item) != nullptr)
            removeItem(item);
        if (dynamic_cast<EllipseMaskItem*>(item) != nullptr)
            removeItem(item);
    }
    _mask_handler->clearGraphicMaskItems(_currentData);
}

void DetectorScene::resetScene()
{
    clearPeakItems();
    clear();
    loadMasksFromData();
    _params.masks = false;
    _currentData = nullptr;
    _currentFrameIndex = 0;
    _zoomrect = nullptr;
    _zoomStack.clear();
    _image = nullptr;
    clearIntegrationRegion();
    _lastClickedGI = nullptr;
}

void DetectorScene::resetElements()
{
    clearPeakItems();
    clear();
    _zoomrect = nullptr;
    _image = nullptr;
    clearIntegrationRegion();
    _lastClickedGI = nullptr;
    loadMasksFromData();
    _params.masks = false;
}

void DetectorScene::setUnitCell(ohkl::UnitCell* cell)
{
    _dataset_graphics->setUnitCell(cell);
}

void DetectorScene::showDirectBeam(bool show)
{
    _params.directBeam = show;
}

Eigen::Vector3d DetectorScene::getBeamSetterPosition() const
{
    return {
        _beam_pos_setter->pos().x(), _beam_pos_setter->pos().y(),
        static_cast<double>(_currentFrameIndex)};
}

void DetectorScene::setBeamSetterPos(QPointF pos)
{
    _current_beam_position = pos;
    _beam_pos_setter->setPos(pos);
}

void DetectorScene::onCrosshairChanged(int size, int linewidth)
{
    _beam_pos_setter->setSize(size);
    _beam_pos_setter->setLinewidth(linewidth);
}

void DetectorScene::toggleMasks()
{
    _params.masks = !_params.masks;
    // setMasksVisible(_params.masks);
    _mask_handler->setVisibleFlags(_currentData, _params.masks);
}

QPointF DetectorScene::beamSetterCoords()
{
    return _current_beam_position;
}

void DetectorScene::setPeak(ohkl::Peak3D* peak)
{
    _peak = peak;
    drawIntegrationRegion();
}

void DetectorScene::loadMasksFromData()
{
    if (!_currentData)
        return;
    clearMasks();

    _mask_handler->rebuildMasks(_currentData);
    for (auto& m : _mask_handler->getGraphicalMaskItems(_currentData))
        addItem(m);
    emit signalMaskChanged();
    update();
}


void DetectorScene::addMasks()
{
    for (const auto& item : _mask_handler->getGraphicalMaskItems(_currentData))
        addItem(item);
}

std::shared_ptr<MaskHandler> DetectorScene::getMaskHandler()
{
    return _mask_handler;
}
