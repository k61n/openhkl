//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/graphics/DetectorView.cpp
//! @brief     Implements class DetectorView
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/graphics/DetectorView.h"

#include "gui/graphics/DetectorScene.h"
#include "core/experiment/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/raw/IDataReader.h"
#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>

DetectorView::DetectorView(QWidget* parent) : QGraphicsView(parent), _scene(new DetectorScene(this))
{
    setRenderHints(
        QPainter::Antialiasing | QPainter::SmoothPixmapTransform
        | QPainter::HighQualityAntialiasing);
    setScene(_scene);
    // Make sure that first views are rescaled, especially first created one
    connect(_scene, &DetectorScene::dataChanged, this, [&]() { fitInView(_scene->sceneRect()); });
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setInteractive(true);
    // Invert the y-axis so that (0,0) coordinate is at bottom left (and not top
    // left) This match detector coordinates in NSXTool
    scale(1, -1);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void DetectorView::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    fitInView(_scene->sceneRect());
}

DetectorScene* DetectorView::getScene()
{
    return _scene;
}

void DetectorView::copyViewToClipboard()
{
    // Create the image with the exact size of the shrunk scene
    QImage image(this->rect().size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    this->render(&painter);
    painter.end();
    QApplication::clipboard()->setImage(image, QClipboard::Clipboard);
}

void DetectorView::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy))
        copyViewToClipboard();
    QGraphicsView::keyPressEvent(event);
}

void DetectorView::fitScene()
{
    fitInView(_scene->sceneRect());
}

void DetectorView::fixDetectorAspectRatio(bool value)
{
    const nsx::Detector* detector = _scene->getData()->reader()->diffractometer()->detector();

    if (value) {
        int h = this->height();
        double dw = detector->width();
        double dh = detector->height();
        resize(int(h * dw / dh), h);
    } else {
        fitInView(_scene->sceneRect());
    }
}
