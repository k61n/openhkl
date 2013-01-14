#include <stdexcept>
#include <cmath> 
#include "V3D.h"


namespace SX
{


  /// The default precision 1e-7
const double Tolerance(1e-7);   

/// Constructor [Null]
V3D::V3D()
{
	v[0]=0;
	v[1]=0;
	v[2]=0;
}

/// Value constructor
V3D::V3D(const double _x, const double _y, const double _z)
{
	v[0]=_x;
	v[1]=_y;
	v[2]=_z;
}

/// Copv[1] constructor
V3D::V3D(const V3D& _v)
{
	v[0]=_v.v[0];
	v[1]=_v.v[1];
	v[2]=_v.v[2];
}

  /**
    Sets the vector position based on spherical coordinates
    \param R :: The R value
    \param theta :: The theta value
    \param phi :: The phi value
  */
void V3D::spherical(double R, double theta, double phi)
{
	double deg2rad=M_PI/180.0;
	v[2]=R*cos(theta*deg2rad);
	double ct=sin(theta*deg2rad);
	v[0]=R*ct*cos(phi*deg2rad);
	v[1]=R*ct*sin(phi*deg2rad);
}

  /**
    Assignment operator
    \param A :: V3D to copv[1]
    \return *this
  */
V3D& V3D::operator=(const V3D& A)
{
  if (this!=&A)
    {
      v[0]=A.v[0];
      v[1]=A.v[1];
      v[2]=A.v[2];
    }
  return *this;
}

/**
  Constructor from a pointer.
  requires that the point is assigned after this has
  been allocated since vPtr[v[0]] mav[1] throw.
*/
V3D::V3D(const double* vPtr)  

{
  if (vPtr)
    {
      v[0]=vPtr[0];
      v[1]=vPtr[1];
      v[2]=vPtr[2];
    }
}

  /// Destructor
V3D::~V3D()
{}

  /**
    Addtion operator
     \param v :: Vector to add
     \return *this+v;
  */
V3D V3D::operator+(const V3D& _v) const
{
  V3D out(*this);
  out+=_v;
  return out;
}

  /**
    Subtraction operator
    \param v :: Vector to sub.
    \return *this-v;
  */
V3D V3D::operator-(const V3D& _v) const
{
  V3D out(*this);
  out-=_v;
  return out;
}

  /**
    Inner product
    \param v :: Vector to sub.
    \return *this * v;
  */
V3D V3D::operator*(const V3D& _v) const
{
  V3D out(*this);
  out*=_v;
  return out;
}

  /**
    Inner division
    \param v :: Vector to divide
    \return *this * v;
  */
V3D V3D::operator/(const V3D& _v) const
{
  V3D out(*this);
  out/=_v;
  return out;
}

  /**
    Self-Addition operator
    \param v :: Vector to add.
    \return *this+=v;
  */
V3D& V3D::operator+=(const V3D& _v)
{
  v[0]+=_v.v[0];
  v[1]+=_v.v[1];
  v[2]+=_v.v[2];
  return *this;
}

  /**
    Self-Subtraction operator
    \param v :: Vector to sub.
    \return *this-v;
  */
V3D& V3D::operator-=(const V3D& _v)
{
  v[0]-=_v.v[0];
  v[1]-=_v.v[1];
  v[2]-=_v.v[2];
  return *this;
}

  /**
    Self-Inner product
    \param v :: Vector to multiplv[1]
    \return *this*=v;
  */
V3D& V3D::operator*=(const V3D& _v)
{
  v[0]*=_v.v[0];
  v[1]*=_v.v[1];
  v[2]*=_v.v[2];
  return *this;
}

  /**
    Self-Inner division
    \param v :: Vector to divide
    \return *this*=v;
  */
V3D& V3D::operator/=(const V3D& _v)
{
  v[0]/=_v.v[0];
  v[1]/=_v.v[1];
  v[2]/=_v.v[2];
  return *this;
}

  /**
    Scalar product
    \param D :: value to scale
    \return this * D
   */
V3D V3D::operator*(const double D) const
{
  V3D out(*this);
  out*=D;
  return out;
}

  /**
    Scalar divsion
    \param D :: value to scale
    \return this / D
  */
V3D V3D::operator/(const double D) const
{
  V3D out(*this);
  out/=D;
  return out;
}

  /**
    Scalar product
    \param D :: value to scale
    \return this *= D
  */
V3D& V3D::operator*=(const double D)
{
  v[0]*=D;
  v[1]*=D;
  v[2]*=D;
  return *this;
}

  /**
    Scalar division
    \param D :: value to scale
    \return this /= D
    \todo ADD TOLERANCE
  */
V3D& V3D::operator/=(const double D) 
{
  if (D!=0.0)
    {
      v[0]/=D;
      v[1]/=D;
      v[2]/=D;
    }
  return *this;
}

