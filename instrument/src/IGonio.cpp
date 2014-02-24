#include "IGonio.h"
#include <algorithm>
#include <stdexcept>
#include <Units.h>
#include <Eigen/Geometry>

namespace SX
{
namespace Instrument
{

using SX::Units::deg;
using Eigen::Quaterniond;
IGonio::IGonio(unsigned int n):_labels(n,""),_axes(n),_limits(n,std::pair<double,double>(-360.0*deg,360.0*deg))
{
}

IGonio::IGonio(std::initializer_list<const char*> labels):_labels(labels.begin(),labels.end())
{
	//! Ensure that the labels are unique
	std::vector<std::string> temp(_labels);
	std::sort(temp.begin(),temp.end());
	auto it = std::adjacent_find (temp.begin(),temp.end());
	if (it!=temp.end())
	{
		_labels.empty();
	    throw std::invalid_argument("Found duplicate entries in the labels of the axis when constructing the Goniometer");
	}
	//! Axis per default is rotation CCW around z
	std::size_t s=_labels.size();
	_axes.resize(s);
	_limits.resize(s);
	//! Setting up limits from -infinity to +infinity by default
	std::fill(_limits.begin(),
			_limits.end(),
			std::pair<double,double>(-360.0*deg,360.0*deg));
}

IGonio::~IGonio()
{
}
std::string& IGonio::axisLabel(unsigned int i)
{
	isAxisValid(i);
	return _labels[i];
}

const std::string& IGonio::axisLabel(unsigned int i) const
{
	isAxisValid(i);
	return _labels[i];
}

RotAxis& IGonio::axis(unsigned int i)
{
	isAxisValid(i);
	return _axes[i];
}

const RotAxis& IGonio::axis(unsigned int i) const
{
	isAxisValid(i);
	return _axes[i];
}


RotAxis& IGonio::axis(const char* label)
{
	unsigned int i=isAxisValid(label);
	return _axes[i];
}

const RotAxis& IGonio::axis(const char* label) const
{
	unsigned int i=isAxisValid(label);
	return _axes[i];
}

double& IGonio::lowLimit(unsigned int i)
{
	isAxisValid(i);
	return _limits[i].first;
}
double& IGonio::highLimit(unsigned int i)
{
	isAxisValid(i);
	return _limits[i].second;
}
double& IGonio::lowLimit(const char* label)
{
	unsigned int i=isAxisValid(label);
	return _limits[i].first;
}
double& IGonio::highLimit(const char* label)
{
	unsigned int i=isAxisValid(label);
	return _limits[i].second;
}


Matrix3d IGonio::anglesToMatrix(std::initializer_list<double> l)
{
	if (l.size()!=_labels.size())
		throw std::invalid_argument("Number of arguments angles different from the number of axes on the goniometer");
	// Get rotation matrix of the right most angle
	auto it=l.begin();
	int i=0;
	Quaterniond result=_axes[i++].getQuat(*it);
	it++;
	// Quat multiplication is faster than matrix.
	for(;it!=l.end();++it)
	{
		result=result*_axes[i++].getQuat(*it);
	}
	return result.toRotationMatrix();
}

std::ostream& operator<<(std::ostream& os,IGonio& g)
{
	os << "Goniometer with rotation: \n";
	for (unsigned int i=0;i<g._labels.size();i++)
	{
		os << g._labels[i] << " " << g._axes[i] << ", angular limits: "
		   << g._limits[i].first/Units::deg << "," << g._limits[i].second/Units::deg
		   <<  std::endl;
	}
	return os;
}

void IGonio::isAxisValid(unsigned int i) const
{
	if (i>=_labels.size())
		throw std::invalid_argument("Trying to access non-valid axis");
}

unsigned int IGonio::isAxisValid(const char* label) const
{
	std::string rhs(label);
	for (unsigned int i=0;i<_labels.size();++i)
	{
		if (_labels[i].compare(rhs)==0)
			return i;
	}
	//! If not found
	throw std::invalid_argument("Could not find the label "+rhs+" as a goniometer axis");
}


}
}
