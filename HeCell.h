/*
 * HeCell.h
 *
 *  Created on: Jul 8, 2012
 *      Author: chapon
 */

#ifndef HECELL_H_
#define HECELL_H_
#include "NumorSet.h"
#include "DeadTime.h"
#include "Scan1D.h"
#include <tuple>
#include <memory>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit_nlin.h>
#include <vector>
#include <cmath>


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

int exp_f(const gsl_vector* params, void* data, gsl_vector* f)
{
	size_t n = ((struct data *)data)->n;
   double *x=((struct data *)data)->x;
   double *y = ((struct data *)data)->y;
   double *sigma = ((struct data *) data)->sigma;

   double A = gsl_vector_get (params, 0);
   double alpha = gsl_vector_get (params, 1);

   for (std::size_t  i = 0; i < n; i++)
	 {
	   /* Model Yi = A * exp(-lambda * i)  */
	   double Yi = A * exp (-alpha * x[i]);
	   gsl_vector_set (f, i, (Yi - y[i])/sigma[i]);
	 }

   return GSL_SUCCESS;
}

int  exp_df (const gsl_vector * params, void *data,
              gsl_matrix * J)
 {
   size_t n = ((struct data *)data)->n;
   double *x=((struct data *) data)->x;
   double *sigma = ((struct data *) data)->sigma;

   double A = gsl_vector_get (params, 0);
   double alpha = gsl_vector_get (params, 1);


   for (std::size_t i = 0; i < n; i++)
	 {
	   /* Jacobian matrix J(i,j) = dfi / dxj, */
	   /* where fi = (Yi - yi)/sigma[i],      */
	   /*       Yi = A * exp(-lambda * i) + b  */
	   /* and the xj are the parameters (A,lambda,b) */

	   double s = sigma[i];
	   double e = exp(-alpha * x[i]);
	   gsl_matrix_set (J, i, 0, e/s);
	   gsl_matrix_set (J, i, 1, -x[i] * A * e/s);
	 }
   return GSL_SUCCESS;
 }

int    exp_fdf (const gsl_vector * x, void *data,
               gsl_vector * f, gsl_matrix * J)
     {
       exp_f (x, data, f);
       exp_df (x, data, J);

       return GSL_SUCCESS;
     }


class HeCell
{
public:
	HeCell(const char* name=0);
	virtual ~HeCell();
	//! Set active deadtime correction
	void setDeadTime(const std::shared_ptr<DTCorrection>& dt);
	// ! Attach a set of numors that characterizes this Cell
	void setNumors(const std::shared_ptr<NumorSet>& numors);
	//! Reset the numors currently attached.
	void resetNumors();
	//! Get the data in the form of time in seconds and polarisations
	void  calculateDecay();
	//! Fit the 3He cell decay with exponential law of the type P[t]=P[0]*exp(-alpha*t) where
	//! A=P[0] is the initial polar and alpha is the depolarization rate. Return also the
	//! sigmas for A0 and alpha Return 1 if success
	int fitExponential(double& A0,double& A0s, double& alpha, double& alphas);
	//! Get the scan
	const std::shared_ptr<Scan1D>& getDecay() const;
	//! Add a peak in the list to validate measurement of the Cell.
	//! Data in the set are used only if they correspond to one of the listed peak.
	void addPeak(double h, double k, double l);
	//! Calculate polarisation and error at the time given by a numor. Return the deltat in seconds
	//! with respect to starting time of the cell.
	double getPolar(const Numor& numor, double& Pcell, double& Pcells);
	double normalizebpb(const Numor& numor, double& polar, double& polars);
private:
	std::string _name;
	std::shared_ptr<NumorSet> _numors;
	std::shared_ptr<DTCorrection> _deadt;
	boost::posix_time::ptime _starttime;
	std::shared_ptr<Scan1D> _pvst;
	std::vector<std::tuple<double,double,double> > _peaks;
	//! Initial polarization of the cell and sigma and decay rate (s-1) and error.
	double _P0, _P0s, _alpha0, _alpha0s;
};

} /* namespace SX */
#endif /* HECELL_H_ */
