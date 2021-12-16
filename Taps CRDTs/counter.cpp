/*
    Copyright (c) 2021 [Tapasvi Patel, Rushab Roihan, Vishwa Gandhi, Winston Sun]

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "counter.hpp"

namespace operation
{

template<typename T>
counter<T>::counter()
{
    this->payload = T();
    this->num_increments = 0;
    this->num_decrements = 0;
}

template<typename T>
counter<T>::counter(T payload)
{
    this->payload = payload;
    payload > 0 ? this->num_increments = payload : this->num_decrements = payload;
}

template<typename T>
counter<T>::counter(const counter<T>& rhs)
{
    this->payload = rhs.payload;
    rhs.payload > 0 ? this->num_increments = rhs.payload : this->num_decrements = rhs.payload;
}

template<typename T>
counter<T>::~counter()
{
    ;
}

template<typename T>
T counter<T>::query()
{
    return this->payload;
}

template<typename T>
void counter<T>::increment()
{
    this->payload++;
    this->num_increments++;
}

template<typename T>
void counter<T>::decrement()
{
    this->payload--;
    this->num_decrements++;
}

template<typename T>
counter<T> counter<T>::operator+(const counter<T>& rhs)
{
    return counter<T>(this->payload + rhs.payload);
}

template<typename T>
counter<T> counter<T>::operator-(const counter<T>& rhs)
{
    return counter<T>(this->payload - rhs.payload);
}

/*
* modulo operator is specialized for int32_t type only
*/
template<>
counter<int32_t> counter<int32_t>::operator%(const counter<T>& rhs)
{
    return counter<int32_t>(this->payload % rhs.payload);
}

template<typename T>
counter<T> counter<T>::operator^(const counter<T>& rhs)
{
    return counter<T>(this->payload ^ rhs.payload);
}

template<typename T>
bool counter<T>::operator&(const counter<T>& rhs)
{
    return this->payload & rhs.payload;
}

template<typename T>
bool counter<T>::operator|(const counter<T>& rhs)
{
    return this->payload | rhs.payload;
}

template<typename T>
bool counter<T>::operator~(const counter<T>& rhs)
{
    return this->payload ~ rhs.payload;
}
    
template<typename T>
void counter<T>::operator=(const counter<T>& rhs)
{
    this->payload = rhs.payload;
    rhs.payload - this->payload > 0 ? this->num_increments += rhs.payload - this->payload : this->num_decrements -= this->payload - rhs.payload;
}

template<typename T>
bool counter<T>::operator<(const counter<T>& rhs)
{
    return this->payload < rhs.payload;
}

template<typename T>
bool counter<T>::operator>(const counter<T>& rhs)
{
    return this->payload > rhs.payload;
}

template<typename T>
bool counter<T>::operator<=(const counter<T>& rhs)
{
    return this->payload <= rhs.payload;
}

template<typename T>
bool counter<T>::operator>=(const counter<T>& rhs)
{
    return this->payload >= rhs.payload;
}

template<typename T>
void counter<T>::operator++()
{
    this->increment();
}

template<typename T>
void counter<T>::operator--()
{
    this->decrement();
}

template<typename T>
bool counter<T>::operator==(const counter<T>& rhs)
{
    return this->payload == rhs.payload;
}

template<typename T>
bool counter<T>::operator!=(const counter<T>& rhs)
{
    return this->payload != rhs.payload;
}

template<typename T>
void counter<T>::operator+=(const counter<T>& rhs)
{
    this->payload += rhs.payload;
    rhs.payload > 0 ? this->num_increments += rhs.payload : this->num_decrements += rhs.payload;
}

template<typename T>
void counter<T>::operator-=(const counter<T>& rhs)
{
    this->payload -= rhs.payload;
    rhs.payload > 0 ? this->num_decrements += rhs.payload : this->num_increments += rhs.payload;
}

}
