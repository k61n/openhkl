/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

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

#include <cmath>
#include <iostream>
#include <iomanip>

#include "Profile.h"
#include "../utils/MinimizerGSL.h"

using nsx::Utils::MinimizerGSL;
using nsx::Utils::Lorentzian;
using nsx::Utils::Gaussian;

static const double g_pi = double(M_PI);

namespace nsx {
namespace Crystal {

Profile::Profile(const Utils::Lorentzian &lor, const Utils::Gaussian &gauss):
    _lorentz(lor),
    _gauss(gauss)
{
}

bool Profile::fit(const Eigen::VectorXd &y, int max_iter)
{
    MinimizerGSL min;
    Eigen::VectorXd wt;
    wt.resize(y.size());

    const int num_params = 6;

    // too few data points to fit
    if (y.size() <= num_params) {
        return false;
    }

    for (int i = 0; i < wt.size(); ++i) {
        wt(i) = 1.0;
    }

    // produce some educated guesses
    double mu = 0.0;
    double var = 0.0;
    Eigen::VectorXd rho = y / y.sum();
    const double sum = y.sum();

    for (auto i = 0; i < y.size(); ++i) {
        mu += i*rho(i);
        var += i*i*rho(i);
    }
    var -= mu*mu;
    const double sigma = std::sqrt(var);
    const double b = sum / (g_pi*mu);
    const double a = std::sqrt( std::fabs(sum * b / g_pi));
    const double a2 = std::sqrt(y.sum() / std::sqrt(2*3.141592*sigma*sigma));

    auto func = [y, num_params](const Eigen::VectorXd params, Eigen::VectorXd& res) -> int {
        assert(params.size() == num_params);
        assert(res.size() == y.size());

        Lorentzian lor(params(0), params(1), params(2));
        Gaussian gauss(params(3), params(4), params(5));
        Profile pro(lor, gauss);

        for (auto i = 0; i < y.size(); ++i) {
            res(i) = pro.evaluate(i) - y(i);
        }

        return 0;
    };

    Eigen::VectorXd params(num_params);
    params << a, b, mu,
            a2, mu, sigma;

    min.initialize(num_params, y.size());
    min.setParams(params);
    min.setWeights(wt);
    min.set_f(func);

    if (min.fit(max_iter) == false) {
        std::cout << "Profile fit failed: " << min.getStatusStr() << std::endl;
        return false;
    }

    auto p = min.params();
    _lorentz = Lorentzian(p(0), p(1), p(2));
    _gauss = Gaussian(p(3), p(4), p(5));
    return true;
}

double Profile::evaluate(double x) const
{
    return 0.5*_gauss.evaluate(x) + 0.5*_lorentz.evaluate(x);
}

double Profile::integrate() const
{
    return 0.5*_gauss.integrate() + 0.5*_lorentz.integrate();
}

bool Profile::goodFit(const Eigen::VectorXd &y, double eps) const
{
    Eigen::VectorXd x(y.size());
    for (auto i = 0; i < y.size(); ++i) {
        x(i) = evaluate(i);
    }
    auto diff = (x-y).norm();
    return diff < eps*y.norm() && diff < eps*x.norm();
}

} // namespace Crystal
} // end namespace nsx

