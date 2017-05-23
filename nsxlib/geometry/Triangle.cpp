#include "Triangle.h"


namespace nsx {

Triangle::Triangle(const Eigen::Vector3d& a,const Eigen::Vector3d& b,const Eigen::Vector3d& c):_A(a),_B(b),_C(c)
{
  calculate();
}

void Triangle::calculate()
{
	_AB=_B-_A;
	_AC=_C-_A;
	_normal=(_AB.cross(_AC));
	_d=-(_A[0]*_B[1]*_C[2]+_A[1]*_B[2]*_C[0]+_A[2]*_B[0]*_C[1]-_C[0]*_B[1]*_A[2]-_A[0]*_C[1]*_B[2]-_B[0]*_A[1]*_C[2]);
	double xmin=std::min(_A[0],_B[0]);
	_xmin=std::min(xmin,_C[0]);
	double xmax=std::max(_A[0],_B[0]);
	_xmax=std::max(xmax,_C[0]);
	double zmin=std::min(_A[2],_B[2]);
	_zmin=std::min(zmin,_C[2]);
	double zmax=std::max(_A[2],_B[2]);
	_zmax=std::max(zmax,_C[2]);
	double dot00=_AB.squaredNorm();
	double dot11=_AC.squaredNorm();
	double dot01=_AB.dot(_AC);
	double dot002d=_AB[0]*_AB[0]+_AB[2]*_AB[2];
	double dot112d=_AC[0]*_AC[0]+_AC[2]*_AC[2];
	double dot012d=_AB[0]*_AC[0]+_AB[2]*_AC[2];
	double overd=dot00*dot11-dot01*dot01;
	double overd2d=dot002d*dot112d-dot012d*dot012d;
	_dot00=dot00/overd;
	_dot01=dot01/overd;
	_dot11=dot11/overd;
	_dot002d=dot002d/overd2d;
	_dot112d=dot112d/overd2d;
	_dot012d=dot012d/overd2d;
}

bool Triangle::rayIntersect(const Eigen::Vector3d& point,const Eigen::Vector3d& dir, double& t1) const
{
	//Face is either flat or pointing in wrong direction.
	double direction=_normal.dot(dir);
	if (std::fabs(direction)<1e-10)
		return false;
	//Compute time of intersection with the plane containing the triangle
	t1=-(_normal.dot(point)+_d)/direction;

	if (t1<0) // Face on other side
		return false;

	// Vector difference between point of intersection V and A
	Eigen::Vector3d VA=point+t1*dir-_A;
	double dot02=VA.dot(_AB);
	double dot12=VA.dot(_AC);
	// Intersect triangle if uu>=0 and vv>=0 and (uu+vv)<1
	double uu=(_dot11*dot02-_dot01*dot12);
	double vv=(_dot00*dot12-_dot01*dot02);

	return (uu>=0 && vv >=0 && (uu+vv)<=1);
}

void Triangle::rotate(const Eigen::Matrix<double,3,3>& rot)
{
	_A=rot*_A;
	_B=rot*_B;
	calculate();
}

} // end namespace nsx
