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

#pragma once

#include <functional>
#include <random>
#include <vector>

#include "GeometryTypes.h"

namespace nsx {

//! Class used to compute absorption correction by Monte-Carlo integration.
class MCAbsorption {
public:
	//! Define absorption Engine with a rectangular source of WxH
	MCAbsorption(double width, double height, double pos);
	~MCAbsorption();
	//! Set the sample
	void setSample(ConvexHull* sample, double muScat, double muAbs);
	//! Run the Monte-Carlo calculation
	double run(unsigned int nIterations, const Eigen::Vector3d& outV, const Eigen::Matrix3d& sampleOrientation) const;


private:
	//! Description of the sample in terms of individual oriented triangles
	ConvexHull* _sample;
	double _width, _height, _pos;
	double _muScat, _muAbs;
    //! A random uniform distribution of double [0,1[
	std::function<double (void)> _random;

};

} // end namespace nsx
