/*
 * DeadTime.h
 *
 *  Created on: Jul 3, 2012
 *      Author: chapon
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
	// ! Create a deadtime correction from first and second order correction a and b
	//!  and respective errors
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
