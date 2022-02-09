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
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/loader/XFileHandler.h"
#include "core/peak/Peak3D.h"
#include "gui/MainWin.h"
#include "gui/graphics_items/EllipseItem.h"
#include "gui/graphics_items/MaskItem.h"
#include "gui/graphics_items/PeakItemGraphic.h"
#include "gui/graphics_items/PlottableItem.h"
#include "gui/graphics_items/SXGraphicsItem.h"
#include "gui/graphics_tools/CutLineItem.h"
#include "gui/graphics_tools/CutSliceItem.h"
#include "gui/graphics_tools/CutterItem.h"
#include "gui/items/PeakCollectionItem.h"
#include "gui/items/PeakItem.h"
#include "gui/models/PeakCollectionModel.h"
#include "gui/models/Session.h"
#include "gui/subwindows/PeakWindow.h"
#include "gui/utility/ColorButton.h"
#include "gui/utility/LinkedComboBox.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

#include <QCheckBox>
#include <QDebug>
#include <QDoubleSpinBox>
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
    , _currentIntensity(3000)
    , _cursorMode(PIXEL)
    , _mode(ZOOM)
    , _zoomstart(0, 0)
    , _zoomend(0, 0)
    , _zoomrect(nullptr)
    , _selectionRect(nullptr)
    , _zoomStack()
    , _peak_model_1(nullptr)
    , _peak_model_2(nullptr)
    , _peak_graphics_items()
    , _itemSelected(false)
    , _image(nullptr)
    , _lastClickedGI(nullptr)
    , _logarithmic(false)
    , _drawIntegrationRegion1(true)
    , _drawIntegrationRegion2(true)
    , _drawDirectBeam(false)
    , _draw3rdParty(true)
    , _colormap(new ColorMap())
    , _integrationRegion1(nullptr)
    , _integrationRegion2(nullptr)
    , _selected_peak_gi(nullptr)
    , _peakPxColor1(QColor(255, 255, 0, 128)) // yellow, alpha = 0.5
    , _peakPxColor2(QColor(251, 163, 0, 128)) // dark yellow, alpha = 0.5
    , _bkgPxColor1(QColor(0, 255, 0, 128)) // green, alpha = 0.5
    , _bkgPxColor2(QColor(0, 100, 0, 128)) // dark green, alpha = 0.5
    , _3rdparty_color(Qt::black)
    , _3rdparty_size(10)
    , _beam_color(Qt::black)
    , _old_beam_color(Qt::gray)
    , _beam_size(20)
    , _selected_peak(nullptr)
    , _unit_cell(nullptr)
    , _peak_center_data(nullptr)
{
}

void DetectorScene::linkPeakModel1(PeakCollectionModel* source)
{
    _peak_model_1 = source;
    connect(
        _peak_model_1, &PeakCollectionModel::dataChanged, this,
        &DetectorScene::peakModelDataChanged);
}

void DetectorScene::linkPeakModel2(PeakCollectionModel* source)
{
    _peak_model_2 = source;
    connect(
        _peak_model_2, &PeakCollectionModel::dataChanged, this,
        &DetectorScene::peakModelDataChanged);
}

PeakCollectionModel* DetectorScene::peakModel1() const
{
    return _peak_model_1;
}

PeakCollectionModel* DetectorScene::peakModel2() const
{
    return _peak_model_2;
}

void DetectorScene::unlinkPeakModel1()
{
    _peak_model_1 = nullptr;
    connect(
        this, &DetectorScene::signalChangeSelectedFrame, this,
        &DetectorScene::peakModelDataChanged);
}

void DetectorScene::link3rdPartyPeaks(nsx::PeakCenterDataSet* pcd)
{
    _peak_center_data = pcd;
    drawPeakitems();
}

void DetectorScene::linkDirectBeamPositions(const std::vector<nsx::DetectorEvent>& events)
{
    _direct_beam_events = events;
}

