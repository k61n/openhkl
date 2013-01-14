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

#ifndef  NSXTOOL_CONSTANTS_H_
#define NSXTOOL_CONSTANTS_H_
#include <cmath>
#include <complex>

namespace SX
{
namespace Constants
{
	const double deg2rad=M_PI/180.0;
	const double rad2deg=180.0/M_PI;
	const double twopi=2.0*M_PI;
	const std::complex<double> twopij=std::complex<double>(0.0,twopi);

} // Namespace Constant

namespace Units
{
	enum Angular {Rad=0,Deg=1};
	enum Rotation {CCW=0,CW=1};

} //namespace Units


} //namespace SX

#endif /*NSXTOOL_CONSTANTS_H_*/
