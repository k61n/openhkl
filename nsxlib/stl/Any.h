#pragma once

#include <stdexcept>
#include <type_traits>
#include <typeindex>

namespace nsx {

class Any {

private:

    struct Base {

        virtual Base* clone()=0;

        virtual ~Base(){}        
    };

    template<typename U>
    struct Variant : public Base {

        Variant(const U& value) : Base() {
            _value = new U{value};
        }

        Variant(U& value) : Base() {
            _value = new U{value};
        }

        Variant(U&& value) : Base() {
            _value = new U{std::forward<U>(value)};
        }

        U& get() {
            return *(reinterpret_cast<U*>(_value));
        }

        virtual Base* clone() override {
            return new Variant(get());
        }

        virtual ~Variant() {
            U* uptr = reinterpret_cast<U*>(_value);
            uptr->~U();
            delete uptr;
        }

        void* _value;

    };

public:

    Any() : _type_index(typeid(int)), _immutability(false) {
        _variant = new Variant<int>(0);
    }

    Any(const Any& other) : _type_index(other._type_index) {
        _variant = other._variant->clone();
    }

    Any(Any& other) : _type_index(other._type_index) {
        _variant = other._variant->clone();
    }

    Any(Any&& other) : _type_index(other._type_index) {
        _variant = other._variant;
        other._variant = nullptr;
    }

    template <typename T>
    Any(const T& value, bool immutability=false) : _type_index(typeid(T)), _immutability(immutability) {
        _variant = new Variant<T>(value);
    }

    template <typename T>
    Any(T& value, bool immutability=false) : _type_index(typeid(T)), _immutability(immutability) {
        _variant = new Variant<T>(value);
    }

    template<typename T> 
    Any(T&& value, bool immutability=false) : _type_index(typeid(T)), _immutability(immutability) {
        _variant = new Variant<T>(std::forward<T>(value));
    }

    Any& operator=(const Any& other) {
        if (_immutability && (_type_index != other._type_index)) {
            throw std::runtime_error("This Any object is type immutable.");
        }
        delete _variant;
        _type_index = other._type_index;
        _variant = other._variant->clone();
        return *this;
    }

    Any& operator=(Any& other) {
        if (_immutability && (_type_index != other._type_index)) {
            throw std::runtime_error("This Any object is type immutable.");
        }
        delete _variant;
        _type_index = other._type_index;
        _variant = other._variant->clone();
        return *this;
    }

    Any& operator=(Any&& other) {
        if (_immutability && (_type_index != other._type_index)) {
            throw std::runtime_error("This Any object is type immutable.");
        }
        delete _variant;
        _type_index = other._type_index;
        _variant = other._variant;
        other._variant = nullptr;
        return *this;
    }

    template <typename T>
    Any& operator=(const T& value) {
        if (_immutability && (_type_index != typeid(T))) {
            throw std::runtime_error("This Any object is type immutable.");
        }
        delete _variant;
        _variant = new Variant<T>(value);
        return *this;
    }

    template <typename T>
    Any& operator=(T& value) {
        if (_immutability && (_type_index != typeid(T))) {
            throw std::runtime_error("This Any object is type immutable.");
        }
        delete _variant;
        _variant = new Variant<T>(value);
        return *this;
    }

    template <typename T>
    Any& operator=(T&& value) {
        if (_immutability && (_type_index != typeid(T))) {
            throw std::runtime_error("This Any object is type immutable.");
        }
        delete _variant;
        _type_index = typeid(T);
        _variant = new Variant<T>(std::forward<T>(value));
        return *this;
    }    

    template <typename V>
    V& as() {
        if (_type_index != typeid(V)) {
            throw std::runtime_error("Invalid type");
        }
        return dynamic_cast<Variant<V>*>(_variant)->get();
    }    

    template <typename V>
    V as() const {
        if (_type_index != typeid(V)) {
            throw std::runtime_error("Invalid type");
        }
        return dynamic_cast<Variant<V>*>(_variant)->get();
    }


    ~Any(){
        delete _variant;
    }

    const std::type_index type() {return _type_index;}

    void setImmutability(bool immutability) {_immutability = immutability;}

private:

    std::type_index _type_index;

    Base* _variant;

    bool _immutability=false;
};

} // end namespace nsx
