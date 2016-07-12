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

#ifndef NSXTOOL_LATTICESOLUTION_H_
#define NSXTOOL_LATTICESOLUTION_H_

#include <Eigen/Dense>

#include <memory>

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

class LatticeMinimizer;

struct LatticeSolution
{
	friend class LatticeMinimizer;
	LatticeSolution();

	LatticeSolution(const LatticeSolution& ubsol);

    LatticeSolution(std::shared_ptr<Instrument::Detector> detector,
                    std::shared_ptr<Instrument::Sample> sample,
                    std::shared_ptr<Instrument::Source> source,
                    const Eigen::VectorXd& values,
                    const Eigen::MatrixXd& cov,
                    const std::vector<bool>& fixedParameters);

	LatticeSolution& operator=(const LatticeSolution& ubsol);

    std::shared_ptr<Instrument::Detector> _detector;
    std::shared_ptr<Instrument::Sample> _sample;
    std::shared_ptr<Instrument::Source> _source;
	Eigen::VectorXd _latticeParams;
    Eigen::Matrix<double,9,9> _covLatticeParams;
    double _sourceOffset;
    double _sigmaSourceOffset;
	Eigen::VectorXd _detectorOffsets;
	Eigen::VectorXd _sigmaDetectorOffsets;
	Eigen::VectorXd _sampleOffsets;
	Eigen::VectorXd _sigmaSampleOffsets;
	std::vector<bool> _fixedParameters;

	friend std::ostream& operator<<(std::ostream& os, const LatticeSolution& solution);
};

} // end namespace Crystal

} // end namespace SX

#endif /* NSXTOOL_LATTICESOLUTION_H_ */
