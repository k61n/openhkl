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

#ifndef DEADTIME_H_
#define DEADTIME_H_
#include <vector>

namespace SX{

//! Simple dead-time correction of polynomial form.
//
class DTCorrection
{
public:
	// ! Create a deadtime correction from of polynomial form
	//!  and respective errors.
	//! Dead time correction is such that \f$I_{new}=\frac{I_{old}}{1.0-\alpha*\frac{I_{old}}{t}+...}\f$
	DTCorrection(const char* name=0);
	~DTCorrection();
	//! Set the term of degree N
	void setTerm(unsigned int N, double alpha, double salpha);
	//! Get the term of degree N
	std::pair<double,double> getTerm(unsigned int N) const;
	//! Apply the deadtime correction to a measurement of count/time.
	void apply(double& count, double& scount, double time) const;
private:
	std::string _name;
	//! The terms in the polynomial expansions and their sigmas
	//! stores as a vector of pair.
	std::vector<std::pair<double,double> > _ai;
};

} // end namespace SX

#endif /* DEADTIME_H_ */
