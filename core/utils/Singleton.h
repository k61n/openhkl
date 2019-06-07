/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini
    2017- Laurent C. Chapon, Eric C. Pellegrini, Jonathan M. Fisher

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
 *
 */

#pragma once

#include <memory>

namespace nsx {

/**
 * @brief generic singleton class templated on the type of the object to be
 * singletonized, a constructor class and a destructor class.
 *
 * The concrete construction and destruction of the singleton instance are
 * assigned to external classes in order to provide singleton mechanism for
 * classes with a level of inheritance deeper than 1.
 */
template <typename T, template <class> class Constructor, template <class> class Destructor>
class Singleton {
public:
    //! return an instance of the class to be singletonized
    static T* Instance()
    {
        static std::unique_ptr<T, Destructor<T>> ptr(Constructor<T>::construct());
        // static std::unique_ptr<T> ptr(Constructor<T>::construct());
        return ptr.get();
    }
};

/**
 * @brief actually constructs the unique instance of the class to be
 * singletonized
 */
template <class T> class Constructor {
public:
    static T* construct() { return new T; }
};

/**
 * @brief actually destroys the unique instance of the class to be singletonized
 */
template <class T> class Destructor {
public:
    void operator()(T* p) { delete p; }
};

} // end namespace nsx