void DetectorScene::linkOldDirectBeamPositions(const std::vector<nsx::DetectorEvent>& events)
{
    _old_direct_beam_events = events;
}

void DetectorScene::unlinkPeakModel2()
{
    _peak_model_2 = nullptr;
}

void DetectorScene::peakModelDataChanged()
{
    loadCurrentImage();
    drawPeakitems();
    update();
}

void DetectorScene::clearPeakItems()
{
    if (!_currentData)
        return;

    // _peak_graphics_items can be out of sync (pointer may get deleted outside). Therefore
    // do not use it for removing items from the scene (may cause crash)
    for (auto item : items()) {
        if (dynamic_cast<PeakItemGraphic*>(item) != nullptr)
            removeItem(item);
        if (dynamic_cast<PeakCenterGraphic*>(item) != nullptr) // Remove 3rd party centers
            removeItem(item);
        if (dynamic_cast<DirectBeamGraphic*>(item) != nullptr) // Remove direct beam position``
            removeItem(item);
    }

    _peak_graphics_items.clear();
}

void DetectorScene::drawPeakitems()
{
    clearPeakItems();
    if (_peak_model_1)
        drawPeakModelItems(_peak_model_1);
    if (_peak_model_2)
        drawPeakModelItems(_peak_model_2);
    if (_draw3rdParty)
        draw3rdPartyItems();
    if (_drawDirectBeam)
        drawDirectBeamPositions();
    loadCurrentImage();
}

