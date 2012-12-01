/*
 * Scan1D.h
 *
 *  Created on: Jul 18, 2012
 *      Author: chapon
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
