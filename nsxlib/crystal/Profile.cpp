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

static const double g_pi = double(M_PI);

namespace SX {
namespace Crystal {

Profile::Profile(double a, double b, double x0)
{
    setParams(a, b, x0);
}

bool Profile::fit(const Eigen::VectorXd &y, int max_iter)
{
    Minimizer min;
    Eigen::VectorXd wt;
    wt.resize(y.size());

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

    auto func = [y](const Eigen::VectorXd params, Eigen::VectorXd& res) -> int {
        assert(params.size() == 3);
        assert(res.size() == y.size());
        Profile lor(params(0), params(1), params(2));

        for (auto i = 0; i < y.size(); ++i) {
            res(i) = lor.evaluate(i) - y(i);
        }
        return 0;
    };

    min.initialize(3, y.size());
    min.setParams(Eigen::Vector3d(guess_a, guess_b, guess_x));
    min.setWeights(wt);
    min.set_f(func);

//    min.setxTol(1e-15);
//    min.setfTol(1e-15);
//    min.setgTol(1e-15);

    bool result = min.fit(max_iter);
    auto p = min.params();

    if (result) {
        auto p = min.params();
        setParams(p(0), p(1), p(2));
        return true;
    }
    std::cout << "Lorentzian fit failed: " << min.getStatusStr() << std::endl;
    return false;
}

double Profile::evaluate(double x) const
{
    const double u = x-_x0;
    return _a*_a / (u*u+_b*_b);
}

double Profile::integrate() const
{
    return _a*_a*g_pi / _b;
}

void Profile::setParams(double a, double b, double x0)
{
    _a = a;
    _b = b;
    _x0 = x0;
}

double Profile::getA() const
{
    return _a;
}

double Profile::getB() const
{
    return _b;
}

double Profile::getX() const
{
    return _x0;
}

} // namespace Crystal
} // namespace SX
