/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
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

#include <map>
#include <set>
#include <vector>

#include <Eigen/Dense>
#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>

#include <Detector.h>
#include <Peak3D.h>
#include <Sample.h>

#ifndef NSXTOOL_UBMINIMIZER_H_
#define NSXTOOL_UBMINIMIZER_H_

namespace SX
{

namespace Crystal
{

// Generic functor
template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
class Functor
{
public:
    typedef _Scalar Scalar;
    enum {InputsAtCompileTime = NX,ValuesAtCompileTime = NY};
    typedef Eigen::Matrix<Scalar,InputsAtCompileTime,1> InputType;
    typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,1> ValueType;
    typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,InputsAtCompileTime> JacobianType;

    Functor();
    Functor(int inputs, int values);

    virtual int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const=0;

    virtual int inputs() const;
    virtual int values() const;
    virtual ~Functor()=0;;

protected:
    int m_inputs, m_values;

};

template<typename _Scalar, int NX, int NY>
Functor<_Scalar,NX,NY>::Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime)
{
}

template<typename _Scalar, int NX, int NY>
Functor<_Scalar,NX,NY>::Functor(int inputs, int values) : m_inputs(inputs), m_values(values)
{
}

template<typename _Scalar, int NX, int NY>
Functor<_Scalar,NX,NY>::~Functor()
{
}

template<typename _Scalar, int NX, int NY>
int Functor<_Scalar,NX,NY>::inputs() const
{
	return m_inputs;
}

template<typename _Scalar, int NX, int NY>
int Functor<_Scalar,NX,NY>::values() const {
    return m_values;
}

struct UBFunctor : public Functor<double>
{
	//! Default constructor
	UBFunctor();
	//! Copy constructor
	UBFunctor(const UBFunctor& other);
	//! Assignment operator
	UBFunctor& operator=(const UBFunctor& other);
	//! Destructor
	~UBFunctor();
	/*
	 * @brief Call operator
	 * @param x the input parameters
	 * @param fvec the residuals
	 */
	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const;
	/*
	 * @brief Add a peak (e.g. an observation) to the minimizer
	 * @param peak the peak to be added
	 */
	void addPeak(const Peak3D& peak);
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
	/*
	 * @brief Set the detector related to the peaks collected for the minimization
	 * @param detector the detector
	 */
	void setDetector(SX::Instrument::Detector* detector);
	/*
	 * @brief Set the sample related to the peaks collected for the minimization
	 * @param sample the sample
	 */
	void setSample(SX::Instrument::Sample* sample);
	//! Reset all the parameters (e.g. UB matrix + detector and sample offsets) to zero
	void resetParameters();
	/*
	 * @brief Set the offsets that will be fixed during the minization
	 * @param idx the index of the offset (starting from 9)
	 */
	void setFixedParameters(unsigned int idx);

	std::vector<Peak3D> _peaks;
	SX::Instrument::Detector* _detector;
	SX::Instrument::Sample* _sample;
	std::set<int> _fixedParameters;
};

class UBMinimizer;

struct UBSolution
{
	UBSolution(const UBSolution& ubsol);
	friend class UBMinimizer;
	UBSolution();
	UBSolution(SX::Instrument::Detector* detector,SX::Instrument::Sample* sample, Eigen::VectorXd values, Eigen::VectorXd sigmas, std::vector<bool> fixedParameters);
	UBSolution& operator=(const UBSolution& ubsol);
	SX::Instrument::Detector* _detector;
	SX::Instrument::Sample* _sample;
    Eigen::Matrix3d _ub;
    Eigen::Matrix3d _sigmaub;
	Eigen::VectorXd _detectorOffsets;
	Eigen::VectorXd _sigmaDetectorOffsets;
	Eigen::VectorXd _sampleOffsets;
	Eigen::VectorXd _sigmaSampleOffsets;
	std::vector<bool> _fixedParameters;
};

class UBMinimizer
{
public:
	//! Default constructor
	UBMinimizer();
	/*
	 * @brief Add a peak (e.g. an observation) to the minimizer
	 * @param peak the peak to be added
	 */
	void addPeak(const Peak3D& peak);
	//! Reset all the parameters (e.g. UB matrix + detector and sample offsets) to zero
	void resetParameters();
	/*
	 * @brief Set the detector related to the peaks collected for the minimization
	 * @param detector the detector
	 */
	void setDetector(SX::Instrument::Detector* detector);
	/*
	 * @brief Set the offsets that will be fixed during the minization
	 * @param idx the index of the offset (starting from 9)
	 */
	void setFixedParameters(unsigned int idx);
	/*
	 * @brief Set the maximum number of iteration used by the LM engine
	 * @param max the maximum number of iteration
	 */
	void setMaxIter(unsigned int max);
	/*
	 * @brief Set the sample related to the peaks collected for the minimization
	 * @param sample the sample
	 */
	void setSample(SX::Instrument::Sample* sample);
	/*
	 * @brief Set the starting values of the UB matrix
	 * @param ub the UB matrix
	 */
	void setStartingUBMatrix(const Eigen::Matrix3d& ub);
	/*
	 * @brief Set the starting value for a given parameter
	 * @param idx the index of the parameter
	 * @param value the value of the parameter to be fixed
	 */
	void setStartingValue(unsigned int idx, double value);
	/*
	 * @brief Unset the starting value for a given parameter
	 * @param idx the index of the parameter
	 */
	void unsetStartingValue(unsigned int idx);
	/*
	 * @brief Run the minimization
	 * @return the status of the minimization (1 if everything OK)
	 */
	int run();
	/*
	 * @brief Returns the solution of the last minization
	 * @return the solution
	 */
	const UBSolution& getSolution() const;
private:
	UBFunctor _functor;
	Eigen::NumericalDiff<UBFunctor> _numDiff;
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<UBFunctor>,double> _minimizer;
	UBSolution _solution;
	std::map<unsigned int,double> _start;
};


} // end namespace Crystal

} // end namespace SX

#endif /* NSXTOOL_UBMINIMIZER_H_ */
