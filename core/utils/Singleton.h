#ifndef CORE_UTILS_SINGLETON_H
#define CORE_UTILS_SINGLETON_H

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

#endif // CORE_UTILS_SINGLETON_H
