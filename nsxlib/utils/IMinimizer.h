/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

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

#ifndef NSXTOOL_IMINIMIZER_H_
#define NSXTOOL_IMINIMIZER_H_

#include <functional>
#include <Eigen/Dense>

namespace nsx {
namespace Utils {

class IMinimizer {
public:
    using f_type = std::function<int(const Eigen::VectorXd&, Eigen::VectorXd&)>;

    IMinimizer();
    virtual ~IMinimizer();

    template <typename Fun_>
    void set_f(Fun_ functor)
    {
        _f = static_cast<f_type>(functor);
    }

    void setParams(const Eigen::VectorXd& x);
    void setWeights(const Eigen::VectorXd& wt);

    Eigen::MatrixXd covariance();
    Eigen::MatrixXd jacobian();
    Eigen::VectorXd params();

    void setxTol(double xtol);
    void setgTol(double gtol);
    void setfTol(double ftol);

    int numIterations();

    virtual void initialize(int params, int values);
    virtual void deinitialize();
    virtual bool fit(int max_iter) = 0;
    virtual const char* getStatusStr() = 0;

protected:
    int _numValues, _numParams, _numIter;

    double _xtol;
    double _gtol;
    double _ftol;

    Eigen::VectorXd _x,  _wt;
    Eigen::MatrixXd _jacobian, _covariance;

    f_type _f;
};

} // namespace Utils
} // end namespace nsx

#endif // NSXTOOL_IMINIMIZER_H_
