//  ***********************************************************************************************
//
//  XSECTIONTool: data reduction for neutron single-crystal diffraction
//
//! @file      lib/ISingleton.h
//! @brief     Defines classes ISingleton, Constructor, Destructor
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef XSECTION_LIB_ISINGLETON_H
#define XSECTION_LIB_ISINGLETON_H

#include <memory>

namespace xsection {

//! generic singleton class templated on the type of the object to be
//! singletonized, a constructor class and a destructor class.
//!
//! The concrete construction and destruction of the singleton instance are
//! assigned to external classes in order to provide singleton mechanism for
//! classes with a level of inheritance deeper than 1.

template <typename T, template <class> class Constructor, template <class> class Destructor>
class ISingleton {
public:
    //! return an instance of the class to be singletonized
    static T* instance()
    {
        static std::unique_ptr<T, Destructor<T>> ptr(Constructor<T>::construct());
        // static std::unique_ptr<T> ptr(Constructor<T>::construct());
        return ptr.get();
    }
};

//! actually constructs the unique instance of the class to be singletonized
template <class T> class Constructor {
public:
    static T* construct() { return new T; }
};

//! actually destroys the unique instance of the class to be singletonized
template <class T> class Destructor {
public:
    void operator()(T* p) { delete p; }
};

} // namespace xsection

#endif // XSECTION_LIB_ISINGLETON_H
