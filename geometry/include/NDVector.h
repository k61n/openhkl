#ifndef NSXTOOL_NDVECTOR_H_
#define NSXTOOL_NDVECTOR_H_

#include <iostream>
#include <stdexcept>
#include <cmath>

namespace SX
{

namespace Geometry
{

template <typename T> class NDVector
{
public:
	//! default constructor
	NDVector();

	//! default constructor
	NDVector(size_t dim);

	//! copy constructor
	NDVector(size_t dim, const NDVector<T>&);

	//! constructor from a pointer of type T
	NDVector(size_t dim, const T*);

	//! constructor from a constant value
	NDVector(size_t dim, const T val);

	//! destructor
	~NDVector();

	//! assignment operator
	NDVector<T>& operator=(const NDVector<T>&);

	//! add two ND vectors
	NDVector<T> operator+(const NDVector<T>& other) const;

	//! add a constant vector
	NDVector<T> operator+(const T val) const;

	//! subtract two ND vectors
	NDVector<T> operator-(const NDVector<T>& other) const;

	//! substract a constant vector
	NDVector<T> operator-(const T val) const;

	//! multiply two ND vectors
	NDVector<T> operator*(const NDVector<T>& other) const;

	//! multiply by a constant vector
	NDVector<T> operator*(const T val) const;

	//! divide two ND vectors
	NDVector<T> operator/(const NDVector<T>& other) const;

	//! divide by a constant vector
	NDVector<T> operator/(const T val) const;

	//! inplace addition
	NDVector<T>& operator+=(const NDVector<T>& other);

	//! inplace addition by a constant vector
	NDVector<T>& operator+=(const T val);

	//! inplace substraction
	NDVector<T>& operator-=(const NDVector<T>& other);

	//! inplace substraction by a constant vector
	NDVector<T>& operator-=(const T val);

	//! inplace multiplication by another ND vector
	NDVector<T>& operator*=(const NDVector<T>& other);

	//! inplace multiplication by a constant vector
	NDVector<T>& operator*=(const T val);

	//! inplace division by another ND vector
	NDVector<T>& operator/=(const NDVector<T>& other);

	//! inplace division by a constant vector
	NDVector<T>& operator/=(const T val);

	//! Reset a ND vector from another vector
	void operator()(const NDVector<T>& other);

	//! Reset a ND vector from a pointer of type T
	void operator()(const T* ptr);

	//! constant access to one component of a ND vector
	inline const T& operator[](const unsigned int index) const
	{
		if (index < 0 || index >= _dim)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! access to one component of a ND vector
	inline T& operator[](const unsigned int index)
	{
		if (index < 0 || index >= _dim)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! constant access to one component of a ND vector
	inline const T& operator()(const unsigned int index) const
	{
		if (index < 0 || index >= _dim)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! access to one component of a ND vector
	inline T& operator()(const unsigned int index)
	{
		if (index < 0 || index >= _dim)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! inplace addition by another ND vector
	void add(const NDVector<T>& other);

	//! inplace addition by a pointer of type T
	void add(const T* ptr);

	//! inplace addition by a constant
	void add(const T val);

	//! inplace addition by a scaled vector
	void addtimes(const NDVector<T>& other, T val);

	//! inplace substraction by another ND vector
	void substract(const NDVector<T>& other);

	//! inplace substraction by a pointer of type T
	void substract(const T* ptr);

	//! inplace substraction by a constant
	void substract(const T val);

	//! inplace substraction by a scaled vector
	void substracttimes(const NDVector<T>& other, T val);

	//! inplace normalization
	double normalize();

	//! returns the norm of a ND vector
	double norm() const;

	//! returns the squared norm of a ND vector
	double norm2() const;

	// ND-distance between two points defined as vectors
	double distance(const NDVector<T>&) const;

	//! scalar product between two ND vectors
	inline T scalar_prod(const NDVector<T>& other) const
	{
		T prod;
		for (size_t i=0; i < _dim; ++i)
		{
			prod += _vect[i]*other._vect[i];
		}
		return prod;
	}

	//! scalar product between a ND vectors and a pointer of type T
	inline T scalar_prod(const T* ptr) const
	{
		T prod;
		for (size_t i=0; i < _dim; ++i)
		{
			prod += _vect[i]*ptr[i];
		}
		return prod;
	}

	// send a ND vector to a stream
	void printSelf(std::ostream&) const;

private:
	size_t _dim;
	T* _vect;
};

template<typename T>
NDVector<T>::NDVector() : _dim(3), _vect(new T[3])
{
	for (size_t i=0;i<_dim;++i)
	{
		_vect[i] = 0;
	}
}

template<typename T>
NDVector<T>::NDVector(size_t dim) : _dim(dim), _vect(new T[dim])
{
	for (size_t i=0;i<_dim;++i)
	{
		_vect[i] = 0;
	}
}

template<typename T>
NDVector<T>::NDVector(size_t dim, const NDVector<T>& other) : _dim(dim), _vect(new T[dim])
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i]=other._vect[i];
	}
}

template<typename T>
NDVector<T>::NDVector(size_t dim, const T* ptr) : _dim(dim), _vect(new T[dim])
{
	if (ptr)
    {
		for (size_t i=0; i<_dim; ++i)
		{
			_vect[i] = ptr[i];
		}
    }
}

template<typename T>
NDVector<T>::NDVector(size_t dim, const T val) : _dim(dim), _vect(new T[dim])
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] = val;
	}
}

template<typename T>
NDVector<T>::~NDVector()
{
	delete [] _vect;
}