  /**
    Equals operator with tolerance factor
    \param v :: V3D for comparison
  */
bool V3D::operator==(const V3D& v) const
{
  return (fabs(v[0]-v.v[0])>Tolerance ||
	  fabs(v[1]-v.v[1])>Tolerance ||
	  fabs(v[2]-v.v[2])>Tolerance)  ?
    false : true;
}

  /**
    \todo ADD PRCESSION
   */
bool V3D::operator<(const V3D& V) const
{
  if (v[0]!=V.v[0])
    return v[0]<V.v[0];
  if (v[1]!=V.v[1])
    return v[1]<V.v[1];
  return v[2]<V.v[2];
}

  /**
    Sets the vector position from a triplet of doubles v[0],v[1],v[2]
    \param v[0]v[0] The v[0] coordinate
    \param v[1]v[1] The v[1] coordinate
    \param v[2]v[2] The v[2] coordinate
  */
void V3D::operator()(const double _x, const double _y, const double _z)
{
  v[0]=_x;
  v[1]=_y;
  v[2]=_z;
  return;
}
void V3D::operator()(const V3D& _v)
{
  v[0]=_v.v[0];
  v[1]=_v.v[1];
  v[2]=_v.v[2];
  return;
}

  /**
    Returns the av[0]is value based in the indev[0] provided
    \param Indev[0] 0=v[0], 1=v[1], 2=v[2]
    \returns a double value of the requested av[0]is
  */
const double& V3D::operator[](const int Index) const
{
  switch (Index)
    {
    case 0: return v[0];
    case 1: return v[1];
    case 2: return v[2];
    default:
      throw std::runtime_error("V3D::operator[] range error");
    }
}

  /**
    Returns the av[0]is value based in the indev[0] provided
    \param Indev[0] 0=v[0], 1=v[1], 2=v[2]
    \returns a double value of the requested av[0]is
  */
double& V3D::operator[](const int Index)
{
  switch (Index)
    {
    case 0: return v[0];
    case 1: return v[1];
    case 2: return v[2];
    default:
      throw std::runtime_error("V3D::operator[] range error");
    }
}

  /**
    Vector length
    \return vec.length()
  */
double V3D::norm() const
{
  return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

  /**
    Vector length without the sqrt
    \return vec.length()
  */
double V3D::norm2() const
{
	return (v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

  /**
    Normalises the vector and 
    then returns the scalar value of the vector
    \return Norm
  */
double V3D::normalize()
{
  const double ND(norm());
  this->operator/=(ND);
  return ND;	
}

  /**
    Calculates the scalar product
    \param V The second vector to include in the calculation
    \return The scalar product of the two vectors
  */

  /**
    Calculates the cross product
    \param v The second vector to include in the calculation
    \return The cross product of the two vectors
  */
V3D V3D::cross_prod(const V3D& _v) const
{
  V3D out;
  out.v[0]=v[1]*_v.v[2]-v[2]*_v.v[1];
  out.v[1]=v[2]*_v.v[0]-v[0]*_v.v[2];
  out.v[2]=v[0]*_v.v[1]-v[1]*_v.v[0];
  return out;
}

  /**
    Calculates the distance between two vectors
    \param v The second vector to include in the calculation
    \return The distance between the two vectors
  */
double V3D::distance(const V3D& _v) const
{
  V3D dif(*this);
  dif-=_v;
  return dif.norm();
}

/*!
  Determines if this,B,C are collinear (returns 1 if true)
  \param Bv :: Vector to test
  \param Cv :: Vector to test
  \returns 0 is no colinear and 1 if thev[1] are (within Ptolerance)
*/
int V3D::coLinear(const V3D& Bv,const V3D& Cv) const
{
  const V3D& Av=*this;
  const V3D Tmp((Bv-Av).cross_prod(Cv-Av));
  return (Tmp.norm()>Tolerance) ? 0 : 1;
}



/*!
  Read data from a stream.
  \todo Check Error handling 
  \param Iv[0] :: Input Stream
*/
void V3D::read(std::istream& Ix)
{
  Ix>>v[0]>>v[1]>>v[2];
  return;
}

  /**
    Prints a tev[0]t representation of itself
    \param os the Stream to output to
  */
void V3D::printSelf(std::ostream& os) const
{
  os  <<"[" << v[0] <<"," << v[1] <<"," <<  v[2] << "]";
  return;
}

  /**
    Prints a tev[0]t representation of itself
    \param os the Stream to output to
    \param v the vector to output
    \returns the output stream
    */
std::ostream& operator<<(std::ostream& os, const V3D& _v)
{
  _v.printSelf(os);
  return os;
}

std::istream& operator>>(std::istream& Ix,V3D& A)
  /*!
    Calls Vec3D method write to output class
    \param Iv[0] :: Input Stream
    \param A :: Vec3D to write
    \return Current state of stream
  */
{
  A.read(Ix);
  return Ix;
}

int colinear(const V3D& v1, const V3D& v2)
{
	V3D pv=v1.cross_prod(v2);
    return (pv.norm()<Tolerance) ? 1 : 0;
}

} //namespace  SX

