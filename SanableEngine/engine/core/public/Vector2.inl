#pragma once

#include <cmath>
#include <glm/glm.hpp>

///
/// Template class for two element vectors
/// Variant of three-element vector from https://github.com/jpmec/vector3
/// Modifications by Robert Christensen
///
template<class TObj>
class Vector2
{
public:
    Vector2();
    Vector2(const TObj x, const TObj y);
    Vector2(const glm::vec2& v);

    // utility operations
    Vector2<TObj>& zero();
    Vector2<TObj>& set(const TObj x, const TObj y);
    Vector2<TObj>& normalize(); //inline version
    Vector2<TObj> normalized() const; //copying version

    // math operations
    const TObj mgn() const;
    const TObj mgnSq() const;
    const TObj sum() const;
    const TObj dot(const Vector2<TObj>&) const;
    const Vector2<TObj> abs() const;

    // operators
    Vector2<TObj>& operator= (const Vector2<TObj>& v);        // assignment
    Vector2<TObj>& operator= (const glm::vec2& v);            // assignment (conversion)
    operator glm::vec2() const;                               // outbound conversion

    const TObj operator[] (const int i) const;             // indexing
    TObj& operator[] (const int i);                        // indexing

    const Vector2<TObj> operator-();                       // unary negate

    Vector2<TObj>& operator+=(const TObj s);                  // scalar addition
    Vector2<TObj>& operator-=(const TObj s);                  // scalar subtraction
    Vector2<TObj>& operator*=(const TObj s);                  // scalar multiplication
    Vector2<TObj>& operator/=(const TObj s);                  // scalar division

    Vector2<TObj>& operator+=(const Vector2<TObj>& v);        // vector addition
    Vector2<TObj>& operator-=(const Vector2<TObj>& v);        // vector subtraction
    Vector2<TObj>& operator*=(const Vector2<TObj>& v);        // element-wise multiplication
    Vector2<TObj>& operator/=(const Vector2<TObj>& v);        // element-wise division

    inline const Vector2<TObj> operator+(const TObj rhs) const { return Vector2<TObj>(*this) += rhs; }
    inline const Vector2<TObj> operator-(const TObj rhs) const { return Vector2<TObj>(*this) -= rhs; }
    inline const Vector2<TObj> operator*(const TObj rhs) const { return Vector2<TObj>(*this) *= rhs; }
    inline const Vector2<TObj> operator/(const TObj rhs) const { return Vector2<TObj>(*this) /= rhs; }

    inline const Vector2<TObj> operator+(const Vector2<TObj>& rhs) const { return Vector2<TObj>(*this) += rhs; }
    inline const Vector2<TObj> operator-(const Vector2<TObj>& rhs) const { return Vector2<TObj>(*this) -= rhs; }
    inline const Vector2<TObj> operator*(const Vector2<TObj>& rhs) const { return Vector2<TObj>(*this) *= rhs; }
    inline const Vector2<TObj> operator/(const Vector2<TObj>& rhs) const { return Vector2<TObj>(*this) /= rhs; }

    const Vector2<TObj> operator < (const TObj s) const;            // compare each element with s, return vector of 1 or 0 based on test
    const Vector2<TObj> operator > (const TObj s) const;

    const Vector2<TObj> operator < (const Vector2<TObj>& v) const;  // element-wise less than comparion, return vector of 1 or 0 based on test
    const Vector2<TObj> operator > (const Vector2<TObj>& v) const;  // element-wise greater than comparion, return vector of 1 or 0 based on test

    bool operator == (const Vector2<TObj>& v) const;             // test vector for equality
    bool operator != (const Vector2<TObj>& v) const;             // test vector for inequality
    
    union
    {
        TObj _v[2];
        struct
        {
            TObj x;
            TObj y;
        };
    };
};

typedef Vector2<float> Vector2f;

//
// Function definitions
//


template <class TObj>
Vector2<TObj>::Vector2()
{
    _v[0] = 0.0;
    _v[1] = 0.0;
}


template<class TObj>
Vector2<TObj>::Vector2(const glm::vec2& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
}

