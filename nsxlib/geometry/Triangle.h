/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

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

#ifndef NSXLIB_TRIANGLE_H
#define NSXLIB_TRIANGLE_H

#include <Eigen/Dense>

namespace nsx {

//! Small helper class that stores information for fast computation
// of intersection between lines and triangle in 3D.
struct Triangle
{
	Triangle() = delete;
	Triangle(const Eigen::Vector3d& p1,const Eigen::Vector3d& p2,const Eigen::Vector3d& p3);
	~Triangle(){}
	//! Test whether a ray generated in the xz plane and with direction y positive
	//! hits the bounding box of this triangle. Return false if inside the box.
	inline bool isOutsideBB(double px, double pz) const {
		return (px<_xmin || px>_xmax || pz<_zmin || pz>_zmax);
	}
	bool rayIntersect(const Eigen::Vector3d& point, const Eigen::Vector3d& dir,double& t1) const;
	void calculate();
	//! Rotation of the face
	void rotate(const Eigen::Matrix<double,3,3>& rotation);
	//! Vector values of point A and vector B-A and C-A
	Eigen::Vector3d _A,_B,_C,_AB,_AC;
	//! Face normal to the plane. Pointing outside the Hull.
	Eigen::Vector3d _normal;
	//! d value of the corresponding plane ax+by+cz+d=0;
	double _d;
	double _dot00,_dot11,_dot01;
	double _dot002d, _dot112d, _dot012d;
	//! Bounding box of the triangle.
	double _xmin,_zmin,_xmax,_zmax;

};

} // end namespace nsx

#endif // NSXLIB_TRIANGLE_H
