#ifndef NSXTOOL_NDVECTOR_H_
#define NSXTOOL_NDVECTOR_H_

#include <iostream>
#include <stdexcept>
#include <cmath>

namespace SX
{

namespace Geometry
{

template <typename T, size_t D> class NDVector
{
public:
	//! default constructor
	NDVector();

	//! copy constructor
	NDVector(const NDVector<T,D>&);

	//! constructor from a pointer of type T
	NDVector(const T*);

	//! destructor
	~NDVector();

	//! assignment operator
	NDVector<T,D>& operator=(const NDVector<T,D>&);

	//! add two ND vectors
	NDVector<T,D> operator+(const NDVector<T,D>& other) const;

	//! add a constant vector
	NDVector<T,D> operator+(const T val) const;

	//! subtract two ND vectors
	NDVector<T,D> operator-(const NDVector<T,D>& other) const;

	//! substract a constant vector
	NDVector<T,D> operator-(const T val) const;

	//! multiply two ND vectors
	NDVector<T,D> operator*(const NDVector<T,D>& other) const;

	//! multiply by a constant vector
	NDVector<T,D> operator*(const T val) const;

	//! divide two ND vectors
	NDVector<T,D> operator/(const NDVector<T,D>& other) const;

	//! divide by a constant vector
	NDVector<T,D> operator/(const T val) const;

	//! inplace addition
	NDVector<T,D>& operator+=(const NDVector<T,D>& other);

	//! inplace addition by a constant vector
	NDVector<T,D>& operator+=(const T val);

	//! inplace substraction
	NDVector<T,D>& operator-=(const NDVector<T,D>& other);

	//! inplace substraction by a constant vector
	NDVector<T,D>& operator-=(const T val);

	//! inplace multiplication by another ND vector
	NDVector<T,D>& operator*=(const NDVector<T,D>& other);

	//! inplace multiplication by a constant vector
	NDVector<T,D>& operator*=(const T val);

	//! inplace division by another ND vector
	NDVector<T,D>& operator/=(const NDVector<T,D>& other);

	//! inplace division by a constant vector
	NDVector<T,D>& operator/=(const T val);

	//! Reset a ND vector from another vector
	void operator()(const NDVector<T,D>& other);

	//! Reset a ND vector from a pointer of type T
	void operator()(const T* ptr);

	//! constant access to one component of a ND vector
	inline const T& operator[](const unsigned int index) const
	{
		if (index < 0 || index >= D)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! access to one component of a ND vector
	inline T& operator[](const unsigned int index)
	{
		if (index < 0 || index >= D)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! constant access to one component of a ND vector
	inline const T& operator()(const unsigned int index) const
	{
		if (index < 0 || index >= D)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! access to one component of a ND vector
	inline T& operator()(const unsigned int index)
	{
		if (index < 0 || index >= D)
			throw std::runtime_error("NDVector::operator[] range error");
		return _vect[index];
	}

	//! inplace addition by another ND vector
	void add(const NDVector<T,D>& other);

	//! inplace addition by a pointer of type T
	void add(const T* ptr);

	//! inplace addition by a constant
	void add(const T val);

	//! inplace addition by a scaled vector
	void addtimes(const NDVector<T,D>& other, T val);

	//! inplace substraction by another ND vector
	void substract(const NDVector<T,D>& other);

	//! inplace substraction by a pointer of type T
	void substract(const T* ptr);

	//! inplace substraction by a constant
	void substract(const T val);

	//! inplace substraction by a scaled vector
	void substracttimes(const NDVector<T,D>& other, T val);

	//! inplace normalization
	double normalize();

	//! returns the norm of a ND vector
	double norm() const;

	//! returns the squared norm of a ND vector
	double norm2() const;

	// ND-distance between two points defined as vectors
	double distance(const NDVector<T,D>&) const;

	//! scalar product between two ND vectors
	inline T scalar_prod(const NDVector<T,D>& other) const
	{
		T prod;
		for (int i=0; i < D; ++i)
		{
			prod += _vect[i]*other._vect[i];
		}
		return prod;
	}

	//! scalar product between a ND vectors and a pointer of type T
	inline T scalar_prod(const T* ptr) const
	{
		T prod;
		for (int i=0; i < D; ++i)
		{
			prod += _vect[i]*ptr[i];
		}
		return prod;
	}

	// send a ND vector to a stream
	void printSelf(std::ostream&) const;

private:
	T* _vect;
};

template<typename T, size_t D>
NDVector<T,D>::NDVector() : _vect(new T[D])
{
	for (int i=0;i<D;++i)
	{
		_vect[i] = 0;
	}
}

template<typename T, size_t D>
NDVector<T,D>::NDVector(const NDVector<T,D>& other) : _vect(new T[D])
{
	for (int i=0; i<D; ++i)
	{
		_vect[i]=other._vect[i];
	}
}

template<typename T, size_t D>
NDVector<T,D>::NDVector(const T* ptr) : _vect(new T[D])
{
	if (ptr)
    {
		for (int i=0; i<D; ++i)
		{
			_vect[i] = ptr[i];
		}
    }
}

template<typename T, size_t D>
NDVector<T,D>::~NDVector()
{
	delete [] _vect;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator=(const NDVector<T,D>& other)
{
	if (this!=&other)
	{
		for (int i=0; i<D; ++i)
		{
			_vect[i] = other._vect[i];
		}
	}
	return *this;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator+(const NDVector<T,D>& other) const
{
	NDVector<T,D> out(*this);
	out += other;
	return out;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator+(const T val) const
{
	NDVector<T,D> out(*this);
	out += val;
	return out;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator-(const NDVector<T,D>& other) const
{
	NDVector<T,D> out(*this);
	out -= other;
	return out;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator-(const T val) const
{
	NDVector<T,D> out(*this);
	out -= val;
	return out;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator*(const NDVector<T,D>& other) const
{
	NDVector<T,D> out(*this);
	out *= other;
	return out;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator*(const T val) const
{
	NDVector<T,D> out(*this);
	out *= val;
	return out;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator/(const NDVector<T,D>& other) const
{
	NDVector<T,D> out(*this);
	out /= other;
	return out;
}

template<typename T, size_t D>
NDVector<T,D> NDVector<T,D>::operator/(const T fact) const
{
	NDVector<T,D> out(*this);
	out /= fact;
	return out;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator+=(const NDVector<T,D>& other)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] += other._vect[i];
	}
	return *this;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator+=(const T val)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] += val;
	}
	return *this;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator-=(const NDVector<T,D>& other)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] -= other._vect[i];
	}
	return *this;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator-=(const T val)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] -= val;
	}
	return *this;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator*=(const NDVector<T,D>& other)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] *= other._vect[i];
	}
	return *this;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator*=(const T fact)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] *= fact;
	}
	return *this;
}


template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator/=(const NDVector<T,D>& other)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] /= other._vect[i];
	}
	return *this;
}

