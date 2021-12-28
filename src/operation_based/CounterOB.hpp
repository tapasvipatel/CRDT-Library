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
#include <vector>

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"

namespace crdt
{
namespace operation
{

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
    CounterMetadata(uint32_t id) : CrdtMetaData(CrdtType::CounterOBType)
    {
        this->id = id;
        this->num_increments = 0;
        this->num_decrements = 0;
    }

    CounterMetadata(uint32_t id, T num_increments, T num_decrements) : CrdtMetaData(CrdtType::CounterOBType)
    {
        this->id = id;
        this->num_increments = num_increments;
        this->num_decrements = num_decrements;
    }

    ~CounterMetadata()
    {
        ;
    }

    const T& getNumIncrements() const
    {
        return this->num_increments;
    }

    const T& getNumDecrements() const
    {
        return this->num_decrements;
    }

    void setNumIncrements(T num_increments)
    {
        this->num_increments = num_increments;
    }

    void setNumDecrements(T num_decrements)
    {
        this->num_decrements = num_decrements;
    }
};

/*
* template class for CRDT counter
*/
template<typename T=uint32_t>
class CounterOB : CrdtObject<T>
{
private:
    uint32_t id;
    T payload;
    T num_increments;
    T num_decrements;
    std::unordered_map<uint32_t, CounterMetadata<T>> replica_metadata;

protected:
    bool merge(std::vector<uint32_t> replica_ids)
    {
        return false;
    }

    bool serialize(std::string& buffer)
    {
        return false;
    }

    bool deserialize(std::string& buffer)
    {
        return false;
    }

    bool exportDB()
    {
        return false;
    }

    bool importDB()
    {
        return false;
    }

public:
    CounterOB()
    {
        this->payload = T();
        this->num_increments = 0;
        this->num_decrements = 0;
        this->id = 1;   // TODO: fix logic
        // TODO: add to global list
    }

    CounterOB(T payload)
    {
        this->payload = payload;
        payload > 0 ? this->num_increments = payload : this->num_decrements = payload;
        this->id = 1;   // TODO: fix logic
        // TODO: add to global list
    }

    CounterOB(const CounterOB<T>& rhs)
    {
        this->payload = rhs.payload;
        rhs.payload > 0 ? this->num_increments = rhs.payload : this->num_decrements = rhs.payload;
        this->id = 1;   // TODO: fix logic
        // TODO: add to global list
    }

    ~CounterOB()
    {
        // TODO: remove from global list
        ;  
    }

    CounterOB<T> operator+(const CounterOB<T>& rhs)
    {
        return CounterOB<T>(this->payload + rhs.payload);
    }

    CounterOB<T> operator-(const CounterOB<T>& rhs)
    {
        return CounterOB<T>(this->payload - rhs.payload);
    }

    template<uint32_t>
    CounterOB<uint32_t> operator%(const CounterOB<uint32_t>& rhs)
    {
        return CounterOB<uint32_t>(this->payload % rhs.payload);
    }

    CounterOB<T> operator^(const CounterOB<T>& rhs)
    {
        return CounterOB<T>(this->payload ^ rhs.payload);
    }

    bool operator&(const CounterOB<T>& rhs)
    {
        return this->payload & rhs.payload;
    }

    bool operator|(const CounterOB<T>& rhs)
    {
        return this->payload | rhs.payload;
    }

    void operator=(const CounterOB<T>& rhs)
    {
        this->payload = rhs.payload;
        rhs.payload - this->payload > 0 ? this->num_increments += rhs.payload - this->payload : this->num_decrements -= this->payload - rhs.payload;
    }

    bool operator<(const CounterOB<T>& rhs)
    {
        return this->payload < rhs.payload;
    }

    bool operator>(const CounterOB<T>& rhs)
    {
        return this->payload > rhs.payload;
    }

    bool operator<=(const CounterOB<T>& rhs)
    {
        return this->payload <= rhs.payload;
    }

    bool operator>=(const CounterOB<T>& rhs)
    {
        return this->payload >= rhs.payload;
    }

    CounterOB<T>& operator++(int)
    {
        this->payload++;
        this->num_increments++;
        return *this;
    }

    CounterOB<T>& operator--(int)
    {
        this->payload--;
        this->num_decrements++;
        return *this;
    }

    bool operator==(const CounterOB<T>& rhs)
    {
        return this->payload == rhs.payload;
    }

    bool operator!=(const CounterOB<T>& rhs)
    {
        return this->payload != rhs.payload;
    }

    void operator+=(const CounterOB<T>& rhs)
    {
        this->payload += rhs.payload;
        rhs.payload > 0 ? this->num_increments += rhs.payload : this->num_decrements += rhs.payload;
    }

    void operator-=(const CounterOB<T>& rhs)
    {
        this->payload -= rhs.payload;
        rhs.payload > 0 ? this->num_decrements += rhs.payload : this->num_increments += rhs.payload;
    }

#ifdef BUILD_TESTING
    const uint32_t& query_id() const
    {
        return this->id;
    }

    const T& query_payload() const
    {
        return this->payload;
    }

    const T& query_num_increments() const
    {
        return this->num_increments;
    }

    const T& query_num_decrements() const
    {
        return this->num_decrements;
    }
#endif
};

}   // namespace operation
}   // namespace crdt

#endif  // __COUNTEROB_H__
