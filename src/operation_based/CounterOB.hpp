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

#ifndef __COUNTEROB_H__
#define __COUNTEROB_H__

#include <unordered_map>

#include "CrdtHandle.hpp"
#include "CrdtObject.hpp"

/*
* metadata template class for CRDT counter
*/
template<typename T=uint32_t>
class CounterMetadata : CrdtMetaData
{
private:
    uint32_t id;
    T num_increments;
    T num_decrements;
public:
    CounterMetadata(uint32_t id) : CrdtMetaData(CrdtType::CounterOB);
    CounterMetadata(uint32_t id, T num_increments, T num_decrements) : CrdtMetaData(CrdtType::CounterOB);
    ~CounterMetadata();

    const T& getNumIncrements() const;
    const T& getNumDecrements() const;
    void setNumIncrements(T num_increments);
    void setNumDecrements(T num_decrements);
};

/*
* template class for CRDT counter
*/
template<typename T=uint32_t>
class CounterOB : CrdtObject
{
private:
    static uint32_t next_available_id;
    static void initializeCounterOB();
    static uint32_t consumeNextAvailableID();

private:
    uint32_t id;
    T payload;
    T num_increments;
    T num_decrements;
    std::unordered_map<uint32_t, CounterMetadata> external_replica_metadata;
protected:
    const T& query() const;
    bool merge(uint32_t replica_id);
    bool serialize(std::string& buffer);
    bool deserialize(std::string& buffer);
    bool exportDB();
    bool importDB();
public:
    CounterOB<T>();
    CounterOB<T>(T payload);
    CounterOB<T>(const CounterOB<T>& rhs);
    ~CounterOB();

    CounterOB<T> operator+(const CounterOB<T>& rhs);
    CounterOB<T> operator-(const CounterOB<T>& rhs);
    CounterOB<T> operator%(const CounterOB<T>& rhs);
    CounterOB<T> operator^(const CounterOB<T>& rhs);
    bool operator&(const CounterOB<T>& rhs);
    bool operator|(const CounterOB<T>& rhs);
    bool operator~(const CounterOB<T>& rhs);
    void operator=(const CounterOB<T>& rhs);
    bool operator<(const CounterOB<T>& rhs);
    bool operator>(const CounterOB<T>& rhs);
    bool operator<=(const CounterOB<T>& rhs);
    bool operator>=(const CounterOB<T>& rhs);
    void operator++();
    void operator--();
    bool operator==(const CounterOB<T>& rhs);
    bool operator!=(const CounterOB<T>& rhs);
    void operator+=(const CounterOB<T>& rhs);
    void operator-=(const CounterOB<T>& rhs);
};

#endif  // __COUNTEROB_H__