template<typename T>
NDVector<T>& NDVector<T>::operator=(const NDVector<T>& other)
{
	if (this!=&other)
	{
		for (size_t i=0; i<_dim; ++i)
		{
			_vect[i] = other._vect[i];
		}
	}
	return *this;
}

template<typename T>
NDVector<T> NDVector<T>::operator+(const NDVector<T>& other) const
{
	NDVector<T> out(*this);
	out += other;
	return out;
}

template<typename T>
NDVector<T> NDVector<T>::operator+(const T val) const
{
	NDVector<T> out(*this);
	out += val;
	return out;
}

template<typename T>
NDVector<T> NDVector<T>::operator-(const NDVector<T>& other) const
{
	NDVector<T> out(*this);
	out -= other;
	return out;
}

template<typename T>
NDVector<T> NDVector<T>::operator-(const T val) const
{
	NDVector<T> out(*this);
	out -= val;
	return out;
}

template<typename T>
NDVector<T> NDVector<T>::operator*(const NDVector<T>& other) const
{
	NDVector<T> out(*this);
	out *= other;
	return out;
}

template<typename T>
NDVector<T> NDVector<T>::operator*(const T val) const
{
	NDVector<T> out(*this);
	out *= val;
	return out;
}

template<typename T>
NDVector<T> NDVector<T>::operator/(const NDVector<T>& other) const
{
	NDVector<T> out(*this);
	out /= other;
	return out;
}

template<typename T>
NDVector<T> NDVector<T>::operator/(const T fact) const
{
	NDVector<T> out(*this);
	out /= fact;
	return out;
}

template<typename T>
NDVector<T>& NDVector<T>::operator+=(const NDVector<T>& other)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] += other._vect[i];
	}
	return *this;
}

template<typename T>
NDVector<T>& NDVector<T>::operator+=(const T val)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] += val;
	}
	return *this;
}

template<typename T>
NDVector<T>& NDVector<T>::operator-=(const NDVector<T>& other)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] -= other._vect[i];
	}
	return *this;
}

template<typename T>
NDVector<T>& NDVector<T>::operator-=(const T val)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] -= val;
	}
	return *this;
}

template<typename T>
NDVector<T>& NDVector<T>::operator*=(const NDVector<T>& other)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] *= other._vect[i];
	}
	return *this;
}

template<typename T>
NDVector<T>& NDVector<T>::operator*=(const T fact)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] *= fact;
	}
	return *this;
}


template<typename T>
NDVector<T>& NDVector<T>::operator/=(const NDVector<T>& other)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] /= other._vect[i];
	}
	return *this;
}

template<typename T>
NDVector<T>& NDVector<T>::operator/=(const T fact)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] /= fact;
	}
	return *this;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const NDVector<T>& v)
{
	v.printSelf(os);
	return os;
}

template<typename T>
void NDVector<T>::operator()(const NDVector<T>& other)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] = other._vect[i];
	}
	return;
}

template<typename T>
void NDVector<T>::operator()(const T* ptr)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] = ptr[i];
	}
	return;
}

template<typename T>
void NDVector<T>::add(const NDVector<T>& other)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] += other[i];
	}
}

template<typename T>
void NDVector<T>::add(const T* ptr)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] += ptr[i];
	}
}

template<typename T>
void NDVector<T>::add(const T val)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] += val;
	}
}

template<typename T>
void NDVector<T>::addtimes(const NDVector<T>& other,T val)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] += other._vect[i]*val;
	}
	return;
}

template<typename T>
void NDVector<T>::substract(const NDVector<T>& other)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] -= other[i];
	}
}

template<typename T>
void NDVector<T>::substract(const T* ptr)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] -= ptr[i];
	}
}

template<typename T>
void NDVector<T>::substract(const T val)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] -= val;
	}
}

template<typename T>
void NDVector<T>::substracttimes(const NDVector<T>& other,T fact)
{
	for (size_t i=0; i<_dim; ++i)
	{
		_vect[i] -= other._vect[i]*fact;
	}
	return;
}

template<typename T>
double NDVector<T>::normalize()
{
	const double length(norm());
	this->operator/=(length);
	return length;
}

template<typename T>
double NDVector<T>::norm() const
{
	T norm;
	for (size_t i=0; i<_dim; ++i)
	{
		norm += _vect[i]*_vect[i];
	}
 	return sqrt(static_cast<double>(norm));
}

template<typename T>
double NDVector<T>::norm2() const
{
	T norm;
	for (size_t i=0; i<_dim; ++i)
	{
		norm += _vect[i]*_vect[i];
	}
	return static_cast<double>(norm);
}

template<typename T>
double NDVector<T>::distance(const NDVector<T>& other) const
{
	NDVector<T> dif(*this);
	dif -= other;
	return dif.norm();
}

template<typename T>
void NDVector<T>::printSelf(std::ostream& os) const
{
	os << "[";
	for (size_t i=0; i<_dim; ++i)
	{
		os << _vect[i] << " , ";
	}
	os << "]";
	return;
}

typedef NDVector<double> V1D;
typedef NDVector<double> V2D;
typedef NDVector<double> V3D;

typedef NDVector<int> V1DI;
typedef NDVector<int> V2DI;
typedef NDVector<int> V3DI;
typedef NDVector<float> V1DF;
typedef NDVector<float> V2DF;
typedef NDVector<float> V3DF;
typedef NDVector<double> V1DD;
typedef NDVector<double> V2DD;
typedef NDVector<double> V3DD;

} // Namespace Geometry

} // namespace SX

#endif /*NXSTOOL_NDVECTOR_H_*/
