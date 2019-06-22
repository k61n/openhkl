//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      base/utils/Singleton.h
//! @brief     Defines classes Singleton, Constructor, Destructor
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef BASE_UTILS_SINGLETON_H
#define BASE_UTILS_SINGLETON_H

#include <memory>

namespace nsx {

//! Generic singleton class template.

//! Templated on the type of the object to be singletonized, a constructor class
//! and a destructor class.
//!
//! The concrete construction and destruction of the singleton instance are
//! assigned to external classes in order to provide singleton mechanism for
//! classes with a level of inheritance deeper than 1.

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

} // namespace nsx

#endif // BASE_UTILS_SINGLETON_H
