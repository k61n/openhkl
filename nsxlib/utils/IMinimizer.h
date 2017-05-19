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

#ifndef NSXLIB_IMINIMIZER_H
#define NSXLIB_IMINIMIZER_H

#include <functional>
#include <Eigen/Dense>

namespace nsx {

class IMinimizer {
public:
    using f_type = std::function<int(const Eigen::VectorXd&, Eigen::VectorXd&)>;

    IMinimizer();

    virtual const char* getStatusStr()=0;
    virtual bool fit(int max_iter)=0;

    virtual void initialize(int params, int values)=0;
    virtual void deinitialize()=0;

    virtual void setParams(const Eigen::VectorXd& x)=0;
    virtual void setWeights(const Eigen::VectorXd& wt)=0;

    virtual int numIterations()=0;

    virtual void setxTol(double xtol)=0;
    virtual void setgTol(double gtol)=0;
    virtual void setfTol(double ftol)=0;

    virtual Eigen::MatrixXd jacobian()=0;
    virtual Eigen::VectorXd params()=0;

    virtual Eigen::MatrixXd covariance()=0;

protected:
    int _numValues, _numParams, _numIter;

    double _xtol;
    double _gtol;
    double _ftol;

    Eigen::VectorXd _x,  _wt;
    Eigen::MatrixXd _jacobian, _covariance;

    f_type _f;
};

} // end namespace nsx

#endif // NSXLIB_IMINIMIZER_H
