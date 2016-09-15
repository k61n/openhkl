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

 Forshungszentrum Juelich GmbH
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

#include "MinimizerEigen.h"


#include <stdlib.h>
#include <stdio.h>


#include <Eigen/Dense>
#include <unsupported/Eigen/NumericalDiff>
#include <unsupported/Eigen/NonLinearOptimization>
//#include <unsupported/Eigen/LevenbergMarquardt>


namespace SX {

namespace Utils {


MinimizerEigen::MinimizerEigen():
    IMinimizer()
{
}

MinimizerEigen::~MinimizerEigen()
{
    deinitialize();
}

void MinimizerEigen::initialize(int params, int values)
{
    IMinimizer::initialize(params, values);
}

bool MinimizerEigen::fit(int max_iter)
{
    if (!_f)
        return false;

    _fdf = std::unique_ptr<fdf_type>(new fdf_type(_f, _numParams, _numValues));
    _lm = std::unique_ptr<lm_type>(new lm_type(*_fdf));

    _lm->parameters.xtol = _xtol;
    _lm->parameters.ftol = _ftol;
    _lm->parameters.gtol = _gtol;

    _lm->parameters.maxfev = max_iter;

    int status = _lm->minimize(_x);

    // evaluate the jacobian
    _fdf->df(_x, _jacobian);

    _numIter = _lm->iter;

    return (status == 1);
}

void MinimizerEigen::deinitialize()
{
    IMinimizer::deinitialize();
    _f = nullptr;
}


//Eigen::MatrixXd MinimizerEigen::covariance()
//{
//    // The upper N*N block of the FJAC matrix
//    Eigen::MatrixXd fjac = _lm->fjac.block(0, 0, _numParams, _numParams);

//    // The R * P^t matrix
//    Eigen::MatrixXd RPt =
//            fjac.triangularView<Eigen::Upper>()*(_lm->permutation.toDenseMatrix().cast<double>().transpose());

//    Eigen::MatrixXd JtJ = RPt.transpose()*RPt;

//    // Remove the fixed parameters before inverting J^t * J
//    int removed=0;

////    for (unsigned int i=0;i<fParams.size();++i)
////    {
////        if (fParams[i])
////        {
////            Utils::removeColumn(JtJ,i-removed);
////            Utils::removeRow(JtJ,i-removed);
////            removed++;
////        }
////    }

//    // The covariance matrix
//    Eigen::MatrixXd covariance=JtJ.inverse();

//    //covariance *= mse;
//}



const char *MinimizerEigen::getStatusStr()
{
    //return gsl_strerror(_status);
    return "";
}




/*
int UBMinimizer::runEigen(unsigned int maxIter)
{

    int nParams=_functor.inputs();
    Eigen::VectorXd x=Eigen::VectorXd::Zero(nParams);

    for (auto it=_start.begin();it!=_start.end();++it)
        x[it->first] = it->second;

    typedef Eigen::NumericalDiff<UBFunctor> NumDiffType;
    NumDiffType numdiff(_functor);
    Eigen::LevenbergMarquardt<NumDiffType> minimizer(numdiff);
    minimizer.parameters.xtol=1e-11;
    minimizer.parameters.maxfev=maxIter;
    int status = minimizer.minimize(x);

    if (status==1)
    {
        std::vector<bool> fParams(x.size(),false);
        for (auto it : _functor._fixedParameters)
            fParams[it] = true;

        // Create a vector to calculate final residuals
        Eigen::VectorXd fvec(_functor.values());
        // Calculate final residuals
        _functor(x,fvec);

        // The MSE is computed as SSE/dof where dof is the number of degrees of freedom
        int nFreeParameters=_functor.inputs()-_functor._fixedParameters.size();
        double mse = fvec.squaredNorm()/(_functor.values()-nFreeParameters);

        // Computation of the covariance matrix
        // The covariance matrix is obtained from the estimated jacobian computed through a QR decomposition
        // (see for example:
        // 		- http://en.wikipedia.org/wiki/Non-linear_least_squares
        // 		- https://github.com/scipy/scipy/blob/v0.14.0/scipy/optimize/minpack.py#L256
        // 		- http://osdir.com/ml/python.scientific.user/2005-03/msg00067.html)

        // CMinPack does not provide directly the jacobian but the so called FJAC that is NOT the jacobian
        // FJAC is an output M by N array where M is the number of observations (_functor.values()) and N the number of parameters (_functor.inputs()).
        // The upper N by N submatrix of FJAC contains the upper triangular matrix R with diagonal elements of nonincreasing magnitude such that
        //
        //		P^t * (J^t * J) * P = R^t * R,
        //
        // where P is a permutation matrix and J is the final calculated jacobian
        // From (J^t * J) we can get directly get the covariance matrix C using the formula
        //
        // 		C = (J^t * J)^-1

        // The upper N*N block of the FJAC matrix
        Eigen::MatrixXd fjac=minimizer.fjac.block(0,0,_functor.inputs(),_functor.inputs());

        // The R * P^t matrix
        Eigen::MatrixXd RPt = fjac.triangularView<Eigen::Upper>()*(minimizer.permutation.toDenseMatrix().cast<double>().transpose());

        Eigen::MatrixXd JtJ = RPt.transpose()*RPt;

        // Remove the fixed parameters before inverting J^t * J
        int removed=0;

        for (unsigned int i=0;i<fParams.size();++i)
        {
            if (fParams[i])
            {
                Utils::removeColumn(JtJ,i-removed);
                Utils::removeRow(JtJ,i-removed);
                removed++;
            }
        }

        // The covariance matrix
        Eigen::MatrixXd covariance=JtJ.inverse();

        covariance *= mse;


    }

    return status;

}
*/
} // namespace Utils

} // namespace SX
