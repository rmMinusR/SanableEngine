#pragma once

#include <cmath>

///
/// Template class for three element vectors.
/// From https://github.com/jpmec/vector3
/// Slight modifications by Robert Christensen
///
template<class T>
class Vector3
{
public:
    Vector3();
    Vector3(const T x, const T y, const T z);
    Vector3(const Vector3<T>& v);

    // utility operations
    Vector3<T>& zero();
    Vector3<T>& set(const T x, const T y, const T z);
    Vector3<T>& normalize();

    // math operations
    const T norm() const;
    const T sum() const;
    const T dot(const Vector3<T>&) const;
    const Vector3<T> cross(const Vector3<T>&) const;
    const Vector3<T> abs() const;

    // operators
    Vector3<T>& operator= (const Vector3<T>& v);        // assignment

    const T operator[] (const int i) const;             // indexing
    T& operator[] (const int i);                        // indexing

    const Vector3<T> operator-();                       // unary negate

    Vector3<T>& operator+=(const T s);                  // scalar addition
    Vector3<T>& operator-=(const T s);                  // scalar subtraction
    Vector3<T>& operator*=(const T s);                  // scalar multiplication
    Vector3<T>& operator/=(const T s);                  // scalar division

    Vector3<T>& operator+=(const Vector3<T>& v);        // vector addition
    Vector3<T>& operator-=(const Vector3<T>& v);        // vector subtraction
    Vector3<T>& operator*=(const Vector3<T>& v);        // element-wise multiplication
    Vector3<T>& operator/=(const Vector3<T>& v);        // element-wise division

    const Vector3<T> operator < (const T s);            // compare each element with s, return vector of 1 or 0 based on test
    const Vector3<T> operator > (const T s);

    const Vector3<T> operator < (const Vector3<T>& v);  // element-wise less than comparion, return vector of 1 or 0 based on test
    const Vector3<T> operator > (const Vector3<T>& v);  // element-wise greater than comparion, return vector of 1 or 0 based on test

    bool operator == (const Vector3<T>& v);             // test vector for equality
    bool operator != (const Vector3<T>& v);             // test vector for inequality

    T* ptr(){return _v;}                                // return reference to array (use with caution)

    //Getters and setters
    inline float getX() const { return _v[0]; }
	inline float getY() const { return _v[1]; }
	inline float getZ() const { return _v[2]; }
	inline void setX(float s) { _v[0] = s; }
    inline void setY(float s) { _v[1] = s; }
    inline void setZ(float s) { _v[2] = s; }

private:
    T _v[3];
};


//
// Binary operations
//
/*
inline const Vector3<T> operator+ (const Vector3<T>& v, const T& s);  // scalar operations
inline const Vector3<T> operator- (const Vector3<T>& v, const T& s);
inline const Vector3<T> operator* (const Vector3<T>& v, const T& s);
inline const Vector3<T> operator/ (const Vector3<T>& v, const T& s);

inline const Vector3<T> operator+ (const Vector3<T>& v1, const Vector3<T>& v2);  // element-wise addition
inline const Vector3<T> operator- (const Vector3<T>& v1, const Vector3<T>& v2);  // element-wise subtraction
inline const Vector3<T> operator* (const Vector3<T>& v1, const Vector3<T>& v2);  // dot product
inline const Vector3<T> operator^ (const Vector3<T>& v1, const Vector3<T>& v2);  // cross product
*/

//
// Function definitions
//


template <class T>
Vector3<T>::Vector3()
{
    _v[0] = 0.0;
    _v[1] = 0.0;
    _v[2] = 0.0;
}


template <class T>
Vector3<T>::Vector3(const Vector3<T>& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
}


template <class T>
Vector3<T>::Vector3(const T x, const T y, const T z)
{
    _v[0] = x;
    _v[1] = y;
    _v[2] = z;
}


