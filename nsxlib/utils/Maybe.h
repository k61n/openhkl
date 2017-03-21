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

#ifndef NSXTOOL_MAYBE_H_
#define NSXTOOL_MAYBE_H_

#include <type_traits>
#include <stdexcept>

namespace SX {
namespace Utils {

//! Maybe monad
template<typename T>
class Maybe {
public:
    using no_ref = typename std::remove_reference<T>::type;
    //using value_type = no_ref * const;
    using value_type = no_ref;

    explicit Maybe(): _isNothing(true) {}
    explicit Maybe(const T& a): _value(a), _isNothing(false) {}
    explicit Maybe(T&& a): _value(std::move(a)), _isNothing(false) {}

    explicit Maybe(const Maybe& other): _value(other._value), _isNothing(other._isNothing) {}
    explicit Maybe(Maybe&& other): _value(std::move(other._value)), _isNothing(other._isNothing) {}

    bool isNothing() const
    {
        return _isNothing;
    }

    value_type& get() //const
    {
        if (_isNothing) {
            throw std::runtime_error("Maybe: get() called on nothing");
        }
        return _value;
    }

    Maybe& operator=(const Maybe& other)
    {
        _value = other._value;
        _isNothing = other._isNothing;
        return *this;
    }

    Maybe& operator=(Maybe&& other)
    {
        _value = std::move(other._value);
        _isNothing = other._isNothing;
        return *this;
    }

    Maybe& operator=(const value_type& value)
    {
        _value = value;
        _isNothing = false;
        return *this;
    }

    Maybe& operator=(value_type&& value)
    {
        _value = std::move(value);
        _isNothing = false;
        return *this;
    }

private:
    value_type _value;
    bool _isNothing;
};

} // namespace Utils
} // namespace SX

#endif // NSXTOOL_GAUSSIAN_H_
