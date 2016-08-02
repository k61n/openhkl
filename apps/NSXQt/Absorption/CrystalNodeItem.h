#ifndef CRYSTALNODEITEM_H
#define CRYSTALNODEITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <cmath>
#include <iostream>

//!
//! @class CrystalNodeItem: Class uded to define points on a crystal.
//! In order to work, the center of rotation must be
//! set using the static method setCenter.
//! Each node is first selected by invoking the
//! initiate method which set the position of a point
//! using the screen coordinates (x,y) and the rotation angle
//! of the crystal at which the image was recorded.
//! The second step is to invoke the method adjust
//! at a different crystal angle, which determines the
//! missing coordinate. The node is displayed as an ellipse
//! with set colors and respond to mouse interaction.

class CrystalNodeItem : public QGraphicsItem
{
public:
    explicit CrystalNodeItem(QGraphicsItem *parent = 0);
    //! Method called when point is first created
    void initiate(double posx, double posy,double yc, double angle);
    //! Method called when point is moved in another frame
    void adjust(double posy,double yc, double angle);
    //! Overloaded
    QRectF boundingRect() const;
    //! Paint method
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    //! Methiod called when the frame is rotated to a new angle
    void rotate(double angle,double yc);
    //! Get the position coordinates of this node
    void getCoordinates(double& x, double& y, double& z);
signals:

public slots:
    private:
      double _p1x,_p1y,_p1z,_angle;
};

#endif // CRYSTALNODEITEM_H
