#include "Quat.h"
#include "V3D.h"
#include <cmath>
#include <boost/test/floating_point_comparison.hpp>
#include <stdexcept>
#include <cstdlib>

namespace SX
{


/// Use boost float comparison
boost::test_tools::close_at_tolerance<double> quat_tol(boost::test_tools::percent_tolerance(1e-6));


/*! Null Constructor
 * Initialize the quaternion with the identity q=1.0+0i+0j+0k;
 */
Quat::Quat():w(1),a(0),b(0),c(0)
{
}

/*!
 * Construct a Quat between two vectors.
 * v=(src+des)/�src+des�
 * w=v.des
 * (a,b,c)=(v x des)
 * @param src the source position
 * @param des the destination position
 */
Quat::Quat(const V3D& src,const V3D& des)
{
	V3D v = (src+des);
	v.normalize();
	V3D cross=v.cross_prod(des);
	w = v.scalar_prod(des);
	a = cross[0];
	b = cross[1];
	c = cross[2];
}

//! Constructor with values
Quat::Quat(const double _w,const double _a, const double _b, const double _c):w(_w),a(_a),b(_b),c(_c)
{
}

//! Copy constructor
Quat::Quat(const Quat& _q)
{
	w=_q.w;
	a=_q.a;
	b=_q.b;
	c=_q.c;
}

/*! Constructor from an angle and axis.
 * This construct a  quaternion to represent a rotation
 * of an angle _deg around the _axis. The _axis does not need to be a unit vector
 *
 * \param _deg :: angle of rotation
 * \param _axis :: axis to rotate about
 * */
Quat::Quat(const double _rad,const V3D& _axis)
{
	setAngleAxis(_rad,_axis);
}

/** Assignment Operator
 * @param q the Quat to copy
 * @returns a pointer to this
 */
Quat& Quat::operator=(const Quat& q)
{
	if (this!=&q)
	{
		w=q.w;
		a=q.a;
		b=q.b;
		c=q.c;
	}
	return *this;
}

/** Sets the quat values from four doubles
 * @param ww the value for w
 * @param aa the value for a
 * @param bb the value for b
 * @param cc the value for c
 */
void Quat::set(const double ww, const double aa, const double bb, const double cc)
{
	w=ww;
	a=aa;
	b=bb;
	c=cc;
	return;
}

/** Constructor from an angle and axis.
 * \param _rad :: angle of rotation
 * \param _axis :: axis to rotate about
 *
 * This construct a  quaternion to represent a rotation
 * of an angle _deg around the _axis. The _axis does not need to be a unit vector
 * */
void Quat::setAngleAxis(const double _rad, const V3D& _axis)
{
	w=cos(0.5*_rad);
	double s=sin(0.5*_rad);
	V3D temp(_axis);
	temp.normalize();
	a=s*temp[0];
	b=s*temp[1];
	c=s*temp[2];
	return;
}

bool Quat::isNull(const double tolerance) const
{
	double pw=std::fabs(w)-1;
	return (std::fabs(pw)<tolerance);
}

void Quat::getAngleAxis(double& _rad,double& _ax0, double& _ax1, double& _ax2) const
{
	// If it represents a rotation of 0(2\pi), get an angle of 0 and axis (0,0,1)
	if (isNull(1e-5))
	{
		_rad=0;
		_ax0=0;
		_ax1=0;
		_ax2=1.0;
		return;
	}
	// Angle in radians
	_rad=2*acos(w);
	// Prefactor for the axis part
	double s=sin(_rad);
	_ax0=a/s;_ax1=b/s;_ax2=c/s;
	return;
}

/** Set the rotation (both don't change rotation axis).
 * \param deg :: angle of rotation
 */
void Quat::setRotation(const double rad)
{
	w = cos(0.5*rad);
}

/** Sets the quat values from four doubles
 * @param ww the value for w
 * @param aa the value for a
 * @param bb the value for b
 * @param cc the value for c
 */
void Quat::operator()(const double ww, const double aa, const double bb, const double cc)
{
	this->set(ww,aa,bb,cc);
}

/** Sets the quat values from an angle and a vector
 * @param angle in radians
 * @param axis the axis of rotation
 */
void Quat::operator()(const double angle, const V3D& axis)
{
	this->setAngleAxis(angle,axis);
}

void Quat::operator()(const Quat& q)
{
	w=q.w;
	a=q.a;
	b=q.b;
	c=q.c;
	return;
}


//! Destructor
Quat::~Quat()
{}


/*! Re-initialise a quaternion to identity.
 */
void Quat::init()
{
	w=1.0;
	a=b=c=0.0;
	return;
}

/*! Quaternion addition operator
 * \param _q :: the quaternion to add
 * \return *this+_q
 */
Quat Quat::operator+(const Quat& _q) const
{
	return Quat(w+_q.w,a+_q.a,b+_q.b,c+_q.c);
}

/*! Quaternion self-addition operator
 * \param _q :: the quaternion to add
 * \return *this+=_q
 */
Quat& Quat::operator+=(const Quat& _q)
{
	w+=_q.w;a+=_q.a;b+=_q.b;c+=_q.c;
	return *this;
}

/*! Quaternion subtraction operator
 * \param _q :: the quaternion to add
 * \return *this-_q
 */
Quat Quat::operator-(const Quat& _q) const
{
	return Quat(w-_q.w,a-_q.a,b-_q.b,c-_q.c);
}

/*! Quaternion self-substraction operator
 * \param _q :: the quaternion to add
 * \return *this-=_q
 */
Quat& Quat::operator-=(const Quat& _q)
{
	w-=_q.w;
	a-=_q.a;
	b-=_q.b;
	c-=_q.c;
	return *this;
}

/*! Quaternion multiplication operator
 * \param _q :: the quaternion to multiply
 * \return *this*_q
 *
 *  Quaternion multiplication is non commutative
 *  in the same way multiplication of rotation matrices
 *  isn't.
 */
Quat Quat::operator*(const Quat& _q) const
{
	double w1,a1,b1,c1;
	w1=w*_q.w-a*_q.a-b*_q.b-c*_q.c;
	a1=w*_q.a+_q.w*a+b*_q.c-_q.b*c;
	b1=w*_q.b+_q.w*b-a*_q.c+c*_q.a;
	c1=w*_q.c+_q.w*c+a*_q.b-_q.a*b;
	return Quat(w1,a1,b1,c1);
}

/*! Quaternion self-multiplication operator
 * \param _q :: the quaternion to multiply
 * \return *this*=_q
 */
Quat& Quat::operator*=(const Quat& _q)
{
	double w1,a1,b1,c1;
	w1=w*_q.w-a*_q.a-b*_q.b-c*_q.c;
	a1=w*_q.a+_q.w*a+b*_q.c-_q.b*c;
	b1=w*_q.b+_q.w*b-a*_q.c+c*_q.a;
	c1=w*_q.c+_q.w*c+a*_q.b-_q.a*b;
	w=w1;a=a1;b=b1;c=c1;
	return (*this);
}

/*! Quaternion equal operator
 * \param q :: the quaternion to compare
 *
 * Compare two quaternions at 1e-6%tolerance.
 * Use boost close_at_tolerance method
 */
bool Quat::operator==(const Quat& q) const
{
	return (quat_tol(w,q.w) && quat_tol(a,q.a) && quat_tol(b,q.b) && quat_tol(c,q.c));
}

/*! Quaternion non-equal operator
 * \param _q :: the quaternion to compare
 *
 * Compare two quaternions at 1e-6%tolerance.
 *  Use boost close_at_tolerance method
 */
bool Quat::operator!=(const Quat& _q) const
{
	return (!operator==(_q));
}

/*! Quaternion normalization
 *
 * Divide all elements by the quaternion norm
 */
void Quat::normalize()
{
	double overnorm;
	if(len2()==0)
		overnorm=1.0;
	else
		overnorm=1.0/len2();
	w*=overnorm;
	a*=overnorm;
	b*=overnorm;
	c*=overnorm;
	return;
}

/*! Quaternion complex conjugate
 *
 *  Reverse the sign of the 3 imaginary components of the
 *  quaternion
 */
void Quat::conjugate()
{
	a*=-1.0;
	b*=-1.0;
	c*=-1.0;
	return;
}

/*! Quaternion length
 *
 */
double Quat::len() const
{
	return sqrt(len2());
}

/*! Quaternion norm (length squared)
 *
 */
double Quat::len2() const
{
	return (w*w+a*a+b*b+c*c);
}

/*! Inverse a quaternion
 *
 */
void Quat::inverse()
{
	conjugate();
	normalize();
	return;
}

/*! 	Rotate a vector.
 *  \param v :: the vector to be rotated
 *
 *   The quaternion needs to be normalized beforehand to
 *   represent a rotation. If q is thequaternion, the rotation
 *   is represented by q.v.q-1 where q-1 is the inverse of
 *   v.
 */
void Quat::rotate(V3D& v) const
 {
 	Quat qinvert(*this);
 	qinvert.inverse();
 	Quat pos(0.0,v[0],v[1],v[2]);
 	pos*=qinvert;
 	pos=(*this)*pos;
 	v[0]=pos[1];
 	v[1]=pos[2];
 	v[2]=pos[3];
 }

/*! Convert quaternion rotation to an OpenGL matrix [4x4] matrix
 * stored as an linear array of 16 double
 * The function glRotated must be called
 * param mat The output matrix
 */
void Quat::GLMatrix(double* mat) const
{
	double aa      = a * a;
	double ab      = a * b;
	double ac      = a * c;
	double aw      = a * w;
	double bb      = b * b;
	double bc      = b * c;
	double bw      = b * w;
	double cc      = c * c;
	double cw      = c * w;
	*mat  = 1.0 - 2.0 * ( bb + cc );++mat;
	*mat  =     2.0 * ( ab + cw );++mat;
	*mat  =     2.0 * ( ac - bw );++mat;
	*mat  =0;++mat;
	*mat  =     2.0 * ( ab - cw );++mat;
	*mat  = 1.0 - 2.0 * ( aa + cc );++mat;
	*mat  =     2.0 * ( bc + aw );++mat;
	*mat  = 0;++mat;
	*mat  =     2.0 * ( ac + bw );mat++;
	*mat  =     2.0 * ( bc - aw );mat++;
	*mat = 1.0 - 2.0 * ( aa + bb );mat++;
	for (int i=0;i<4;++i)
	{
		*mat=0;mat++;
	}
	*mat=1.0;
	return;
}

Matrix33<double> Quat::toMatrix() const
{
	Matrix33<double> rot;
	double aa      = a * a;
	double ab      = a * b;
	double ac      = a * c;
	double aw      = a * w;
	double bb      = b * b;
	double bc      = b * c;
	double bw      = b * w;
	double cc      = c * c;
	double cw      = c * w;
	double a00= 1.0 - 2.0 * ( bb + cc );
	double a10  =     2.0 * ( ab + cw );
	double a20  =     2.0 * ( ac - bw );
	double a01  =     2.0 * ( ab - cw );
	double a11  = 1.0 - 2.0 * ( aa + cc );
	double a21  =     2.0 * ( bc + aw );
	double a02  =     2.0 * ( ac + bw );
	double a12 =     2.0 * ( bc - aw );
	double a22 = 1.0 - 2.0 * ( aa + bb );
	rot.set(a00,a01,a02,a10,a11,a12,a20,a21,a22);
	return rot;
}

/*!
 * Converts the GL Matrix into Quat
 */
void Quat::setQuat(double mat[16])
{
	double tr,s,q[4];
	int i,j,k;
	int nxt[3]={1,2,0};
	tr=mat[0]+mat[5]+mat[10];
	if(tr>0.0)
	{
		s=sqrt(tr+1.0);
		w=s/2.0;
		s=0.5/s;
		a=(mat[6]-mat[9])*s;
		b=(mat[8]-mat[2])*s;
		c=(mat[1]-mat[4])*s;
	}else{
		i=0;
		if(mat[5]>mat[0])i=1;
		if(mat[10]>mat[i*5])i=2;
		j=nxt[i];
		k=nxt[j];
		s=sqrt(mat[i*5]-(mat[j*5]+mat[k*5])+1.0);
		q[i]=s*0.5;
		if(s!=0.0)s=0.5/s;
		q[3]=(mat[j*4+k]-mat[k*4+j])*s;
		q[j]=(mat[i*4+j]+mat[j*4+i])*s;
		q[k]=(mat[i*4+k]+mat[k*4+i])*s;
		a=q[0];
		b=q[1];
		c=q[2];
		w=q[3];
	}
}
/** Bracket operator overload
 * returns the internal representation values based on an index
 * @param Index the index of the value required 0=w, 1=a, 2=b, 3=c
 * @returns a double of the value requested
 */
const double& Quat::operator[](const int Index) const
{
	switch (Index)
	    {
	    case 0: return w;
	    case 1: return a;
	    case 2: return b;
	    case 3: return c;
	    default:
	      throw std::runtime_error("Quat::operator[] range error");
	}
}

/** Bracket operator overload
 * returns the internal representation values based on an index
 * @param Index the index of the value required 0=w, 1=a, 2=b, 3=c
 * @returns a double of the value requested
 */
double& Quat::operator[](const int Index)
{
	switch (Index)
	    {
	    case 0: return w;
	    case 1: return a;
	    case 2: return b;
	    case 3: return c;
	    default:
	      throw std::runtime_error("Quat::operator[] range error");
	}
}

/** Prints a string representation of itself
 * @param os the stream to output to
 */
void Quat::printSelf(std::ostream& os) const
{
	os << "[" << w << "," << a << "," << b << "," << c << "]";
	return;

}

/**  Read data from a stream in the format returned by printSelf ("[w,a,b,c]").
 *   @param IX :: Input Stream
 *   @throw std::runtime_error if the input is of wrong format
*/
void Quat::readSelf(std::istream& IX)
{
    IX.ignore(1,'{'); IX >> w;
    IX.ignore(1,','); IX >> a;
    IX.ignore(1,','); IX >> b;
    IX.ignore(1,','); IX >> c;
    IX.ignore(1,'}');

    return;
}

/** Prints a string representation
 * @param os the stream to output to
 * @param q the quat to output
 * @returns the stream
 */
std::ostream& operator<<(std::ostream& os,const Quat& q)
{
	q.printSelf(os);
	return os;
}

/**  Reads in a quat from an input stream
 *   @param ins The input stream
 *   @param q The quat
 */
std::istream& operator>>(std::istream& ins,Quat& q)
{
    q.readSelf(ins);
    return ins;
}


} // namespace SX

