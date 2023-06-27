//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/graphics/CrystalScene.cpp
//! @brief     Implements class CrystalScene
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <cmath>
#include <sstream>

#include <QGraphicsTextItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWidget>

#include "base/utils/Units.h"

// #include "apps/dialogs/DialogCalibrateDistance.h"
#include "gui/graphics/CrystalFaceItem.h"
#include "gui/graphics/CrystalNodeItem.h"
#include "gui/graphics/CrystalScene.h"
#include "gui/graphics_items/PinItem.h"
#include "gui/graphics_tools/RulerItem.h"

CrystalScene::CrystalScene(ohkl::ConvexHull* hull, QWidget* parent)
    : QGraphicsScene(parent)
    , pixmapitem(nullptr)
    , _ruler(nullptr)
    , _pin(nullptr)
    , _hull(hull)
    , _text(nullptr)
{
    distance = 1.0;
    //    _distancedialog = new DialogCalibrateDistance();
    //    connect(
    //        _distancedialog, SIGNAL(on_calibrateDistanceSpinBox_valueChanged(double)), this,
    //        SLOT(getDistance(double)));
    //    connect(
    //        _distancedialog, SIGNAL(on_calibrateDistanceButtonOK_accepted()), this,
    //        SLOT(stampScale()));
    _ruler = new RulerItem;
    _pin = new PinItem;
    _pinCreated = false;
}

CrystalScene::~CrystalScene() = default;

void CrystalScene::loadImage(QString filename)
{
    _pix.load(filename);
    if (!pixmapitem) {
        pixmapitem = addPixmap(_pix);
        int w = pixmapitem->pixmap().width();
        pixmapitem->setScale(800.0 / w);
    } else
        pixmapitem->setPixmap(_pix);

    QImage image = pixmapitem->pixmap().toImage();
    image = image.convertToFormat(QImage::Format_Indexed8);
    QVector<QRgb> table = image.colorTable();

    int i = 0;
    for (auto it = table.begin(); it != table.end(); ++it, i++)
        image.setColor(i, *it + 200);
}

void CrystalScene::changeBrigthness(int a)
{
    QImage image = _pix.toImage();
    for (int y = 0; y < image.height(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            double newR = qRed(line[x]) / 255.0;
            double newG = qGreen(line[x]) / 255.0;
            double newB = qBlue(line[x]) / 255.0;
            if (a < 0.0) {
                newR *= (1.0 + a / 100.0);
                newG *= (1.0 + a / 100.0);
                newB *= (1.0 + a / 100.0);
            } else {
                newR += ((1 - newR) * a / 100.0);
                newG += ((1 - newG) * a / 100.0);
                newB += ((1 - newB) * a / 100.0);
            }
            line[x] = qRgb(
                static_cast<int>(newR * 255), static_cast<int>(newG * 255),
                static_cast<int>(newB * 255));
        }
    }
    pixmapitem->setPixmap(QPixmap::fromImage(image));
}

void CrystalScene::activateCalibrateDistance()
{
    mode = calibrateDistance;
}

void CrystalScene::activatePickCenter()
{
    mode = pickCenter;
}

void CrystalScene::activatePickingPoints()
{
    mode = pickingPoint;
}

void CrystalScene::activateRemovingPoints()
{
    mode = removingPoint;
}

void CrystalScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF pos = event->scenePos();
    if (mode == calibrateDistance) {
        _ruler->startDrawingAt(pos);
        addItem(_ruler);
        update();
    } else if (mode == pickingPoint || mode == nodeSelected) {
        QGraphicsItem* temp = itemAt(pos.x(), pos.y(), QTransform());
        if (dynamic_cast<CrystalNodeItem*>(temp)) {
            mode = nodeSelected;
            _current = dynamic_cast<CrystalNodeItem*>(temp);
        } else {
            mode = pickingPoint;
            CrystalNodeItem* n = new CrystalNodeItem;
            double yc, zc;
            _pin->getCenter(yc, zc, _rotationAngle);
            n->initiate(pos.x(), pos.y(), yc, _rotationAngle);
            addItem(n);
        }
    } else if (mode == pickCenter) {
        if (!_pinCreated) {
            addItem(_pin);
            _pin->initiate(pos.y(), _rotationAngle);
            update();
            _pinCreated = true;
        }
    } else if (mode == removingPoint) {
        QGraphicsItem* temp = itemAt(pos.x(), pos.y(), QTransform());
        if (CrystalNodeItem* temp2 = dynamic_cast<CrystalNodeItem*>(temp)) {
            double x, y, z;
            temp2->getCoordinates(x, y, z);
            _hull->removeVertex(
                Eigen::Vector3d(x * aspectratio, y * aspectratio, z * aspectratio), 1e-3);
            removeItem(temp);
        }
    }
}

void CrystalScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (mode == calibrateDistance) {
        if (_ruler) {
            _ruler->moveTipAt(event->scenePos());
            update();
        }
    } else if (mode == pickCenter && _pinCreated) {
        if (event->buttons() == Qt::LeftButton) {
            _pin->adjust(event->scenePos().y(), _rotationAngle);
            update();
        }
    } else if (mode == nodeSelected) {
        if (event->buttons() == Qt::LeftButton) {
            double yc, zc;
            _pin->getCenter(yc, zc, _rotationAngle);
            _current->adjust(event->scenePos().y(), yc, _rotationAngle);
        }
    }
}

void CrystalScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    //! When mouse is released in calibrateDistanceMode,
    //! open widget to enter distance then close.
    QPointF point = event->scenePos();
    if (mode == calibrateDistance) {
        scaleex = point.x();
        scaleey = point.y();
        mode = none;
        // _distancedialog->show();
    } else if (mode == pickCenter) {
        _pin->adjust(point.y(), _rotationAngle);
        calibrateCenterOK(point.x(), point.y());
    } else if (mode == nodeSelected) {
        double yc, zc;
        _pin->getCenter(yc, zc, _rotationAngle);
        _current->adjust(point.y(), yc, _rotationAngle);
    }
}

void CrystalScene::getDistance(double val)
{
    distance = val;
}

void CrystalScene::stampScale()
{
    QGraphicsItem* stamp = _ruler->stampRuler(distance, QPointF(5, 550));
    addItem(stamp);
    calibrateDistanceOK(npixels, distance);
    _ruler->setDistance(distance);
    _ruler->activateDistanceDisplay();
    if (_ruler)
        removeItem(_ruler);
    aspectratio = _ruler->getConversion();
}

void CrystalScene::setRotationAngle(double angle)
{
    _rotationAngle = angle;
    QList<QGraphicsItem*> list = items();
    double yc, zc;
    _pin->getCenter(yc, zc, _rotationAngle);
    for (QList<QGraphicsItem*>::iterator it = list.begin(); it != list.end(); it++) {
        CrystalNodeItem* temp = dynamic_cast<CrystalNodeItem*>(*it);
        if (temp)
            temp->rotate(angle, yc);
        CrystalFaceItem* temp2 = dynamic_cast<CrystalFaceItem*>(*it);
        if (temp2)
            temp2->rotate(angle, yc);
    }
    _pin->rotate(angle);
}

void CrystalScene::triangulate()
{
    _hull->reset();
    QList<QGraphicsItem*> list = items();

    for (QList<QGraphicsItem*>::iterator it = list.begin(); it != list.end(); it++) {
        CrystalNodeItem* temp = dynamic_cast<CrystalNodeItem*>(*it);
        if (temp) {
            double x, y, z;
            temp->getCoordinates(x, y, z);
            _hull->addVertex(Eigen::Vector3d{x * aspectratio, y * aspectratio, z * aspectratio});
        }
    }

    try {
        _hull->updateHull();
    } catch (std::exception& e) {
        QMessageBox::critical(nullptr, "OpenHKL", e.what());
        return;
    }

    double yc, zc;
    _pin->getCenter(yc, zc, _rotationAngle);
    for (QList<QGraphicsItem*>::iterator it = list.begin(); it != list.end(); it++) {
        CrystalFaceItem* temp2 = dynamic_cast<CrystalFaceItem*>(*it);
        if (temp2)
            removeItem(temp2);
    }

    for (const auto& face : _hull->faces()) {
        const Eigen::Vector3d& a = face[0];
        const Eigen::Vector3d& b = face[1];
        const Eigen::Vector3d& c = face[2];

        CrystalFaceItem* temp = new CrystalFaceItem();
        temp->set(a / aspectratio, b / aspectratio, c / aspectratio);
        temp->rotate(_rotationAngle, yc);
        addItem(temp);
    }

    update();
}

void CrystalScene::drawText(QString text)
{
    if (!_text) {
        _text = addText(text);
        _text->setDefaultTextColor(QColor(Qt::red));
    } else
        _text->setPlainText(text);
}
