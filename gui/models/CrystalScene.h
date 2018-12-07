#pragma once

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>
#include <QString>

#include <nsxlib/GeometryTypes.h>

class QGraphicsPixmapItem;
class QGraphicsTextItem;
class QWidget;

class CrystalNodeItem;
class DialogCalibrateDistance;
class PinItem;
class RulerItem;

class CrystalScene : public QGraphicsScene {
    Q_OBJECT
public:
    //! Enum describing the type of user interaction
    enum crystalSelectionMode {
        calibrateDistance=1,
        pickingPoint=2,
        pickCenter=3,
        nodeSelected=4,
        removingPoint=5,
        none=0
    };

    //! Constructors
    CrystalScene(nsx::ConvexHull* hull,QWidget *parent = 0);
    ~CrystalScene();
    //! Mouse interactions
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public slots:
    //! Load an image from full filename including directory
    void loadImage(QString filename);
    //!
    void drawText(QString text);
    void activateCalibrateDistance();
    void activatePickCenter();
    void activatePickingPoints();
    void activateRemovingPoints();
    void getDistance(double val);
    //! Stampe the scale in the scene, based
    //! on the calibrated distance.
    //! Valid until new scale is defined.
    void stampScale();
    void setRotationAngle(double);
    void triangulate();
    void changeBrigthness(int a);
signals:
    void calibrateDistanceOK(double npixels, double distance);
    void calibrateCenterOK(double x, double y);
private:
    //! Position of the mouse at the beginning of a selection
    int _startxmouse, _startymouse;
    //! Selectrion mode
    crystalSelectionMode mode;
    QGraphicsPixmapItem* pixmapitem;
    //! The coordinates of the points for the scale
    double scalebx,scaleby,scaleex, scaleey;
    //! Calibrate distance dialog
    DialogCalibrateDistance* _distancedialog;
    double distance;
    double npixels;
    double aspectratio;
    double _rotationAngle;
    RulerItem* _ruler;
    CrystalNodeItem* _current;
    PinItem* _pin;
    bool _pinCreated;
    nsx::ConvexHull* _hull;
    QPixmap _pix;
    QGraphicsTextItem* _text;
};
