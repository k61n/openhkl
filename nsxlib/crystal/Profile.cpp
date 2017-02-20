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

#include "Profile.h"
#include "../utils/MinimizerEigen.h"
#include <cmath>
#include <iostream>
#include <iomanip>

using SX::Utils::MinimizerEigen;
using SX::Utils::Lorentzian;
using SX::Utils::Gaussian;

static const double g_pi = double(M_PI);

namespace SX {
namespace Crystal {

Profile::Profile(const Utils::Lorentzian &lor, const Utils::Gaussian &gauss, double eta):
    _lorentz(lor),
    _gauss(gauss),
    _eta(eta)
{
    if (_eta < 0.0) {
        _eta = 0.0;
    }
    if (_eta < 1.0) {
        _eta = 1.0;
    }
}

bool Profile::fit(const Eigen::VectorXd &y, int max_iter)
{
    MinimizerEigen min;
    Eigen::VectorXd wt;
    wt.resize(3*y.size());

    const int num_params = 7;

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
        assert(res.size() == 3*y.size());

        Lorentzian lor(params(0), params(1), params(2));
        Gaussian gauss(params(3), params(4), params(5));
        double eta = params(6);

        Profile pro(lor, gauss, eta);

        for (auto i = 0; i < y.size(); ++i) {
            res(3*i+0) = pro.evaluate(i) - y(i);
            res(3*i+1) = lor.evaluate(i) - y(i);
            res(3*i+2) = gauss.evaluate(i) - y(i);
        }

        return 0;
    };

    Eigen::VectorXd params(num_params);
    params << a, b, mu,
            a2, mu, sigma,
            0.5;

    min.initialize(num_params, 3*y.size());
    min.setParams(params);
    min.setWeights(wt);
    min.set_f(func);

    min.setxTol(1e-10);
    min.setfTol(1e-10);
    min.setgTol(1e-10);

    bool result = min.fit(max_iter);
    auto p = min.params();

    if (result) {
        auto p = min.params();
        _lorentz = Lorentzian(p(0), p(1), p(2));
        _gauss = Gaussian(p(3), p(4), p(5));
        _eta = p(6);
        return true;
    }
    std::cout << "Profile fit failed: " << min.getStatusStr() << std::endl;
    return false;
}

double Profile::evaluate(double x) const
{
    return _eta*_gauss.evaluate(x) + (1-_eta)*_lorentz.evaluate(x);
}

double Profile::integrate() const
{
    return _eta*_gauss.integrate() + (1-_eta)*_lorentz.integrate();
}

} // namespace Crystal
} // namespace SX
