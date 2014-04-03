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

#ifndef NSXTOOL_IMODIFIER1_H_
#define NSXTOOL_IMODIFIER1_H_

#include "Memento.h"
#include "State.h"

namespace SX
{

namespace Instrument
{

using namespace SX::Kernel;

template <typename mem_type>
class IModifier1
{
public:
    typedef Memento<mem_type> MementoType;
	void loadMemento(MementoType* state);
    MementoType* saveMemento();

    virtual ~IModifier1();

protected:
	mem_type _state;
};

template <typename mem_type>
void IModifier1<mem_type>::loadMemento(MementoType* m)
{
	_state = m->getState();
}

template <typename mem_type>
typename IModifier1<mem_type>::MementoType* IModifier1<mem_type>::saveMemento()
{
    return new MementoType(_state);
}

template <typename mem_type>
IModifier1<mem_type>::~IModifier1()
{
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

typedef Eigen::Matrix<double,3,1> Vector3D;
typedef Eigen::Matrix<double,4,4> HomMatrix;

class Rotator1 : public IModifier1<HomMatrix>
{
public:
	Rotator1();
	Rotator1(const Vector3D& axis);
	Rotator1(const Vector3D& axis, double angle);

	void setAngle(double angle);

private:
	Vector3D _axis;
};

Rotator1::Rotator1() : _axis(0,0,0), _state(HomMatrix::Identity())
{
}

Rotator1::Rotator1(const Vector3D& axis) : _axis(axis), _state(HomMatrix::Identity())
{
}

Rotator1::Rotator1(const Vector3D& axis, double angle) : _axis(axis)
{
    Eigen::Quaternion<double> quat(Eigen::AngleAxis<double>(_axis,angle));
	_state=HomMatrix::Zero();
	_state(3,3) = 1.0;
	_state.block(0,0,3,3)=quat.toRotationMatrix();
}

void Rotator1::setAngle(double angle)
{
    Eigen::Quaternion<double> quat(Eigen::AngleAxis<double>(_axis,angle));
	_state=HomMatrix::Zero();
	_state(3,3) = 1.0;
	_state.block(0,0,3,3)=quat.toRotationMatrix();
}

} // end namespace Instrument

} // end namespace SX

#endif /* NSXTOOL_IMODIFIER1_H_ */
