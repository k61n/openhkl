#include <cmath>
#include <sstream>

#include <QGraphicsTextItem>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWidget>

#include <nsxlib/ConvexHull.h>
#include <nsxlib/Triangle.h>
#include <nsxlib/Units.h>

#include "CrystalNodeItem.h"
#include "CrystalFaceItem.h"
#include "CrystalScene.h"
#include "DialogCalibrateDistance.h"
#include "PinItem.h"
#include "RulerItem.h"

CrystalScene::CrystalScene(nsx::ConvexHull* hull, QWidget *parent) :
    QGraphicsScene(parent), pixmapitem(0),_ruler(0),_pin(0),_hull(hull),_text(nullptr)
{
    distance=1.0;
   _distancedialog = new DialogCalibrateDistance();
    connect(_distancedialog,SIGNAL(on_calibrateDistanceSpinBox_valueChanged(double)),this,SLOT(getDistance(double)));
    connect(_distancedialog,SIGNAL(on_calibrateDistanceButtonOK_accepted()),this,SLOT(stampScale()));
    _ruler = new RulerItem;
    _pin = new PinItem;
    _pinCreated = false;

}

CrystalScene::~CrystalScene()
{
}

void CrystalScene::loadImage(QString filename)
{
    _pix.load(filename);
   if (!pixmapitem)
   {
    pixmapitem=addPixmap(_pix);
    int w=pixmapitem->pixmap().width();
    pixmapitem->setScale(800.0/w);
   }
   else
       pixmapitem->setPixmap(_pix);

   QImage image=pixmapitem->pixmap().toImage();
   image = image.convertToFormat(QImage::Format_Indexed8);
   QVector<QRgb> table=image.colorTable();

   int i=0;
   for (auto it=table.begin();it!=table.end();++it,i++)
       image.setColor(i,*it+200);
}

void CrystalScene::changeBrigthness(int a)
{
    QImage image=_pix.toImage();
    for (int y=0;y<image.height();++y)
    {
        QRgb* line=reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x=0;x<image.width();++x)
        {
            double newR=qRed(line[x])/255.0;
            double newG=qGreen(line[x])/255.0;
            double newB=qBlue(line[x])/255.0;
            if (a < 0.0)
            {
                newR*=( 1.0 + a/100.0);
                newG*=(1.0  + a/100.0);
                newB*=(1.0  + a/100.0);
            }
            else
            {
                newR+=((1 - newR) * a/100.0);
                newG+=((1 - newG) * a/100.0);
                newB+=((1 - newB) * a/100.0);
            }
            line[x]=qRgb((int)(newR*255),(int)(newG*255),(int)(newB*255));
        }
    }
    pixmapitem->setPixmap(QPixmap::fromImage(image));
}

void CrystalScene::activateCalibrateDistance()
{
    mode=calibrateDistance;
}

void CrystalScene::activatePickCenter()
{
    mode=pickCenter;
}

void CrystalScene::activatePickingPoints()
{
    mode=pickingPoint;
}

void CrystalScene::activateRemovingPoints()
{
    mode=removingPoint;
}

void CrystalScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
     QPointF pos=event->scenePos();
     if (mode==calibrateDistance)
     {
         _ruler->startDrawingAt(pos);
         addItem(_ruler);
         update();
     }
     else if (mode==pickingPoint || mode==nodeSelected)
     {

         QGraphicsItem* temp=itemAt(pos.x(),pos.y(),QTransform());
         if (dynamic_cast<CrystalNodeItem*>(temp))
         {
             mode=nodeSelected;
             _current=dynamic_cast<CrystalNodeItem*>(temp);
         }
         else
         {
             mode=pickingPoint;
            CrystalNodeItem* n=new CrystalNodeItem;
            double yc,zc;
            _pin->getCenter(yc,zc,_rotationAngle);
            n->initiate(pos.x(),pos.y(),yc,_rotationAngle);
            addItem(n);
         }

      }
     else if (mode==pickCenter)
     {
         if (!_pinCreated)
         {
            addItem(_pin);
            _pin->initiate(pos.y(),_rotationAngle);
            update();
            _pinCreated=true;
         }
     }
     else if (mode==removingPoint)
     {
         QGraphicsItem* temp=itemAt(pos.x(),pos.y(),QTransform());
         if (CrystalNodeItem* temp2=dynamic_cast<CrystalNodeItem*>(temp))
         {
             double x,y,z;
             temp2->getCoordinates(x,y,z);
            _hull->removeVertex(Eigen::Vector3d(x*aspectratio,y*aspectratio,z*aspectratio),1e-3);
             removeItem(temp);
         }
     }

}

void CrystalScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (mode==calibrateDistance)
    {
        if (_ruler)
    {
        _ruler->moveTipAt(event->scenePos());
        update();
    }
    }
    else if (mode==pickCenter && _pinCreated)
    {
        if(event->buttons() == Qt::LeftButton)
        {
            _pin->adjust(event->scenePos().y(),_rotationAngle);
            update();
        }
    }
    else if (mode==nodeSelected)
    {
        if (event->buttons() == Qt::LeftButton)
        {
            double yc,zc;
            _pin->getCenter(yc,zc,_rotationAngle);
            _current->adjust(event->scenePos().y(),yc,_rotationAngle);
        }
    }
}

void CrystalScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //! When mouse is released in calibrateDistanceMode,
    //! open widget to enter distance then close.
    QPointF point=event->scenePos();
    if (mode==calibrateDistance)
    {
        scaleex=point.x();
        scaleey=point.y();
        mode=none;
        _distancedialog->show();
    }
    else if (mode==pickCenter)
    {
        _pin->adjust(point.y(),_rotationAngle);
        calibrateCenterOK(point.x(),point.y());
    }
    else if (mode==nodeSelected)
    {
        double yc,zc;
        _pin->getCenter(yc,zc,_rotationAngle);
        _current->adjust(point.y(),yc,_rotationAngle);
    }
}

void CrystalScene::getDistance(double val)
{
    distance=val;
}

void CrystalScene::stampScale()
{
    QGraphicsItem* stamp=_ruler->stampRuler(distance,QPointF(5,550));
    addItem(stamp);
    calibrateDistanceOK(npixels,distance);
    _ruler->setDistance(distance);
    _ruler->activateDistanceDisplay();
    if (_ruler)
        removeItem(_ruler);
    aspectratio=_ruler->getConversion();
}

void CrystalScene::setRotationAngle(double angle)
{
    _rotationAngle=angle;
    QList<QGraphicsItem*> list=items();
    double yc, zc;
    _pin->getCenter(yc,zc,_rotationAngle);
    for (QList<QGraphicsItem*>::iterator it=list.begin();it!=list.end();it++)
    {
        CrystalNodeItem* temp=dynamic_cast<CrystalNodeItem*>(*it);
        if (temp)
        {
            temp->rotate(angle,yc);
        }
        CrystalFaceItem* temp2=dynamic_cast<CrystalFaceItem*>(*it);
        if (temp2)
        {
            temp2->rotate(angle,yc);
        }
    }
    _pin->rotate(angle);
}

void CrystalScene::triangulate()
{
   _hull->reset();
    QList<QGraphicsItem*> list=items();

    for (QList<QGraphicsItem*>::iterator it=list.begin();it!=list.end();it++)
    {
        CrystalNodeItem* temp=dynamic_cast<CrystalNodeItem*>(*it);
        if (temp)
        {
            double x,y,z;
            temp->getCoordinates(x,y,z);
           _hull->addVertex(Eigen::Vector3d(x*aspectratio,y*aspectratio,z*aspectratio));
        }
    }

    try
    {
   _hull->updateHull();
    }
    catch(std::exception& e)
    {
        QMessageBox::critical(nullptr, tr("NSXTool"), tr(e.what()));
        return;
    }
    const std::vector<nsx::Triangle>& tcache=_hull->createFaceCache();
    std::vector<nsx::Triangle>::const_iterator it;
    double yc, zc;
    _pin->getCenter(yc,zc,_rotationAngle);
    for (QList<QGraphicsItem*>::iterator it=list.begin();it!=list.end();it++)
    {
        CrystalFaceItem* temp2=dynamic_cast<CrystalFaceItem*>(*it);
        if (temp2)
           removeItem(temp2);
    }
    for (it=tcache.begin();it!=tcache.end();++it)
    {
        const Eigen::Vector3d& a=it->_A;
        const Eigen::Vector3d& ab=it->_AB;
        const Eigen::Vector3d& ac=it->_AC;
        CrystalFaceItem* temp=new CrystalFaceItem();
        temp->set(a/aspectratio,(ab+a)/aspectratio,(ac+a)/aspectratio);
        temp->rotate(_rotationAngle,yc);
        addItem(temp);
    }
    update();
}

void CrystalScene::drawText(QString text)
{
    if (!_text)
    {
        _text=addText(text);
        _text->setDefaultTextColor(QColor(Qt::red));
    }
    else
        _text->setPlainText(text);
}

