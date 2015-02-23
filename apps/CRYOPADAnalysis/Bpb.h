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

#ifndef  NSXTOOL_BPB_H_
#define NSXTOOL_BPB_H_
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
	void calculateFlipping(const SX::Data::Numor& n, double& fr, double& frs,DTCorrection& correction, bool asymmetry=false);
	//! Strip out a data array from a numor consisting of a bpb measurement
	std::vector<double> getFlippingData(const SX::Data::Numor&);
} /* namespace SX */
#endif /* SX_BPB_H_ */
