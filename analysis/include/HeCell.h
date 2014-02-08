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

#ifndef NSXTOOL_HECELL_H_
#define NSXTOOL_HECELL_H_
#include "NumorSet.h"
#include "DeadTime.h"
#include "Scan1D.h"
#include <boost/tuple/tuple.hpp>
#include <vector>
#include <cmath>
#include <boost/shared_ptr.hpp>
#include <cminpack-1/cminpack.h>

namespace SX
{

struct data
{
	int n;
	double* x;
	double* y;
	double* sigma;
};

// Fit with A*exp(-a*t)
int ExpDecay(void* p, int m,int n,const double* x, double* fvec, int iflag)
{
	double a=x[0];
	double b=x[1];
	const double *y = ((data*)p)->y;
	const double *t = ((data*)p)->x;
	const double *s = ((data*)p)->sigma;

	const int nn=((data*)p)->n;
	for (int i = 0; i < nn; ++i)
	{
	  fvec[i] = y[i] - ((x[0]*exp(-x[1]*t[i]))/s[i]);
	}
		  return 0;

}


class HeCell
{
public:
	HeCell(const char* name=0);
	virtual ~HeCell();
	//! Set active deadtime correction
	void setDeadTime(const boost::shared_ptr<DTCorrection>& dt);
	// ! Attach a set of numors that characterizes this Cell
	void setNumors(const boost::shared_ptr<SX::Data::NumorSet>& numors);
	//! Reset the numors currently attached.
	void resetNumors();
	//! Get the data in the form of time in seconds and polarisations
	void  calculateDecay();
	//! Fit the 3He cell decay with exponential law of the type P[t]=P[0]*exp(-alpha*t) where
	//! A=P[0] is the initial polar and alpha is the depolarization rate. Return also the
	//! sigmas for A0 and alpha Return 1 if success
	int fitExponential(double& A0,double& A0s, double& alpha, double& alphas);
	//! Get the scan
	const boost::shared_ptr<SX::Data::Scan1D>& getDecay() const;
	//! Add a peak in the list to validate measurement of the Cell.
	//! Data in the set are used only if they correspond to one of the listed peak.
	void addPeak(double h, double k, double l);
	//! Calculate polarisation and error at the time given by a numor. Return the deltat in seconds
	//! with respect to starting time of the cell.
	double getPolar(const SX::Data::Numor& numor, double& Pcell, double& Pcells);
	//! Get the normalization constant at time t of the numor
	double normalizebpb(const SX::Data::Numor& numor, double& polar, double& polars);
private:
	std::string _name;
	boost::shared_ptr<SX::Data::NumorSet> _numors;
	boost::shared_ptr<DTCorrection> _deadt;
	boost::posix_time::ptime _starttime;
	boost::shared_ptr<SX::Data::Scan1D> _pvst;
	std::vector<boost::tuple<double,double,double> > _peaks;
	//! Initial polarization of the cell and sigma and decay rate (s-1) and error.
	double _P0, _P0s, _alpha0, _alpha0s;
};

} /* namespace SX */
#endif /* NSXTOOL_HECELL_H_ */
