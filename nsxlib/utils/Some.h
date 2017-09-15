#pragma once

#include <algorithm>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace nsx {

struct Alloc
{
    template<typename D, typename V>
    D *copy(V &&v) { return new D{std::forward<V>(v)}; }

    template<typename D, typename V, typename B>
    B *move(V &&v, B *&p) { B *q = p; p = nullptr; return q; }

    template<typename D>
    void free(D *p) { delete p; }
};

//-----------------------------------------------------------------------------

template<size_t N = 16>
class Store
{
    char space[N];

    template<typename T>
    static constexpr bool
    fits() { return sizeof(typename std::decay<T>::type) <= N; }

public:
    template<typename D, typename V>
    D *copy(V &&v)
    {
        return fits<D>() ? new(space) D{std::forward<V>(v)} :
                           new        D{std::forward<V>(v)};
    }

    template<typename D, typename V, typename B>
    B *move(V &&v, B *&p)
    {
        B *q = fits<D>() ? copy<D>(std::forward<V>(v)) : p;
        p = nullptr;
        return q;
    }

    template<typename D>
    void free(D *p) { fits<D>() ? p->~D() : delete p; }
};

/*!
 * \class Some
 * \brief Class to emulate and extend the boost::any feature
 * Code taken as is from https://codereview.stackexchange.com/questions/48344/yet-another-any-class-implementation-named-some
 * Added some comments when possible given the overall complexity of the class
 * Basically, this is quite close to boost::any, with a number of differences. From my perspective it could be a complete replacement to boost::any, that I like to call some.
 *
 * Major differences:
 *   - Templated on a memory-management object that is roughly what an allocator is for STL containers, yet with a different, custom interface.
 *   - Default "allocator" type provides customizable storage space on stack. Similarly to small string optimization, and without run-time overhead, objects not larger than this space are placed on stack; larger ones on the free store.
 *
 * Minor differences:
 *   - Member-function, reference (not pointer)-based, type-checked/-unchecked data access (casting).
 *   - Simplified type-checked access via dynamic_cast instead of manual typeid checking. Using std::bad_cast instead of a custom exception type.
 *   - Type provided for casting should be already decayed; no references removed.
 *   - Conversion operators.
 *   - Free of const_cast hacks.
 *   - No type() functionality exposed; is<T>() check interface instead. Internally, custom type identification mechanism bypassing typeid and RTTI.
 *   - Move semantics fully supported. No constraints e.g. on reading from const rvalue references or providing non-const rvalue references to temporary objects.
 *   - Using empty base optimization, empty objects are stored with no space overhead on top of the required virtual function table.
*/

template<typename A = Store<>>
class Some : A
{
    using id = size_t;

    template<typename T>
    struct type { static void id() { } };

    template<typename T>
    static id type_id() { return reinterpret_cast<id>(&type<T>::id); }

//-----------------------------------------------------------------------------

    template<typename T>
    using decay = typename std::decay<T>::type;

    template<typename T>
    using none = typename std::enable_if<!std::is_same<Some, T>::value>::type;

//-----------------------------------------------------------------------------

    struct base
    {
        virtual ~base() { }
        virtual bool is(id) const = 0;
        virtual base *copy(A&) const = 0;
        virtual base *move(A&, base*&) = 0;
        virtual void free(A&) = 0;
    } *p = nullptr;

//-----------------------------------------------------------------------------

    template<typename T>
    struct data : base, std::tuple<T>
    {
        using std::tuple<T>::tuple;

        T       &get()      & { return std::get<0>(*this); }
        T const &get() const& { return std::get<0>(*this); }

        bool is(id i) const override { return i == type_id<T>(); }

        base *copy(A &a) const override
        {
            return a.template copy<data>(get());
        }

        base *move(A &a, base *&p) override
        {
            return a.template move<data>(std::move(get()), p);
        }

        void free(A &a) override { a.free(this); }
    };

//-----------------------------------------------------------------------------

    //! Return a static cast of the Some object to type T
    template<typename T>
    T &stat() { return static_cast<data<T>*>(p)->get(); }

    //! Return a const static cast of the Some object to type T
    template<typename T>
    T const &stat() const { return static_cast<data<T> const*>(p)->get(); }

    //! Return a dynamic cast of the Some object to type T
    template<typename T>
    T &dyn() { return dynamic_cast<data<T>&>(*p).get(); }

    //! Return a const dynamic cast of the Some object to type T
    template<typename T>
    T const &dyn() const { return dynamic_cast<data<T> const&>(*p).get(); }

    base* move(Some &s)       { return s.p->move(*this, s.p); }
    base* copy(Some const &s) { return s.p->copy(*this); }

    base* read(Some &&s)      { return s.p ? move(s) : s.p; }
    base* read(Some const &s) { return s.p ? copy(s) : s.p; }

    template<typename V, typename U = decay<V>, typename = none<U>>
    base* read(V &&v) { return A::template copy<data<U>>(std::forward<V>(v)); }

    template<typename X>
    Some& assign(X&& x)
    {
        if (!p) {
            p = read(std::forward<X>(x));
        } else {
            Some t{std::move(*this)};
            try        { p = read(std::forward<X>(x)); }
            catch(...) { p = move(t); throw; }
        }
        return *this;
    }

    //! Swap this Some object with another
    void swap(Some &s)
    {
        if (!p) {
            p = read(std::move(s));
        } else if (!s.p) {
            s.p = move(*this);
        } else {
            Some t{std::move(*this)};
            try {
                p = move(s);
            } catch(...) {
                p = move(t); throw;
            }
            s.p = move(t);
        }
    }

public:

    //! Default constructor
    Some() {}

    //! Copy constructor
    Some(const Some& s) : p{read(s)} { }

    //! Move constructor
    Some(Some&& s)      : p{read(std::move(s))} { }

    //! Destructor
    ~Some() { if (p) p->free(*this); }

    //! Type-conversion constructor
    template<typename V, typename = none<decay<V>>>
    Some(V &&v) : p{read(std::forward<V>(v))} { }

    //! Copy assignment operator
    Some &operator=(const Some& s) { return assign(s); }

    //! Move assignment operator
    Some &operator=(Some&& s)      { return assign(std::move(s)); }

    //! Move type-conversion operator
    template<typename V, typename = none<decay<V>>>
    Some& operator=(V&& v) { return assign(std::forward<V>(v)); }

    //! Swap two Some objects
    friend void swap(Some &s, Some &r) { s.swap(r); }

    //! Clear the actual typed-object stored inside Some
    void clear() { if(p) { p->free(*this); p = nullptr; } }

    //! Check whether the Some object has been assigned to a typed-object
    bool empty() const { return p == nullptr; }

    //! Return true if the Some object store an object of type T
    template<typename T>
    bool is() const { return p ? p->is(type_id<T>()) : false; }

    //! Cast the Some object to type T if this is an rvalue
    template<typename T> T&&      cast()     && { return std::move(dyn<T>()); }
    //! Cast the Some object to type T if this is an lvalue
    template<typename T> T&       cast()      & { return dyn<T>(); }
    //! Cast the Some object to type T
    template<typename T> const T& cast() const& { return dyn<T>(); }

    //! Cast operator to type T if this is an rvalue
    template<typename T> operator T     &&()     && { return std::move(cast<T>()); }
    //! Cast operator to type T if this is an lvalue
    template<typename T> operator T      &()      & { return cast<T>(); }
    //! Cast operator to type T
    template<typename T> operator T const&() const& { return cast<T>(); }
};

} // end namespace nsx
