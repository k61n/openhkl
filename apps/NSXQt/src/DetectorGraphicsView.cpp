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
    // Called when the user interact with the zoom in the scene
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

void DetectorGraphicsView::resizeToDetector()
{
    SX::Data::IData* data=_scene->getData();
    SX::Instrument::Detector* det=data->getDiffractometer()->getDetector();
    double w=det->getWidth();
    double h=det->getHeigth();
    double gw=this->width();
    double gh=this->height();
    this->resize(gw,gw*h/w);
    fitInView(_scene->sceneRect());
}

