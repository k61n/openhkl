#pragma once

#include <Eigen/Dense>

#include <QGraphicsItem>

class CrystalFaceItem : public QGraphicsItem {
public:
    explicit CrystalFaceItem(QGraphicsItem* parent = 0);
    void set(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const Eigen::Vector3d& p3);
    void rotate(double angle, double yc);
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    Eigen::Vector3d _p1, _p2, _p3, _normal;
    double nz;
    QPointF* _points;
};
