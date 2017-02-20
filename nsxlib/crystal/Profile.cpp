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
#include "../utils/Minimizer.h"
#include <cmath>
#include <iostream>
#include <iomanip>

using SX::Utils::Minimizer;
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

}

bool Profile::fit(const Eigen::VectorXd &y, int max_iter)
{
    Minimizer min;
    Eigen::VectorXd wt;
    wt.resize(y.size());

    const int num_params = 7;

    for (int i = 0; i < wt.size(); ++i) {
        wt(i) = 1.0;
    }

    // produce some educated guesses
    double guess_x = 0.0;
    const double sum = y.sum();

    for (auto i = 0; i < y.size(); ++i) {
        guess_x += i*y(i) / sum;
    }

    const double guess_b = sum / (g_pi*guess_x);
    const double guess_a = std::sqrt( std::fabs(sum * guess_b / g_pi));

    auto func = [y, num_params](const Eigen::VectorXd params, Eigen::VectorXd& res) -> int {
        assert(params.size() == num_params);
        assert(res.size() == y.size());

        Lorentzian lor(params(0), params(1), params(2));
        Gaussian gauss(params(3), params(4), params(5));
        double eta = params(6);

        if ( eta < 0.0) {
            eta = 0.0;
        }
        if (eta > 1.0) {
            eta = 1.0;
        }

        Profile pro(lor, gauss, eta);

        for (auto i = 0; i < y.size(); ++i) {
            res(i) = pro.evaluate(i) - y(i);
        }
        return 0;
    };

    Eigen::VectorXd params(num_params);
    params << guess_a, guess_b, guess_x, guess_a, guess_b, guess_x, 0.5;

    min.initialize(num_params, y.size());
    min.setParams(params);
    min.setWeights(wt);
    min.set_f(func);

    min.setxTol(1e-15);
    min.setfTol(1e-15);
    min.setgTol(1e-15);

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
