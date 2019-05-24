#pragma once

#include <Eigen/Dense>

namespace nsx {

//! Small helper class that stores information for fast computation
// of intersection between lines and triangle in 3D.
struct Triangle {
    Triangle() = delete;
    Triangle(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const Eigen::Vector3d& p3);
    ~Triangle() {}
    //! Test whether a ray generated in the xz plane and with direction y positive
    //! hits the bounding box of this triangle. Return false if inside the box.
    inline bool isOutsideBB(double px, double pz) const
    {
        return (px < _xmin || px > _xmax || pz < _zmin || pz > _zmax);
    }
    bool rayIntersect(const Eigen::Vector3d& point, const Eigen::Vector3d& dir, double& t1) const;
    void calculate();
    //! Rotation of the face
    void rotate(const Eigen::Matrix<double, 3, 3>& rotation);
    //! Vector values of point A and vector B-A and C-A
    Eigen::Vector3d _A, _B, _C, _AB, _AC;
    //! Face normal to the plane. Pointing outside the Hull.
    Eigen::Vector3d _normal;
    //! d value of the corresponding plane ax+by+cz+d=0;
    double _d;
    double _dot00, _dot11, _dot01;
    double _dot002d, _dot112d, _dot012d;
    //! Bounding box of the triangle.
    double _xmin, _zmin, _xmax, _zmax;
};

} // end namespace nsx
