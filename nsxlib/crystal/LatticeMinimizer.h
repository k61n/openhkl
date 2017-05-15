/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
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

#ifndef NSXTOOL_LATTICEMINIMIZER_H_
#define NSXTOOL_LATTICEMINIMIZER_H_

#include <Eigen/Dense>

#include "LatticeFunctor.h"
#include "LatticeSolution.h"

namespace nsx {

class Detector;
class Sample;
class Source;

class Peak3D;

class LatticeMinimizer
{
public:
	//! Default constructor
	LatticeMinimizer();
	/*
	 * @brief Add a peak (e.g. an observation) to the minimizer
	 * @param peak the peak to be added
	 */
	void addPeak(const Peak3D& peak);
	/*
	 * @brief Clear the peaks
	 */
	void clearPeaks();
	/*
	 * @brief Define whether or not a given instrument offset will be fixed during the minization
	 * @param idx the index of the offset (starting from 9)
	 */
//	void refineInstrParameter(unsigned int idx, bool refine);
	/*
	 * @brief Reset the parameters
	 */
	void resetOffsets();

	//! Set constraint
	void setConstraint(unsigned int idx, unsigned int target, double factor=1.0);

	//! Set fixed value for a given lattice parameter
	void setConstant(unsigned int idx, double value);

	/*
	 * @brief Set the detector related to the peaks collected for the minimization
	 * @param detector the detector
	 */
    void setDetector(std::shared_ptr<Instrument::Detector> detector);
	/*
	 * @brief Set the sample related to the peaks collected for the minimization
	 * @param sample the sample
	 */
    void setSample(std::shared_ptr<Instrument::Sample> sample);
	/*
	 * @brief Set the starting values of the UB matrix
	 * @param ub the UB matrix
	 */
    void setSource(std::shared_ptr<Instrument::Source> source);

	void setStartingLattice(double a, double b, double c, double alpha, double beta, double gamma, bool constant=false);

	/*
	 * @brief Set the starting value for a given parameter
	 * @param idx the index of the parameter
	 * @param value the value of the parameter to be fixed
	 */
	void setStartingValue(unsigned int idx, double value, bool constant=false);
	/*
	 * @brief Unset the starting value for a given parameter
	 * @param idx the index of the parameter
	 */
	void unsetStartingValue(unsigned int idx);
	/*
	 * @brief Run the minimization
	 * @return the status of the minimization (1 if everything OK)
	 */
	int run(unsigned int maxIter);
	/*
	 * @brief Returns the solution of the last minimization
	 * @return the solution
	 */
	const LatticeSolution& getSolution() const;

private:
	LatticeFunctor _functor;
	LatticeSolution _solution;
	std::map<unsigned int,double> _start;
};

} // end namespace nsx

#endif /* NSXTOOL_LATTICEMINIMIZER_H_ */
