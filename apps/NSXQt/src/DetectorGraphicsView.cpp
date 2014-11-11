#include "DetectorGraphicsView.h"
#include "DetectorScene.h"

#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>
#include "IData.h"
#include "Detector.h"

DetectorGraphicsView::DetectorGraphicsView(QWidget* parent) : QGraphicsView(parent), _scene(new DetectorScene(this))
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    setScene(_scene);
    // Make sure that first views are rescaled, especially first created one
    connect(_scene,&DetectorScene::dataChanged,this,[&](){fitInView(_scene->sceneRect());});
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
}

void DetectorGraphicsView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    fitInView(_scene->sceneRect());
}

DetectorScene* DetectorGraphicsView::getScene()
{
    return _scene;
}

