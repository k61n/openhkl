#include <cmath>

#include <QtOpenGL>

#include "GLTrackball.h"

/*
#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
*/
#include <iostream>

GLTrackball::GLTrackball(int viewport_w, int viewport_h):_w(viewport_w),_h(viewport_h)
{
  reset();
  // Rotation speed defines as such is equal 1 in relative units,
  // i.e. the trackball will follow exactly the displacement of the mouse
  // on the sceen. The factor 180/M_PI is simply rad to deg conversion. THis
  // prevent recalculation of this factor every time a generateRotationTo call is issued.
  _rotationspeed = 0.2;
  _modelCenter = Eigen::Vector3d::Zero();
  _hasOffset = false;
  _quaternion=Eigen::Quaterniond::Identity();
}

GLTrackball::~GLTrackball()
{

}

void GLTrackball::initRotationFrom(int a,int b)
{
  projectOnSphere(a,b,_lastpoint);
}

void GLTrackball::generateRotationTo(int a,int b)
{
  Eigen::Vector3d newpoint;
  projectOnSphere( a, b, newpoint );

  // Create a quaternion which brings the lastpoint to new point through a rotation
  Eigen::Quaterniond temp=Eigen::Quaterniond::FromTwoVectors(_lastpoint,newpoint);
  // Left multiply
  _quaternion = temp*_quaternion;
  // Get the corresponding OpenGL rotation matrix
  _rotationmatrix=Eigen::Affine3d(_quaternion);
 }


void GLTrackball::IssueRotation() const
{

    // Translate if offset is defined
    if (_hasOffset)
    {
      glTranslated( _modelCenter[0], _modelCenter[1], _modelCenter[2] );
    }
    // Rotate with respect to the centre
    glMultMatrixd( _rotationmatrix.data());
    // Translate back
    if ( _hasOffset )
      glTranslated( - _modelCenter[0], - _modelCenter[1], - _modelCenter[2] );
  return;
}

void GLTrackball::setModelCenter(double x,double y, double z)
{
  _modelCenter << x,y,z;
  if ( _modelCenter[0]==0 && _modelCenter[1]==0 && _modelCenter[2]==0 )
    _hasOffset=false;
  else
    _hasOffset=true;
}

Eigen::Vector3d GLTrackball::getModelCenter() const
{
  return _modelCenter;
}

void GLTrackball::projectOnSphere(int a,int b,Eigen::Vector3d& point)
{
  // z initiaised to zero if out of the sphere
  double x,y,z=0;

  x=static_cast<double>((2.0*a-_w)/_w);
  y=static_cast<double>((_h-2.0*b)/_h);
  double norm=x*x+y*y;
  if (norm>1.0) // The point is inside the sphere
  {
    norm=sqrt(norm);
    x/=norm;
    y/=norm;
  }
  else // The point is outside the sphere, so project to nearest point on circle
    z=sqrt(1.0-norm);
  // Set-up point
  point<< x,y,z;
}

void GLTrackball::setRotationSpeed(double r)
{
  // Rotation speed needs to contains conversion to degrees.
  //
  if ( r > 0 ) _rotationspeed = r;
}

void GLTrackball::reset()
{
  //Reset rotation,scale and translation
  _quaternion.Identity();
  _rotationmatrix=Eigen::Affine3d::Identity();
}

void GLTrackball::setViewport(int w, int h)
{
    _w=w;_h=h;
}