template<typename T, size_t D>
NDVector<T,D>& NDVector<T,D>::operator/=(const T fact)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] /= fact;
	}
	return *this;
}




template<typename T, size_t D>
std::ostream& operator<<(std::ostream& os, const NDVector<T,D>& v)
{
	v.printSelf(os);
	return os;
}

template<typename T, size_t D>
void NDVector<T,D>::operator()(const NDVector<T,D>& other)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] = other._vect[i];
	}
	return;
}

template<typename T, size_t D>
void NDVector<T,D>::operator()(const T* ptr)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] = ptr[i];
	}
	return;
}

template<typename T, size_t D>
void NDVector<T,D>::add(const NDVector<T,D>& other)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] += other[i];
	}
}

template<typename T, size_t D>
void NDVector<T,D>::add(const T* ptr)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] += ptr[i];
	}
}

template<typename T, size_t D>
void NDVector<T,D>::add(const T val)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] += val;
	}
}

template<typename T, size_t D>
void NDVector<T,D>::addtimes(const NDVector<T,D>& other,T val)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] += other._vect[i]*val;
	}
	return;
}

template<typename T, size_t D>
void NDVector<T,D>::substract(const NDVector<T,D>& other)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] -= other[i];
	}
}

template<typename T, size_t D>
void NDVector<T,D>::substract(const T* ptr)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] -= ptr[i];
	}
}

template<typename T, size_t D>
void NDVector<T,D>::substract(const T val)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] -= val;
	}
}

template<typename T, size_t D>
void NDVector<T,D>::substracttimes(const NDVector<T,D>& other,T fact)
{
	for (int i=0; i<D; ++i)
	{
		_vect[i] -= other._vect[i]*fact;
	}
	return;
}

template<typename T, size_t D>
double NDVector<T,D>::normalize()
{
	const double length(norm());
	this->operator/=(length);
	return length;
}

template<typename T, size_t D>
double NDVector<T,D>::norm() const
{
	T norm;
	for (int i=0; i<D; ++i)
	{
		norm += _vect[i]*_vect[i];
	}
 	return sqrt(static_cast<double>(norm));
}

template<typename T, size_t D>
double NDVector<T,D>::norm2() const
{
	T norm;
	for (int i=0; i<D; ++i)
	{
		norm += _vect[i]*_vect[i];
	}
	return static_cast<double>(norm);
}

template<typename T, size_t D>
double NDVector<T,D>::distance(const NDVector<T,D>& other) const
{
	NDVector<T,D> dif(*this);
	dif -= other;
	return dif.norm();
}

template<typename T, size_t D>
void NDVector<T,D>::printSelf(std::ostream& os) const
{
	os << "[";
	for (int i=0; i<D; ++i)
	{
		os << _vect[i] << " , ";
	}
	os << "]";
	return;
}

typedef NDVector<double,1> V1D;
typedef NDVector<double,2> V2D;
typedef NDVector<double,3> V3D;

typedef NDVector<int,1> V1DI;
typedef NDVector<int,2> V2DI;
typedef NDVector<int,3> V3DI;
typedef NDVector<float,1> V1DF;
typedef NDVector<float,2> V2DF;
typedef NDVector<float,3> V3DF;
typedef NDVector<double,1> V1DD;
typedef NDVector<double,2> V2DD;
typedef NDVector<double,3> V3DD;

} // Namespace Geometry

} // namespace SX

#endif /*NXSTOOL_NDVECTOR_H_*/
