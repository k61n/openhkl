/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
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

#ifndef NSXTOOL_FFTINDEXING_H_
#define NSXTOOL_FFTINDEXING_H_

#include <vector>

#include <Eigen/Dense>

#include "Units.h"

namespace SX
{

namespace Crystal
{

typedef unsigned int uint;

using namespace SX::Units;

struct tVector
{
    tVector(const Eigen::Vector3d& v,double quality):_vect(v),_quality(quality)
	{
	}
    Eigen::Vector3d _vect;
    double _quality;
};


class FFTIndexing
{
public:
	FFTIndexing(double amax=100.0,double thetaStep=1.8*deg,double phiStep=1.8*deg);
	std::vector<tVector> find(std::vector<Eigen::Vector3d> qVectors);
	virtual ~FFTIndexing();

private:
	static int nSubdiv;
	double _amax;
	double _thetaStep;
	double _phiStep;
};

} // end namespace Crystal

} // end namespace SX

#endif /* NSXTOOL_FFTINDEXING_H_ */
