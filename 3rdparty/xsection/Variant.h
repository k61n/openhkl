//  ***********************************************************************************************
//
//  XSECTIONTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/Variant.h
//! @brief     Defines class Variant
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef XSECTION_LIB_VARIANT_H
#define XSECTION_LIB_VARIANT_H

#include <iostream>
#include <typeindex>

namespace xsection {

//! Template meta-class for fetching the type of the first template argument of
//! a Variant
template <typename... Args> struct FirstArg {
};

//! Template meta-class for fetching the type of the first template argument of
//! a Variant
template <typename T, typename... Args> struct FirstArg<T, Args...> {
    using type = T;
};

//! Template meta-class for computing the maximum size of the types of a Variant
template <typename... Args> struct MaxSizeOf;

//! Template meta-class for computing the maximum size of the types of a Variant
template <typename T> struct MaxSizeOf<T> {
    static_assert(!std::is_reference<T>::value, "Variant does not support reference types");
    static constexpr size_t value = sizeof(T);
};

//! Template meta-class for computing the maximum size of the types of a Variant
template <typename T, typename... Args> struct MaxSizeOf<T, Args...> {
    static_assert(!std::is_reference<T>::value, "Variant does not support reference types");
    static constexpr size_t value = sizeof(T) > MaxSizeOf<Args...>::value
        ? sizeof(T)
        : MaxSizeOf<Args...>::value;
};

//! Template meta-class for determining whether one of the types of a Variant is
//! of type T
template <typename... Args> struct IsOneOf {
    static constexpr bool value = false;
};

//! Template meta-class for determining whether one of the types of a Variant is
//! of type T
template <typename T, typename S, typename... Args> struct IsOneOf<T, S, Args...> {
    static constexpr bool value = IsOneOf<T, Args...>::value;
};

//! Template meta-class for determining whether one of the types of a Variant is
//! of type T
template <typename T, typename... Args> struct IsOneOf<T, T, Args...> {
    static constexpr bool value = true;
};

//! Template meta-class to destruct a Variant
template <typename... Args> struct DestructStorage;

//! Template meta-class to destruct a Variant
template <typename T, typename... S> struct DestructStorage<T, S...> {
    DestructStorage(const std::type_index& type, void* ptr)
    {
        if (type == typeid(T))
            reinterpret_cast<T*>(ptr)->~T();
        else
            DestructStorage<S...>(type, ptr);
    }
};

//! Template meta-class to destruct a Variant
template <> struct DestructStorage<> {
    DestructStorage(const std::type_index& /*type*/, void* /*ptr*/) {}
};

//! Template meta-class to copy the storage of a Variant
template <typename... Args> struct CopyStorage;

template <typename T, typename... S> struct CopyStorage<T, S...> {
    CopyStorage(void* dst_ptr, const std::type_index& type, const void* src_ptr)
    {
        if (type == typeid(T))
            new (dst_ptr) T(*reinterpret_cast<const T*>(src_ptr));
        else
            CopyStorage<S...>(dst_ptr, type, src_ptr);
    }
};

//! Template meta-class to copy the storage of a Variant
template <> struct CopyStorage<> {
    CopyStorage(void* /*dst_ptr*/, const std::type_index& /*type*/, const void* /*src_ptr*/)
    {
        throw std::runtime_error("CopyStorage: could not match type");
    }
};

//! Template meta-class to move the storage of a Variant
template <typename... Args> struct MoveStorage;

//! Template meta-class to move the storage of a Variant
template <typename T, typename... S> struct MoveStorage<T, S...> {
    MoveStorage(void* dst_ptr, const std::type_index& type, const void* src_ptr)
    {
        if (type == typeid(T)) {
            const T* t_ptr = reinterpret_cast<const T*>(src_ptr);
            new (dst_ptr) T(std::move(*t_ptr));
        } else {
            MoveStorage<S...>(dst_ptr, type, src_ptr);
        }
    }
};

//! Template meta-class to move the storage of a Variant
template <> struct MoveStorage<> {
    MoveStorage(void* /*dst_ptr*/, const std::type_index& /*type*/, const void* /*src_ptr*/)
    {
        throw std::runtime_error("MoveStorage: could not match type");
    }
};

//! Class that implements a variadic template variant class similar as
//! std::variant class defined in C++17
template <typename... Args> class Variant {

public:
    template <typename... OtherArgs> struct is_subset {
        static constexpr bool value = true;
    };

    template <typename T, typename... OtherArgs> struct is_subset<T, OtherArgs...> {
        static constexpr bool value = IsOneOf<T, Args...>::value && is_subset<OtherArgs...>::value;
    };
    Variant() : _typeid(typeid(typename FirstArg<Args...>::type))
    {
        using T = typename FirstArg<Args...>::type;
        new (_storage) T();
    }

    //! Construct by value
    template <typename T> Variant(T&& value) : _typeid(typeid(T))
    {

        using S = typename std::remove_const<typename std::remove_reference<T>::type>::type;
        static_assert(IsOneOf<S, Args...>::value, "");

        _typeid = typeid(S);

        new (_storage) S(std::forward<T>(value));
    }

    //! Copy constructor from another const Variant
    Variant(const Variant& other) : _typeid(other._typeid)
    {
        // Copy the other storage to the current storage
        CopyStorage<Args...>(_storage, _typeid, other._storage);
    }

    //! Copy constructor from another non-const Variant
    Variant(Variant& other) : _typeid(other._typeid)
    {
        // Copy the other storage to the current storage
        CopyStorage<Args...>(_storage, _typeid, other._storage);
    }

    //! Move constructor from another Variant
    Variant(Variant&& other) : _typeid(other._typeid)
    {
        // Move the other storage to the current storage
        MoveStorage<Args...>(_storage, _typeid, other._storage);
    }

    //! Copy constructor from another const subset Variant
    template <typename... OtherArgs>
    Variant(const Variant<OtherArgs...>& other) : _typeid(other._typeid)
    {
        static_assert(is_subset<OtherArgs...>::value, "");
        // Copy the other storage to the current storage
        CopyStorage<OtherArgs...>(_storage, _typeid, other._storage);
    }

    //! Copy constructor from another non-const subset Variant
    template <typename... OtherArgs> Variant(Variant<OtherArgs...>& other) : _typeid(other._typeid)
    {
        static_assert(is_subset<OtherArgs...>::value, "");
        // Copy the other storage to the current storage
        CopyStorage<OtherArgs...>(_storage, _typeid, other._storage);
    }

    //! Move constructor from another subset Variant
    template <typename... OtherArgs> Variant(Variant<OtherArgs...>&& other) : _typeid(other._typeid)
    {
        static_assert(is_subset<OtherArgs...>::value, "");
        // Move the other storage to the current storage
        MoveStorage<OtherArgs...>(_storage, _typeid, other._storage);
    }

    //! Destructor (note: requires use of helper)
    ~Variant() { DestructStorage<Args...>(_typeid, _storage); }

    //! Copy assignment operator from another const Variant
    Variant& operator=(const Variant& other)
    {

        if (this != &other) {
            // Destroy the current storage
            DestructStorage<Args...>(_typeid, _storage);

            // Copy the other storage to the current storage
            _typeid = other._typeid;
            CopyStorage<Args...>(_storage, _typeid, other._storage);
        }

        return *this;
    }

    //! Copy assignment operator from another non-const Variant
    Variant& operator=(Variant& other)
    {

        if (this != &other) {
            // Destroy the current storage
            DestructStorage<Args...>(_typeid, _storage);

            // Copy the other storage to the current storage
            _typeid = other._typeid;
            CopyStorage<Args...>(_storage, _typeid, other._storage);
        }

        return *this;
    }

    //! Move assignment operator from another Variant
    Variant& operator=(Variant&& other)
    {

        if (this != &other) {
            // Destroy the current storage
            DestructStorage<Args...>(_typeid, _storage);

            // Move the other storage to the current storage
            _typeid = other._typeid;
            MoveStorage<Args...>(_storage, _typeid, other._storage);
        }

        return *this;
    }

    //! Copy assignment operator from a subset Variant
    template <typename... OtherArgs> Variant& operator=(const Variant<OtherArgs...>& other)
    {
        static_assert(is_subset<OtherArgs...>::value, "");

        // Destroy the current storage
        DestructStorage<Args...>(_typeid, _storage);

        // Copy the other storage to the current storage
        _typeid = other._typeid;
        CopyStorage<OtherArgs...>(_storage, _typeid, other._storage);

        return *this;
    }

    //! Copy assignment operator from a subset variant
    template <typename... OtherArgs> Variant& operator=(Variant<OtherArgs...>& other)
    {
        static_assert(is_subset<OtherArgs...>::value, "");

        // Destroy the current storage
        DestructStorage<Args...>(_typeid, _storage);

        // Copy the other storage to the current storage
        _typeid = other._typeid;
        CopyStorage<OtherArgs...>(_storage, _typeid, other._storage);

        return *this;
    }

    //! Move assignment operator from a subset variant
    template <typename... OtherArgs> Variant& operator=(Variant<OtherArgs...>&& other)
    {
        static_assert(is_subset<OtherArgs...>::value, "");

        // Destroy the current storage
        DestructStorage<Args...>(_typeid, _storage);

        // Move the other storage to the current storage
        _typeid = other._typeid;
        MoveStorage<OtherArgs...>(_storage, _typeid, other._storage);

        return *this;
    }

    //!  Assignment operator from universal reference
    template <typename T> Variant& operator=(T&& value)
    {

        using S = typename std::remove_const<typename std::remove_reference<T>::type>::type;
        static_assert(IsOneOf<S, Args...>::value, "");

        // Destroy the current storage
        DestructStorage<Args...>(_typeid, _storage);

        // Reallocate the current storage with the new value
        _typeid = typeid(S);
        new (_storage) S(std::forward<T>(value));
    }

    //! Returns true is the variant curren type is of type T
    template <typename T> bool is() const { return (_typeid == typeid(T)); }

    //! Returns const reference to underlying object. Throws if type is wrong
    template <typename T> const T& as() const
    {
        static_assert(IsOneOf<T, Args...>::value, "T must be one of Args");

        if (_typeid == typeid(T))
            return *reinterpret_cast<const T*>(_storage);
        else {
            throw std::runtime_error(
                "error: can not cast Variant from " + std::string(_typeid.name()) + " to "
                + std::string(typeid(T).name()));
        }
    }

    template <typename... OtherArgs> friend class Variant;

private:
    // size needed for stack storage
    static constexpr size_t _bufsize = MaxSizeOf<Args...>::value;

    // internal storage buffer
    char _storage[_bufsize];

    // typeid needed for calling the destructor
    std::type_index _typeid;
};

} // namespace xsection

#endif // XSECTION_LIB_VARIANT_H
