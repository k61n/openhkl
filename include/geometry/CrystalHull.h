/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
	BP 156
	6, rue Jules Horowitz
	38042 Grenoble Cedex 9
	France
	chapon[at]ill.fr

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef  NSXTOOL_CRYSTALHULL_H_
#define NSXTOOL_CRYSTALHULL_H_

#include <string>
#include <vector>
#include <iostream>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include "V3D.h"
#include "Matrix33.h"

namespace SX
{
// Define geometry Kernel
typedef  CGAL::Exact_predicates_inexact_constructions_kernel  Kernel;
typedef Kernel::Point_3 Point_3;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron_3;
typedef Polyhedron_3::Facet_const_iterator  Faceit;
typedef Polyhedron_3::Edge_const_iterator Edgeit;
typedef Polyhedron_3::Vertex_const_iterator Vertexit;

struct TriangleCache
{
	TriangleCache();
	~TriangleCache();
	//! Construct the triangle cache from the polyhedron face.
	TriangleCache operator()(Polyhedron_3::Face& f);
	//! Test whether a ray generated in the xz plane and with direction y positive
	//! hits the bounding box of this triangle. Return false if inside the box.
	inline bool isOutsideBB(double& px, double& pz) const {
		return (px<_xmin || px>_xmax || pz<_zmin || pz>_zmax);
	}
	//! Vector values of point A and vector B-A and C-A
	V3D _A,_AB,_AC;
	//! Face normal to the plane. Pointing outside the Hull.
	V3D _normal;
	//! d value of the corresponding plane ax+by+cz+d=0;
	double _d;
	double _dot00,_dot11,_dot01;
	double _dot002d, _dot112d, _dot012d;
	//! Bounding box of the triangle.
	double _xmin,_zmin,_xmax,_zmax;
};

//! @class: CrystalHull. Allows to construct a hull from
//! a set of points defined on the crystal surface.
class CrystalHull
{
public:
	CrystalHull(const char* name=0);
	virtual ~CrystalHull();
	//! addPoint on the Crystal surface. Hull is computed only when generate
	//! method is called.
	void addPoint(double x, double y, double z);
	//! Remove point from the crystal surface. If point if found, then removed.
	//! if the Hull is already
	void removePoint(double x, double y, double z);
	//! Generate the Hull. Return number of faces.
	//! This method generate the Hull, and also compute
	//! the plane equations for each face and compute the face normals.
	std::size_t  generate();
	void rotateHull(const Matrix33<double>& rot);
	//! Rotate the hull with the rotation matrix. First index line, second column.
	void rotateHull(double m01,double m02, double m03,
			double m10, double m11, double m12,
			double m20,double m21, double m22);
	//! Compute the volume of the Hull
	double getVolume() const;
	//! Compute the centroid of the convex Hull
	V3D getCentroid() const;
	//! Return the number of points that were used to construct the Hull
	std::size_t nPoints() const;
	//! Return the number of vertices
	std::size_t nVertices() const;
	//! Return the number of edges
	std::size_t nEdges() const;
	//! Return the number of faces
	std::size_t nFaces() const;
	//! Get the list of vertices making up the Hull. Points inside the Hull will not be listed.
	std::vector<V3D> getVertices() const;
	//! Get the edges making up the Hull as a pair of vectors.
	std::vector<std::pair<V3D,V3D> > getEdges() const;
	//! Get the faces.
	const std::vector<TriangleCache>& getFaces() const;
	//! Compute intersection time of a ray with the Hull.
	//! Return number of intersections
	//! Supposes that the user already knows that the ray is outside. Times
	//! t1 and t2 corresponds to the time of entrance and exit of the hull. This supposes
	//! that the Hull has no points with y-cordinates smaller than py
	int rayIntersectFromOutside(double px, double py, double pz, double& t1, double& t2) const;
	//! Compute intersection time of a ray with the Hull.
	//! Return number of intersections
	//! Supposes that the user already knows that the ray is inside. Only
	//! the oriented faces with normal vector in the same  direction of the ray
	//! are tested. t1 corresponds to the time of exit as pout=(px,py,pz)+t1*(dirx,diy,dirz)
	int rayIntersectFromInside(double px, double py , double pz, double dirx, double diry, double dirz, double& t1) const;
	//! Calculate the full path length inside the crystal  of a ray of initial position px,py,pz,and scattering with unit vector Svx, Svy, Svz
	//! The point of scattering within the incident path is set by the value rd01 which must be a random number between 0 and 1
	double scatteringPathLength(double px, double py, double pz, double Svx, double Svy, double Svz,double rd01) const;
private:
	std::string _name;
	//! Points from which convex hull is generated
	std::vector<Point_3> _points;
	//! Convex Hull
	Polyhedron_3 _polyhedron;
	//! Vector storing all triangle informations to be cached for fast intersection computations.
	std::vector<TriangleCache> _tcache;
	//! True if only already computed.
	bool _isgenerated;
};

//! Print CrystalHull information to a stream
std::ostream& operator<<(std::ostream& cout, const CrystalHull& hull);

} /* namespace SX */

#endif /* NSXTOOL_CRYSTALHULL_H_ */
