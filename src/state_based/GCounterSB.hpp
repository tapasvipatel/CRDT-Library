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

#ifndef __GCOUNTERSB_H__
#define __GCOUNTERSB_H__

#include "../CrdtHandle.hpp"
#include "../CrdtObject.hpp"
#include <map>
#include <vector>


namespace crdt
{
namespace state
{

/*
* metadata template class for CRDT counter
*/
template<typename T=int32_t>
class GCounterMetadata : CrdtMetaData
{
private:
    uint32_t id;
    T payload;

public:
    GCounterMetadata(uint32_t id) : CrdtMetaData(CrdtType::GCounterSBType)
    {
        this->id = id;
        this->payload = 0;
    }

    ~GCounterMetadata()
    {
        ;
    }
    
    void merge(T payload)
    {
        this->payload = max(this->payload, payload);
    }

#ifdef BUILD_TESTING
    void updatePayload(T payload)
    {
        this->payload = payload;
    }

#endif
};

/*
* template class for CRDT Gcounter
*/
template<typename T=int32_t>
class GCounterSB : CrdtObject<T>
{
private:
    uint32_t id;
    T payload;
    std::map<uint32_t,GCounterMetadata<T>> replica_metadata;

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

    bool updateInternalPayload()
    {
        T curr = T();
        std::map<uint32_t,GCounterMetadata<T>>::iterator it;

        for(it = this->replica_metadata.begin(); it != this->replica_metadata.end(); it++)
        {
            curr += it->second.payload;
        }

        this->payload = curr;
    }

    bool updateExternalPayload()
    {
        std::map<uint32_t,GCounterMetadata<T>>::iterator it;
        it = this->replica_metadata.find(this->id);
        it->second.payload = this->payload;
    }

public:
    GCounterSB(uint32_t id, T payload)
    {
        this->id = id;
        this->payload = payload;
    }
    
    ~GCounterSB()
    {
        ;
    }

#ifdef BUILD_TESTING
    const T& queryId() const
    {
        return this->id;
    }

    const T& queryPayload() const
    {
        return this->payload;
    }

    void addExternalReplica(GCounterMetadata external_replica_metadata)
    {
        this->replica_metadata.insert(std::pair<uint32_t, GCounterMetadata>(external_replica_metadata.id, external_replica_metadata));
    }
#endif
};

}   // namespace state
}   // namespace crdt

#endif  // __GCOUNTERSB_H__
