#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

class GLTrackball {
public:
    GLTrackball(int viewport_w, int viewport_h);
    ~GLTrackball();
    //! Call when the mouse button is pressed to initiate rotation
    void initRotationFrom(int, int);
    //! Call when the mouse is moving during a rotation
    void generateRotationTo(int, int);
    //! Change the rotation speed
    void setRotationSpeed(double);
    //! To be called in the application drawing the OpenGL Scene
    void IssueRotation() const;
    //! Set Model center
    void setModelCenter(double x, double y, double z);
    //! Get Model center
    Eigen::Vector3d getModelCenter() const;
    //! Reset Trackball
    void reset();
    void setViewport(int w, int h);

private:
    //! Project a point on the trackball sphere from viewport coordinates x,y
    void projectOnSphere(int x, int y, Eigen::Vector3d& p);
    //! Generate a 3D point coordinates from coordinates on the viewport.
    void generateTranslationPoint(int x, int y, Eigen::Vector3d& p);
    //! Previous point selected on sphere
    Eigen::Vector3d _lastpoint;
    //! Rotation matrix stored as a quaternion
    Eigen::Quaterniond _quaternion;
    //! Rotation matrix (4x4 stored as linear array) used in OpenGL
    Eigen::Affine3d _rotationmatrix;
    //! Rotation speed of the trackball
    double _rotationspeed;
    //! Center of rotation
    Eigen::Vector3d _modelCenter;
    //! Is the centre of rotation offcentered
    bool _hasOffset;
    //! Current size of the viewport
    double _w, _h;
};