template <class TObj>
Vector2<TObj>::Vector2(const TObj x, const TObj y)
{
    _v[0] = x;
    _v[1] = y;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::zero()
{
    _v[0] = 0.0;
    _v[1] = 0.0;
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::set(const TObj x, const TObj y)
{
    _v[0] = x;
    _v[1] = y;
    return *this;
}


template <class TObj>
inline const TObj Vector2<TObj>::operator [] (const int i) const
{
    return _v[i];
}


template <class TObj>
TObj& Vector2<TObj>::operator [] (const int i)
{
    return _v[i];
}


template <class TObj>
inline const Vector2<TObj> Vector2<TObj>::abs() const
{
    return Vector2<TObj>(std::abs(_v[0]), std::abs(_v[1]));
}


template <class TObj>
inline const TObj Vector2<TObj>::sum() const
{
    return _v[0] + _v[1];
}


template <class TObj>
inline const TObj Vector2<TObj>::dot(const Vector2<TObj>& v) const
{
    return _v[0]*v[0] + _v[1]*v[1];
}


template <class TObj>
inline const TObj Vector2<TObj>::mgnSq() const
{
    return dot(*this);
}

template<class TObj>
inline const TObj Vector2<TObj>::mgn() const
{
    return (TObj) sqrt(mgnSq());
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::normalize()
{
    return *this /= mgn();
}

template<class TObj>
Vector2<TObj> Vector2<TObj>::normalized() const
{
    Vector2<TObj> cpy = *this;
    cpy.normalize();
    return cpy;
}

template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator= (const Vector2<TObj>& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    return *this;
}

template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator= (const glm::vec2& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    return *this;
}

template<class TObj>
Vector2<TObj>::operator glm::vec2() const
{
    glm::vec2 v;
    v[0] = _v[0];
    v[1] = _v[1];
    return v;
}

template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator += (const Vector2<TObj>& v)
{
    _v[0] += v[0];
    _v[1] += v[1];
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator += (TObj v)
{
    _v[0] += v;
    _v[1] += v;
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator -= (const Vector2<TObj>& v)
{
    _v[0] -= v[0];
    _v[1] -= v[1];
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator -= (TObj v)
{
    _v[0] -= v;
    _v[1] -= v;
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator *= (TObj v)
{
    _v[0] *= v;
    _v[1] *= v;
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator *= (const Vector2<TObj>& v)
{
    _v[0] *= v[0];
    _v[1] *= v[1];
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator /= (TObj v)
{
    _v[0] /= v;
    _v[1] /= v;
    return *this;
}


template <class TObj>
Vector2<TObj>& Vector2<TObj>::operator /= (const Vector2<TObj>& v)
{
    _v[0] /= v[0];
    _v[1] /= v[1];
    return *this;
}


template<class TObj>
inline const Vector2<TObj> Vector2<TObj>::operator < (const TObj s) const
{
    return Vector2<TObj>(_v[0]<s, _v[1]<s);
}


template<class TObj>
inline const Vector2<TObj> Vector2<TObj>::operator > (const TObj s) const
{
    return Vector2<TObj>(_v[0]>s, _v[1]>s);
}


template<class TObj>
inline const Vector2<TObj> Vector2<TObj>::operator < (const Vector2<TObj>& v) const
{
    return Vector2<TObj>(_v[0]<v[0], _v[1]<v[1]);
}


template<class TObj>
inline const Vector2<TObj> Vector2<TObj>::operator > (const Vector2<TObj>& v) const
{
    return Vector2<TObj>(_v[0]>v[0], _v[1]>v[1]);
}


template<class TObj>
inline const Vector2<TObj> Vector2<TObj>::operator - ()
{
    return Vector2<TObj>(-_v[0], -_v[1]);
}


template <class TObj>
inline bool Vector2<TObj>::operator == (const Vector2<TObj>& v) const
{
    return    _v[0] == v[0]
           && _v[1] == v[1];
}


template <class TObj>
inline bool Vector2<TObj>::operator != (const Vector2<TObj>& v) const
{
    return    _v[0] != v[0]
           || _v[1] != v[1];
}



template<class TObj>
inline const Vector2<TObj> operator && (const Vector2<TObj>& v1, const Vector2<TObj>& v2)
{
    return Vector2<TObj>(v1[0]&&v2[0], v1[1]&&v2[1]);
}


template<class TObj>
inline const Vector2<TObj> operator || (const Vector2<TObj>& v1, const Vector2<TObj>& v2)
{
    return Vector2<TObj>(v1[0]||v2[0], v1[1]||v2[1]);
}


template<typename T>
struct Vector2_consts
{
    static const Vector2<T> zero;
    static const Vector2<T> one;
    static const Vector2<T> X;
    static const Vector2<T> Y;
};

template<typename T> const Vector2<T> Vector2_consts<T>::zero { 0, 0 };
template<typename T> const Vector2<T> Vector2_consts<T>::one  { 1, 1 };
template<typename T> const Vector2<T> Vector2_consts<T>::X { 1, 0 };
template<typename T> const Vector2<T> Vector2_consts<T>::Y { 0, 1 };
