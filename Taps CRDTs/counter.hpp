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

#ifndef __COUNTER_H__
#define __COUNTER_H__

#include "crdt_object.hpp"

/*
* template class for CRDT counter
*/
template<typename T=int32_t>
class counter : crdt_object
{
private:
    T payload;
    int32_t num_increments;
    int32_t num_decrements;
protected:
    T& query();
public:
    counter<T>();
    counter<T>(T payload);
    counter<T>(const counter<T>& rhs);
    ~counter();

    counter<T> operator+(const counter<T>& rhs);
    counter<T> operator-(const counter<T>& rhs);
    counter<T> operator%(const counter<T>& rhs);
    counter<T> operator^(const counter<T>& rhs);
    bool operator&(const counter<T>& rhs);
    bool operator|(const counter<T>& rhs);
    bool operator~(const counter<T>& rhs);
    void operator=(const counter<T>& rhs);
    bool operator<(const counter<T>& rhs);
    bool operator>(const counter<T>& rhs);
    bool operator<=(const counter<T>& rhs);
    bool operator>=(const counter<T>& rhs);
    void operator++();
    void operator--();
    bool operator==(const counter<T>& rhs);
    bool operator!=(const counter<T>& rhs);
    void operator+=(const counter<T>& rhs);
    void operator-=(const counter<T>& rhs);
};

#endif  // __COUNTER_H__
