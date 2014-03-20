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

#ifndef NSXTOOL_TRANSLATOR_H_
#define NSXTOOL_TRANSLATOR_H_

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "IModifier.h"

namespace SX
{

namespace Instrument
{

typedef Eigen::Matrix<double,3,1> Vector3d;
typedef Eigen::Matrix<double,4,4> HomMatrix;

template <typename ...Ts>
class Translator : public IModifier<Vector3d>
{
public:
	Translator(Ts... vals);
	HomMatrix getTransformation() const;
};

template <typename ...Ts>
Translator<Ts...>::Translator(Ts... vals) : IModifier(vals...)
{
}

template <typename ...Ts>
HomMatrix Translator<Ts...>::getTransformation() const
{
	HomMatrix trans=HomMatrix::Zero();
	trans(3,3) = 1.0;
	trans.block(0,3,3,1)=std::get<0>(_state.get());

	return trans;

}

} // end namespace Instrument

} // end namespace SX

#endif /* ROTATOR_H_ */
