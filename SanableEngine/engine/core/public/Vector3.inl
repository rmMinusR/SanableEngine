#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "Vector2.inl"

///
/// Template class for three element vectors.
/// From https://github.com/jpmec/vector3
/// Slight modifications by Robert Christensen
///
template<class TObj>
class Vector3
{
public:
    Vector3();
    Vector3(const TObj x, const TObj y, const TObj z);
    Vector3(const Vector3<TObj>& v);
    Vector3(const Vector2<TObj>& xy, const TObj z);
    Vector3(const TObj x, const Vector2<TObj>& yz);
    Vector3(const glm::vec3& v);

    // utility operations
    Vector3<TObj>& zero();
    Vector3<TObj>& set(const TObj x, const TObj y, const TObj z);
    Vector3<TObj>& normalize(); //inline version
    Vector3<TObj> normalized() const; //copying version

    // math operations
    const TObj mgn() const;
    const TObj mgnSq() const;
    const TObj sum() const;
    const TObj dot(const Vector3<TObj>&) const;
    const Vector3<TObj> cross(const Vector3<TObj>&) const;
    const Vector3<TObj> abs() const;

    // operators
    Vector3<TObj>& operator= (const Vector3<TObj>& v);        // assignment
    Vector3<TObj>& operator= (const glm::vec3& v);            // assignment (conversion)
    operator glm::vec3() const;                               // outbound conversion

    //slicing and swizzling
#define DEF_SWIZZLE(field0, field1) inline const Vector2<TObj> field0##field1() const { return Vector2<TObj>(field0, field1); }
    DEF_SWIZZLE(x, y) DEF_SWIZZLE(y, x)
    DEF_SWIZZLE(y, z) DEF_SWIZZLE(z, y)
    DEF_SWIZZLE(z, x) DEF_SWIZZLE(x, z)
#undef DEF_SWIZZLE

    const TObj operator[] (const int i) const;             // indexing
    TObj& operator[] (const int i);                        // indexing

    const Vector3<TObj> operator-();                       // unary negate

    Vector3<TObj>& operator+=(const TObj s);                  // scalar addition
    Vector3<TObj>& operator-=(const TObj s);                  // scalar subtraction
    Vector3<TObj>& operator*=(const TObj s);                  // scalar multiplication
    Vector3<TObj>& operator/=(const TObj s);                  // scalar division

    Vector3<TObj>& operator+=(const Vector3<TObj>& v);        // vector addition
    Vector3<TObj>& operator-=(const Vector3<TObj>& v);        // vector subtraction
    Vector3<TObj>& operator*=(const Vector3<TObj>& v);        // element-wise multiplication
    Vector3<TObj>& operator/=(const Vector3<TObj>& v);        // element-wise division

    inline const Vector3<TObj> operator+(const TObj rhs) const { return Vector3<TObj>(*this) += rhs; }
    inline const Vector3<TObj> operator-(const TObj rhs) const { return Vector3<TObj>(*this) -= rhs; }
    inline const Vector3<TObj> operator*(const TObj rhs) const { return Vector3<TObj>(*this) *= rhs; }
    inline const Vector3<TObj> operator/(const TObj rhs) const { return Vector3<TObj>(*this) /= rhs; }

    inline const Vector3<TObj> operator+(const Vector3<TObj>& rhs) const { return Vector3<TObj>(*this) += rhs; }
    inline const Vector3<TObj> operator-(const Vector3<TObj>& rhs) const { return Vector3<TObj>(*this) -= rhs; }
    inline const Vector3<TObj> operator*(const Vector3<TObj>& rhs) const { return Vector3<TObj>(*this) *= rhs; }
    inline const Vector3<TObj> operator/(const Vector3<TObj>& rhs) const { return Vector3<TObj>(*this) /= rhs; }

    const Vector3<TObj> operator < (const TObj s) const;            // compare each element with s, return vector of 1 or 0 based on test
    const Vector3<TObj> operator > (const TObj s) const;

    const Vector3<TObj> operator < (const Vector3<TObj>& v) const;  // element-wise less than comparion, return vector of 1 or 0 based on test
    const Vector3<TObj> operator > (const Vector3<TObj>& v) const;  // element-wise greater than comparion, return vector of 1 or 0 based on test

    bool operator == (const Vector3<TObj>& v) const;             // test vector for equality
    bool operator != (const Vector3<TObj>& v) const;             // test vector for inequality
    
    union
    {
        TObj _v[3];
        struct
        {
            TObj x;
            TObj y;
            TObj z;
        };
    };
};

typedef Vector3<float> Vector3f;

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


template <class TObj>
Vector3<TObj>::Vector3()
{
    _v[0] = 0.0;
    _v[1] = 0.0;
    _v[2] = 0.0;
}


template <class TObj>
Vector3<TObj>::Vector3(const Vector3<TObj>& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
}

template<class TObj>
Vector3<TObj>::Vector3(const Vector2<TObj>& xy, const TObj z)
{
    x = xy.x;
    y = xy.y;
    z = z;
}

template<class TObj>
Vector3<TObj>::Vector3(const TObj x, const Vector2<TObj>& yz)
{
    x = x;
    y = yz._v[0];
    z = yz._v[1];
}

template<class TObj>
Vector3<TObj>::Vector3(const glm::vec3& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
}

