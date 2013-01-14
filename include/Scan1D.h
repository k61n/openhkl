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

#ifndef SCAN1D_H_
#define SCAN1D_H_
#include <vector>
#include <string>

namespace SX
{
//! Class to hold one-dimensional scans, of the form x,y,sigma.
//! If sigma vector is not given, gaussian errors are assumed.
//
typedef std::vector<double> vdouble;

class Scan1D
{
public:
	Scan1D(const char* xname,const  char* yname);
	Scan1D(char* xname, char* yname, const vdouble& x, const vdouble& y, const vdouble& e);
	virtual ~Scan1D();
	//! Setters
	void setData(const vdouble& x, const vdouble& y, const vdouble& e);
	//! Getters, const and not
	const vdouble& getX() const;
	const vdouble& getY() const;
	const vdouble& getE() const;
	vdouble& getX();
	vdouble& getY();
	vdouble& getE();
	// ! Return the number of points in the scan.
	std::size_t npoints() const;
	//! Clear x,y,e arrays.
	void clear();
private:
	std::string _xname, _yname;
	vdouble _x, _y, _e;
};

} /* namespace SX */
#endif /* SCAN1D_H_ */
