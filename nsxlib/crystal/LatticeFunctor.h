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

#ifndef NSXTOOL_LATTICEFUNCTOR_H_
#define NSXTOOL_LATTICEFUNCTOR_H_

#include <map>
#include <set>
#include <vector>
#include <memory>

#include "LMFunctor.h"
#include "LatticeConstraintParser.h"

namespace SX
{

namespace Instrument
{
class Detector;
class Sample;
class Source;
}

namespace Crystal
{

class Peak3D;

/** @brief LatticeFunctor is used to refine lattice parameters and instrument offsets
 */
struct LatticeFunctor : public Utils::LMFunctor<double>
{
	//! Default constructor
	LatticeFunctor();
	//! Copy constructor
	LatticeFunctor(const LatticeFunctor& other);
	//! Assignment operator
	LatticeFunctor& operator=(const LatticeFunctor& other);
	//! Destructor
	~LatticeFunctor();
	/*
	 * @brief Call operator
	 * @param x the input parameters
	 * @param fvec the residuals
	 */
	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const;

	/*
	 * @brief Returns the number of inputs of the functor (e.g. the number of parameters)
	 * @return the number of inputs
	 */
	int inputs() const;
	/*
	 * @brief Returns the number of values of the functor (e.g. the number of observations)
	 * @return the number of inputs
	 */
	int values() const;

	std::vector<Peak3D> _peaks;
    std::shared_ptr<Instrument::Detector> _detector;
    std::shared_ptr<Instrument::Sample> _sample;
    std::shared_ptr<Instrument::Source> _source;
	SX::Utils::constraints_set _constraints;
	std::map<unsigned int,double> _constants;
};

} // end namespace Crystal

} // end namespace SX

#endif /* NSXTOOL_LATTICEFUNCTOR_H_ */
