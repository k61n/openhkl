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

#ifndef NSXLIB_PACKER_H
#define NSXLIB_PACKER_H

#include <Eigen/Dense>

namespace nsx {

template <typename... Ts> class Packer {
public:
    const Eigen::VectorXd& pack() { return _data; }
    void unpack() {}
protected:
    Eigen::VectorXd _data;
    int _pos;
};

template <typename T, typename... Ts>
struct Packer<T, Ts...>: public Packer<Ts...> {
public:

    const Eigen::VectorXd& pack(T x, Ts... xs) {
        using super = Packer<Ts...>;
        _offset = super::push(x);
        return super::pack(xs...);
    }

    void unpack(T& x, Ts&... xs) {
    }

private:
    size_t _offset;
};

} // end namespace nsx

#endif // NSXLIB_PACKER_H
