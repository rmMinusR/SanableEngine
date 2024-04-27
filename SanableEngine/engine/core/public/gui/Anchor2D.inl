#pragma once

#include "math/Vector2.inl"

template<typename T>
struct Anchor2D
{
	Vector2<T> ratio;
	Vector2<T> offset;

	inline Anchor2D();
	inline Anchor2D(Vector2<T> ratio);
	inline Anchor2D(Vector2<T> ratio, Vector2<T> offset);

	inline Vector2<T> calcAnchor(Vector2<T> ratioRef) const;
	inline Vector2<T> calcPivot(Vector2<T> ratioRef) const;
	inline void setRatioKeepingPosition(Vector2<T> newVal, Vector2<T> ratioRef);
	inline void setOffsetKeepingPosition(Vector2<T> newVal, Vector2<T> ratioRef);

	inline static Anchor2D<T> centered();
	inline static Anchor2D<T> topLeft();
};


template<typename T>
inline Anchor2D<T>::Anchor2D() :
	Anchor2D(Vector2_consts<float>::zero, Vector2_consts<float>::zero)
{
}

template<typename T>
Anchor2D<T>::Anchor2D(Vector2<T> ratio) :
	Anchor2D(ratio, Vector2_consts<float>::zero)
{
}

template<typename T>
Anchor2D<T>::Anchor2D(Vector2<T> ratio, Vector2<T> offset) :
	ratio(ratio),
	offset(offset)
{
}

template<typename T>
Vector2<T> Anchor2D<T>::calcAnchor(Vector2<T> ratioRef) const
{
	return ratioRef*ratio + offset;
}

template<typename T>
Vector2<T> Anchor2D<T>::calcPivot(Vector2<T> ratioRef) const
{
	return -calcAnchor(ratioRef);
}

template<typename T>
void Anchor2D<T>::setRatioKeepingPosition(Vector2<T> newVal, Vector2<T> ratioRef)
{
	Vector2<T> curPos = calcAnchor(ratioRef);
	ratio = newVal;
	offset = curPos - ratioRef*ratio;
}

template<typename T>
void Anchor2D<T>::setOffsetKeepingPosition(Vector2<T> newVal, Vector2<T> ratioRef)
{
	Vector2<T> curPos = calcAnchor(ratioRef);
	offset = newVal;
	ratio = (curPos-offset) / ratioRef;
}

template<typename T>
Anchor2D<T> Anchor2D<T>::centered()
{
	return Anchor2D(Vector2<T>(0.5f, 0.5f), Vector2<T>(0, 0));
}

template<typename T>
Anchor2D<T> Anchor2D<T>::topLeft()
{
	return Anchor2D<T>(Vector2<T>(0, 0), Vector2<T>(0, 0));
}
