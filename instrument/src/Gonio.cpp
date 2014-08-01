#include "Gonio.h"
#include <algorithm>
#include <stdexcept>
#include <Units.h>
#include <Eigen/Geometry>
#include "RotAxis.h"
#include "TransAxis.h"

namespace SX
{
namespace Instrument
{

using SX::Units::deg;
using Eigen::Quaterniond;
Gonio::Gonio(const std::string& label):_label(label)
{
}

Gonio::~Gonio()
{
}

Axis* Gonio::axis(unsigned int i)
{
	isAxisValid(i);
	return _axes[i];
}


Axis* Gonio::axis(const char* label)
{
	unsigned int i=isAxisValid(label);
	return _axes[i];
}

Axis* Gonio::addRotation(const std::string& label, const Vector3d& axis,RotAxis::Direction dir)
{
	_axes.push_back(new RotAxis(label,axis,dir));
	return _axes.back();
}

Axis* Gonio::addTranslation(const std::string& label, const Vector3d& axis)
{
	_axes.push_back(new TransAxis(label,axis));
	return _axes.back();
}


void Gonio::isAxisValid(unsigned int i) const
{
	if (i>=_axes.size())
		throw std::invalid_argument("Trying to access non-valid axis");
}

unsigned int Gonio::isAxisValid(const char* label) const
{
	std::string rhs(label);
	for (unsigned int i=0;i<_axes.size();++i)
	{
		if (_axes[i]->getLabel().compare(rhs)==0)
			return i;
	}
	//! If not found
	throw std::invalid_argument("Could not find the label "+rhs+" as a goniometer axis");
}


}
}
