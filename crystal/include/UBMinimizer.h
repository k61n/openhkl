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

#include <Eigen/Dense>

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
class Minimizer
{
public:
    typedef _Scalar Scalar;
    enum {InputsAtCompileTime = NX,ValuesAtCompileTime = NY};
    typedef Eigen::Matrix<Scalar,InputsAtCompileTime,1> InputType;
    typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,1> ValueType;
    typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,InputsAtCompileTime> JacobianType;

    Minimizer();
    Minimizer(int inputs, int values);

    virtual int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const=0;

    virtual int inputs() const;
    virtual int values() const;
    virtual ~Minimizer()=0;;

protected:
    int m_inputs, m_values;

};

template<typename _Scalar, int NX, int NY>
Minimizer<_Scalar,NX,NY>::Minimizer() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime)
{
}

template<typename _Scalar, int NX, int NY>
Minimizer<_Scalar,NX,NY>::Minimizer(int inputs, int values) : m_inputs(inputs), m_values(values)
{
}

template<typename _Scalar, int NX, int NY>
Minimizer<_Scalar,NX,NY>::~Minimizer()
{
}

template<typename _Scalar, int NX, int NY>
int Minimizer<_Scalar,NX,NY>::inputs() const
{
	return m_inputs;
}

template<typename _Scalar, int NX, int NY>
int Minimizer<_Scalar,NX,NY>::values() const {
    return m_values;
}

class UBMinimizer : public Minimizer<double>
{
public:
	UBMinimizer();
	~UBMinimizer();
	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const;

	double calcSSE(const Eigen::VectorXd& x);

	void addPeak(const Peak3D& peak);

	int inputs() const;
	int values() const;

	void setDetector(SX::Instrument::Detector* detector);
	void setSample(SX::Instrument::Sample* sample);

private:
	std::vector<Peak3D> _peaks;
	SX::Instrument::Detector* _detector;
	SX::Instrument::Sample* _sample;
};

} // end namespace Crystal

} // end namespace SX

#endif /* NSXTOOL_UBMINIMIZER_H_ */