template <class TObj>
Vector3<TObj>::Vector3(const TObj x, const TObj y, const TObj z)
{
    _v[0] = x;
    _v[1] = y;
    _v[2] = z;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::zero()
{
    _v[0] = 0.0;
    _v[1] = 0.0;
    _v[2] = 0.0;
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::set(const TObj x, const TObj y, const TObj z)
{
    _v[0] = x;
    _v[1] = y;
    _v[2] = z;
    return *this;
}


template <class TObj>
inline const TObj Vector3<TObj>::operator [] (const int i) const
{
    return _v[i];
}


template <class TObj>
TObj& Vector3<TObj>::operator [] (const int i)
{
    return _v[i];
}


template <class TObj>
inline const Vector3<TObj> Vector3<TObj>::abs() const
{
    return Vector3<TObj>(std::abs(_v[0]), std::abs(_v[1]), std::abs(_v[2]));
}


template <class TObj>
inline const TObj Vector3<TObj>::sum() const
{
    return _v[0] + _v[1] + _v[2];
}


template <class TObj>
inline const TObj Vector3<TObj>::dot(const Vector3<TObj>& v) const
{
    return _v[0]*v[0] + _v[1]*v[1] + _v[2]*v[2];
}


template <class TObj>
inline const Vector3<TObj> Vector3<TObj>::cross(const Vector3<TObj>& v) const
{
    return Vector3<TObj>(
      (_v[1] * v[2]) - (_v[2] * v[1]),
      (_v[2] * v[0]) - (_v[0] * v[2]),
      (_v[0] * v[1]) - (_v[1] * v[0])
    );
}


template <class TObj>
inline const TObj Vector3<TObj>::mgnSq() const
{
    return dot(*this);
}

template<class TObj>
inline const TObj Vector3<TObj>::mgn() const
{
    return (TObj) sqrt(mgnSq());
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::normalize()
{
    return *this /= mgn();
}

template<class TObj>
Vector3<TObj> Vector3<TObj>::normalized() const
{
    Vector3<TObj> cpy = *this;
    cpy.normalize();
    return cpy;
}

template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator= (const Vector3<TObj>& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
    return *this;
}

template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator= (const glm::vec3& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
    return *this;
}

template<class TObj>
Vector3<TObj>::operator glm::vec3() const
{
    glm::vec3 v;
    v[0] = _v[0];
    v[1] = _v[1];
    v[2] = _v[2];
    return v;
}

template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator += (const Vector3<TObj>& v)
{
    _v[0] += v[0];
    _v[1] += v[1];
    _v[2] += v[2];
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator += (TObj v)
{
    _v[0] += v;
    _v[1] += v;
    _v[2] += v;
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator -= (const Vector3<TObj>& v)
{
    _v[0] -= v[0];
    _v[1] -= v[1];
    _v[2] -= v[2];
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator -= (TObj v)
{
    _v[0] -= v;
    _v[1] -= v;
    _v[2] -= v;
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator *= (TObj v)
{
    _v[0] *= v;
    _v[1] *= v;
    _v[2] *= v;
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator *= (const Vector3<TObj>& v)
{
    _v[0] *= v[0];
    _v[1] *= v[1];
    _v[2] *= v[2];
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator /= (TObj v)
{
    _v[0] /= v;
    _v[1] /= v;
    _v[2] /= v;
    return *this;
}


template <class TObj>
Vector3<TObj>& Vector3<TObj>::operator /= (const Vector3<TObj>& v)
{
    _v[0] /= v[0];
    _v[1] /= v[1];
    _v[2] /= v[2];
    return *this;
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator < (const TObj s) const
{
    return Vector3<TObj>(_v[0]<s, _v[1]<s, _v[2]<s);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator > (const TObj s) const
{
    return Vector3<TObj>(_v[0]>s, _v[1]>s, _v[2]>s);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator < (const Vector3<TObj>& v) const
{
    return Vector3<TObj>(_v[0]<v[0], _v[1]<v[1], _v[2]<v[2]);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator > (const Vector3<TObj>& v) const
{
    return Vector3<TObj>(_v[0]>v[0], _v[1]>v[1], _v[2]>v[2]);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator - ()
{
    return Vector3<TObj>(-_v[0], -_v[1], -_v[2]);
}


template <class TObj>
inline bool Vector3<TObj>::operator == (const Vector3<TObj>& v) const
{
    return    _v[0] == v[0]
           && _v[1] == v[1]
           && _v[2] == v[2];
}


template <class TObj>
inline bool Vector3<TObj>::operator != (const Vector3<TObj>& v) const
{
    return    _v[0] != v[0]
           || _v[1] != v[1]
           || _v[2] != v[2];
}



template<class TObj>
inline const Vector3<TObj> operator && (const Vector3<TObj>& v1, const Vector3<TObj>& v2)
{
    return Vector3<TObj>(v1[0]&&v2[0], v1[1]&&v2[1], v1[2]&&v2[2]);
}


template<class TObj>
inline const Vector3<TObj> operator || (const Vector3<TObj>& v1, const Vector3<TObj>& v2)
{
    return Vector3<TObj>(v1[0]||v2[0], v1[1]||v2[1], v1[2]||v2[2]);
}


template <class TObj>
inline const Vector3<TObj> operator ^ (const Vector3<TObj>& v1, const Vector3<TObj>& v2)
{
    return v1.cross(v2);
}


template<typename T>
struct Vector3_consts
{
    static const Vector3<T> zero;
    static const Vector3<T> one;
    static const Vector3<T> X;
    static const Vector3<T> Y;
    static const Vector3<T> Z;
};

template<typename T> const Vector3<T> Vector3_consts<T>::zero { 0, 0, 0 };
template<typename T> const Vector3<T> Vector3_consts<T>::one  { 1, 1, 1 };
template<typename T> const Vector3<T> Vector3_consts<T>::X { 1, 0, 0 };
template<typename T> const Vector3<T> Vector3_consts<T>::Y { 0, 1, 0 };
template<typename T> const Vector3<T> Vector3_consts<T>::Z { 0, 0, 1 };
