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

#ifndef NSXTOOL_MINIMIZEREIGEN_H_
#define NSXTOOL_MINIMIZEREIGEN_H_

#include <functional>
#include <memory>

#include <Eigen/Dense>
#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>

#include "LMFunctor.h"
#include "IMinimizer.h"

namespace SX {

namespace Utils {

class MinimizerEigen: public IMinimizer {


public:
    using f_type = std::function<int(const Eigen::VectorXd&, Eigen::VectorXd&)>;

    class functor_type: public LMFunctor<double> {
    public:
        functor_type(f_type f, int params, int values): LMFunctor(params, values), _f(f) {}

        int operator()(const Eigen::VectorXd& x, Eigen::VectorXd& r) const override { return _f(x, r); }
    private:
        f_type _f;
    };

    using fdf_type = Eigen::NumericalDiff<functor_type>;
    using lm_type = Eigen::LevenbergMarquardt<fdf_type>;

    MinimizerEigen();
    ~MinimizerEigen();

    void initialize(int params, int values) override;
    void deinitialize() override;

    const char* getStatusStr() override;
    bool fit(int max_iter) override;

private:
    std::unique_ptr<fdf_type> _fdf;
    std::unique_ptr<lm_type> _lm;
};

} // namespace Utils

} // namespace SX

#endif // NSXTOOL_MINIMIZEREIGEN_H_
