#include "CrystalHull.h"
#include <cmath>
#include <algorithm>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Aff_transformation_3.h>

namespace SX
{

TriangleCache::TriangleCache():_A(0,0,0),_AB(0,0,0),_AC(0,0,0),_normal(0,0,0),
		_d(0),_dot00(0),_dot11(0),_dot01(0),_dot002d(0),_dot112d(0),_dot012d(0),
		_xmin(0),_zmin(0),_xmax(0),_zmax(0)
{}

TriangleCache::~TriangleCache()
{
}
TriangleCache TriangleCache::operator ()(Polyhedron_3::Facet& f)
{
	Polyhedron_3::Halfedge_handle h=f.halfedge();
	Point_3& a=h->vertex()->point();
	Point_3& b=h->next()->vertex()->point();
	Point_3& c=h->opposite()->vertex()->point();
	TriangleCache temp;
	temp._A(a[0],a[1],a[2]);
	temp._AB(b[0]-a[0],b[1]-a[1],b[2]-a[2]);
	temp._AC(c[0]-a[0],c[1]-a[1],c[2]-a[2]);
	temp._normal=temp._AB.cross_prod(temp._AC);
	temp._d=-(a[0]*b[1]*c[2]+a[1]*b[2]*c[0]+a[2]*b[0]*c[1]-c[0]*b[1]*a[2]-a[0]*c[1]*b[2]-b[0]*a[1]*c[2]);
	double xmin=std::min(a[0],b[0]);
	temp._xmin=std::min(xmin,c[0]);
	double xmax=std::max(a[0],b[0]);
	temp._xmax=std::max(xmax,c[0]);
	double zmin=std::min(a[2],b[2]);
	temp._zmin=std::min(zmin,c[2]);
	double zmax=std::max(a[2],b[2]);
	temp._zmax=std::max(zmax,c[2]);
	double dot00=temp._AB.norm2();
	double dot11=temp._AC.norm2();
	double dot01=temp._AB.scalar_prod(temp._AC);
	double dot002d=temp._AB[0]*temp._AB[0]+temp._AB[2]*temp._AB[2];
	double dot112d=temp._AC[0]*temp._AC[0]+temp._AC[2]*temp._AC[2];
	double dot012d=temp._AB[0]*temp._AC[0]+temp._AB[2]*temp._AC[2];
	double overd=dot00*dot11-dot01*dot01;
	double overd2d=dot002d*dot112d-dot012d*dot012d;
	temp._dot00=dot00/overd;
	temp._dot01=dot01/overd;
	temp._dot11=dot11/overd;
	temp._dot002d=dot002d/overd2d;
	temp._dot112d=dot112d/overd2d;
	temp._dot012d=dot012d/overd2d;

	return temp;
}

CrystalHull::CrystalHull(const char* name):_name((name==0) ? " " : name)
{
	//
	_points.reserve(100);
	_isgenerated=false;
}

CrystalHull::~CrystalHull()
{
}

void CrystalHull::addPoint(double x, double y, double z)
{
	// Add the 3D point in the list.
	Point_3 p(x,y,z);
	// If the point is already in, do nothing
	std::vector<Point_3>::iterator it=std::find(_points.begin(),_points.end(),p);
	if (it==_points.end())
		_points.push_back(p);
}
void CrystalHull::removePoint(double x, double y, double z)
{
	//! Remove the point if already existing in the vector.

	std::remove_if(_points.begin(),_points.end(),std::bind2nd(std::equal_to<Point_3>(),Point_3(x,y,z)));
}

std::size_t CrystalHull::nPoints() const
{
	return _points.size();
}
std::size_t CrystalHull::nVertices() const
{
	return _polyhedron.size_of_vertices();
}

std::size_t CrystalHull::nEdges() const
{
	return _polyhedron.size_of_halfedges()/2;
}

std::size_t CrystalHull::nFaces() const
{
	return _polyhedron.size_of_facets();
}

std::size_t CrystalHull::generate()
{
	// Clear polyhedron if already exists
	_polyhedron.clear();
	// Run convex hull
	  CGAL::convex_hull_3(_points.begin(), _points.end(), _polyhedron);
	  if (_polyhedron.size_of_facets())
		  _isgenerated=true; // Hull has been generated
	  else
		  _isgenerated=false;
	  // Generate triangles
	  _tcache.resize(_polyhedron.size_of_facets());
	 std::transform(_polyhedron.facets_begin(),_polyhedron.facets_end(),_tcache.begin(),TriangleCache());
	  // Return the numbers of facets
	  return _polyhedron.size_of_facets();
}

double CrystalHull::getVolume() const
{
	double volume=0;
	std::vector<TriangleCache>::const_iterator it;
	for (it=_tcache.begin();it!=_tcache.end();it++)
	{
		volume+=(it->_A.scalar_prod(it->_normal))/6.0;
	}
	return volume;

}

V3D CrystalHull::getCentroid() const
{
	Vertexit it;
	V3D result(0,0,0);
	// Add all points coordinates / number of points
	for (it=_polyhedron.vertices_begin();it!=_polyhedron.vertices_end();++it)
	{
		const Point_3& p1=it->point();
		result.add(p1[0],p1[1],p1[2]);
	}
	result/=static_cast<double>(_polyhedron.size_of_vertices());
	return result;
}

std::vector<V3D> CrystalHull::getVertices() const
{
	Vertexit it;
	std::vector<V3D> result(_polyhedron.size_of_vertices());
	int i=0;
	for (it=_polyhedron.vertices_begin();it!=_polyhedron.vertices_end();++it)
	{
		const Point_3& p1=it->point();
		result[i++](p1[0],p1[1],p1[2]);
	}
	return result;
}



int CrystalHull::rayIntersectFromOutside(double x, double y, double z,double& t1, double& t2) const
{
	int inters=0;
	bool found_entry=false;
	bool found_exit=false;
	std::vector<TriangleCache>::const_iterator it;
	for (it=_tcache.begin();it!=_tcache.end();++it)
	{
		// Does not hit bb skip?
		if (it->isOutsideBB(x,z))
			continue;
		//  Component of normal vector // y
		double n=it->_normal[1];
		// Already found entry or exit or the face is almost perpendicular, skip
		if (std::fabs(n)<1e-8 || (found_entry && n <0) || (found_exit && n >0))
			continue;

		const V3D& v1=it->_AB;
		const V3D& v2=it->_AC;
		double xv=(x-it->_A[0]);
		double zv=(z-it->_A[2]);
		double dot02=xv*v1[0]+zv*v1[2];
		double dot12=xv*v2[0]+zv*v2[2];
		// Intersect triangle if uu>=0 and vv>=0 and (uu+vv)<1
		double uu=(it->_dot112d*dot02-it->_dot012d*dot12);
		if (uu<0)
			continue;
		double vv=(it->_dot002d*dot12-it->_dot012d*dot02);
		if (vv<0 || (uu+vv)>1)
		   continue;
		// Sure that we have a hit, compute time of intersection.
		double t=-(it->_normal.scalar_prod(x,y,z)+it->_d)/n;
		if (n<0)
		{
			t1=t;
			found_entry=true;
		}
		else
		{
			t2=t;
			found_exit=true;
		}
		if (++inters==2) break; // Increment inters. If found 2 intersections, no need to test other facets
	}
	return inters;
}

int CrystalHull::rayIntersectFromInside(double px, double py, double pz,double dirx, double diry, double dirz, double& t1) const
{
	int inters=0;
	std::vector<TriangleCache>::const_iterator it;
	for (it=_tcache.begin();it!=_tcache.end();++it)
	{
		const V3D& n=it->_normal;
		//Face is either flat or pointing in wrong direction.
		if (n.scalar_prod(dirx,diry,dirz)<1e-8)
			continue;
		//Compute time of intersection
		t1=-(n.scalar_prod(px,py,pz)+it->_d)/n.scalar_prod(dirx,diry,dirz);
		if (t1<0) // Face on other side
			continue;
		const V3D& a=it->_A;
		// Vector difference between point of intersection V and A
		V3D VA(px+dirx*t1-a[0],py*diry*t1-a[1],py*dirx*t1-a[2]);
		double dot02=VA.scalar_prod(it->_AB);
		double dot12=VA.scalar_prod(it->_AC);
		// Intersect triangle if uu>=0 and vv>=0 and (uu+vv)<1
		double uu=(it->_dot11*dot02-it->_dot01*dot12);
		if (uu<0)
			continue;
		double vv=(it->_dot00*dot12-it->_dot01*dot02);
		if (vv<0 || (uu+vv)>1)
		   continue;
		if (++inters==1) // Intersection found
			break;
	}
	return inters;
}

double CrystalHull::scatteringPathLength(double x, double y, double z, double Svx, double Svy, double Svz, double rd01) const
{
	double t1,t2;
	double l=0;
	rayIntersectFromOutside(x,y,z,t1,t2);
	l+=rd01*(t2-t1);
	rayIntersectFromInside(x,y+t1+l,z,Svx,Svy,Svz,t1);
	l+=t1;
	return l;
}

void CrystalHull::rotateHull(const Matrix33<double>& rot)
{
	if (!rot.isRotation())
		throw std::runtime_error("Rotation not valid");
	//
	CGAL::Aff_transformation_3<Kernel> rotc(rot(0,0),rot(0,1),rot(0,2),rot(1,0),rot(1,1),rot(1,2),rot(2,0),rot(2,1),rot(2,2));
	std::transform(_polyhedron.points_begin(),_polyhedron.points_end(),_polyhedron.points_begin(),rotc);
	//Recalculate triangle cache.
	std::transform(_polyhedron.facets_begin(),_polyhedron.facets_end(),_tcache.begin(),TriangleCache());
}
void CrystalHull::rotateHull(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22)
{
	Matrix33<double> rot(m00,m01,m02,m10,m11,m12,m20,m21,m22);
	rotateHull(rot);
}
std::vector<std::pair<V3D,V3D> > CrystalHull::getEdges() const
{
	std::vector<std::pair<V3D,V3D> > temp;
	Edgeit it;
	for (it=_polyhedron.edges_begin();it!=_polyhedron.edges_end();++it)
	{
		const Point_3& p1=it->vertex()->point();
		const Point_3& p2=it->prev()->vertex()->point();
		temp.push_back(std::pair<V3D,V3D>(V3D(p1[0],p1[1],p1[2]),V3D(p2[0],p2[1],p2[2])));
	}
	return temp;
}

const std::vector<TriangleCache>& CrystalHull::getFaces() const
{
	return _tcache;
}

std::ostream& operator<<(std::ostream& os,const  CrystalHull& hull)
{
	os << "Crystal triangulated from " << hull.nPoints() << " points" << std::endl;
	os << "The triangulated polyhedron contains " << hull.nFaces() << " faces, "
	<< hull.nVertices() << " vertices, and "
	<< hull.nEdges() << " edges" << std::endl;
	os <<  "Volume of the crystal: " << hull.getVolume() << " mm^3"  << std::endl;
	os << "Centroid: " << hull.getCentroid();
	return os;
}


} /* namespace SX */

