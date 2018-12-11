#pragma once

#include <QGraphicsItem>

//!
/* @class PinItem: Class used to calibrate
  the position of the crystal pin as function
  of the rotation angle.

  */


class PinItem : public QGraphicsItem {
public:
    PinItem(QGraphicsItem* parent = 0);
    void initiate(double posy, double angle);
    //! Method called when point is moved in another frame
    void adjust(double posy, double angle);
    //! Overloaded
    QRectF boundingRect() const;
    //! Paint method
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    //! Methiod called when the frame is rotated to a new angle
    void rotate(double angle);
    bool iscalibrated() const;
    void getCenter(double& y, double& z, double angle);

private:
    double _p1y, _p2y;
    double _angle;
    double _currentangle;
    bool _calibrated;
};
