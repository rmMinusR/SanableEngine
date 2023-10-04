#pragma once

#include <cmath>
#include <glm/glm.hpp>

///
/// Template class for three element vectors.
/// From https://github.com/jpmec/vector3
/// Slight modifications by Robert Christensen
///
template<class TObj>
class Vector3
{
public:
    constexpr Vector3();
    constexpr Vector3(const TObj x, const TObj y, const TObj z);
    constexpr Vector3(const Vector3<TObj>& v);
    Vector3(const glm::vec3& v);

    // utility operations
    Vector3<TObj>& zero();
    Vector3<TObj>& set(const TObj x, const TObj y, const TObj z);
    Vector3<TObj>& normalize();

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

    const Vector3<TObj> operator < (const TObj s);            // compare each element with s, return vector of 1 or 0 based on test
    const Vector3<TObj> operator > (const TObj s);

    const Vector3<TObj> operator < (const Vector3<TObj>& v);  // element-wise less than comparion, return vector of 1 or 0 based on test
    const Vector3<TObj> operator > (const Vector3<TObj>& v);  // element-wise greater than comparion, return vector of 1 or 0 based on test

    bool operator == (const Vector3<TObj>& v);             // test vector for equality
    bool operator != (const Vector3<TObj>& v);             // test vector for inequality

    TObj* ptr(){return _v;}                                // return reference to array (use with caution)

    //Getters and setters
    inline float getX() const { return _v[0]; }
	inline float getY() const { return _v[1]; }
	inline float getZ() const { return _v[2]; }
	inline void setX(float s) { _v[0] = s; }
    inline void setY(float s) { _v[1] = s; }
    inline void setZ(float s) { _v[2] = s; }

private:
    TObj _v[3];
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


template <class TObj>
constexpr Vector3<TObj>::Vector3()
{
    _v[0] = 0.0;
    _v[1] = 0.0;
    _v[2] = 0.0;
}


template <class TObj>
constexpr Vector3<TObj>::Vector3(const Vector3<TObj>& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
}

template<class TObj>
Vector3<TObj>::Vector3(const glm::vec3& v)
{
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
}

template <class TObj>
constexpr Vector3<TObj>::Vector3(const TObj x, const TObj y, const TObj z)
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
    TObj n = mgn();
    if(n){
      _v[0]/=n;
      _v[1]/=n;
      _v[2]/=n;
    }
    return *this;
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
inline const Vector3<TObj> Vector3<TObj>::operator < (const TObj s)
{
    return Vector3<TObj>(_v[0]<s, _v[1]<s, _v[2]<s);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator > (const TObj s)
{
    return Vector3<TObj>(_v[0]>s, _v[1]>s, _v[2]>s);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator < (const Vector3<TObj>& v)
{
    return Vector3<TObj>(_v[0]<v[0], _v[1]<v[1], _v[2]<v[2]);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator > (const Vector3<TObj>& v)
{
    return Vector3<TObj>(_v[0]>v[0], _v[1]>v[1], _v[2]>v[2]);
}


template<class TObj>
inline const Vector3<TObj> Vector3<TObj>::operator - ()
{
    return Vector3<TObj>(-_v[0], -_v[1], -_v[2]);
}


template <class TObj>
inline bool Vector3<TObj>::operator == (const Vector3<TObj>& v)
{
    return    _v[0] == v[0]
           && _v[1] == v[1]
           && _v[2] == v[2];
}


template <class TObj>
inline bool Vector3<TObj>::operator != (const Vector3<TObj>& v)
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
inline const Vector3<TObj> operator + (const Vector3<TObj>& v, const TObj& s)
{
    return Vector3<TObj>(v) += s;
}


template <class TObj>
inline const Vector3<TObj> operator - (const Vector3<TObj>& v, const TObj& s)
{
    return Vector3<TObj>(v) -= s;
}


template <class TObj>
inline const Vector3<TObj> operator * (const Vector3<TObj>& v, const TObj& s)
{
    return Vector3<TObj>(v) *= s;
}


template <class TObj>
inline const Vector3<TObj> operator / (const Vector3<TObj>& v, const TObj& s)
{
    return Vector3<TObj>(v) /= s;
}


template <class TObj>
inline const Vector3<TObj> operator + (const Vector3<TObj>& v1, const Vector3<TObj>& v2)
{
    return Vector3<TObj>(v1) += v2;
}


template <class TObj>
inline const Vector3<TObj> operator - (const Vector3<TObj>& v1, const Vector3<TObj>& v2)
{
    return Vector3<TObj>(v1) -= v2;
}


template <class TObj>
inline const TObj operator * (const Vector3<TObj>& v1, const Vector3<TObj>& v2)
{
    return v1.dot(v2);
}


template <class TObj>
inline const Vector3<TObj> operator ^ (const Vector3<TObj>& v1, const Vector3<TObj>& v2)
{
    return v1.cross(v2);
}


template<typename T>
struct Vector3_consts
{
    static constexpr Vector3<T> zero { 0, 0, 0 };
    static constexpr Vector3<T> one  { 1, 1, 1 };
    static constexpr Vector3<T> X { 1, 0, 0 };
    static constexpr Vector3<T> Y { 0, 1, 0 };
    static constexpr Vector3<T> Z { 0, 0, 1 };
};

