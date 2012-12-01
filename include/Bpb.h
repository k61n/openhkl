/*
 * Bpb.h
 *
 *  Created on: Nov 30 2012
 *      Author: chapon
 */

#ifndef  SX_BPB_H_
#define SX_BPB_H_
#include <vector>
#include <algorithm>
#include "DeadTime.h"
#include "Numor.h"

namespace SX
{
	//! Calculate flipping ratios from raw data.
    //! Data must consist of array with 18 elements:
	//! Left background    : cl+, cl-, tl+, tl-, ml+, ml-
	//! Peak                              : cp+, cp-, tp+, tp-, mp+, mp-
	//! Right background  : cr+,cr-,tr+,tr-,mr+,mr-
	void calculateFlipping(const std::vector<double>& data,double& FR, double& FRS,DTCorrection& correction,bool asymmetry=false );
	//! Convert flipping ratio (and error) to asymmetry (and error)
	void converttoAsymmetry(double& FR, double& FRS);
	//! Get bpb measurement from a numor. Throws if not bpb
	void calculateFlipping(const Numor& n, double& fr, double& frs,DTCorrection& correction, bool asymmetry=false);
	//! Strip out a data array from a numor consisting of a bpb measurement
	std::vector<double> getFlippingData(const Numor&);
} /* namespace SX */
#endif /* SX_BPB_H_ */
