#include "DetectorGraphicsView.h"
#include "DetectorScene.h"

#include <QPainter>
#include <QToolTip>
#include <QMouseEvent>

DetectorGraphicsView::DetectorGraphicsView(QWidget* parent) : QGraphicsView(parent), _scene(new DetectorScene(this))
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    setScene(_scene);
    // Make sure that first views are rescaled, especially first created one
    connect(_scene,&DetectorScene::hasReceivedData,this,[&](){fitInView(_scene->sceneRect());});
    // Called when the user interact with the zoom in the scene
    connect(_scene,&DetectorScene::zoomChanged,this,[&](){fitInView(_scene->sceneRect());});
}

void DetectorGraphicsView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    fitInView(_scene->sceneRect());
}

DetectorScene* const DetectorGraphicsView::getScene()
{
    return _scene;
}

