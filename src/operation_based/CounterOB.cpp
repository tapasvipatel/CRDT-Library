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

#include "CounterOB.hpp"

namespace crdt
{
namespace operation
{

/*******************************************************************************************
CounterMetadata
*******************************************************************************************/
    /*
template<typename T>
CounterMetadata<T>::CounterMetadata(uint32_t id) : CrdtMetaData(CrdtType::CounterOBType)
{
    this->id = id;
    this->num_increments = 0;
    this->num_decrements = 0;
}

template<typename T>
CounterMetadata<T>::CounterMetadata(uint32_t id, T num_increments, T num_decrements) : CrdtMetaData(CrdtType::CounterOBType)
{
    this->id = id;
    this->num_increments = num_increments;
    this->num_decrements = num_decrements;
}

template<typename T>
CounterMetadata<T>::~CounterMetadata()
{
    ;
}

template<typename T>
const T& CounterMetadata<T>::getNumIncrements() const
{
    return this->num_increments;
}

template<typename T>
const T& CounterMetadata<T>::getNumDecrements() const
{
    return this->num_decrements;
}

template<typename T>
void CounterMetadata<T>::setNumIncrements(T num_increments)
{
    this->num_increments = num_increments;
}

template<typename T>
void CounterMetadata<T>::setNumDecrements(T num_decrements)
{
    this->num_decrements = num_decrements;
}
*/
/*******************************************************************************************
Constructor & Destructor
*******************************************************************************************/
template<typename T>
void CounterOB<T>::initializeCounterOB()
{
    CounterOB::next_available_id = 0;
}
template<typename T>
uint32_t consumeNextAvailableID()
{
    return CounterOB<T>::next_available_id++;
}

/*
template<typename T>
CounterOB<T>::CounterOB()
{
    this->payload = T();
    this->num_increments = 0;
    this->num_decrements = 0;
    this->id = CounterOB::consumeNextAvailableID();
    // add to global list
}

template<typename T>
CounterOB<T>::CounterOB(T payload)
{
    this->payload = payload;
    payload > 0 ? this->num_increments = payload : this->num_decrements = payload;
    this->id = CounterOB::consumeNextAvailableID();
    // add to global list
}

template<typename T>
CounterOB<T>::CounterOB(const CounterOB<T>& rhs)
{
    this->payload = rhs.payload;
    rhs.payload > 0 ? this->num_increments = rhs.payload : this->num_decrements = rhs.payload;
    this->id = CounterOB::consumeNextAvailableID();
    // add to global list
}

template<typename T>
CounterOB<T>::~CounterOB()
{
    // remove from global list
    ;
}
*/

/*******************************************************************************************
Protected Methods
*******************************************************************************************/
//template<typename T>
//const T& CounterOB<T>::query() const
//{
//    return this->payload;
//}

/*******************************************************************************************
Public Methods
*******************************************************************************************/
/*
template<typename T>
CounterOB<T> CounterOB<T>::operator+(const CounterOB<T>& rhs)
{
    return CounterOB<T>(this->payload + rhs.payload);
}

template<typename T>
CounterOB<T> CounterOB<T>::operator-(const CounterOB<T>& rhs)
{
    return CounterOB<T>(this->payload - rhs.payload);
}

/*
* modulo operator is specialized for uint32_t type only
*/
/*
template<>
CounterOB<uint32_t> CounterOB<uint32_t>::operator%(const CounterOB<uint32_t>& rhs)
{
    return CounterOB<uint32_t>(this->payload % rhs.payload);
}

template<typename T>
CounterOB<T> CounterOB<T>::operator^(const CounterOB<T>& rhs)
{
    return CounterOB<T>(this->payload ^ rhs.payload);
}

template<typename T>
bool CounterOB<T>::operator&(const CounterOB<T>& rhs)
{
    return this->payload & rhs.payload;
}

template<typename T>
bool CounterOB<T>::operator|(const CounterOB<T>& rhs)
{
    return this->payload | rhs.payload;
}
    
template<typename T>
void CounterOB<T>::operator=(const CounterOB<T>& rhs)
{
    this->payload = rhs.payload;
    rhs.payload - this->payload > 0 ? this->num_increments += rhs.payload - this->payload : this->num_decrements -= this->payload - rhs.payload;
}

template<typename T>
bool CounterOB<T>::operator<(const CounterOB<T>& rhs)
{
    return this->payload < rhs.payload;
}

template<typename T>
bool CounterOB<T>::operator>(const CounterOB<T>& rhs)
{
    return this->payload > rhs.payload;
}

template<typename T>
bool CounterOB<T>::operator<=(const CounterOB<T>& rhs)
{
    return this->payload <= rhs.payload;
}

template<typename T>
bool CounterOB<T>::operator>=(const CounterOB<T>& rhs)
{
    return this->payload >= rhs.payload;
}

template<typename T>
void CounterOB<T>::operator++()
{
    this->payload++;
    this->num_increments++;
}

template<typename T>
void CounterOB<T>::operator--()
{
    this->payload--;
    this->num_decrements++;
}

template<typename T>
bool CounterOB<T>::operator==(const CounterOB<T>& rhs)
{
    return this->payload == rhs.payload;
}

template<typename T>
bool CounterOB<T>::operator!=(const CounterOB<T>& rhs)
{
    return this->payload != rhs.payload;
}

template<typename T>
void CounterOB<T>::operator+=(const CounterOB<T>& rhs)
{
    this->payload += rhs.payload;
    rhs.payload > 0 ? this->num_increments += rhs.payload : this->num_decrements += rhs.payload;
}

template<typename T>
void CounterOB<T>::operator-=(const CounterOB<T>& rhs)
{
    this->payload -= rhs.payload;
    rhs.payload > 0 ? this->num_decrements += rhs.payload : this->num_increments += rhs.payload;
}
*/

}   // namespace operation
}   // namespace crdt
