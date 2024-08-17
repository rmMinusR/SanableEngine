#pragma once

template<typename T>
struct TemplatedStaticsContainer
{
    static const int val;
};

template<typename T> const int TemplatedStaticsContainer<T>::val = 2;