void DetectorScene::drawDirectBeamPositions()
{
    for (auto&& event : _direct_beam_events) {
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

    for (auto&& event : _old_direct_beam_events) {
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

void DetectorScene::drawPeakModelItems(PeakCollectionModel* model)
{
    if (model == nullptr || model->root() == nullptr)
        return;

    std::vector<PeakItem*> peak_items = model->root()->peakItems();

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
}

void DetectorScene::draw3rdPartyItems()
{
    if (!_peak_center_data)
        return;
    _peak_center_items.clear();
    nsx::XFileHandler* xfh = _peak_center_data->getFrame(_currentFrameIndex);

    if (!xfh)
        return;

    for (const Eigen::Vector3d& vector : xfh->getPeakCenters()) {
        PeakCenterGraphic* center = new PeakCenterGraphic(vector);
        center->setColor(_3rdparty_color);
        center->setSize(_3rdparty_size);
        _peak_center_items.emplace_back(center);
    }

    if (_peak_center_items.empty())
        return;

    for (auto peak : _peak_center_items)
        addItem(peak);
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

void DetectorScene::slotChangeSelectedFrame(int frame)
{
    if (!_currentData)
        return;

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
    CutterItem* cutter(nullptr);
    MaskItem* mask(nullptr);
    EllipseMaskItem* ellipse_mask(nullptr);
    //_masks.emplace_back(new graphicsItem(nullptr));

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
        } else {
            if (_peak_model_1) {
                // _peak_model_2 is only relevant in DetectorWindow, ignore here.
                std::vector<nsx::Peak3D*> peaks =
                    _peak_model_1->root()->peakCollection()->getPeakList();
                if (CutterItem* p = dynamic_cast<CutterItem*>(_lastClickedGI)) {
                    // delete p....
                    _lastClickedGI = nullptr;
                    removeItem(p);
                } else if (PlottableItem* p = dynamic_cast<PlottableItem*>(_lastClickedGI))
                    gGui->updatePlot(p);
                else if (MaskItem* p = dynamic_cast<MaskItem*>(_lastClickedGI)) {
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
                } else if (EllipseMaskItem* p = dynamic_cast<EllipseMaskItem*>(_lastClickedGI)) {
                    auto it = findMask(p);
                    if (it != _masks.end()) {
                        it->second = new nsx::EllipseMask(*p->getAABB());
                        _currentData->addMask(it->second);
                        _lastClickedGI = nullptr;
                    }
                    _currentData->maskPeaks(peaks);
                    update();
                    updateMasks();
                }
            } else {
                if (MaskItem* p = dynamic_cast<MaskItem*>(_lastClickedGI)) {
                    // add a new mask
                    auto it = findMask(p);
                    if (it != _masks.end()) {
                        it->second = new nsx::BoxMask(*p->getAABB());
                        _currentData->addMask(it->second);
                        _lastClickedGI = nullptr;
                    }
                    update();
                    updateMasks();
                } else if (EllipseMaskItem* p = dynamic_cast<EllipseMaskItem*>(_lastClickedGI)) {
                    auto it = findMask(p);
                    if (it != _masks.end()) {
                        it->second = new nsx::EllipseMask(*p->getAABB());
                        _currentData->addMask(it->second);
                        _lastClickedGI = nullptr;
                    }
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
                peak_item->peak()->setRejectionFlag(nsx::RejectionFlag::ManuallyRejected, true);
                peak_item->peak()->setSelected(false);
                peak_item->setCenterColor(Qt::red);
            // If the item is a mask graphics item, remove its corresponding mask from
            // the data, update the std::vector of mask graphics items and update the scene
            } else if (MaskItem* mask_item = dynamic_cast<MaskItem*>(item)) {
                auto it = findMask(mask_item);
                if (it != _masks.end()) {
                    _currentData->removeMask(it->second);
                    _masks.erase(it);
                    update();
                    updateMasks();
                    removeItem(item);
                }
            } else if (EllipseMaskItem* ellipse_item = dynamic_cast<EllipseMaskItem*>(item)) {
                auto it = findMask(ellipse_item);
                if (it != _masks.end()) {
                    _currentData->removeMask(it->second);
                    _masks.erase(it);
                    update();
                    updateMasks();
                    removeItem(item);
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
                peak_item->peak()->setRejectionFlag(nsx::RejectionFlag::NotRejected, true);
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
    if (!_currentData)
        return;
    nsx::Diffractometer* instr = _currentData->diffractometer();
    const nsx::Detector& det = _currentData->detector();

    const int nrows = int(det.nRows());
    const int ncols = int(det.nCols());

    const int col = static_cast<int>(event->lastScenePos().x());
    const int row = static_cast<int>(event->lastScenePos().y());

    if (col < 0 || col > ncols - 1 || row < 0 || row > nrows - 1)
        return;
    const int intensity = _currentFrame(row, col);

    const nsx::Monochromator& mono = instr->source().selectedMonochromator();
    double wave = mono.wavelength();

    QString ttip;

    nsx::DirectVector pos = _currentData->detector().pixelPosition(col, row);


    bool has_state = true;
    nsx::InstrumentState state;
    try {
        state = nsx::InterpolatedState::interpolate(_currentData->instrumentStates(),
                                                    _currentFrameIndex);
    } catch (std::range_error& e) {
        // May get an interpolation error on the last frame of the set. Skip the tooltip if we
        // need an interpolated state in this instance.
        if (!(_cursorMode == PIXEL))
            has_state = false;
    }
    if (!has_state)
        return;

    switch (_cursorMode) {
        case PIXEL: {
            ttip = QString("(%1,%2) I:%3").arg(col).arg(row).arg(intensity);
            break;
        }
        case GAMMA_NU: {
            double gamma = state.gamma(pos);
            double nu = state.nu(pos);
            ttip = QString("(%1,%2) I: %3")
                       .arg(gamma / nsx::deg, 0, 'f', 3)
                       .arg(nu / nsx::deg, 0, 'f', 3)
                       .arg(intensity);
            break;
        }
        case THETA: {
            double th2 = state.twoTheta(pos);
            ttip = QString("(%1) I: %2").arg(th2 / nsx::deg, 0, 'f', 3).arg(intensity);
            break;
        }
        case D_SPACING: {
            double th2 = state.twoTheta(pos);
            ttip = QString("(%1) I: %2").arg(wave / (2 * sin(0.5 * th2)), 0, 'f', 3).arg(intensity);
            break;
        }
        case MILLER_INDICES: {
            if (_unit_cell) {
                nsx::ReciprocalVector q = state.sampleQ(pos);
                nsx::MillerIndex miller_indices(q, *_unit_cell);

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
            break;
        }
        default: break;
    }
    QToolTip::showText(event->screenPos(), ttip);
}

void DetectorScene::loadCurrentImage()
{
    if (!_currentData)
        return;

    // Full image size, front of the stack
    QRect full = _zoomStack.front();
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
    clearIntegrationRegion();
    if (_drawIntegrationRegion1 || _drawIntegrationRegion2) {
        refreshIntegrationOverlay();
    }

    setSceneRect(_zoomStack.back());
    emit dataChanged();

    if (PlottableItem* p = dynamic_cast<PlottableItem*>(_lastClickedGI))
        gGui->updatePlot(p);
}

void DetectorScene::refreshIntegrationOverlay()
{
    if (!_peak_model_1 && !_peak_model_2)
        return;

    Eigen::MatrixXi mask(_currentData->nRows(), _currentData->nCols());
    mask.setConstant(int(EventType::EXCLUDED));

    if (_peak_model_1 && _drawIntegrationRegion1) {
        getIntegrationMask(_peak_model_1, mask, _int_region_type_1);
        QImage* region_img = getIntegrationRegionImage(mask, _peakPxColor1, _bkgPxColor1);
        if (!_integrationRegion1) {
            _integrationRegion1 = addPixmap(QPixmap::fromImage(*region_img));
            _integrationRegion1->setZValue(-1);
        } else {
            _integrationRegion1->setPixmap(QPixmap::fromImage(*region_img));
        }
    }

    if (_peak_model_2 && _drawIntegrationRegion2) {
        mask.setConstant(int(EventType::EXCLUDED));
        getIntegrationMask(_peak_model_2, mask, _int_region_type_2);
        QImage* region_img = getIntegrationRegionImage(mask, _peakPxColor2, _bkgPxColor2);
        if (!_integrationRegion2) {
            _integrationRegion2 = addPixmap(QPixmap::fromImage(*region_img));
            _integrationRegion2->setZValue(-1);
        } else {
            _integrationRegion2->setPixmap(QPixmap::fromImage(*region_img));
        }
    }
}

QImage* DetectorScene::getIntegrationRegionImage(
    const Eigen::MatrixXi& mask, QColor& peak, QColor& bkg)
{
    QImage* region_img = new QImage(mask.cols(), mask.rows(), QImage::Format_ARGB32);

    for (int c = 0; c < mask.cols(); ++c) {
        for (int r = 0; r < mask.rows(); ++r) {
            EventType ev = EventType(mask(r, c));
            QColor color;

            switch (ev) {
                case EventType::PEAK: color = peak; break;
                case EventType::BACKGROUND: color = bkg; break;
                default: color = Qt::transparent; break;
            }

            // todo: what about unselected peaks?
            region_img->setPixelColor(QPoint(c, r), color);
        }
    }
    return region_img;
}

void DetectorScene::getIntegrationMask(
    PeakCollectionModel* model, Eigen::MatrixXi& mask,
    nsx::RegionType region_type /* = nsx::RegionType::VariableEllipsoid */)
{
    if (model == nullptr || model->root() == nullptr)
        return;

    std::vector<PeakItem*> peak_items = model->root()->peakItems();

    double peak_end, bkg_begin, bkg_end;
    for (PeakItem* peak_item : peak_items) {
        nsx::Peak3D* peak = peak_item->peak();
        if (_preview_int_regions_1 && model == _peak_model_1) {
            peak_end = _peak_end_1;
            bkg_begin = _bkg_begin_1;
            bkg_end = _bkg_end_1;
        } else if (_preview_int_regions_2 && model == _peak_model_2) {
            peak_end = _peak_end_2;
            bkg_begin = _bkg_begin_2;
            bkg_end = _bkg_end_2;
        } else {
            peak_end = peak_item->peak()->peakEnd();
            bkg_begin = peak_item->peak()->bkgBegin();
            bkg_end = peak_item->peak()->bkgEnd();
        }
        nsx::IntegrationRegion region(peak, peak_end, bkg_begin, bkg_end, region_type);
        if (region.isValid())
            region.updateMask(mask, _currentFrameIndex);
    }
}

void DetectorScene::initIntRegionFromPeakWidget(
    const PeakViewWidget::Set& set, bool alt /* = false */)
{
    if (!alt) {
        _preview_int_regions_1 = set.previewIntRegion->isChecked();
        _int_region_type_1 = static_cast<nsx::RegionType>(set.regionType->currentIndex());
        _peak_end_1 = set.peakEnd->value();
        _bkg_begin_1 = set.bkgBegin->value();
        _bkg_end_1 = set.bkgEnd->value();
        _drawIntegrationRegion1 = set.drawIntegrationRegion->isChecked();
        _peakPxColor1 = set.colorIntPeak->color();
        _bkgPxColor1 = set.colorIntBkg->color();
        _peakPxColor1.setAlphaF(set.alphaIntegrationRegion->value());
        _bkgPxColor1.setAlphaF(set.alphaIntegrationRegion->value());
    } else { // alternative colour scheme for second overlay
        _preview_int_regions_2 = set.previewIntRegion->isChecked();
        _int_region_type_2 = static_cast<nsx::RegionType>(set.regionType->currentIndex());
        _peak_end_1 = set.peakEnd->value();
        _peak_end_2 = set.peakEnd->value();
        _bkg_begin_2 = set.bkgBegin->value();
        _bkg_end_2 = set.bkgEnd->value();
        _drawIntegrationRegion2 = set.drawIntegrationRegion->isChecked();
        _peakPxColor2 = set.colorIntPeak->color();
        _bkgPxColor2 = set.colorIntBkg->color();
        _peakPxColor2.setAlphaF(set.alphaIntegrationRegion->value());
        _bkgPxColor2.setAlphaF(set.alphaIntegrationRegion->value());
    }
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
    if (_integrationRegion1 && !flag) {
        removeItem(_integrationRegion1);
        delete _integrationRegion1;
        _integrationRegion1 = nullptr;
    }
    if (_integrationRegion2 && !flag) {
        removeItem(_integrationRegion2);
        delete _integrationRegion2;
        _integrationRegion2 = nullptr;
    }

    _drawIntegrationRegion1 = flag;
    _drawIntegrationRegion2 = flag;

    loadCurrentImage();
}

void DetectorScene::clearIntegrationRegion()
{
    if (_integrationRegion1) {
        removeItem(_integrationRegion1);
        delete _integrationRegion1;
        _integrationRegion1 = nullptr;
    }

    if (_integrationRegion2) {
        removeItem(_integrationRegion2);
        delete _integrationRegion2;
        _integrationRegion2 = nullptr;
    }
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
    _integrationRegion1 = nullptr;
    _integrationRegion2 = nullptr;
    _masks.clear();
    _lastClickedGI = nullptr;
}

std::vector<std::pair<QGraphicsItem*, nsx::IMask*>>::iterator DetectorScene::findMask(
    QGraphicsItem* item)
{
    return std::find_if(
        _masks.begin(), _masks.end(),
        [item](const std::pair<QGraphicsItem*, nsx::IMask*>& x) { return x.first == item; });
}

void DetectorScene::setUnitCell(nsx::UnitCell* cell)
{
    _unit_cell = cell;
}

void DetectorScene::setup3rdPartyPeaks(bool draw, const QColor& color, int size)
{
    _draw3rdParty = draw;
    _3rdparty_color = color;
    _3rdparty_size = size;
}

void DetectorScene::showDirectBeam(bool show)
{
    _drawDirectBeam = show;
}
