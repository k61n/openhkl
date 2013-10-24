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

#ifndef SX_IGONIO_H_
#define SX_IGONIO_H_
#include <string>
#include <vector>
#include <initializer_list>
#include <utility>
#include <Matrix33.h>
#include <RotAxis.h>


namespace SX {
namespace Geometry {


/* !
 * \brief Class IGonio.
 * Base class for all goniometers (system of several rotation axis). Any number of axis can be used.
 * The rotation is applied in reverse order of the given order. For example, when defining a goniometer with
 * 3 axes a,b,c, it is supposed that in such collection of axes, b is attached to the a shaft and c is attached to the
 * b shaft. Such gonio will rotate a vector v into a.b.c.v.
 * Once the IGonio is constructed, it is not allowed to change the number of axes.
 * Axes, their labels and respective limits can be modified by the class methods or by template accessor:
 * e.g. Axis<0>(g)=RotAxis(UnitZ,CW).
 */
class IGonio {
public:
	//! Initialize an empty Gonio with naxes
	IGonio(unsigned int naxes);
	//! Initialize a gonio from a list of labels of the axis, (e.g omega, chi, phi) parsed as {"omega","chi","phi"}
	IGonio(std::initializer_list<const char*> labels);
	//! Destructor
	virtual ~IGonio();
	//! Get the label for axis # _I
	const std::string& axisLabel(unsigned int i) const;
	std::string& axisLabel(unsigned int i);
	//! Accesor to Axis number _I
	RotAxis& axis(unsigned int i);
	RotAxis& axis(const char* label);
	//! Const-accessor to Axis _I
	const RotAxis& axis(unsigned int i) const;
	const RotAxis& axis(const char* label) const;
	//! Lower limit for Axis _I
	double& lowLimit(unsigned int i);
	double& lowLimit(const char * label);
	//! Upper limits for Axis _I
	double& highLimit(unsigned int i);
	double& highLimit(const char* label);
	//! Limits as initializer_list
	//! Return the rotation matrix corresponding to this set of angles. Throw if angles outside limits.
	Matrix33<double> anglesToMatrix(std::initializer_list<double> angles);
	//!
	friend std::ostream& operator<<(std::ostream& os,IGonio&);
protected:
	void isAxisValid(unsigned int i) const;
	unsigned int isAxisValid(const char*) const;
	std::vector<std::string> _labels;
	std::vector<RotAxis> _axes;
	std::vector<std::pair<double,double> > _limits;
};

template<unsigned int _N> RotAxis& Axis(IGonio& g)
{
	return g.axis(_N);
}

template<unsigned int _N> std::string& AxisLabel(IGonio& g)
{
	return g.axisLabel(_N);
}

template<unsigned int _N> double& LowLimit(IGonio& g)
{
	return g.lowLimit(_N);
}

template<unsigned int _N> double& HighLimit(IGonio& g)
{
	return g.highLimit(_N);
}

} // End namespace Geometry
} // End namespace SX

#endif /* SX_IGONIO_H_ */