template <class T>
Vector3<T>& Vector3<T>::zero()
{
    _v[0] = 0.0;
    _v[1] = 0.0;
    _v[2] = 0.0;
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::set(const T x, const T y, const T z)
{
    _v[0] = x;
    _v[1] = y;
    _v[2] = z;
    return *this;
}


template <class T>
inline const T Vector3<T>::operator [] (const int i) const
{
    return _v[i];
}


template <class T>
T& Vector3<T>::operator [] (const int i)
{
    return _v[i];
}


template <class T>
inline const Vector3<T> Vector3<T>::abs() const
{
    return Vector3<T>(std::abs(_v[0]), std::abs(_v[1]), std::abs(_v[2]));
}


template <class T>
inline const T Vector3<T>::sum() const
{
    return _v[0] + _v[1] + _v[2];
}


template <class T>
inline const T Vector3<T>::dot(const Vector3<T>& v) const
{
    return _v[0]*v[0] + _v[1]*v[1] + _v[2]*v[2];
}


template <class T>
inline const Vector3<T> Vector3<T>::cross(const Vector3<T>& v) const
{
    return Vector3<T>(
      (_v[1] * v[2]) - (_v[2] * v[1]),
      (_v[2] * v[0]) - (_v[0] * v[2]),
      (_v[0] * v[1]) - (_v[1] * v[0])
    );
}


template <class T>
inline const T Vector3<T>::norm() const
{
    return (T) sqrt(dot(*this)); // cast to type
}


template <class T>
Vector3<T>& Vector3<T>::normalize()
{
    T n = norm();
    if(n){
      _v[0]/=n;
      _v[1]/=n;
      _v[2]/=n;
    }
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator= (const Vector3<T>& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator += (const Vector3<T>& v)
{
    _v[0] += v[0];
    _v[1] += v[1];
    _v[2] += v[2];
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator += (T v)
{
    _v[0] += v;
    _v[1] += v;
    _v[2] += v;
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator -= (const Vector3<T>& v)
{
    _v[0] -= v[0];
    _v[1] -= v[1];
    _v[2] -= v[2];
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator -= (T v)
{
    _v[0] -= v;
    _v[1] -= v;
    _v[2] -= v;
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator *= (T v)
{
    _v[0] *= v;
    _v[1] *= v;
    _v[2] *= v;
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator *= (const Vector3<T>& v)
{
    _v[0] *= v[0];
    _v[1] *= v[1];
    _v[2] *= v[2];
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator /= (T v)
{
    _v[0] /= v;
    _v[1] /= v;
    _v[2] /= v;
    return *this;
}


template <class T>
Vector3<T>& Vector3<T>::operator /= (const Vector3<T>& v)
{
    _v[0] /= v[0];
    _v[1] /= v[1];
    _v[2] /= v[2];
    return *this;
}


template<class T>
inline const Vector3<T> Vector3<T>::operator < (const T s)
{
    return Vector3<T>(_v[0]<s, _v[1]<s, _v[2]<s);
}


template<class T>
inline const Vector3<T> Vector3<T>::operator > (const T s)
{
    return Vector3<T>(_v[0]>s, _v[1]>s, _v[2]>s);
}


template<class T>
inline const Vector3<T> Vector3<T>::operator < (const Vector3<T>& v)
{
    return Vector3<T>(_v[0]<v[0], _v[1]<v[1], _v[2]<v[2]);
}


template<class T>
inline const Vector3<T> Vector3<T>::operator > (const Vector3<T>& v)
{
    return Vector3<T>(_v[0]>v[0], _v[1]>v[1], _v[2]>v[2]);
}


template<class T>
inline const Vector3<T> Vector3<T>::operator - ()
{
    return Vector3<T>(-_v[0], -_v[1], -_v[2]);
}


template <class T>
inline bool Vector3<T>::operator == (const Vector3<T>& v)
{
    return    _v[0] == v[0]
           && _v[1] == v[1]
           && _v[2] == v[2];
}


template <class T>
inline bool Vector3<T>::operator != (const Vector3<T>& v)
{
    return    _v[0] != v[0]
           || _v[1] != v[1]
           || _v[2] != v[2];
}



template<class T>
inline const Vector3<T> operator && (const Vector3<T>& v1, const Vector3<T>& v2)
{
    return Vector3<T>(v1[0]&&v2[0], v1[1]&&v2[1], v1[2]&&v2[2]);
}


template<class T>
inline const Vector3<T> operator || (const Vector3<T>& v1, const Vector3<T>& v2)
{
    return Vector3<T>(v1[0]||v2[0], v1[1]||v2[1], v1[2]||v2[2]);
}


template <class T>
inline const Vector3<T> operator + (const Vector3<T>& v, const T& s)
{
    return Vector3<T>(v) += s;
}


template <class T>
inline const Vector3<T> operator - (const Vector3<T>& v, const T& s)
{
    return Vector3<T>(v) -= s;
}


template <class T>
inline const Vector3<T> operator * (const Vector3<T>& v, const T& s)
{
    return Vector3<T>(v) *= s;
}


template <class T>
inline const Vector3<T> operator / (const Vector3<T>& v, const T& s)
{
    return Vector3<T>(v) /= s;
}


template <class T>
inline const Vector3<T> operator + (const Vector3<T>& v1, const Vector3<T>& v2)
{
    return Vector3<T>(v1) += v2;
}


template <class T>
inline const Vector3<T> operator - (const Vector3<T>& v1, const Vector3<T>& v2)
{
    return Vector3<T>(v1) -= v2;
}


template <class T>
inline const T operator * (const Vector3<T>& v1, const Vector3<T>& v2)
{
    return v1.dot(v2);
}


template <class T>
inline const Vector3<T> operator ^ (const Vector3<T>& v1, const Vector3<T>& v2)
{
    return v1.cross(v2);
}